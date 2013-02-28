#ifndef OGRE_MANAGER_H
#define OGRE_MANAGER_H

#include "Singleton.h"
#include <OgrePrerequisites.h>
#include <OgreWindowEventUtilities.h>
#include <OgreShadowCameraSetup.h>

/*------------------------------------------------
				 OGRE图形模块封装类
-------------------------------------------------*/
class DeferredShadingSystem;

class COgreManager : public Ogre::WindowEventListener, public CSingleton<COgreManager>
{
	DECLEAR_SINGLETON(COgreManager);
	COgreManager(void);
	~COgreManager(void);

public:
	//编辑器与客户端初始化不同.后4个参数用于编辑器初始化
	bool Init(bool bEditor, HWND externalHwnd=nullptr, HWND hwndParent=nullptr, int width=0, int height=0);
	//更新图形.返回true表示游戏继续进行,返回false表示退出.
	bool Update(float dt);
	void Shutdown();
	bool IsInit() { return m_bHasInit; }

	void			MessagePump()		{ Ogre::WindowEventUtilities::messagePump(); }
	bool			IsMainWndClosed();
	bool			IsMainWndActive();
	Ogre::Timer*	GetTimer()			{ return m_Timer; }
	void			GetMainWndHandle(unsigned long& hwnd);

	//开启/关闭延迟渲染
	void			EnableDeferredShading(bool bEnable);
	void			InitShadowConfig();

	Ogre::Root*			mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::Viewport*		m_pViewport;
	Ogre::Timer*		m_Timer;
	Ogre::SceneManager*	m_pSceneMgr;
	Ogre::Camera*		m_pMainCamera;
	DeferredShadingSystem*		m_pDS;
	Ogre::ShadowCameraSetupPtr mPSSMSetup;

private:
	bool				m_bHasInit;

	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
};

#define RenderManager COgreManager::GetSingleton()

#endif
