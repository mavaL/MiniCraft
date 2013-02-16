#include "Scene.h"
#include <OgreResourceGroupManager.h>
#include "SceneSerializer.h"

using namespace Ogre;

Scene::Scene()
:m_pTerrain(nullptr)
,m_terrainGroup(nullptr)
,m_terrainOption(nullptr)
{

}

void Scene::Load( const std::string& sceneName, const std::string& sceneGroup, SceneSerializer* pHandler )
{
	pHandler->LoadScene(sceneName, sceneGroup);
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

void Scene::Save()
{

}

void Scene::Reset()
{

}

void Scene::_SetTerrainParam( Ogre::TerrainGroup* group, Ogre::TerrainGlobalOptions* option, Ogre::Terrain* terrain )
{
	assert(m_terrainGroup ==nullptr && m_terrainOption == nullptr && m_pTerrain == nullptr);
	m_terrainGroup = group;
	m_terrainOption = option;
	m_pTerrain = terrain;
}
