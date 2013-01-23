#ifndef OGRE_MANAGER_H
#define OGRE_MANAGER_H

#include "Singleton.h"
#include "GameDefine.h"

/*------------------------------------------------
				 OGRE图形模块封装类
-------------------------------------------------*/


class COgreManager : public Ogre::WindowEventListener, public CSingleton<COgreManager>
{
	DECLEAR_SINGLETON(COgreManager);
	COgreManager(void);
	~COgreManager(void);

public:
	bool Init();
	//更新图形.返回true表示游戏继续进行,返回false表示退出.
	bool Update(float dt);
	void Shutdown();
	bool IsInit() { return m_bHasInit; }

public:
	void			MessagePump()		{ Ogre::WindowEventUtilities::messagePump(); }
	bool			IsMainWndClosed()	{ return mWindow->isClosed(); }
	bool			IsMainWndActive()	{ return mWindow->isActive(); }
	Ogre::Timer*	GetTimer()			{ return m_Timer; }
	void			GetMainWndHandle(unsigned long& hwnd) { mWindow->getCustomAttribute("WINDOW", &hwnd);}
	Ogre::RenderWindow*	GetRenderWindow()	{ return mWindow; }
	Ogre::Viewport*	GetViewport()		{ return m_pViewport; }

private:
	Ogre::Root*			mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::Viewport*		m_pViewport;
	Ogre::Timer*		m_Timer;
	bool				m_bHasInit;

	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
};

#endif
