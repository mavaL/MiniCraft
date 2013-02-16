#include "SceneSerializer.h"
#include "Scene.h"
#include <Ogre.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

using namespace Ogre;

SceneSerializer::SceneSerializer()
:m_pSerializerMgr(nullptr)
,m_pOwner(nullptr)
,m_sceneGroup(StringUtil::BLANK)
{

}

void SceneSerializer::LoadScene( const std::string& sceneName, const std::string& sceneGroup )
{
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(sceneName, sceneGroup);
	char* szData = strdup(stream->getAsString().c_str());

	m_sceneGroup = sceneGroup;

	//parse
	rapidxml::xml_document<> XMLDoc;
	rapidxml::xml_node<>* XMLRoot;
	XMLDoc.parse<0>(szData);

	// Grab the scene node
	XMLRoot = XMLDoc.first_node("scene");

	// Validate the File
	const float SCENE_VERSION = 0.2;
	const String strVer = XMLRoot->first_attribute("formatVersion")->value();
	float fVersion = StringConverter::parseReal(strVer);
	if(fVersion != SCENE_VERSION)
	{
		assert(0);
		return;
	}

	rapidxml::xml_node<>* pElement = XMLRoot->first_node("terrain");
	assert(pElement);
	_LoadTerrain(pElement);

	pElement = XMLRoot->first_node("objects");
	assert(pElement);
	_LoadObjects(pElement);

	free(szData);
}

void SceneSerializer::SaveScene()
{

}

void SceneSerializer::_LoadTerrain( rapidxml::xml_node<>* node )
{
	TerrainGlobalOptions* option = new TerrainGlobalOptions;

	float worldSize = StringConverter::parseReal(node->first_attribute("worldSize")->value());
	int mapSize = StringConverter::parseInt(node->first_attribute("mapSize")->value());
	//bool colourmapEnabled = DotSceneLoader::getAttribBool(XMLNode, "colourmapEnabled");
	//int colourMapTextureSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("colourMapTextureSize")->value());
	//int compositeMapDistance = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningCompositeMapDistance")->value());
	int maxPixelError = StringConverter::parseInt(node->first_attribute("tuningMaxPixelError")->value());

	//     Ogre::Vector3 lightdir(0, -0.3, 0.75);
	//     lightdir.normalise();
	//     Ogre::Light* l = mSceneMgr->createLight("tstLight");
	//     l->setType(Ogre::Light::LT_DIRECTIONAL);
	//     l->setDirection(lightdir);
	//     l->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
	//     l->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	option->setMaxPixelError((Ogre::Real)maxPixelError);
	option->setCompositeMapDistance(3000);
	option->setUseRayBoxDistanceCalculation(true);
	// mTerrainGlobalOptions->setLightMapDirection(lightdir);
	option->setCompositeMapAmbient(m_pSerializerMgr->getAmbientLight());
	//mTerrainGlobalOptions->setCompositeMapDiffuse(l->getDiffuseColour());

	//mSceneMgr->destroyLight("tstLight");

	TerrainGroup* pTerrainGroup = new TerrainGroup(m_pSerializerMgr, Terrain::ALIGN_X_Z, mapSize, worldSize);
	pTerrainGroup->setOrigin(Vector3::ZERO);

	//加载地形数据
	pTerrainGroup->setResourceGroup(m_sceneGroup);
	pTerrainGroup->defineTerrain(0, 0, "terrain.dat");
	pTerrainGroup->loadTerrain(0, 0);
	pTerrainGroup->freeTemporaryResources();

	Terrain* pTerrain = pTerrainGroup->getTerrain(0, 0);

	m_pOwner->_SetTerrainParam(pTerrainGroup, option, pTerrain);
}
