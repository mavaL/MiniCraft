#include "stdafx.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "GameDefine.h"
#include "OgreManager.h"
#include "DeferredShading/TerrainMaterialGeneratorD.h"
#include "DeferredShading/DeferredShading.h"

using namespace Ogre;

namespace Kratos
{
	const Ogre::String SCENE_VERSION = "0.4";

	SceneSerializer::SceneSerializer()
		:m_pOwner(nullptr)
		,m_sceneGroup(StringUtil::BLANK)
	{

	}

	void SceneSerializer::LoadScene( const STRING& sceneName, const STRING& sceneGroup, Scene* pOwner )
	{
		DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(sceneName, sceneGroup);
		char* szData = strdup(stream->getAsString().c_str());

		m_pOwner = pOwner;
		m_sceneGroup = sceneGroup;

		//parse
		rapidxml::xml_document<> XMLDoc;
		rapidxml::xml_node<>* XMLRoot;
		XMLDoc.parse<0>(szData);

		// Grab the scene node
		XMLRoot = XMLDoc.first_node("scene");

		// Validate the File
		const String strVer = XMLRoot->first_attribute("formatVersion")->value();
		if(strVer != SCENE_VERSION)
		{
			assert(0 && "文件格式与编辑器版本不匹配!");
			return;
		}

		rapidxml::xml_node<>* pElement = XMLRoot->first_node("terrain");
		assert(pElement);
		_LoadTerrain(pElement);

		pElement = XMLRoot->first_node("effect");
		assert(pElement);
		_LoadEffect(pElement);

		pElement = XMLRoot->first_node("objects");
		assert(pElement);
		_LoadObjects(pElement);

		free(szData);
	}

	void SceneSerializer::SaveScene(const STRING& fullPath, Scene* pOwner)
	{
		using namespace rapidxml;

		m_pOwner = pOwner;

		xml_document<> doc;  
		//文件头
		xml_node<>* rot = doc.allocate_node(rapidxml::node_pi, doc.allocate_string("xml version='1.0' encoding='utf-8'"));
		doc.append_node(rot);

		//scene节
		xml_node<>* sceneNode =   doc.allocate_node(node_element, "scene");
		sceneNode->append_attribute(doc.allocate_attribute("formatVersion", doc.allocate_string(SCENE_VERSION.c_str())));
		doc.append_node(sceneNode);

		//effect节
		xml_node<>* effectNode =   doc.allocate_node(node_element, "effect");
		_SaveEffect(&doc, effectNode);
		sceneNode->append_node(effectNode);

		//terrain节
		xml_node<>* terrainNode =   doc.allocate_node(node_element, "terrain");
		_SaveTerrain(&doc, terrainNode);
		sceneNode->append_node(terrainNode);

		//object节
		xml_node<>* objsNode =   doc.allocate_node(node_element, "objects");
		_SaveObjects(&doc, objsNode);
		sceneNode->append_node(objsNode);

		std::ofstream out(fullPath);
		out << doc;
	}

	void SceneSerializer::_LoadTerrain( rapidxml::xml_node<>* node )
	{
		Ogre::SceneManager* sm = RenderManager.m_pSceneMgr;
		TerrainGlobalOptions* option = new TerrainGlobalOptions;

		float worldSize = StringConverter::parseReal(node->first_attribute("worldSize")->value());
		int mapSize = StringConverter::parseInt(node->first_attribute("mapSize")->value());
		int maxPixelError = StringConverter::parseInt(node->first_attribute("tuningMaxPixelError")->value());

		option->setMaxPixelError((Ogre::Real)maxPixelError);
		option->setVisibilityFlags(eRenderType_Terrain);
		option->setQueryFlags(eQueryType_Terrain);

		TerrainGroup* pTerrainGroup = new TerrainGroup(sm, Terrain::ALIGN_X_Z, mapSize, worldSize);
		pTerrainGroup->setOrigin(Vector3::ZERO);

		RenderManager.SetRenderingStyle();

		//加载地形数据
		const STRING dataFilename = m_pOwner->GetSceneName() + "_terrain.dat";
		pTerrainGroup->setResourceGroup(m_sceneGroup);
		pTerrainGroup->defineTerrain(0, 0, dataFilename);
		pTerrainGroup->loadTerrain(0, 0);
		pTerrainGroup->freeTemporaryResources();

		m_pOwner->m_pTerrain = pTerrainGroup->getTerrain(0, 0);
		m_pOwner->m_terrainGroup = pTerrainGroup;
		m_pOwner->m_terrainOption = option;
	}

