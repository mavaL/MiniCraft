#include "OgreManager.h"
#include <Ogre.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include "DeferredShading/DeferredShading.h"
#include "DeferredShading/TerrainMaterialGeneratorD.h"

using namespace Ogre;


COgreManager::COgreManager(void)
: mRoot(0)
,mWindow(0)
,m_Timer(NULL)
,m_bHasInit(false)
,m_pViewport(NULL)
,m_pSceneMgr(NULL)
,m_pMainCamera(NULL)
,m_pDS(nullptr)
,m_dlaa(nullptr)
,m_ssao(nullptr)
{
}

COgreManager::~COgreManager(void)
{
}

bool COgreManager::Init(bool bEditor, HWND externalHwnd, HWND hwndParent,int width, int height)
{
	//资源配置文件和插件配置文件
	String ResourceCfg, PluginCfg;
#ifdef _DEBUG
	if (bEditor)
		ResourceCfg = "resources_editor_d.cfg";
	else
		ResourceCfg = "resources_d.cfg";
	PluginCfg = "plugins_d.cfg";
#else
	if (bEditor)
		ResourceCfg = "resources_editor.cfg";
	else
		ResourceCfg = "resources.cfg";
	PluginCfg = "plugins.cfg";
#endif

	mRoot = new Ogre::Root(PluginCfg);

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
	
	if (bEditor)
	{
		RenderSystem* rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
		assert(rs);
		mRoot->setRenderSystem(rs);
		mRoot->initialise(false);

		NameValuePairList params;
		params["externalWindowHandle"] = StringConverter::toString((unsigned int)externalHwnd);
		params["parentWindowHandle"] = StringConverter::toString((unsigned int)hwndParent);
		mWindow = mRoot->createRenderWindow("MainWindow", width, height, false, &params);
	}
	else
	{
		if(mRoot->restoreConfig() || mRoot->showConfigDialog())
			mWindow = mRoot->initialise(true, "Game : MiniCraft");
		else
			return false;
	}

	m_pSceneMgr = mRoot->createSceneManager(ST_GENERIC, "DefaultSceneMgr");
	m_pMainCamera = m_pSceneMgr->createCamera("MainCamera");
	m_pMainCamera->setNearClipDistance(1);
	m_pMainCamera->setFarClipDistance(500);
	m_pViewport = mWindow->addViewport(m_pMainCamera);
	m_pViewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
	m_pMainCamera->setAspectRatio(
		(Ogre::Real)m_pViewport->getActualWidth()/(Ogre::Real)m_pViewport->getActualHeight());

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//Set initial mouse clipping size
	windowResized(mWindow);
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	m_pDS = new DeferredShadingSystem(m_pViewport, m_pSceneMgr, m_pMainCamera);
	m_pDS->initialize();
	m_pDS->setActive(false);

	m_ssao = CompositorManager::getSingleton().addCompositor(m_pViewport, "DeferredShading/SSAO");
	//m_dlaa = CompositorManager::getSingleton().addCompositor(m_pViewport, "DLAA");
	//assert(m_dlaa);
	assert(m_ssao);
	//m_dlaa->setEnabled(false);
	m_ssao->setEnabled(false);

	PSSMShadowCameraSetup* pssmSetup = new PSSMShadowCameraSetup;
	mPSSMSetup.bind(pssmSetup);

	m_Timer = new Ogre::Timer();
	m_Timer->reset();
	mWindow->setActive(true);
	m_bHasInit = true;

	return true;
}

//Adjust mouse clipping area
void COgreManager::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

// 	const OIS::MouseState &ms = mMouse->getMouseState();
// 	ms.width = width;
// 	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void COgreManager::windowClosed(Ogre::RenderWindow* rw)
{
// 	//Only close for window that created OIS (the main window in these demos)
// 	if( rw == mWindow )
// 		CInputManager::GetSingleton().Shutdown();
}

void COgreManager::Shutdown()
{
	if (m_pDS)
	{
		delete m_pDS;
		m_pDS = nullptr;
	}

	mPSSMSetup.setNull();

	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	//销毁输入系统
	windowClosed(mWindow);
	//销毁Root
	delete mRoot;
}

bool COgreManager::Update(float dt)
{
	if(mWindow->isClosed())
		return false;

	mRoot->renderOneFrame(dt);

	return true;
}

bool COgreManager::IsMainWndClosed()
{
	return mWindow->isClosed();
}

bool COgreManager::IsMainWndActive()
{
	return mWindow->isActive();
}

void COgreManager::GetMainWndHandle( unsigned long& hwnd )
{
	mWindow->getCustomAttribute("WINDOW", &hwnd);
}

// void COgreManager::EnableDLAA( bool bEnable )
// {
// 	m_dlaa->setEnabled(bEnable);
// }

void COgreManager::EnableSSAO( bool bEnable )
{
	m_ssao->setEnabled(bEnable);
	m_effectCfg.bSSAO = bEnable;
}

void COgreManager::EnableShadow( bool bEnable )
{
	if(bEnable)
		m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
	else
		m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

	m_effectCfg.bShadow = bEnable;
}

void COgreManager::ResetEffect()
{
	m_effectCfg.Reset();
	EnableShadow(m_effectCfg.bShadow);
	EnableSSAO(m_effectCfg.bSSAO);
}

void COgreManager::SetSSAOParam( const Ogre::String& name, float val, bool bRemoveAndAdd )
{
	assert(m_ssao);

	// remove compositor first
	if(bRemoveAndAdd)
		CompositorManager::getSingleton().removeCompositor(m_pViewport, "DeferredShading/SSAO");

	Material* mat = static_cast<Material*>(MaterialManager::getSingleton().getByName("SSAO/Crytek").get());
	assert(mat);
	mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant(name, val);

	// adding again
	if(bRemoveAndAdd)
	{
		m_ssao = CompositorManager::getSingleton().addCompositor(m_pViewport, "DeferredShading/SSAO");
		m_ssao->setEnabled(m_effectCfg.bSSAO);
	}
}
