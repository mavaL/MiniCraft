#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "Singleton.h"
#include <boost/bind.hpp>
#include <boost/signal.hpp>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

//输入事件优先级,由高到低
enum eInputEventPriority
{
	eInputEventPriority_GUI			=	0,		//GUI事件响应
	eInputEventPriority_Targeting	=	1,		//鼠标选择目标
	eInputEventPriority_default		=	2,
	eInputEventPriority_Count
};

enum eInputEvent
{
	eInputEvent_MouseMove,
	eInputEvent_MouseBtnDown,
	eInputEvent_MouseBtnUp,
	eInputEvent_KeyDown,
	eInputEvent_KeyUp,
	eInputEvent_Count
};

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
	bool Init();
	void Shutdown();
	bool IsInit() { return m_bIsInit; }
	//捕获输入设备
	void Capture();

	//挂接鼠标点击事件,第二个参数是调用顺序,默认是放到最后
	void BindMouseMove(const MouseMoveHandler& handler, eInputEventPriority priority=eInputEventPriority_default);
	//挂接鼠标松开事件
	void BindMousePressed(const MouseBtnHandler& handler, eInputEventPriority priority=eInputEventPriority_default);
	//挂接鼠标移动事件
	void BindMouseRelease(const MouseBtnHandler& handler, eInputEventPriority priority=eInputEventPriority_default);
	//挂接键盘按下事件
	void BindKeyPressed(const KeyHandler& handler, eInputEventPriority priority=eInputEventPriority_default);
	//挂接键盘松开事件
	void BindKeyReleased(const KeyHandler& handler, eInputEventPriority priority=eInputEventPriority_default);

	//断开挂接输入事件
	void UnbindMouseMove(eInputEventPriority priority);
	void UnbindMousePressed(eInputEventPriority priority);
	void UnbindMouseRelease(eInputEventPriority priority);
	void UnbindKeyPressed(eInputEventPriority priority);
	void UnbindKeyReleased(eInputEventPriority priority);

	//阻塞/沟通挂接连接
	void BlockMouseMove(eInputEventPriority priority, bool bBlock);
	void BlockMousePressed(eInputEventPriority priority, bool bBlock);
	void BlockMouseRelease(eInputEventPriority priority, bool bBlock);
	void BlockKeyPressed(eInputEventPriority priority, bool bBlock);
	void BlockKeyReleased(eInputEventPriority priority, bool bBlock);

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

	typedef std::vector<boost::signals::connection>	ConnectionGroup;
	typedef std::vector<ConnectionGroup>	ConnectionPriorityGroup;
	ConnectionPriorityGroup		m_connections;		//信号连接存储容器

	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg ) { return m_MouseMoveSignal(arg); }
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) { return m_MousePressedSignal(arg, id); }
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) { return m_MouseReleasedSignal(arg, id); }
	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg ) { return m_KeyPressedSignal(arg); }
	virtual bool keyReleased( const OIS::KeyEvent &arg ) { return m_KeyReleasedSignal(arg); }
};

#endif
