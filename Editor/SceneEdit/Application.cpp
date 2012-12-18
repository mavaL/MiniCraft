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
,m_pRenderWnd(nullptr)
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

	m_pSceneMgr = m_pRoot->createSceneManager(ST_GENERIC, SCENE_MANAGER_NAME);
	m_pMainCam = m_pSceneMgr->createCamera("MainCamera");

	NameValuePairList params;
	params["externalWindowHandle"] = StringConverter::toString((unsigned int)hwnd);
	params["parentWindowHandle"] = StringConverter::toString((unsigned int)hParent);
	m_pRenderWnd = m_pRoot->createRenderWindow("MainWindow", width, height, false, &params);

	Viewport* vp = m_pRenderWnd->addViewport(m_pMainCam);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
	m_pMainCam->setAspectRatio((Ogre::Real)vp->getActualWidth() / (Ogre::Real)vp->getActualHeight());
	m_pMainCam->setNearClipDistance(5);
	m_pMainCam->setFixedYawAxis(true);

	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//初始化所有资源
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	m_pRenderWnd->setActive(true);
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

bool Application::Update()
{
	if(m_bQuit)
		return false;

	m_pMouse->capture();
	m_pKeyboard->capture();

	m_pMainCam->moveRelative(m_tranVector * TIME_PER_FRAME * 30);

	m_pRoot->renderOneFrame();

	return true;
}

void Application::Shutdown()
{
	ManipulatorScene::GetSingleton().Shutdown();

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
	case OIS::KC_ESCAPE: m_bQuit = true; break;
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

void Application::OnViewportResized()
{
	if (m_pMainCam)
	{	
		//设备丢失
		m_pRenderWnd->windowMovedOrResized();	
		m_pMainCam->setAspectRatio(m_pRenderWnd->getWidth()/(float)m_pRenderWnd->getHeight()); 
		//reset 设备
		m_pRenderWnd->update();	
	}
}

void Application::RenderAllMeshIcons(CImageList& retImageList, Ogre::StringVectorPtr& retMeshNames)
{
	using namespace Ogre;

	retImageList.Create(MESH_ICON_SIZE, MESH_ICON_SIZE, ILC_COLOR32, 100, 500);

	retMeshNames = ResourceGroupManager::getSingleton().findResourceNames(
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "*.mesh");

	//没有mesh资源
	if(retMeshNames->empty())
		return;

	//准备工作...
	const PixelFormat imageFormat = PF_A8R8G8B8;
	TexturePtr texture = TextureManager::getSingleton().createManual( "MeshIconRtt", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		TEX_TYPE_2D, 256, 256, 0, imageFormat , TU_RENDERTARGET );

	RenderTexture *rttTex = texture->getBuffer()->getRenderTarget();
	SceneManager *SceneMgr = Root::getSingletonPtr()->createSceneManager("OctreeSceneManager", "MeshIconSceneMgr");

	Light *dirl = SceneMgr->createLight("DisplayLight");
	dirl->setDirection(-1,-1,-1);
	dirl->setDiffuseColour(1,1,1);
	dirl->setType(Light::LT_DIRECTIONAL);

	Camera* RTTCam = SceneMgr->createCamera("MeshIconCamera");
	RTTCam->setNearClipDistance(0.01f);
	RTTCam->setFarClipDistance(0);
	RTTCam->setAspectRatio(1);
	RTTCam->setFOVy(Degree(90));
	RTTCam->setPosition(0,0,1);
	RTTCam->lookAt(0,0,0);

	Viewport *v = rttTex->addViewport( RTTCam );
	v->setClearEveryFrame( true );
	v->setBackgroundColour(ColourValue(0,0,0,0));

	Entity *entity;
	unsigned char dataptr[300 * 300 * 6]; 
	unsigned char *dataptr2;
	PixelBox pb(256,256,1,imageFormat, dataptr);
	//开始渲染
	for(size_t i=0; i<retMeshNames->size(); ++i)
	{
		entity = SceneMgr->createEntity("TempEntity", retMeshNames->at(i));
		SceneMgr->getRootSceneNode()->attachObject(entity);

		Vector3 vSize = entity->getBoundingBox().getHalfSize();
		Vector3 vCenter = entity->getBoundingBox().getCenter(); 
		vSize += Vector3(vSize.z, vSize.z, vSize.z);
		float maxsize = max(max(vSize.x,vSize.y),vSize.z);
		vSize = Vector3(0, 0, maxsize * 1.1f) + vCenter;

		RTTCam->setPosition(vSize.x,vSize.y,vSize.z);
		RTTCam->lookAt(vCenter.x,vCenter.y,vCenter.z);

		try
		{
			rttTex->update();
			rttTex->copyContentsToMemory(pb, RenderTarget::FB_FRONT);

			dataptr2 = new unsigned char[MESH_ICON_SIZE*MESH_ICON_SIZE*PixelUtil::getNumElemBytes(imageFormat)];
			PixelBox pb2(MESH_ICON_SIZE,MESH_ICON_SIZE,1,imageFormat, dataptr2);
			Image::scale(pb,pb2);

 			//图像数据加入列表
			CBitmap bm;
			assert(bm.CreateBitmap(MESH_ICON_SIZE, MESH_ICON_SIZE, 1, PixelUtil::getNumElemBits(imageFormat), dataptr2));
			retImageList.Add(&bm, RGB(0,0,0));
		}
		catch(...)
		{
			assert(0);
		}

		entity->detachFromParent();
		SceneMgr->destroyEntity(entity);
	}

	//清理工作...
	rttTex->removeAllViewports();
	Root::getSingletonPtr()->destroySceneManager(SceneMgr);
	TextureManager::getSingletonPtr()->unload(texture->getName());
	TextureManager::getSingletonPtr()->remove(texture->getName());
}


