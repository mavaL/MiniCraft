#include "stdafx.h"
#include "GUIManager.h"
#include "RendererModules\Ogre\CEGUIOgreRenderer.h"
#include "OgreManager.h"


using namespace CEGUI;


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
	assert(COgreManager::GetSingleton().IsInit() && "错误!!图形系统还未初始化!");
	assert(CInputManager::GetSingleton().IsInit() && "错误!!输入系统还未初始化!");

	m_pRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	if(m_pRenderer == NULL)
		return false;

	m_pSystem = &System::getSingleton();
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

	//创建默认鼠标图标
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	m_pSystem->setDefaultMouseCursor("TaharezLook", "MouseArrow");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");

	//日志输入细节设置高点
	//DefaultLogger::getSingleton().setLoggingLevel(Informative);

	//挂接鼠标输入事件,注册处理顺序都为1,即为每次都先进行处理
	CInputManager& InputMgr = CInputManager::GetSingleton();
	InputMgr.BindMousePressed(boost::bind(&CGUIManager::OnInputSys_MousePressed, this, _1, _2), eInputEventPriority_GUI);
	InputMgr.BindMouseRelease(boost::bind(&CGUIManager::OnInputSys_MouseReleased, this, _1, _2), eInputEventPriority_GUI);
	InputMgr.BindMouseMove(boost::bind(&CGUIManager::OnInputSys_MouseMove, this, _1), eInputEventPriority_GUI);

	// subscribe handler to render overlay items
	m_pRenderer->getDefaultRenderingRoot().subscribeEvent(CEGUI::RenderingSurface::EventRenderQueueStarted,
		CEGUI::Event::Subscriber(&CGUIManager::overlayHandler, this));

	 m_pRenderer->getDefaultRenderingRoot().clearGeometry(CEGUI::RQ_OVERLAY);

	m_cmdPanel.Init();
	m_infoPanel.Init();
	WindowManager& wndMgr = WindowManager::getSingleton();
	m_pSystem->setGUISheet(wndMgr.getWindow("Root"));
	m_pSystem->getGUISheet()->addChildWindow(wndMgr.getWindow("InfoPanelFrame"));

	return true;
}

void CGUIManager::Shutdown()
{
	CInputManager& InputMgr = CInputManager::GetSingleton();
	InputMgr.UnbindMouseMove(eInputEventPriority_GUI);
	InputMgr.UnbindMousePressed(eInputEventPriority_GUI);
	InputMgr.UnbindMouseRelease(eInputEventPriority_GUI);

	m_cmdPanel.Destroy();
	m_infoPanel.Destroy();
	if (m_pRenderer)
	{
		OgreRenderer::destroySystem();
		m_pRenderer = NULL;
		m_pSystem = NULL;
	}
}

bool CGUIManager::overlayHandler( const CEGUI::EventArgs& args )
{
// 	if (static_cast<const RenderQueueEventArgs&>(args).queueID != RQ_OVERLAY)
// 		return false;
// 
// 	CEGUI::Font* fnt = CEGUI::System::getSingleton().getDefaultFont();
// 	if (fnt && m_CarInfo != "")
// 	{
// 		//暂时把FPS信息一起在这里显示
// 		const Ogre::RenderTarget::FrameStats& RenderInfo = COgreManager::GetSingleton().GetProfileInfo();
// 		static String LastFPS, AverageFPS, TriangleCount, BatchCount;
// 		LastFPS = String("    LastFPS:") + PropertyHelper::floatToString(RenderInfo.lastFPS);
// 		AverageFPS = String(" AvergeFPS:") + PropertyHelper::floatToString(RenderInfo.avgFPS);
// 		TriangleCount = String(" TriangleCount:") + PropertyHelper::uintToString(RenderInfo.triangleCount);
// 		BatchCount = String("  BatchCount:") + PropertyHelper::uintToString(RenderInfo.batchCount);
// 
// 		m_CarInfo += LastFPS + AverageFPS + TriangleCount + BatchCount;
// 
// 		m_pGeometryBuffer->reset();
// 		fnt->drawText(*m_pGeometryBuffer, m_CarInfo, CEGUI::Vector2(0, 0), 0, colour(0xFFFFFFFF));
// 		m_pGeometryBuffer->draw();
// 	}

	return true;
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

CEGUI::Window* CGUIManager::LoadWindowLayout( const STRING& name )
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
