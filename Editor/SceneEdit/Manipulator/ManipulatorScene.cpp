#include "stdafx.h"
#include "ManipulatorScene.h"
#include "../EditorDefine.h"
#include "Utility.h"
#include "Scene.h"
#include "OgreManager.h"


ManipulatorScene::ManipulatorScene()
:m_sceneName(L"")
,m_bIsSceneReay(false)
{
}

ManipulatorScene::~ManipulatorScene()
{
	
}
void ManipulatorScene::Init()
{
	Ogre::StringVectorPtr loc = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("Scene", "*Scene");
	assert(!loc->empty());
	m_scenePath = Utility::EngineToUnicode(loc->at(0));
	m_scenePath += L"\\";

	m_manipulatorTerrain = new ManipulatorTerrain;
	m_manipulatorObject = new ManipulatorObject;
	m_manipulatorNavMesh = new ManipulatorNavMesh;
	m_manipulatorCamera = new ManipulatorCamera;
	m_manipulatorResource = new ManipulatorResource;
	m_manipulatorGameData = new ManipulatorGameData;

	m_pCurScene = new Scene;
}

void ManipulatorScene::Shutdown()
{
	SAFE_DELETE(m_pCurScene);
	SAFE_DELETE(m_manipulatorTerrain);
	SAFE_DELETE(m_manipulatorObject);
	SAFE_DELETE(m_manipulatorNavMesh);
	SAFE_DELETE(m_manipulatorCamera);
	SAFE_DELETE(m_manipulatorResource);
	SAFE_DELETE(m_manipulatorGameData);
}

void ManipulatorScene::SceneNew(const std::wstring& sceneName)
{
	m_pCurScene->New();

	m_sceneName = sceneName;
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
	
	m_pCurScene->Load(fullpath, this);

	m_bIsSceneReay = true;

	//回调事件
	Excute([](ManipulatorSceneEventCallback* callback){ callback->OnSceneOpen(); });

	OnGizmoNodeReset();
}

void ManipulatorScene::SceneSave()
{
	Ogre::String fullPath = Utility::UnicodeToEngine(m_scenePath+m_sceneName);
	//创建场景文件夹
	::CreateDirectoryA(fullPath.c_str(), NULL);

	fullPath += "\\" + Utility::UnicodeToEngine(m_sceneName) + ".scene";
	m_pCurScene->Save(fullPath, this);

	m_manipulatorGameData->SaveAllXml();
}

void ManipulatorScene::SceneClose()
{
	m_pCurScene->Reset();
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

void ManipulatorScene::_LoadObjects( rapidxml::xml_node<>* node )
{
	m_manipulatorObject->Load(node);
}

void ManipulatorScene::_SaveTerrain( rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode )
{
	m_manipulatorTerrain->Serialize(doc, XMLNode);
}

void ManipulatorScene::_SaveObjects( rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode )
{
	m_manipulatorObject->Serialize(doc, XMLNode);
}


