#include "SceneSerializer.h"
#include "Scene.h"
#include <Ogre.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
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

	void SceneSerializer::LoadScene( const std::string& sceneName, const std::string& sceneGroup, Scene* pOwner )
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

	void SceneSerializer::SaveScene(const std::string& fullPath, Scene* pOwner)
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

		TerrainGroup* pTerrainGroup = new TerrainGroup(sm, Terrain::ALIGN_X_Z, mapSize, worldSize);
		pTerrainGroup->setOrigin(Vector3::ZERO);

#ifdef FORWARD_RENDERING
		RenderManager.m_pDS->setActive(false);
		Ogre::TerrainGlobalOptions::getSingleton().setDefaultMaterialGenerator(
			Ogre::TerrainMaterialGeneratorPtr(new Ogre::TerrainMaterialGeneratorA));
		TerrainMaterialGeneratorA::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile*>(
			TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
		matProfile->setCompositeMapEnabled(false);

#else	//deferred shading
		RenderManager.m_pDS->setActive(true);
		Ogre::TerrainGlobalOptions::getSingleton().setDefaultMaterialGenerator(
			Ogre::TerrainMaterialGeneratorPtr(new Ogre::TerrainMaterialGeneratorD));
		TerrainMaterialGeneratorD::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorD::SM2Profile*>(
			TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
		matProfile->setCompositeMapEnabled(false);
#endif

		//加载地形数据
		pTerrainGroup->setResourceGroup(m_sceneGroup);
		pTerrainGroup->defineTerrain(0, 0, "terrain.dat");
		pTerrainGroup->loadTerrain(0, 0);
		pTerrainGroup->freeTemporaryResources();

		m_pOwner->m_pTerrain = pTerrainGroup->getTerrain(0, 0);
		m_pOwner->m_terrainGroup = pTerrainGroup;
		m_pOwner->m_terrainOption = option;
	}

	void SceneSerializer::_LoadEffect( rapidxml::xml_node<>* node )
	{
		SceneManager* sm = RenderManager.m_pSceneMgr;

		//全局光
		Light* pSunLight = sm->createLight("SunLight");
		pSunLight->setType(Light::LT_DIRECTIONAL);
		pSunLight->setDirection(m_pOwner->GetSunLightDirection());
		pSunLight->setDiffuseColour(m_pOwner->GetSunLightDiffuse());
		// 	pSunLight->setShadowFarClipDistance(250);
		// 	pSunLight->setShadowFarDistance(1000);

		//shadow
		{
			rapidxml::xml_node<>* shadowNode = node->first_node("shadow");

			//环境光
			const String strAmbient = shadowNode->first_attribute("AmbientLight")->value();
			sm->setAmbientLight(Ogre::StringConverter::parseColourValue(strAmbient));

			const bool param1 =		StringConverter::parseBool(shadowNode->first_attribute("EnableShadow")->value());
			const float param2 =	StringConverter::parseReal(shadowNode->first_attribute("FarDistance")->value());
			const float param3 =	StringConverter::parseReal(shadowNode->first_attribute("SplitPadding")->value());
			const Vector3 param4 =	StringConverter::parseVector3(shadowNode->first_attribute("AdjustFactor")->value());
			const bool param5 =		StringConverter::parseBool(shadowNode->first_attribute("UseSimpleAdjust")->value());
			const int param6 =		StringConverter::parseInt(shadowNode->first_attribute("CameraLightDirectionThreshold")->value());
			const Vector3 param7 =	StringConverter::parseVector3(shadowNode->first_attribute("ShadowMapSize")->value());
			const bool param8 =		StringConverter::parseBool(shadowNode->first_attribute("SelfShadow")->value());
			const bool param9 =		StringConverter::parseBool(shadowNode->first_attribute("RenderBackFace")->value());
			const float param10 =	StringConverter::parseReal(shadowNode->first_attribute("PssmLambda")->value());
			const float param11 =	StringConverter::parseReal(shadowNode->first_attribute("LightExtrusionDistance")->value());

			RenderManager.EnableShadow(param1);
			sm->setShadowFarDistance(param2);
			sm->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
			sm->setShadowDirectionalLightExtrusionDistance(param11);
			sm->setShadowCameraSetup(RenderManager.mPSSMSetup);
			sm->setShadowTextureCount(3);
			sm->setShadowTextureConfig(0, param7.x, param7.x, PF_FLOAT32_R);
			sm->setShadowTextureConfig(1, param7.y, param7.y, PF_FLOAT32_R);
			sm->setShadowTextureConfig(2, param7.z, param7.z, PF_FLOAT32_R);
			sm->setShadowTextureSelfShadow(param8);
			sm->setShadowCasterRenderBackFaces(param9);
			sm->setShadowTextureCasterMaterial("Ogre/shadow/depth/caster");

			PSSMShadowCameraSetup* cam = RenderManager.GetShadowCameraSetup();

			cam->setSplitPadding(param3);
			cam->calculateSplitPoints(3, RenderManager.m_pMainCamera->getNearClipDistance(), sm->getShadowFarDistance(), param10);
			cam->setOptimalAdjustFactor(0, param4.x);
			cam->setOptimalAdjustFactor(1, param4.y);
			cam->setOptimalAdjustFactor(2, param4.z);
			cam->setUseSimpleOptimalAdjust(param5);
			cam->setCameraLightDirectionThreshold(Degree(param6));


#ifdef FORWARD_RENDERING
			TerrainMaterialGeneratorA::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile*>(
				TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
			matProfile->setReceiveDynamicShadowsEnabled(true);
			matProfile->setReceiveDynamicShadowsDepth(true);
			matProfile->setReceiveDynamicShadowsPSSM(cam);
#else	//deferred shading
			TerrainGlobalOptions::getSingleton().setRenderQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_2);
			sm->getRenderQueue()->getQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_2)->setShadowsEnabled(false);
#endif
		}

		//ssao
		{
			rapidxml::xml_node<>* ssaoNode = node->first_node("ssao");

			const bool param1 =		StringConverter::parseBool(ssaoNode->first_attribute("EnableSSAO")->value());
			const float param2 =	StringConverter::parseReal(ssaoNode->first_attribute("SampleLength")->value());
			const float param3 =	StringConverter::parseReal(ssaoNode->first_attribute("OffsetScale")->value());
			const float param4 =	StringConverter::parseReal(ssaoNode->first_attribute("DefaultAccessibility")->value());
			const float param5 =	StringConverter::parseReal(ssaoNode->first_attribute("EdgeHighlight")->value());

			RenderManager.SetSSAOParam("cSampleLengthScreenSpace", param2/100, false);
			RenderManager.SetSSAOParam("cOffsetScale", param3/100, false);
			RenderManager.SetSSAOParam("cDefaultAccessibility", param4, false);
			RenderManager.SetSSAOParam("cEdgeHighlight", param5, true);
			RenderManager.EnableSSAO(param1);
		}
	}
}


