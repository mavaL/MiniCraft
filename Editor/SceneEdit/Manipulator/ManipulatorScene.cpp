#include "stdafx.h"
#include "ManipulatorScene.h"
#include "ManipulatorTerrain.h"
#include "../DotSceneSerializer.h"
#include "../DotSceneLoader.h"

ManipulatorScene::ManipulatorScene()
:m_sceneName(L"")
{
	WCHAR path[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, path);
	m_scenePath = path;
	m_scenePath += L"\\..\\..\\..\\..\\Editor\\Scene\\";

	m_manipulatorTerrain = new ManipulatorTerrain;
	m_sceneSerializer = new DotSceneSerialezer;
	m_sceneLoader = new DotSceneLoader;
}

ManipulatorScene::~ManipulatorScene()
{
	
}

void ManipulatorScene::SceneNew(const std::wstring& sceneName)
{
	m_sceneName = sceneName;
	m_manipulatorTerrain->NewFlatTerrain();
}

void ManipulatorScene::SceneOpen(const std::wstring& filepath)
{
	String fullpath(Utility::UnicodeToEngine(filepath)), path, basename, extname;
	Ogre::StringUtil::splitFullFilename(fullpath, basename, extname, path);

	m_sceneName = Utility::EngineToUnicode(basename);

	m_sceneLoader->parseDotScene(fullpath);
}

void ManipulatorScene::SceneSave()
{
	m_sceneSerializer->Serialize(Utility::UnicodeToEngine(m_scenePath+m_sceneName), Utility::UnicodeToEngine(m_sceneName));
}

void ManipulatorScene::SceneClose()
{
	m_manipulatorTerrain->Shutdown();

	Ogre::SceneManager* pSceneMgr = Ogre::Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME);
	assert(pSceneMgr);
	pSceneMgr->clearScene();
}

void ManipulatorScene::Shutdown()
{
	SAFE_DELETE(m_manipulatorTerrain);
	SAFE_DELETE(m_sceneSerializer);
	SAFE_DELETE(m_sceneLoader);
}

const std::wstring ManipulatorScene::GenerateSceneFullPath()
{
	assert(!m_scenePath.empty() && !m_sceneName.empty());

	std::wstring fullPath(m_scenePath);
	fullPath += m_sceneName;
	fullPath += L"\\";

	return fullPath;
}
