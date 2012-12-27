#include "stdafx.h"
#include "Application.h"
#include "EditorDefine.h"
#include "UI/DialogSceneNew.h"
#include "Manipulator/ManipulatorScene.h"
#include "Manipulator/ManipulatorAction.h"
#include "Action/ActionBase.h"
#include "MainFrm.h"


Application::Application()
:m_pRoot(nullptr)
,m_pMainCam(nullptr)
,m_pSceneMgr(nullptr)
,m_pRenderWnd(nullptr)
,m_bRBDown(false)
{

}

void Application::Init( int width, int height, HWND hwnd, HWND hParent )
{
	_InitOgre(width, height, hwnd, hParent);
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
	m_pMainCam = m_pSceneMgr->createCamera(MAIN_CAMERA_NAME);

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

bool Application::Update()
{
	if(!_UpdateInput(TIME_PER_FRAME))
		return false;

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnFrameMove(TIME_PER_FRAME);

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
	dlgFile.GetOFN().lpstrInitialDir = path; 
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

bool Application::_UpdateInput(float dt)
{
	if(GetAsyncKeyState(VK_ESCAPE) < 0)
		return false;

	POINT curPos;
	GetCursorPos(&curPos);

	static POINT lastPos = curPos;

	Vector3	vecMove(Vector3::ZERO);
	//处理摄像机移动
	if(GetAsyncKeyState('W') < 0)
		vecMove.z = -1;
	else if(GetAsyncKeyState('S') < 0)
		vecMove.z =  1;
	if(GetAsyncKeyState('A') < 0)
		vecMove.x = -1;
	else if(GetAsyncKeyState('D') < 0)
		vecMove.x =  1;

	m_pMainCam->moveRelative(vecMove * dt * 50);

	//处理摄像机方向
	if(m_bRBDown)
	{
		m_pMainCam->yaw(Ogre::Radian((lastPos.x-curPos.x)*dt*0.5f));
		m_pMainCam->pitch(Ogre::Radian((lastPos.y-curPos.y)*dt*0.5f));
	}

	lastPos = curPos;
	return true;
}

void Application::OnLButtonDown( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseLButtonDown(param);
}

void Application::OnLButtonUp( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseLButtonUp(param);
}

void Application::OnMouseMove( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseMove(param);
}

void Application::OnRButtonDown( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseRButtonDown(param);
	m_bRBDown = true;
}

void Application::OnRButtonUp( const POINT& pt )
{
	if(!ManipulatorSystem.GetIsSceneReady())
		return;

	SActionParam param;
	_CreateActionParam(pt, param);

	ManipulatorAction::GetSingleton().GetActiveActoin()->OnMouseRButtonDown(param);
	m_bRBDown = false;
}

void Application::_CreateActionParam( const POINT& viewClientPt, SActionParam& retParam )
{
	retParam.m_ptPixel = Ogre::Vector2((float)viewClientPt.x, (float)viewClientPt.y);

	unsigned int vpWidth = m_pRenderWnd->getWidth();
	unsigned int vpHeight = m_pRenderWnd->getHeight();
	const Ogre::Ray ray = m_pMainCam->getCameraToViewportRay(viewClientPt.x/(float)vpWidth, viewClientPt.y/(float)vpHeight);
	retParam.m_bHitTerrain = ManipulatorSystem.GetTerrain().GetRayIntersectPoint(ray, retParam.m_ptTerrain);
}


