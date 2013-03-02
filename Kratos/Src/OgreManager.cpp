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
	m_dlaa = CompositorManager::getSingleton().addCompositor(m_pViewport, "DLAA");
	assert(m_dlaa);
	assert(m_ssao);
	m_dlaa->setEnabled(false);
	m_ssao->setEnabled(false);

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

void COgreManager::EnableDeferredShading(bool bEnable)
{
	if (bEnable)
	{
		m_pDS->setActive(true);
		Ogre::TerrainGlobalOptions::getSingleton().setDefaultMaterialGenerator(
			Ogre::TerrainMaterialGeneratorPtr(new Ogre::TerrainMaterialGeneratorD));
	}
	else
	{
		m_pDS->setActive(false);
		Ogre::TerrainGlobalOptions::getSingleton().setDefaultMaterialGenerator(
			Ogre::TerrainMaterialGeneratorPtr(new Ogre::TerrainMaterialGeneratorA));
	}

	EnableDLAA(bEnable);
	EnableSSAO(bEnable);
}

void COgreManager::InitShadowConfig()
{
	//TODO: 看来PSSM之类的高级阴影技术十分需要teawk各参数
	//		默认参数并不适用每种情况,需要场景根据自己的光源
	//		裁减距离,视锥切分策略等来尝试适用参数.
	//		所以应该在编辑器中提供阴影参数调节功能!
	m_pSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
	m_pSceneMgr->setShadowFarDistance(300);

	// 3 textures per directional light (PSSM)
	m_pSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
	//m_pSceneMgr->setShadowDirectionalLightExtrusionDistance(1000);

	// shadow camera setup
	PSSMShadowCameraSetup* pssmSetup = new PSSMShadowCameraSetup();
	pssmSetup->setSplitPadding(m_pMainCamera->getNearClipDistance());
	pssmSetup->calculateSplitPoints(3, m_pMainCamera->getNearClipDistance(), m_pSceneMgr->getShadowFarDistance(), 0.75f);
	pssmSetup->setOptimalAdjustFactor(0, 0.5f);
	pssmSetup->setOptimalAdjustFactor(1, 0.8f);
	pssmSetup->setOptimalAdjustFactor(2, 2);
	//pssmSetup->setUseSimpleOptimalAdjust(false);
	pssmSetup->setCameraLightDirectionThreshold(Degree(45));

	mPSSMSetup.bind(pssmSetup);

	m_pSceneMgr->setShadowCameraSetup(mPSSMSetup);
	m_pSceneMgr->setShadowTextureCount(3);
	m_pSceneMgr->setShadowTextureConfig(0, 2048, 2048, PF_FLOAT32_R);
	m_pSceneMgr->setShadowTextureConfig(1, 1024, 1024, PF_FLOAT32_R);
	m_pSceneMgr->setShadowTextureConfig(2, 1024, 1024, PF_FLOAT32_R);
	m_pSceneMgr->setShadowTextureSelfShadow(false);
	m_pSceneMgr->setShadowCasterRenderBackFaces(true);
	m_pSceneMgr->setShadowTextureCasterMaterial("Ogre/shadow/depth/caster");

#ifdef FORWARD_RENDERING
	TerrainMaterialGeneratorA::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile*>(
		TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setReceiveDynamicShadowsEnabled(true);
	matProfile->setReceiveDynamicShadowsDepth(true);
	matProfile->setReceiveDynamicShadowsPSSM(pssmSetup);
#else	//deferred shading
	TerrainGlobalOptions::getSingleton().setRenderQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_2);
	m_pSceneMgr->getRenderQueue()->getQueueGroup(RENDER_QUEUE_WORLD_GEOMETRY_2)->setShadowsEnabled(false);
#endif
}

void COgreManager::EnableDLAA( bool bEnable )
{
	m_dlaa->setEnabled(bEnable);
}

bool COgreManager::IsDLAAEnabled() const
{
	return m_dlaa->getEnabled();
}

void COgreManager::EnableSSAO( bool bEnable )
{
	m_ssao->setEnabled(bEnable);
}

bool COgreManager::IsSSAOEnabled() const
{
	return m_ssao->getEnabled();
}
