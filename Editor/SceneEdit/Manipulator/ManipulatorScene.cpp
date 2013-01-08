#include "stdafx.h"
#include "../DotSceneSerializer.h"
#include "../DotSceneLoader.h"
#include "ManipulatorScene.h"
#include "../EditorDefine.h"
#include "Utility.h"


ManipulatorScene::ManipulatorScene()
:m_sceneName(L"")
,m_bIsSceneReay(false)
,m_pSceneMgr(nullptr)
,m_pMainCamera(nullptr)
{
	WCHAR path[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, path);
	m_scenePath = path;
	m_scenePath += L"\\..\\..\\..\\..\\Editor\\Scene\\";
}

ManipulatorScene::~ManipulatorScene()
{
	
}
void ManipulatorScene::Init()
{
	m_sceneSerializer = new DotSceneSerialezer;
	m_sceneLoader = new DotSceneLoader;
	m_manipulatorTerrain = new ManipulatorTerrain;
	m_manipulatorObject = new ManipulatorObject;
	m_manipulatorNavMesh = new ManipulatorNavMesh;
}

void ManipulatorScene::Shutdown()
{
	SAFE_DELETE(m_manipulatorTerrain);
	SAFE_DELETE(m_manipulatorObject);
	SAFE_DELETE(m_manipulatorNavMesh);
	SAFE_DELETE(m_sceneSerializer);
	SAFE_DELETE(m_sceneLoader);
}

void ManipulatorScene::SceneNew(const std::wstring& sceneName)
{
	m_sceneName = sceneName;
	m_manipulatorTerrain->NewFlatTerrain();
	m_bIsSceneReay = true;

	//回调事件
	Excute([](ManipulatorSceneEventCallback* callback){ callback->OnSceneNew(); });

	OnGizmoNodeReset();
}

void ManipulatorScene::SceneOpen(const std::wstring& filepath)
{
	String fullpath(Utility::UnicodeToEngine(filepath)), path, basename, extname;
	Ogre::StringUtil::splitFullFilename(fullpath, basename, extname, path);

	m_sceneName = Utility::EngineToUnicode(basename);
	m_sceneLoader->parseDotScene(fullpath);
	
	m_bIsSceneReay = true;

	//回调事件
	Excute([](ManipulatorSceneEventCallback* callback){ callback->OnSceneOpen(); });

	OnGizmoNodeReset();
}

void ManipulatorScene::SceneSave()
{
	m_sceneSerializer->Serialize(Utility::UnicodeToEngine(m_scenePath+m_sceneName), Utility::UnicodeToEngine(m_sceneName));
}

void ManipulatorScene::SceneClose()
{
	m_manipulatorTerrain->Shutdown();
	m_pSceneMgr->clearScene();
	m_bIsSceneReay = false;

	//回调事件
	Excute([](ManipulatorSceneEventCallback* callback){ callback->OnSceneClose(); });
}

const std::wstring ManipulatorScene::GenerateSceneFullPath()
{
	assert(!m_scenePath.empty() && !m_sceneName.empty());

	std::wstring fullPath(m_scenePath);
	fullPath += m_sceneName;
	fullPath += L"\\";

	return fullPath;
}

void ManipulatorScene::OnGizmoNodeReset()
{
	m_manipulatorTerrain->OnGizmoNodeReset();
	m_manipulatorObject->OnGizmoNodeReset();
	m_manipulatorNavMesh->OnGizmoNodeReset();
}

void ManipulatorScene::OnFrameMove( float dt )
{
	m_manipulatorObject->OnFrameMove(dt);
}


