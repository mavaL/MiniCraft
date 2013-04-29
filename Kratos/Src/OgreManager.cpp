#include "stdafx.h"
#include "OgreManager.h"
#include "DeferredShading/DeferredShading.h"
#include "DeferredShading/TerrainMaterialGeneratorD.h"
#include "DeferredShading/DLight.h"
#include "DeferredShading/LightMaterialGenerator.h"
#include "Effect/DeferredLightEffect.h"

using namespace Ogre;

namespace Kratos
{

	COgreManager::COgreManager(void)
	: mRoot(0)
	,mWindow(0)
	,m_Timer(NULL)
	,m_bHasInit(false)
	,m_pViewport(NULL)
	,m_pSceneMgr(NULL)
	,m_pMainCamera(NULL)
	,m_pDS(nullptr)
	,m_fxaa(nullptr)
	,m_ssao(nullptr)
	,m_sharpen(nullptr)
	,mLightMaterialGenerator(nullptr)
	,m_pSunLight(nullptr)
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
			params["vsync"] = "true";
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
		mLightMaterialGenerator = new LightMaterialGenerator;

		ResetEffect();

		PSSMShadowCameraSetup* pssmSetup = new PSSMShadowCameraSetup;
		mPSSMSetup.bind(pssmSetup);
		m_pSceneMgr->setShadowCameraSetup(mPSSMSetup);

		m_Timer = new Ogre::Timer();
		m_Timer->reset();
		mWindow->setActive(true);
		mWindow->setDeactivateOnFocusChange(false);
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
		SAFE_DELETE(mLightMaterialGenerator);
		SAFE_DELETE(m_pDS);

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

	void COgreManager::EnableFXAA( bool bEnable )
	{
		m_fxaa->setEnabled(bEnable);
		m_effectCfg.bFXAA = bEnable;
	}

	void COgreManager::EnableSSAO( bool bEnable )
	{
		m_ssao->setEnabled(bEnable);
		m_effectCfg.bSSAO = bEnable;
	}

	void COgreManager::EnableSharpen( bool bEnable )
	{
		m_sharpen->setEnabled(bEnable);
		m_effectCfg.bSharpen = bEnable;
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
		CompositorManager& mgr = CompositorManager::getSingleton();
		mgr.removeCompositor(m_pViewport, "DeferredShading/SSAO");
		mgr.removeCompositor(m_pViewport, "Sharpen");
		mgr.removeCompositor(m_pViewport, "FXAA");

		m_ssao = CompositorManager::getSingleton().addCompositor(m_pViewport, "DeferredShading/SSAO");
		m_sharpen = CompositorManager::getSingleton().addCompositor(m_pViewport, "Sharpen");
		m_fxaa = CompositorManager::getSingleton().addCompositor(m_pViewport, "FXAA");
		assert(m_fxaa);
		assert(m_ssao);
		assert(m_sharpen);

		EnableShadow(m_effectCfg.bShadow);
		EnableSSAO(m_effectCfg.bSSAO);
		EnableSharpen(m_effectCfg.bSharpen);
		EnableFXAA(m_effectCfg.bFXAA);
	}

	Ogre::TexturePtr COgreManager::CreateRT(const Ogre::String& name, int w, int h, Ogre::PixelFormat format)
	{
		return TextureManager::getSingleton().createManual( name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
			TEX_TYPE_2D, w, h, 0, format , TU_RENDERTARGET );
	}

	void COgreManager::updateSceneManagersAfterMaterialsChange()
	{
		if(Root::getSingletonPtr() && (Ogre::Pass::getDirtyHashList().size()!=0 || Ogre::Pass::getPassGraveyard().size()!=0))
		{
			Ogre::SceneManagerEnumerator::SceneManagerIterator scenesIter = Root::getSingletonPtr()->getSceneManagerIterator();

			while(scenesIter.hasMoreElements())
			{
				Ogre::SceneManager* pScene = scenesIter.getNext();
				if(pScene)
				{
					Ogre::RenderQueue* pQueue = pScene->getRenderQueue();
					if(pQueue)
					{
						Ogre::RenderQueue::QueueGroupIterator groupIter = pQueue->_getQueueGroupIterator();
						while(groupIter.hasMoreElements())
						{
							Ogre::RenderQueueGroup* pGroup = groupIter.getNext();
							if(pGroup)
								pGroup->clear(false);
						}//end_while(groupIter.hasMoreElements())
					}//end_if(pScene)
				}//end_if(pScene)
			}//end_while(scenesIter.hasMoreElements())      

			// Now trigger the pending pass updates
			Ogre::Pass::processPendingPassUpdates();

		}//end_if(m_Root..
	}

