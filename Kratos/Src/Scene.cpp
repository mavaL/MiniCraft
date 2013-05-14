#include "stdafx.h"
#include "Scene.h"
#include "GameDefine.h"
#include "SceneSerializer.h"
#include "OgreManager.h"

using namespace Ogre;

namespace Kratos
{

	Scene::Scene()
		:m_pTerrain(nullptr)
		,m_terrainGroup(nullptr)
		,m_terrainOption(nullptr)
		,m_sunLightDir(FLOAT3(-0.8f, -1.0f, 0.4f))
		,m_sunLightDiffuse(COLOR::White)
		,m_ambient(COLOR::White)
	{
		m_sunLightDir.normalise();
	}

	void Scene::Load( const STRING& sceneName, const STRING& sceneGroup, SceneSerializer* pHandler )
	{
		STRING extname;
		Ogre::StringUtil::splitBaseFilename(sceneName, m_sceneName, extname);
		pHandler->LoadScene(sceneName, sceneGroup, this);
	}

	void Scene::Load( const STRING& fullPath, SceneSerializer* pHandler )
	{
		String pathname, basename, extname;
		StringUtil::splitFullFilename(fullPath, basename, extname, pathname);

		const String tmpGroup("#Temp");
		ResourceGroupManager::getSingleton().addResourceLocation(pathname, "FileSystem", tmpGroup);

		String sceneName = basename + "." + extname;
		Load(sceneName, tmpGroup, pHandler);
	}

	void Scene::Save(const STRING& fullPath, SceneSerializer* pHandler)
	{
		pHandler->SaveScene(fullPath, this);
	}

	void Scene::Reset()
	{
		COgreManager& mgr = RenderManager;
		mgr.ResetEffect();
		mgr.DestroySunLight();

		if(m_pTerrain)
		{
			m_terrainGroup->removeTerrain(0, 0);
			m_pTerrain = nullptr;
		}
		if(m_terrainGroup)
		{
			delete m_terrainGroup;
			m_terrainGroup = nullptr;
		}
		if(m_terrainOption)
		{
			delete m_terrainOption;
			m_terrainOption = nullptr;
		}

		mgr.m_pSceneMgr->clearScene();
	}

	void Scene::New(const STRING& sceneName)
	{
		m_sceneName = sceneName;

		COgreManager& ogreMgr = COgreManager::GetSingleton();
		Ogre::SceneManager* sm = ogreMgr.m_pSceneMgr;
		//环境光
		sm->setAmbientLight(m_ambient);

		//全局光
		ogreMgr.CreateSunLight(m_sunLightDir, m_sunLightDiffuse);

		float WORLD_SIZE = 128;
		Ogre::uint16 MAP_SIZE = 129;
		Ogre::Vector3 ORIGIN = Ogre::Vector3::ZERO;

		m_terrainOption = new TerrainGlobalOptions;
		m_terrainGroup = new TerrainGroup(sm, Terrain::ALIGN_X_Z, MAP_SIZE, WORLD_SIZE);
		m_terrainGroup->setOrigin(ORIGIN);

		// 	MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
		// 	MaterialManager::getSingleton().setDefaultAnisotropy(7);

		m_terrainOption->setMaxPixelError(8);
		//m_terrainOption->setCompositeMapDistance(3000);
		//m_terrainOption->setUseRayBoxDistanceCalculation(true);
		//m_terrainOption->getDefaultMaterialGenerator()->setDebugLevel(1);
		//m_terrainOption->setLightMapSize(512);
		m_terrainOption->setVisibilityFlags(eRenderType_Terrain);
		m_terrainOption->setQueryFlags(eQueryType_Terrain);

		ogreMgr.SetRenderingStyle();

		Ogre::TerrainMaterialGeneratorA::SM2Profile* matProfile = 
			static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(m_terrainOption->getDefaultMaterialGenerator()->getActiveProfile());
		matProfile->setLightmapEnabled(false);

		// Configure default import settings for if we use imported image
		Terrain::ImportData& defaultimp = m_terrainGroup->getDefaultImportSettings();
		defaultimp.terrainSize = MAP_SIZE;
		defaultimp.worldSize = WORLD_SIZE;
		defaultimp.inputScale = 1.0f;
		defaultimp.minBatchSize = 17;
		defaultimp.maxBatchSize = 65;

		size_t maxLayer = GetTerrainMaxLayer();
		defaultimp.layerList.resize(maxLayer);
		for (size_t iLayer=0; iLayer<maxLayer; ++iLayer)
		{
			defaultimp.layerList[iLayer].worldSize = 128;
			defaultimp.layerList[iLayer].textureNames.clear();
			defaultimp.layerList[iLayer].textureNames.clear();
		}

		//仅初始化地形纹理第1层
		defaultimp.layerList[0].worldSize = 128;
		defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
		defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");

		//初始化平坦地形
		m_terrainGroup->defineTerrain(0, 0, 0.0f);

		// sync load since we want everything in place when we start
		m_terrainGroup->loadTerrain(0, 0);

		m_terrainGroup->freeTemporaryResources();
		m_pTerrain = m_terrainGroup->getTerrain(0, 0);

		{
			ShadowParams& params = ogreMgr.GetShadowParams();
			ogreMgr.GetEffectConfig().bShadow = false;
			params["FarDistance"]					= "300";
			params["SplitPadding"]					= "1";
			params["AdjustFactor"]					= "0.5 0.8 2";
			params["UseSimpleAdjust"]				= "true";
			params["CameraLightDirectionThreshold"] = "45";
			params["ShadowMapSize"]					= "2048 1024 1024";
			params["SelfShadow"]					= "false";
			params["RenderBackFace"]				= "true";
			params["PssmLambda"]					= "0.75";
			params["LightExtrusionDistance"]		= "10000";
		}

		{
			SsaoParams& params = ogreMgr.GetSsaoParams();
			ogreMgr.GetEffectConfig().bSSAO = false;
			params["SampleLength"]			= "20";
			params["OffsetScale"]			= "1";
			params["DefaultAccessibility"]	= "0.5";
			params["EdgeHighlight"]			= "1.99";
		}

		ogreMgr.ApplyShadowParams();
		ogreMgr.ApplySsaoParams();
	}

}


