#include "stdafx.h"
#include "InputManager.h"
#include "OgreManager.h"

namespace Kratos
{

	CInputManager::CInputManager(void)
		: mInputManager(0),
		mMouse(0),
		mKeyboard(0),
		m_bIsInit(false)
	{
		m_connections.resize(eInputEventPriority_Count);
		for(int i=0; i<eInputEventPriority_Count; ++i)
			m_connections[i].resize(eInputEvent_Count);
	}

	CInputManager::~CInputManager(void)
	{
	}

	bool CInputManager::Init()
	{
		//图形系统需初始化
		assert(RenderManager.IsInit() && "错误!!图形系统还未初始化!");

		OIS::ParamList pl;
		unsigned long hwnd = 0;
		RenderManager.GetMainWndHandle(hwnd);
		pl.insert(std::make_pair("WINDOW", Ogre::StringConverter::toString(hwnd)));

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

		OIS::MouseState& mouseState = const_cast<OIS::MouseState&>(mMouse->getMouseState());
		mouseState.width = RenderManager.mWindow->getWidth();
		mouseState.height = RenderManager.mWindow->getHeight();
		mouseState.X.abs = mouseState.width / 2;
		mouseState.Y.abs = mouseState.height / 2;

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

	void CInputManager::BindMouseMove( const MouseMoveHandler& handler, eInputEventPriority priority/*=eInputEventPriority_default*/ )
	{
		m_connections[priority][eInputEvent_MouseMove] = m_MouseMoveSignal.connect(priority, handler);
	}

	void CInputManager::BindMousePressed( const MouseBtnHandler& handler, eInputEventPriority priority/*=eInputEventPriority_default*/ )
	{
		m_connections[priority][eInputEvent_MouseBtnDown] = m_MousePressedSignal.connect(priority, handler);
	}

	void CInputManager::BindMouseRelease( const MouseBtnHandler& handler, eInputEventPriority priority/*=eInputEventPriority_default*/ )
	{
		m_connections[priority][eInputEvent_MouseBtnUp] = m_MouseReleasedSignal.connect(priority, handler);
	}

	void CInputManager::BindKeyPressed( const KeyHandler& handler, eInputEventPriority priority/*=eInputEventPriority_default*/ )
	{
		m_connections[priority][eInputEvent_KeyDown] = m_KeyPressedSignal.connect(priority, handler);
	}

	void CInputManager::BindKeyReleased( const KeyHandler& handler, eInputEventPriority priority/*=eInputEventPriority_default*/ )
	{
		m_connections[priority][eInputEvent_KeyUp] = m_KeyReleasedSignal.connect(priority, handler);
	}

	void CInputManager::UnbindMouseMove( eInputEventPriority priority )
	{
		assert(m_connections[priority][eInputEvent_MouseMove].connected());
		m_connections[priority][eInputEvent_MouseMove].disconnect();
	}

	void CInputManager::UnbindMousePressed( eInputEventPriority priority )
	{
		assert(m_connections[priority][eInputEvent_MouseBtnDown].connected());
		m_connections[priority][eInputEvent_MouseBtnDown].disconnect();
	}

	void CInputManager::UnbindMouseRelease( eInputEventPriority priority )
	{
		assert(m_connections[priority][eInputEvent_MouseBtnUp].connected());
		m_connections[priority][eInputEvent_MouseBtnUp].disconnect();
	}

	void CInputManager::UnbindKeyPressed( eInputEventPriority priority )
	{
		assert(m_connections[priority][eInputEvent_KeyDown].connected());
		m_connections[priority][eInputEvent_KeyDown].disconnect();
	}

	void CInputManager::UnbindKeyReleased( eInputEventPriority priority )
	{
		assert(m_connections[priority][eInputEvent_KeyUp].connected());
		m_connections[priority][eInputEvent_KeyUp].disconnect();
	}

	void CInputManager::BlockMouseMove( eInputEventPriority priority, bool bBlock )
	{
		assert(m_connections[priority][eInputEvent_MouseMove].connected());
		if(bBlock)
			m_connections[priority][eInputEvent_MouseMove].block();
		else
			m_connections[priority][eInputEvent_MouseMove].unblock();
	}

	void CInputManager::BlockMousePressed( eInputEventPriority priority, bool bBlock )
	{
		assert(m_connections[priority][eInputEvent_MouseBtnDown].connected());
		if(bBlock)
			m_connections[priority][eInputEvent_MouseBtnDown].block();
		else
			m_connections[priority][eInputEvent_MouseBtnDown].unblock();
	}

	void CInputManager::BlockMouseRelease( eInputEventPriority priority, bool bBlock )
	{
		assert(m_connections[priority][eInputEvent_MouseBtnUp].connected());
		if(bBlock)
			m_connections[priority][eInputEvent_MouseBtnUp].block();
		else
			m_connections[priority][eInputEvent_MouseBtnUp].unblock();
	}

	void CInputManager::BlockKeyPressed( eInputEventPriority priority, bool bBlock )
	{
		assert(m_connections[priority][eInputEvent_KeyDown].connected());
		if(bBlock)
			m_connections[priority][eInputEvent_KeyDown].block();
		else
			m_connections[priority][eInputEvent_KeyDown].unblock();
	}

	void CInputManager::BlockKeyReleased( eInputEventPriority priority, bool bBlock )
	{
		assert(m_connections[priority][eInputEvent_KeyUp].connected());
		if(bBlock)
			m_connections[priority][eInputEvent_KeyUp].block();
		else
			m_connections[priority][eInputEvent_KeyUp].unblock();
	}

}


