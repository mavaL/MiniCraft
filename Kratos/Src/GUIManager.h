#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "Singleton.h"
#include "CEGUI.h"
#include "InputManager.h"
#include <OgrePrerequisites.h>

namespace CEGUI
{
	class OgreRenderer;
}

namespace Kratos
{
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
		void	Shutdown();
		void	ShowCursor(bool bEnable);
		CEGUI::Window*	LoadWindowLayout(const Ogre::String& name);
		void	UnloadWindowLayout(CEGUI::Window* pWnd);
		void	SetGUISheet(CEGUI::Window* pWnd);

	private:
		CEGUI::OgreRenderer*	m_pRenderer;
		CEGUI::System*			m_pSystem;

		//Hook进渲染事件,绘制自定义数据
		bool overlayHandler(const CEGUI::EventArgs& args);
		//	CEGUI::GeometryBuffer* m_pGeometryBuffer; //绘制赛车速度文字的Buffer


		//挂接输入系统鼠标事件
		bool OnInputSys_MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		bool OnInputSys_MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		bool OnInputSys_MouseMove(const OIS::MouseEvent &arg);
	};

}

#endif
