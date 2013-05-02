#include "stdafx.h"
#include "GUIManager.h"
#include "RendererModules\Ogre\CEGUIOgreRenderer.h"
#include "OgreManager.h"


using namespace CEGUI;

namespace Kratos
{
	CGUIManager::CGUIManager(void)
	: m_pRenderer(NULL)
	,m_pSystem(NULL)
	{
	}

	CGUIManager::~CGUIManager(void)
	{
	}

	bool CGUIManager::Init()
	{
		//图形系统和输入系统需初始化
		assert(RenderManager.IsInit() && "错误!!图形系统还未初始化!");
		assert(CInputManager::GetSingleton().IsInit() && "错误!!输入系统还未初始化!");

		m_pRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
		if(m_pRenderer == NULL)
			return false;

		m_pSystem = &System::getSingleton();

		// set the default resource groups to be used
		Imageset::setDefaultResourceGroup("Imagesets");
		Font::setDefaultResourceGroup("Fonts");
		Scheme::setDefaultResourceGroup("Schemes");
		WidgetLookManager::setDefaultResourceGroup("LookNFeel");
		WindowManager::setDefaultResourceGroup("Layouts");
		AnimationManager::setDefaultResourceGroup("Animation");

		//DefaultLogger::getSingleton().setLoggingLevel(Informative);

		//挂接鼠标输入事件,注册处理顺序都为1,即为每次都先进行处理
		CInputManager& InputMgr = CInputManager::GetSingleton();
		InputMgr.BindMousePressed(boost::bind(&CGUIManager::OnInputSys_MousePressed, this, _1, _2), eInputEventPriority_GUI);
		InputMgr.BindMouseRelease(boost::bind(&CGUIManager::OnInputSys_MouseReleased, this, _1, _2), eInputEventPriority_GUI);
		InputMgr.BindMouseMove(boost::bind(&CGUIManager::OnInputSys_MouseMove, this, _1), eInputEventPriority_GUI);

		return true;
	}

	void CGUIManager::Shutdown()
	{
		CInputManager& InputMgr = CInputManager::GetSingleton();
		InputMgr.UnbindMouseMove(eInputEventPriority_GUI);
		InputMgr.UnbindMousePressed(eInputEventPriority_GUI);
		InputMgr.UnbindMouseRelease(eInputEventPriority_GUI);

		if (m_pRenderer)
		{
			OgreRenderer::destroySystem();
			m_pRenderer = NULL;
			m_pSystem = NULL;
		}
	}

	void CGUIManager::ShowCursor( bool bEnable )
	{
		if(bEnable)
			MouseCursor::getSingleton().show();
		else
			MouseCursor::getSingleton().hide();
	}

	//OIS与CEGUI的鼠标按键ID不同,需要转换
	static CEGUI::MouseButton OIS2CEGUI(OIS::MouseButtonID buttonID)
	{
		switch (buttonID)
		{
		case OIS::MB_Left:
			return CEGUI::LeftButton;

		case OIS::MB_Right:
			return CEGUI::RightButton;

		case OIS::MB_Middle:
			return CEGUI::MiddleButton;

		default:
			return CEGUI::LeftButton;
		}
	}

	bool CGUIManager::OnInputSys_MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		return m_pSystem->injectMouseButtonDown(OIS2CEGUI(id));
	}

	bool CGUIManager::OnInputSys_MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		return m_pSystem->injectMouseButtonUp(OIS2CEGUI(id));
	}

	bool CGUIManager::OnInputSys_MouseMove( const OIS::MouseEvent &arg )
	{
		return m_pSystem->injectMouseMove(static_cast<float>(arg.state.X.rel), static_cast<float>(arg.state.Y.rel));
	}

	CEGUI::Window* CGUIManager::LoadWindowLayout( const Ogre::String& name )
	{
		return WindowManager::getSingleton().loadWindowLayout(name);
	}

	void CGUIManager::SetGUISheet( CEGUI::Window* pWnd )
	{
		m_pSystem->setGUISheet(pWnd);
	}

	void CGUIManager::UnloadWindowLayout( CEGUI::Window* pWnd )
	{
		assert(pWnd);
		WindowManager::getSingleton().destroyWindow(pWnd);
	}

	void CGUIManager::SetCursorMode( eCursorMode mode )
	{
		CEGUI::String imgsetName, imgName;
		switch (mode)
		{
		case eCursorMode_Normal:	{ imgsetName = "cursor.dds"; imgName = "TerranCursor"; } break;
		case eCursorMode_TargetNone: { imgsetName = "cursor-target-none.dds"; imgName = "cursor-target-none"; } break;
		case eCursorMode_TargetAllied: { imgsetName = "cursor-target-allied.dds"; imgName = "cursor-target-allied"; } break;
		case eCursorMode_TargetEnemy: { imgsetName = "cursor-target-enemy.dds"; imgName = "cursor-target-enemy"; } break;
		case eCursorMode_TargetInvalid: { imgsetName = "cursor-target-invalid.dds"; imgName = "cursor-target-invalid"; } break;
		default: assert(0);
		}

		m_pSystem->setDefaultMouseCursor(imgsetName, imgName);
		MouseCursor::getSingleton().setImage(imgsetName, imgName);
	}

}


