/********************************************************************
	created:	14:10:2012   9:15
	filename: 	C:\Users\mavaL\Desktop\SceneEdit\SceneEdit\SceneEdit\Application.h
	author:		maval
	
	purpose:	编辑器应用程序框架类
*********************************************************************/

#pragma once

#include <OgreFrameListener.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

namespace Ogre
{
	class Root;
	class Camera;
	class SceneManager;
	class FrameListener;
	struct FrameEvent;
}

class Application : public Ogre::FrameListener, OIS::MouseListener, OIS::KeyListener
{
public:
	Application();
	~Application() {}

public:
	void		Init(int width, int height, HWND hwnd, HWND hParent);
	void		Run();
	void		Shutdown();

private:
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg ); 

private:
	Ogre::Root*			m_pRoot;
	Ogre::Camera*		m_pMainCam;
	Ogre::SceneManager*	m_pSceneMgr;

	OIS::InputManager*	m_pInputManager;
	OIS::Mouse*			m_pMouse;
	OIS::Keyboard*		m_pKeyboard;

	bool				m_bRButtonDown;
	Ogre::Vector3		m_tranVector;

	bool				m_bQuit;

private:
	void		_InitOgre(int width, int height, HWND hwnd, HWND hParent);
	void		_InitOIS(HWND hwnd);
	void		_LoadScene();
};


