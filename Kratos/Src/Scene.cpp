#include "Scene.h"
#include <OgreResourceGroupManager.h>
#include <OgreSceneManager.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include "SceneSerializer.h"
#include "OgreManager.h"

using namespace Ogre;

namespace Kratos
{

	Scene::Scene()
		:m_pTerrain(nullptr)
		,m_terrainGroup(nullptr)
		,m_terrainOption(nullptr)
		,m_sunLightDir(Ogre::Vector3(-0.8f, -1.0f, 0.4f))
		,m_sunLightDiffuse(Ogre::ColourValue::White)
	{
		m_sunLightDir.normalise();
	}

	void Scene::Load( const std::string& sceneName, const std::string& sceneGroup, SceneSerializer* pHandler )
	{
		pHandler->LoadScene(sceneName, sceneGroup, this);
	}

	void Scene::Load( const std::string& fullPath, SceneSerializer* pHandler )
	{
		String pathname, basename, extname;
		StringUtil::splitFullFilename(fullPath, basename, extname, pathname);

		const String tmpGroup("#Temp");
		ResourceGroupManager::getSingleton().addResourceLocation(pathname, "FileSystem", tmpGroup);

		String sceneName = basename + "." + extname;
		Load(sceneName, tmpGroup, pHandler);
	}

	void Scene::Save(const std::string& fullPath, SceneSerializer* pHandler)
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

	void Scene::New()
	{
		//环境光
		RenderManager.m_pSceneMgr->setAmbientLight(Ogre::ColourValue::White);

		//全局光
		RenderManager.CreateSunLight(m_sunLightDir, m_sunLightDiffuse);

		float WORLD_SIZE = 128;
		Ogre::uint16 MAP_SIZE = 129;
		Ogre::Vector3 ORIGIN = Ogre::Vector3::ZERO;

		m_terrainOption = new TerrainGlobalOptions;
		m_terrainGroup = new TerrainGroup(RenderManager.m_pSceneMgr, Terrain::ALIGN_X_Z, MAP_SIZE, WORLD_SIZE);
		m_terrainGroup->setOrigin(ORIGIN);

		// 	MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
		// 	MaterialManager::getSingleton().setDefaultAnisotropy(7);

		//全局光
		m_terrainOption->setMaxPixelError(8);
		m_terrainOption->setCompositeMapDistance(3000);
		//m_terrainOption->setUseRayBoxDistanceCalculation(true);
		//m_terrainOption->getDefaultMaterialGenerator()->setDebugLevel(1);
		//m_terrainOption->setLightMapSize(512);

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
	}

}


