#include "stdafx.h"
#include "InputManager.h"
#include "OgreManager.h"


CInputManager::CInputManager(void)
: mInputManager(0),
mMouse(0),
mKeyboard(0),
m_bIsInit(false)
{
}

CInputManager::~CInputManager(void)
{
	Shutdown();
}

bool CInputManager::Init()
{
	//图形系统需初始化
	assert(COgreManager::GetSingleton().IsInit() && "错误!!图形系统还未初始化!");

	OIS::ParamList pl;
	unsigned long hwnd = 0;
	COgreManager::GetSingleton().GetMainWndHandle(hwnd);
	pl.insert(std::make_pair(std::string("WINDOW"), Ogre::StringConverter::toString(hwnd)));

	mInputManager = OIS::InputManager::createInputSystem( pl );
	if(mInputManager == NULL)
		return false;

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
	if(!mKeyboard || !mMouse)
	{
		Shutdown();
		return false;
	}

	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);
	m_bIsInit = true;

	return true;
}

void CInputManager::Capture()
{
	//每帧捕获输入设备,才能接受到事件
	mKeyboard->capture();
	mMouse->capture();
}

void CInputManager::Shutdown()
{
	if( mInputManager )
	{
		mInputManager->destroyInputObject( mMouse );
		mInputManager->destroyInputObject( mKeyboard );

		OIS::InputManager::destroyInputSystem(mInputManager);
		mMouse = 0;
		mKeyboard = 0;
		mInputManager = 0;
	}
}

bool CInputManager::mouseMoved( const OIS::MouseEvent &arg )
{
	return m_MouseMoveSignal(arg);	
}

bool CInputManager::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return m_MousePressedSignal(arg, id);
}

bool CInputManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return m_MouseReleasedSignal(arg, id);
}

bool CInputManager::keyPressed( const OIS::KeyEvent &arg )
{
	return m_KeyPressedSignal(arg);
}

bool CInputManager::keyReleased( const OIS::KeyEvent &arg )
{
	return m_KeyReleasedSignal(arg);
}

void CInputManager::BindMouseMove( const MouseMoveHandler& handler, int priority/*=0*/ )
{
	if(priority != 0)
		m_MouseMoveSignal.connect(priority, handler);
	else
		m_MouseMoveSignal.connect(handler);
}

void CInputManager::BindMousePressed( const MouseBtnHandler& handler, int priority/*=0*/ )
{
	if(priority != 0)
		m_MousePressedSignal.connect(priority, handler);
	else
		m_MousePressedSignal.connect(handler);
}

void CInputManager::BindMouseRelease( const MouseBtnHandler& handler, int priority/*=0*/ )
{
	if(priority != 0)
		m_MouseReleasedSignal.connect(priority, handler);
	else
		m_MouseReleasedSignal.connect(handler);
}

void CInputManager::BindKeyPressed( const KeyHandler& handler, int priority/*=0*/ )
{
	if(priority != 0)
		m_KeyPressedSignal.connect(priority, handler);
	else
		m_KeyPressedSignal.connect(handler);
}

void CInputManager::BindKeyReleased( const KeyHandler& handler, int priority/*=0*/ )
{
	if(priority != 0)
		m_KeyReleasedSignal.connect(priority, handler);
	else
		m_KeyReleasedSignal.connect(handler);
}

