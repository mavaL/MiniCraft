#ifndef OGRE_MANAGER_H
#define OGRE_MANAGER_H

#include "Singleton.h"
#include "KratosPrerequisites.h"

namespace OgreBites
{
	class SdkTrayManager;
}

namespace Kratos
{
	//后处理效果配置
	struct SEffectConfig 
	{
		SEffectConfig() { Reset(); }
		void	Reset() 
		{ 
			bShadow = false; 
			bSSAO = false;
			bFXAA = false;
			bSharpen = false;
		}

		bool	bShadow;
		bool	bSSAO;
		bool	bFXAA;
		bool	bSharpen;
	};

	//阴影参数
	typedef Ogre::NameValuePairList ShadowParams;
	//SSAO参数
	typedef Ogre::NameValuePairList SsaoParams;

	typedef std::unordered_map<Ogre::Light*, DLight*>	DeferredLightList;

	/*------------------------------------------------
					OGRE图形模块封装类
	-------------------------------------------------*/

	class COgreManager 
		: public Ogre::WindowEventListener
		, public CSingleton<COgreManager>
	{
		DECLEAR_SINGLETON(COgreManager);
		COgreManager(void);
		~COgreManager(void);

	public:
		//编辑器与客户端初始化不同.后4个参数用于编辑器初始化
		bool			Init(bool bEditor, HWND externalHwnd=nullptr, HWND hwndParent=nullptr, int width=0, int height=0);
		//更新图形.返回true表示游戏继续进行,返回false表示退出.
		bool			Update(float dt);
		void			Shutdown();
		bool			IsInit() { return m_bHasInit; }
		void			ResetEffect();

		//Ogre damn bug.见 http://www.ogre3d.org/forums/viewtopic.php?p=189032#189032
		void			updateSceneManagersAfterMaterialsChange();

		void			MessagePump()		{ Ogre::WindowEventUtilities::messagePump(); }
		bool			IsMainWndClosed();
		bool			IsMainWndActive();
		Ogre::Timer*	GetTimer()			{ return m_Timer; }
		void			GetMainWndHandle(unsigned long& hwnd);
		Ogre::PSSMShadowCameraSetup*	GetShadowCameraSetup() { return (Ogre::PSSMShadowCameraSetup*)mPSSMSetup.get(); }
		void			SetRenderingStyle();
		void			ToggleShowFrameStats();
		//创建RT
		Ogre::TexturePtr	CreateRT(const STRING& name, int w, int h, Ogre::PixelFormat format);
		//创建Entity,带tangent vector
		Ogre::Entity*		CreateEntityWithTangent(const STRING& meshname, Ogre::SceneManager* sm);
		void				BuildTangentOfMesh(const STRING& meshname);

		//创建太阳光主光
		void				CreateSunLight(const FLOAT3& direction, const COLOR& diffuse);
		void				DestroySunLight();
		//0=点光,1=聚光
		DLight*				CreateDLight(int type);
		void				DestroyDLight(DLight* light);
		DeferredLightList&	GetDLightList() { return m_dLightList; }	

		SEffectConfig&	GetEffectConfig() { return m_effectCfg; }
		void			EnableShadow(bool bEnable);
		void			EnableFXAA(bool bEnable);
		void			EnableSSAO(bool bEnable);
		void			EnableSharpen(bool bEnable);
		ShadowParams&	GetShadowParams()	{ return m_shadowParams; }
		SsaoParams&		GetSsaoParams()		{ return m_ssaoParams; }
		void			ApplyShadowParams();
		void			ApplySsaoParams();

		Ogre::Root*			mRoot;
		Ogre::RenderWindow* mWindow;
		Ogre::Viewport*		m_pViewport;
		Ogre::Timer*		m_Timer;
		Ogre::SceneManager*	m_pSceneMgr;
		Ogre::Camera*		m_pMainCamera;
		DeferredShadingSystem*	m_pDS;
		Ogre::ShadowCameraSetupPtr mPSSMSetup;
		bool				m_bSupportVTF;	//显卡是否支持vertex texture fetch

	private:
		void windowResized(Ogre::RenderWindow* rw);
		void windowClosed(Ogre::RenderWindow* rw);
		void updateFrameStats();
		bool checkHardwareSupport();

	private:
		bool				m_bHasInit;
		SEffectConfig		m_effectCfg;
		ShadowParams		m_shadowParams;
		SsaoParams			m_ssaoParams;

		Ogre::CompositorInstance*	m_fxaa;
		Ogre::CompositorInstance*	m_ssao;
		Ogre::CompositorInstance*	m_sharpen;

		DLight*				m_pSunLight;
		DeferredLightList	m_dLightList;
		MaterialGenerator*	mLightMaterialGenerator;		//The material generator for the light geometry

		OgreBites::SdkTrayManager*	m_trayMgr;
	};

}


#define RenderManager Kratos::COgreManager::GetSingleton()

#endif
