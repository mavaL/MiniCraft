#include "Scene.h"
#include <OgreResourceGroupManager.h>
#include <OgreSceneManager.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include "SceneSerializer.h"

using namespace Ogre;

Scene::Scene(Ogre::SceneManager* pSceneMgr)
:m_pTerrain(nullptr)
,m_terrainGroup(nullptr)
,m_terrainOption(nullptr)
,m_pSceneMgr(pSceneMgr)
,m_sunLightDir(Ogre::Vector3(0.55f, -0.3f, 0.75f))
,m_sunLightDiffuse(Ogre::ColourValue::White)
{
	
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
	if(m_pTerrain)
	{
		m_terrainGroup->unloadTerrain(0, 0);
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
	if(m_pSceneMgr)
		m_pSceneMgr->clearScene();
}
