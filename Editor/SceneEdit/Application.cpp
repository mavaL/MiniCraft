#include "stdafx.h"
#include "Application.h"
#include "EditorDefine.h"
#include "UI/DialogSceneNew.h"
#include "Manipulator/ManipulatorScene.h"
#include "Manipulator/ManipulatorAction.h"
#include "Action/ActionBase.h"
#include "MainFrm.h"
#include "Scene.h"
#include "OgreManager.h"


Application::Application()
{
}

void Application::Init( int width, int height, HWND hwnd, HWND hParent )
{
	RenderManager.Init(true, hwnd, hParent, width, height);

	ManipulatorScene& manScene = ManipulatorScene::GetSingleton();
	manScene.Init();
	manScene.GetGameData().LoadAllXml();
}

bool Application::Update()
{
	_UpdateInput(TIME_PER_FRAME);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnFrameMove(TIME_PER_FRAME);
	ManipulatorSystem.OnFrameMove(TIME_PER_FRAME);

	RenderManager.mRoot->renderOneFrame();

	return true;
}

void Application::Shutdown()
{
	ManipulatorSystem.Shutdown();
}

void Application::SceneNew()
{
	DialogSceneNew dlg;
	std::wstring newSceneName;
	if (IDOK == dlg.DoModal(ManipulatorSystem.GetScenePath(), newSceneName))
	{
		//先关闭当前场景
		//TODO:检测改动,提示保存
		SceneClose();
		ManipulatorSystem.SceneNew(newSceneName);
	}
}

void Application::SceneOpen()
{
	std::wstring strFilename;
	CFileDialog dlgFile(TRUE);	
	dlgFile.GetOFN().lpstrFilter = L"*.scene";
	dlgFile.GetOFN().lpstrDefExt = L"scene";
	//dlgFile.GetOFN().lpstrInitialDir = path; 
	dlgFile.GetOFN().lpstrTitle = L"Open Scene"; 

	if(IDOK == dlgFile.DoModal())
	{
		//先关闭当前场景
		//TODO:检测改动,提示保存
		SceneClose();

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
	Ogre::Camera* cam = RenderManager.m_pMainCamera;
	Ogre::RenderWindow* wnd = RenderManager.mWindow;

	//设备丢失
	if (cam)
	{
		wnd->windowMovedOrResized();	
		cam->setAspectRatio(wnd->getWidth()/(float)wnd->getHeight()); 
		//reset 设备
		wnd->update();
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
	Ogre::RenderWindow* wnd = RenderManager.mWindow;

	retParam.m_ptPixel = Ogre::Vector2((float)viewClientPt.x, (float)viewClientPt.y);
	retParam.m_ptRelative.x = viewClientPt.x / (float)wnd->getWidth();
	retParam.m_ptRelative.y = viewClientPt.y / (float)wnd->getHeight();

	static Ogre::Vector2 lastPt = retParam.m_ptPixel;
	retParam.m_ptDeltaRel = retParam.m_ptPixel - lastPt;
	retParam.m_ptDeltaRel.x /= wnd->getWidth();
	retParam.m_ptDeltaRel.y /= wnd->getHeight();

	const Ogre::Ray ray = RenderManager.m_pMainCamera->getCameraToViewportRay(retParam.m_ptRelative.x, retParam.m_ptRelative.y);
	retParam.m_bHitTerrain = ManipulatorSystem.GetTerrain().GetRayIntersectPoint(ray, retParam.m_ptTerrain);

	lastPt = retParam.m_ptPixel;
}