	void SceneSerializer::_LoadEffect( rapidxml::xml_node<>* node )
	{
		COgreManager& ogreMgr = COgreManager::GetSingleton();
		SceneManager* sm = ogreMgr.m_pSceneMgr;

		//全局光
		ogreMgr.CreateSunLight(m_pOwner->GetSunLightDirection(), m_pOwner->GetSunLightDiffuse());

		//shadow
		{
			rapidxml::xml_node<>* shadowNode = node->first_node("shadow");

			//环境光
			const String strAmbient = shadowNode->first_attribute("AmbientLight")->value();
			m_pOwner->m_ambient = Ogre::StringConverter::parseColourValue(strAmbient);
			sm->setAmbientLight(m_pOwner->m_ambient);

			ogreMgr.GetEffectConfig().bShadow = Ogre::StringConverter::parseBool(shadowNode->first_attribute("EnableShadow")->value());

			Kratos::ShadowParams& shadowParams = ogreMgr.GetShadowParams();
			shadowParams["FarDistance"]		= shadowNode->first_attribute("FarDistance")->value();
			shadowParams["SplitPadding"]	= shadowNode->first_attribute("SplitPadding")->value();
			shadowParams["AdjustFactor"]	= shadowNode->first_attribute("AdjustFactor")->value();
			shadowParams["UseSimpleAdjust"] = shadowNode->first_attribute("UseSimpleAdjust")->value();
			shadowParams["CameraLightDirectionThreshold"] = shadowNode->first_attribute("CameraLightDirectionThreshold")->value();
			shadowParams["ShadowMapSize"]	= shadowNode->first_attribute("ShadowMapSize")->value();
			shadowParams["SelfShadow"]		= shadowNode->first_attribute("SelfShadow")->value();
			shadowParams["RenderBackFace"]	= shadowNode->first_attribute("RenderBackFace")->value();
			shadowParams["PssmLambda"]		= shadowNode->first_attribute("PssmLambda")->value();
			shadowParams["LightExtrusionDistance"] = shadowNode->first_attribute("LightExtrusionDistance")->value();

#ifdef FORWARD_RENDERING
			TerrainMaterialGeneratorA::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile*>(
				TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
			matProfile->setReceiveDynamicShadowsEnabled(true);
			matProfile->setReceiveDynamicShadowsDepth(true);
			matProfile->setReceiveDynamicShadowsPSSM(ogreMgr.GetShadowCameraSetup());
#else	//deferred shading
			TerrainGlobalOptions::getSingleton().setRenderQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_2);
			sm->getRenderQueue()->getQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_2)->setShadowsEnabled(false);
#endif
			ogreMgr.ApplyShadowParams();
		}

		//ssao
		{
			rapidxml::xml_node<>* ssaoNode = node->first_node("ssao");

			ogreMgr.GetEffectConfig().bSSAO = StringConverter::parseBool(ssaoNode->first_attribute("EnableSSAO")->value());

			Kratos::SsaoParams& ssaoParams = ogreMgr.GetSsaoParams();
			ssaoParams["SampleLength"]	= ssaoNode->first_attribute("SampleLength")->value();
			ssaoParams["OffsetScale"]	= ssaoNode->first_attribute("OffsetScale")->value();
			ssaoParams["DefaultAccessibility"]	= ssaoNode->first_attribute("DefaultAccessibility")->value();
			ssaoParams["EdgeHighlight"]	= ssaoNode->first_attribute("EdgeHighlight")->value();

			ogreMgr.ApplySsaoParams();
		}

		//sharpen
		bool bSharpen = StringConverter::parseBool(node->first_node("sharpen")->first_attribute("EnableSharpen")->value());
		RenderManager.EnableSharpen(bSharpen);

		//fxaa
		bool bFXAA = StringConverter::parseBool(node->first_node("fxaa")->first_attribute("EnableFXAA")->value());
		RenderManager.EnableFXAA(bFXAA);
	}
}


