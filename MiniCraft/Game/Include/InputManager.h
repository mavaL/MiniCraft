#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "Singleton.h"
#include <boost/bind.hpp>
#include <boost/signal.hpp>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

/*------------------------------------------------
					键盘鼠标输入管理类
-------------------------------------------------*/

class CInputManager : public CSingleton<CInputManager>, public OIS::MouseListener, public OIS::KeyListener
{
	DECLEAR_SINGLETON(CInputManager);
	CInputManager();
	~CInputManager();

	//signal combiner
	struct InputHandler 
	{  
		typedef bool result_type;
		template<typename InputIterator>  
		bool operator()(InputIterator first, InputIterator last) const  
		{    
			while (first != last)
			{     
				if (true == *first)       
					return true;       //某个slot处理了该事件返回true,则不用再继续处理
				++first;    
			}    
			return false; 
		}
	};
	typedef boost::function<bool(const OIS::MouseEvent&)> MouseMoveHandler;
	typedef boost::function<bool(const OIS::MouseEvent&, OIS::MouseButtonID)> MouseBtnHandler;
	typedef boost::function<bool(const OIS::KeyEvent&)> KeyHandler;

public:
	//初始化输入系统
	bool Init();
	//关闭输入系统
	void Shutdown();
	//输入系统是否已初始化
	bool IsInit() { return m_bIsInit; }
	//捕获输入设备
	void Capture();

	//挂接鼠标点击事件,第二个参数是调用顺序,默认是放到最后
	void BindMouseMove(const MouseMoveHandler& handler, int priority=0);
	//挂接鼠标松开事件
	void BindMousePressed(const MouseBtnHandler& handler, int priority=0);
	//挂接鼠标移动事件
	void BindMouseRelease(const MouseBtnHandler& handler, int priority=0);
	//挂接键盘按下事件
	void BindKeyPressed(const KeyHandler& handler, int priority=0);
	//挂接键盘松开事件
	void BindKeyReleased(const KeyHandler& handler, int priority=0);

private:
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;
	bool			m_bIsInit;

	//利用boost signal来管理输入事件
	boost::signal<bool(const OIS::MouseEvent&), InputHandler> m_MouseMoveSignal;
	boost::signal<bool(const OIS::MouseEvent&, OIS::MouseButtonID), InputHandler> m_MousePressedSignal;
	boost::signal<bool(const OIS::MouseEvent&, OIS::MouseButtonID), InputHandler> m_MouseReleasedSignal;
	boost::signal<bool(const OIS::KeyEvent&), InputHandler> m_KeyPressedSignal;
	boost::signal<bool(const OIS::KeyEvent&), InputHandler> m_KeyReleasedSignal;

	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
};

#endif
