#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "Singleton.h"
#include "InputManager.h"
#include "KratosPrerequisites.h"

namespace Kratos
{
	enum eCursorMode
	{
		eCursorMode_Normal,
		eCursorMode_TargetNone,
		eCursorMode_TargetAllied,
		eCursorMode_TargetEnemy,
		eCursorMode_TargetInvalid
	};

	/*------------------------------------------------
					CEGUI模块封装类
	-------------------------------------------------*/

	class CGUIManager : public CSingleton<CGUIManager>
	{
		DECLEAR_SINGLETON(CGUIManager);

		CGUIManager(void);
		virtual ~CGUIManager(void);

	public:
		bool	Init();
		void	Update(float dt);
		void	Shutdown();
		void	ShowCursor(bool bEnable);
		CEGUI::Window*	LoadWindowLayout(const Ogre::String& name);
		void	UnloadWindowLayout(CEGUI::Window* pWnd);
		void	SetGUISheet(CEGUI::Window* pWnd);
		//改变鼠标样式
		void	SetCursorMode(eCursorMode mode);

	private:
		CEGUI::OgreRenderer*	m_pRenderer;
		CEGUI::System*			m_pSystem;

		//挂接输入系统鼠标事件
		bool OnInputSys_MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		bool OnInputSys_MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		bool OnInputSys_MouseMove(const OIS::MouseEvent &arg);
		bool OnInputSys_KeyPressed(const OIS::KeyEvent& arg);
		bool OnInputSys_KeyReleased(const OIS::KeyEvent& arg);
	};

}

#endif
