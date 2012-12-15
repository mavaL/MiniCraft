/********************************************************************
	created:	14:10:2012   9:15
	filename: 	C:\Users\mavaL\Desktop\SceneEdit\SceneEdit\SceneEdit\Application.h
	author:		maval
	
	purpose:	编辑器应用程序框架类
*********************************************************************/


#pragma once

#include <Ogre.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

const	int		EDITOR_VP_WIDTH		=	1024;
const	int		EDITOR_VP_HEIGHT	=	768;

using namespace Ogre;


class Application : public Ogre::FrameListener, OIS::MouseListener, OIS::KeyListener
{
public:
	Application();
	~Application() {}

public:
	void		Init(int width, int height, HWND hwnd, HWND hParent);
	void		Run();
	void		Shutdown();

	void		SceneNew();
	void		SceneOpen();
	void		SceneSave();
	void		SceneClose();

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
	Root*				m_pRoot;
	Camera*				m_pMainCam;
	SceneManager*		m_pSceneMgr;
	OIS::InputManager*	m_pInputManager;
	OIS::Mouse*			m_pMouse;
	OIS::Keyboard*		m_pKeyboard;

	bool				m_bRButtonDown;
	Vector3				m_tranVector;
	bool				m_bQuit;

private:
	void		_InitOgre(int width, int height, HWND hwnd, HWND hParent);
	void		_InitOIS(HWND hwnd);
};


