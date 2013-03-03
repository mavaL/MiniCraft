#include "SceneSerializer.h"
#include "Scene.h"
#include <Ogre.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "OgreManager.h"
#include "DeferredShading/TerrainMaterialGeneratorD.h"


using namespace Ogre;

const Ogre::String SCENE_VERSION = "0.3";

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
		assert(0);
		return;
	}

	SceneManager* sm = RenderManager.m_pSceneMgr;
	//环境光
	const String strAmbient = XMLRoot->first_attribute("AmbientLight")->value();
	sm->setAmbientLight(Ogre::StringConverter::parseColourValue(strAmbient));

	//全局光
	Light* pSunLight = sm->createLight("SunLight");
	pSunLight->setType(Light::LT_DIRECTIONAL);
	pSunLight->setDirection(pOwner->GetSunLightDirection());
	pSunLight->setDiffuseColour(pOwner->GetSunLightDiffuse());
	pSunLight->setShadowFarClipDistance(250);
	pSunLight->setShadowFarDistance(1000);

	rapidxml::xml_node<>* pElement = XMLRoot->first_node("terrain");
	assert(pElement);
	_LoadTerrain(pElement);

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
	const String strAmbient = Ogre::StringConverter::toString(RenderManager.m_pSceneMgr->getAmbientLight());
	sceneNode->append_attribute(doc.allocate_attribute("formatVersion", doc.allocate_string(SCENE_VERSION.c_str())));
	sceneNode->append_attribute(doc.allocate_attribute("AmbientLight", doc.allocate_string(strAmbient.c_str())));
	doc.append_node(sceneNode);

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
	TerrainMaterialGeneratorA::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile*>(
		TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setCompositeMapEnabled(false);
#else	//deferred shading
	TerrainMaterialGeneratorD::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorD::SM2Profile*>(
		TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setCompositeMapEnabled(false);
#endif

 	//在地形材质生成前开启
 	RenderManager.EnableDeferredShading(true);
 
 	//阴影
 	RenderManager.InitShadowConfig();

	//加载地形数据
	pTerrainGroup->setResourceGroup(m_sceneGroup);
	pTerrainGroup->defineTerrain(0, 0, "terrain.dat");
	pTerrainGroup->loadTerrain(0, 0);
	pTerrainGroup->freeTemporaryResources();

	m_pOwner->m_pTerrain = pTerrainGroup->getTerrain(0, 0);
	m_pOwner->m_terrainGroup = pTerrainGroup;
	m_pOwner->m_terrainOption = option;
}
