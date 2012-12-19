#include "stdafx.h"
#include "ManipulatorTerrain.h"
#include "../DotSceneLoader.h"
#include "../DotSceneSerializer.h"
#include "ManipulatorScene.h"
#include <OgreStreamSerialiser.h>
#include <OgreDeflate.h>
#include "../EditorDefine.h"


const	int	TERRAIN_SIZE	=	65;
const	int	WORLD_SIZE	=	128;


ManipulatorTerrain::ManipulatorTerrain()
{
	m_pSceneMgr = Ogre::Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME);
	assert(m_pSceneMgr);
}

void ManipulatorTerrain::NewFlatTerrain()
{
	m_terrainOption.reset(new TerrainGlobalOptions);
	m_terrainGroup.reset(new TerrainGroup(m_pSceneMgr, Terrain::ALIGN_X_Z, (Ogre::uint16)TERRAIN_SIZE, (float)WORLD_SIZE));
	m_terrainGroup->setOrigin(Vector3(0, 0, 0));

	_ConfigureTerrainDefaults();
	
	//初始化平坦地形
	m_terrainGroup->defineTerrain(0, 0, 0.0f);

	// sync load since we want everything in place when we start
	m_terrainGroup->loadTerrain(0, 0);

// 	if (mTerrainsImported)
// 	{
// 		TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
// 		while(ti.hasMoreElements())
// 		{
// 			Terrain* t = ti.getNext()->instance;
// 			initBlendMaps(t);
// 		}
// 	}
	m_terrainGroup->freeTemporaryResources();
}

void ManipulatorTerrain::_ConfigureTerrainDefaults()
{
// 	MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
// 	MaterialManager::getSingleton().setDefaultAnisotropy(7);

	// Configure global
	m_terrainOption->setMaxPixelError(8);
	// testing composite map
	//m_terrainOption->setCompositeMapDistance(3000);
	//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
	//mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
	//mTerrainGlobals->setLightMapSize(256);

	//matProfile->setLightmapEnabled(false);
	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	//mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
	m_terrainOption->setCompositeMapAmbient(m_pSceneMgr->getAmbientLight());
	//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
	//mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Terrain::ImportData& defaultimp = m_terrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = TERRAIN_SIZE;
	defaultimp.worldSize = WORLD_SIZE;
	defaultimp.inputScale = 600;
	defaultimp.minBatchSize = 17;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 128;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 128;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 128;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

void ManipulatorTerrain::Load( rapidxml::xml_node<>* XMLNode )
{
	m_terrainOption.reset(new TerrainGlobalOptions);

	Ogre::Real worldSize = DotSceneLoader::getAttribReal(XMLNode, "worldSize");
	int mapSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("mapSize")->value());
	//bool colourmapEnabled = DotSceneLoader::getAttribBool(XMLNode, "colourmapEnabled");
	//int colourMapTextureSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("colourMapTextureSize")->value());
	//int compositeMapDistance = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningCompositeMapDistance")->value());
	int maxPixelError = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningMaxPixelError")->value());

	//     Ogre::Vector3 lightdir(0, -0.3, 0.75);
	//     lightdir.normalise();
	//     Ogre::Light* l = mSceneMgr->createLight("tstLight");
	//     l->setType(Ogre::Light::LT_DIRECTIONAL);
	//     l->setDirection(lightdir);
	//     l->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
	//     l->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	m_terrainOption->setMaxPixelError((Ogre::Real)maxPixelError);
	//m_terrainOption->setCompositeMapDistance((Ogre::Real)compositeMapDistance);
	// mTerrainGlobalOptions->setLightMapDirection(lightdir);
	m_terrainOption->setCompositeMapAmbient(m_pSceneMgr->getAmbientLight());
	//mTerrainGlobalOptions->setCompositeMapDiffuse(l->getDiffuseColour());

	//mSceneMgr->destroyLight("tstLight");

	m_terrainGroup.reset(new Ogre::TerrainGroup(m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, mapSize, worldSize));
	m_terrainGroup->setOrigin(Ogre::Vector3::ZERO);
	m_terrainGroup->setResourceGroup("General");

	//     rapidxml::xml_node<>* pElement;
	//     rapidxml::xml_node<>* pPageElement;

	//     // Process terrain pages (*)
	//     pElement = XMLNode->first_node("terrainPages");
	//     if(pElement)
	//     {
	//         pPageElement = pElement->first_node("terrainPage");
	//         while(pPageElement)
	//         {
	//             processTerrainPage(pPageElement);
	//             pPageElement = pPageElement->next_sibling("terrainPage");
	//         }
	//     }

	//加载地形数据
	ManipulatorScene& manScene = ManipulatorScene::GetSingleton();
	std::wstring fullPath(manScene.GenerateSceneFullPath());
	fullPath += L"terrain.dat";
	m_terrainGroup->defineTerrain(0, 0, Utility::UnicodeToEngine(fullPath));
	m_terrainGroup->loadTerrain(0, 0);
	m_terrainGroup->freeTemporaryResources();
	//mTerrain->setPosition(mTerrainPosition);
}

void ManipulatorTerrain::Shutdown()
{
	m_terrainGroup.reset();
	m_terrainOption.reset();
}

void ManipulatorTerrain::Serialize( rapidxml::xml_document<>& doc, rapidxml::xml_node<>* XMLNode )
{
	String strWorldSize = Ogre::StringConverter::toString(WORLD_SIZE);
	String strTerrainSize = Ogre::StringConverter::toString(TERRAIN_SIZE);
	String strPixelError = Ogre::StringConverter::toString(m_terrainOption->getMaxPixelError());

	XMLNode->append_attribute(doc.allocate_attribute("worldSize", doc.allocate_string(strWorldSize.c_str())));
	XMLNode->append_attribute(doc.allocate_attribute("mapSize", doc.allocate_string(strTerrainSize.c_str())));
	XMLNode->append_attribute(doc.allocate_attribute("tuningMaxPixelError", doc.allocate_string(strPixelError.c_str())));

	//保存地形数据
	ManipulatorScene& manScene = ManipulatorScene::GetSingleton();
	std::wstring fullPath(manScene.GenerateSceneFullPath());
	fullPath += L"terrain.dat";
	Ogre::DataStreamPtr stream = Ogre::Root::getSingleton().createFileStream(Utility::UnicodeToEngine(fullPath), 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	//文件压缩.地形数据貌似很大,以后研究为什么
	Ogre::DataStreamPtr compressStream(OGRE_NEW Ogre::DeflateStream(Utility::UnicodeToEngine(fullPath), stream));
	Ogre::StreamSerialiser ser(compressStream);

	m_terrainGroup->getTerrain(0, 0)->save(ser);
}
