#include "stdafx.h"
#include "Application.h"
#include "EditorDefine.h"
#include "UI/DialogSceneNew.h"
#include "Manipulator/ManipulatorScene.h"
#include "Manipulator/ManipulatorAction.h"
#include "Action/ActionBase.h"
#include "MainFrm.h"
#include "Scene.h"


Application::Application()
:m_pRoot(nullptr)
,m_pRenderWnd(nullptr)
{
}

void Application::Init( int width, int height, HWND hwnd, HWND hParent )
{
	_InitOgre(width, height, hwnd, hParent);
	ManipulatorSystem.Init();
	ManipulatorSystem.GetGameData().LoadAllXml();
}

void Application::_InitOgre(int width, int height, HWND hwnd, HWND hParent)
{
	using namespace Ogre;

	String ResourceCfg, PluginCfg;
#ifdef _DEBUG
	ResourceCfg = "resources_editor_d.cfg";
	PluginCfg = "plugins_d.cfg";
#else
	ResourceCfg = "resources_editor.cfg";
	PluginCfg = "plugins.cfg";
#endif
	
	m_pRoot = new Ogre::Root(PluginCfg);
	assert(m_pRoot);

	Ogre::ConfigFile cf;
	cf.load(ResourceCfg);
	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}

	RenderSystem* rs = m_pRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
	assert(rs);
	m_pRoot->setRenderSystem(rs);

	m_pRoot->initialise(false);

	Ogre::SceneManager* pSceneMgr = m_pRoot->createSceneManager(ST_GENERIC, SCENE_MANAGER_NAME);
	Ogre::Camera* pCamera = pSceneMgr->createCamera(MAIN_CAMERA_NAME);

	ManipulatorSystem.m_pSceneMgr = pSceneMgr;
	ManipulatorSystem.m_pMainCamera = pCamera;

	NameValuePairList params;
	params["externalWindowHandle"] = StringConverter::toString((unsigned int)hwnd);
	params["parentWindowHandle"] = StringConverter::toString((unsigned int)hParent);
	m_pRenderWnd = m_pRoot->createRenderWindow("MainWindow", width, height, false, &params);

	Viewport* vp = m_pRenderWnd->addViewport(pCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	pCamera->setAspectRatio((Ogre::Real)vp->getActualWidth() / (Ogre::Real)vp->getActualHeight());

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//初始化所有资源
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	m_pRenderWnd->setActive(true);

	//默认查询掩码
	Ogre::MovableObject::setDefaultQueryFlags(eQueryMask_Default);
}

bool Application::Update()
{
	_UpdateInput(TIME_PER_FRAME);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnFrameMove(TIME_PER_FRAME);
	ManipulatorSystem.OnFrameMove(TIME_PER_FRAME);

	m_pRoot->renderOneFrame();

	return true;
}

void Application::Shutdown()
{
	ManipulatorSystem.Shutdown();
	SAFE_DELETE(m_pRoot);
}

void Application::SceneNew()
{
	//先关闭当前场景
	//TODO:检测改动,提示保存
	SceneClose();

	DialogSceneNew dlg;
	std::wstring newSceneName;
	if (IDOK == dlg.DoModal(ManipulatorSystem.GetScenePath(), newSceneName))
	{
		ManipulatorSystem.SceneNew(newSceneName);
	}
}

void Application::SceneOpen()
{
	//先关闭当前场景
	//TODO:检测改动,提示保存
	SceneClose();

	WCHAR path[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, path);

	std::wstring strFilename;
	CFileDialog dlgFile(TRUE);	
	dlgFile.GetOFN().lpstrFilter = L"*.scene";
	dlgFile.GetOFN().lpstrDefExt = L"scene";
	//dlgFile.GetOFN().lpstrInitialDir = path; 
	dlgFile.GetOFN().lpstrTitle = L"Open Scene"; 

	if(IDOK == dlgFile.DoModal())
	{
		strFilename = dlgFile.GetOFN().lpstrFile;
		ManipulatorSystem.SceneOpen(strFilename);
	}
}

void Application::SceneSave()
{
	ManipulatorSystem.SceneSave();
}

void Application::SceneClose()
{
	ManipulatorSystem.SceneClose();
}

void Application::OnViewportResized()
{
	if (ManipulatorSystem.m_pMainCamera)
	{	
		//设备丢失
		m_pRenderWnd->windowMovedOrResized();	
		ManipulatorSystem.m_pMainCamera->setAspectRatio(m_pRenderWnd->getWidth()/(float)m_pRenderWnd->getHeight()); 
		//reset 设备
		m_pRenderWnd->update();	
	}
}

void Application::_UpdateInput(float dt)
{
	//取消当前编辑器激活状态
	if(GetAsyncKeyState(VK_ESCAPE) < 0)
		ManipulatorAction::GetSingleton().SetActiveAction(eActionType_None);
}

void Application::OnLButtonDown( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseLButtonDown(param);

	ManipulatorSystem.GetCamera().OnLButtonDown(param);
}

void Application::OnLButtonUp( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseLButtonUp(param);

	ManipulatorSystem.GetCamera().OnLButtonUp(param);
}

void Application::OnMouseMove( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseMove(param);

	ManipulatorSystem.GetCamera().OnMouseMove(param);
}

void Application::OnRButtonDown( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseRButtonDown(param);

	ManipulatorSystem.GetCamera().OnRButtonDown(param);
}

void Application::OnRButtonUp( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseRButtonDown(param);

	ManipulatorSystem.GetCamera().OnRButtonUp(param);
}

void Application::OnMouseWheel( short nNotch )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	ManipulatorSystem.GetCamera().OnMouseWheel(nNotch);
}

void Application::_CreateActionParam( const POINT& viewClientPt, SActionParam& retParam )
{
	retParam.m_ptPixel = Ogre::Vector2((float)viewClientPt.x, (float)viewClientPt.y);
	retParam.m_ptRelative.x = viewClientPt.x / (float)m_pRenderWnd->getWidth();
	retParam.m_ptRelative.y = viewClientPt.y / (float)m_pRenderWnd->getHeight();

	static Ogre::Vector2 lastPt = retParam.m_ptPixel;
	retParam.m_ptDeltaRel = retParam.m_ptPixel - lastPt;
	retParam.m_ptDeltaRel.x /= m_pRenderWnd->getWidth();
	retParam.m_ptDeltaRel.y /= m_pRenderWnd->getHeight();

	const Ogre::Ray ray = ManipulatorSystem.m_pMainCamera->getCameraToViewportRay(retParam.m_ptRelative.x, retParam.m_ptRelative.y);
	retParam.m_bHitTerrain = ManipulatorSystem.GetTerrain().GetRayIntersectPoint(ray, retParam.m_ptTerrain);

	lastPt = retParam.m_ptPixel;
}