	Ogre::Entity* COgreManager::CreateEntityWithTangent( const Ogre::String& meshname, Ogre::SceneManager* sm )
	{
		MeshPtr mesh = MeshManager::getSingleton().load(meshname, ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
		unsigned short src, dest;
		if (!mesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
			mesh->buildTangentVectors(VES_TANGENT, src, dest);

		return sm->createEntity(mesh);
	}

	DLight* COgreManager::CreateDLight( int dlType )
	{
		Ogre::Light* lightSrc = RenderManager.m_pSceneMgr->createLight();
		Light::LightTypes type = (eDLightType)dlType == eDLightType_Point ? Light::LT_POINT : Light::LT_SPOTLIGHT;
		lightSrc->setType(type);
		lightSrc->setDiffuseColour(COLOR::White);
		lightSrc->setCastShadows(false);

		DLight* dl = new DLight(mLightMaterialGenerator, lightSrc);
		m_dLightList.insert(std::make_pair(lightSrc, dl));

		return dl;
	}

	void COgreManager::DestroyDLight( DLight* light )
	{
		auto iter = m_dLightList.find(light->getParentLight());
		assert(iter != m_dLightList.end());

		m_pSceneMgr->destroyLight(iter->first);
		delete iter->second;
		m_dLightList.erase(iter);
	}

	void COgreManager::CreateSunLight( const Ogre::Vector3& direction, const Ogre::ColourValue& diffuse )
	{
		Ogre::Light* pSunLight = RenderManager.m_pSceneMgr->createLight("SunLight");
		pSunLight->setType(Ogre::Light::LT_DIRECTIONAL);
		pSunLight->setDirection(direction);
		pSunLight->setDiffuseColour(diffuse);

		m_pSunLight = new DLight(mLightMaterialGenerator, pSunLight);
		m_dLightList.insert(std::make_pair(pSunLight, m_pSunLight));
	}

	void COgreManager::DestroySunLight()
	{
		if(m_pSunLight)
		{
			m_dLightList.erase(m_pSunLight->getParentLight());
			SAFE_DELETE(m_pSunLight);
		}
	}

	void COgreManager::SetRenderingStyle()
	{
#ifdef FORWARD_RENDERING
		RenderManager.m_pDS->setActive(false);
		Ogre::TerrainGlobalOptions::getSingleton().setDefaultMaterialGenerator(
			Ogre::TerrainMaterialGeneratorPtr(new Ogre::TerrainMaterialGeneratorA));
		TerrainMaterialGeneratorA::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile*>(
			TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
		matProfile->setCompositeMapEnabled(false);

#else	//deferred shading
		RenderManager.m_pDS->setActive(true);
		Ogre::TerrainGlobalOptions::getSingleton().setDefaultMaterialGenerator(
			Ogre::TerrainMaterialGeneratorPtr(new Kratos::TerrainMaterialGeneratorD));
		TerrainMaterialGeneratorD::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorD::SM2Profile*>(
			TerrainGlobalOptions::getSingleton().getDefaultMaterialGenerator()->getActiveProfile());
		matProfile->setCompositeMapEnabled(false);
#endif
	}

	void COgreManager::ApplyShadowParams()
	{
		EnableShadow(m_effectCfg.bShadow);
		m_pSceneMgr->setShadowFarDistance(Ogre::StringConverter::parseReal(m_shadowParams["FarDistance"]));
		m_pSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
		m_pSceneMgr->setShadowDirectionalLightExtrusionDistance(Ogre::StringConverter::parseReal(m_shadowParams["LightExtrusionDistance"]));
		m_pSceneMgr->setShadowTextureCount(3);
		FLOAT3 texSize = Ogre::StringConverter::parseVector3(m_shadowParams["ShadowMapSize"]);
		m_pSceneMgr->setShadowTextureConfig(0, texSize.x, texSize.x, PF_FLOAT32_R);
		m_pSceneMgr->setShadowTextureConfig(1, texSize.y, texSize.y, PF_FLOAT32_R);
		m_pSceneMgr->setShadowTextureConfig(2, texSize.z, texSize.z, PF_FLOAT32_R);
		m_pSceneMgr->setShadowTextureSelfShadow(Ogre::StringConverter::parseBool(m_shadowParams["SelfShadow"]));
		m_pSceneMgr->setShadowCasterRenderBackFaces(Ogre::StringConverter::parseBool(m_shadowParams["RenderBackFace"]));
		m_pSceneMgr->setShadowTextureCasterMaterial("Ogre/shadow/depth/caster");

		PSSMShadowCameraSetup* cam = GetShadowCameraSetup();

		cam->setSplitPadding(Ogre::StringConverter::parseBool(m_shadowParams["SplitPadding"]));
		cam->calculateSplitPoints(3, m_pMainCamera->getNearClipDistance(), m_pSceneMgr->getShadowFarDistance(), 
			Ogre::StringConverter::parseReal(m_shadowParams["PssmLambda"]));
		FLOAT3 adjust = Ogre::StringConverter::parseVector3(m_shadowParams["AdjustFactor"]);
		cam->setOptimalAdjustFactor(0, adjust.x);
		cam->setOptimalAdjustFactor(1, adjust.y);
		cam->setOptimalAdjustFactor(2, adjust.z);
		cam->setUseSimpleOptimalAdjust(Ogre::StringConverter::parseBool(m_shadowParams["UseSimpleAdjust"]));
		cam->setCameraLightDirectionThreshold(Degree(Ogre::StringConverter::parseReal(m_shadowParams["CameraLightDirectionThreshold"])));
	}

	void COgreManager::ApplySsaoParams()
	{
		Material* mat = static_cast<Material*>(MaterialManager::getSingleton().getByName("SSAO/Crytek").get());
		assert(mat);
		auto gpuParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

		gpuParams->setNamedConstant("cSampleLengthScreenSpace", Ogre::StringConverter::parseReal(m_ssaoParams["SampleLength"])/100);
		gpuParams->setNamedConstant("cOffsetScale", Ogre::StringConverter::parseReal(m_ssaoParams["OffsetScale"])/100);
		gpuParams->setNamedConstant("cDefaultAccessibility", Ogre::StringConverter::parseReal(m_ssaoParams["DefaultAccessibility"]));
		gpuParams->setNamedConstant("cEdgeHighlight", Ogre::StringConverter::parseReal(m_ssaoParams["EdgeHighlight"]));

		ResetEffect();
		RenderManager.EnableSSAO(m_effectCfg.bSSAO);
	}

}


