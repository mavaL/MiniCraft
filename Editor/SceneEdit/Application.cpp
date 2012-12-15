#include "stdafx.h"
#include "Application.h"
#include "UI/DialogSceneNew.h"
#include "Manipulator/ManipulatorScene.h"


Application::Application()
:m_pRoot(nullptr)
,m_pMainCam(nullptr)
,m_pSceneMgr(nullptr)
,m_pInputManager(nullptr)
,m_pMouse(nullptr)
,m_pKeyboard(nullptr)
,m_bRButtonDown(false)
,m_bQuit(false)
{

}

void Application::Init( int width, int height, HWND hwnd, HWND hParent )
{
	_InitOgre(width, height, hwnd, hParent);
	_InitOIS(hParent);
}

void Application::_InitOgre(int width, int height, HWND hwnd, HWND hParent)
{
	using namespace Ogre;

	String ResourceCfg, PluginCfg;
#ifdef _DEBUG
	ResourceCfg = "resources_d.cfg";
	PluginCfg = "plugins_d.cfg";
#else
	ResourceCfg = "resources.cfg";
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

	m_pSceneMgr = m_pRoot->createSceneManager(ST_GENERIC, SCENE_MANAGER_NAME);
	m_pMainCam = m_pSceneMgr->createCamera("MainCamera");

	NameValuePairList params;
	params["externalWindowHandle"] = StringConverter::toString((unsigned int)hwnd);
	params["parentWindowHandle"] = StringConverter::toString((unsigned int)hParent);
	RenderWindow* pRenderWnd = m_pRoot->createRenderWindow("MainWindow", width, height, false, &params);

	Viewport* vp = pRenderWnd->addViewport(m_pMainCam);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	m_pMainCam->setAspectRatio((Ogre::Real)vp->getActualWidth() / (Ogre::Real)vp->getActualHeight());
	m_pMainCam->setNearClipDistance(5);
	m_pMainCam->setFixedYawAxis(true);

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//初始化所有资源
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//Set initial mouse clipping size
	// 	windowResized(mWindow);
	// 	//Register as a Window listener
	// 	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
	m_pRoot->addFrameListener(this);
	pRenderWnd->setActive(true);
}

void Application::_InitOIS(HWND hwnd)
{
	OIS::ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), Ogre::StringConverter::toString((unsigned int)hwnd)));
	pl.insert(std::make_pair(std::string("w32_mouse"), "DISCL_FOREGROUND"));     
	pl.insert(std::make_pair(std::string("w32_mouse"), "DISCL_NONEXCLUSIVE"));

	m_pInputManager = OIS::InputManager::createInputSystem( pl );
	assert(m_pInputManager);

	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputManager->createInputObject( OIS::OISKeyboard, true ));
	m_pMouse = static_cast<OIS::Mouse*>(m_pInputManager->createInputObject( OIS::OISMouse, true ));
	assert(m_pKeyboard && m_pMouse);

	m_pKeyboard->setEventCallback(this);
	m_pMouse->setEventCallback(this);
}

void Application::Run()
{
	//OGRE渲染主循环
	m_pRoot->startRendering();

	ManipulatorScene::GetSingleton().Shutdown();

	//销毁OGRE
	if(m_pRoot)
		m_pRoot->removeFrameListener(this);

	if( m_pInputManager )
	{
		m_pInputManager->destroyInputObject( m_pMouse );
		m_pInputManager->destroyInputObject( m_pKeyboard );

		OIS::InputManager::destroyInputSystem(m_pInputManager);
		m_pMouse = 0;
		m_pKeyboard = 0;
		m_pInputManager = 0;
	}
	SAFE_DELETE(m_pRoot);
}

void Application::Shutdown()
{
	m_bQuit = true;	
}

bool Application::mouseMoved( const OIS::MouseEvent &arg )
{
	if(m_bRButtonDown)
	{
		m_pMainCam->yaw(Ogre::Radian(-arg.state.X.rel * 0.01f));
		m_pMainCam->pitch(Ogre::Radian(-arg.state.Y.rel * 0.01f));
	}
	
	return true;
}

bool Application::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(OIS::MB_Right == id)
		m_bRButtonDown = true;

	return true;
}

bool Application::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(OIS::MB_Right == id)
		m_bRButtonDown = false;

	return true;
}

bool Application::keyPressed( const OIS::KeyEvent &arg )
{
	switch (arg.key)
	{
	case OIS::KC_W: m_tranVector.z = -1; break;
	case OIS::KC_A: m_tranVector.x = -1; break;
	case OIS::KC_S: m_tranVector.z =  1; break;
	case OIS::KC_D: m_tranVector.x =  1; break;
	default: return false;
	}

	return true;
}

bool Application::keyReleased( const OIS::KeyEvent &arg )
{
	switch (arg.key)
	{
	case OIS::KC_W: m_tranVector.z = 0; break;
	case OIS::KC_A: m_tranVector.x = 0; break;
	case OIS::KC_S: m_tranVector.z = 0;	break;
	case OIS::KC_D: m_tranVector.x = 0; break;
	default: return false;
	}

	return true;
}

bool Application::frameRenderingQueued( const Ogre::FrameEvent& evt )
{
	if(m_bQuit)
		return false;

	m_pMouse->capture();
	m_pKeyboard->capture();

	m_pMainCam->moveRelative(m_tranVector * evt.timeSinceLastFrame * 30);

	return true;
}

void Application::SceneNew()
{
	//先关闭当前场景
	//TODO:检测改动,提示保存
	SceneClose();

	DialogSceneNew dlg;
	std::wstring newSceneName;
	if (IDOK == dlg.DoModal(ManipulatorScene::GetSingleton().GetScenePath(), newSceneName))
	{
		ManipulatorScene::GetSingleton().SceneNew(newSceneName);
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
	dlgFile.GetOFN().lpstrInitialDir = path; 
	dlgFile.GetOFN().lpstrTitle = L"Open Scene"; 

	if(IDOK == dlgFile.DoModal())
	{
		strFilename = dlgFile.GetOFN().lpstrFile;
		ManipulatorScene::GetSingleton().SceneOpen(strFilename);
	}
}

void Application::SceneSave()
{
	ManipulatorScene::GetSingleton().SceneSave();
}

void Application::SceneClose()
{
	ManipulatorScene::GetSingleton().SceneClose();

	//重置摄像机位置
	m_pMainCam->setPosition(0,100,0);
	m_pMainCam->lookAt(0,0,20);
}


