String

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SceneEdit.h"
#include "MainFrm.h"
#include "View.h"
#include "EditorDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CXTPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CXTPFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, _AttachDockPane)
	ON_MESSAGE(XTPWM_TASKPANEL_NOTIFY,	OnResPanelNotify)
END_MESSAGE_MAP()


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
:m_wndView(NULL)
{
	
}

CMainFrame::~CMainFrame()
{
	SAFE_DELETE(m_wndView);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!_OnCreateRibbon())
		return -1;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.cx = EDITOR_CLIENT_W;
	cs.cy = EDITOR_CLIENT_H;
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 将焦点前移到视图窗口
	m_wndView->SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 让视图第一次尝试该命令
	if (m_wndView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
	return CXTPFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnClose()
{
	KillTimer(0);

	CSceneEditApp* app = (CSceneEditApp*)AfxGetApp();
	app->m_app.Shutdown();

	CXTPFrameWnd::OnClose();
}

bool CMainFrame::_OnCreateRibbon()
{
	// Initialize the command bars
	if (!InitCommandBars())
		return false;

	_LoadIcon();

	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars == NULL)
		return false;

	CXTPPaintManager::SetTheme(xtpThemeRibbon);

	//关闭默认菜单
	CMenu menu;
	menu.Attach(::GetMenu(m_hWnd));
	SetMenu(NULL);

	//Ribbon - Home
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->Add(_T("Home"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
	if (!pRibbonBar)
		return false;
	pRibbonBar->EnableDocking(0);
	//pRibbonBar->GetTabPaintManager()->m_bSelectOnButtonDown = FALSE;

	//创建系统按钮
	CXTPControlPopup* pControlFile = (CXTPControlPopup*)pRibbonBar->AddSystemButton();
	pControlFile->SetIconId(IDB_GEAR);

	CMenu sysMenu;
	sysMenu.LoadMenu(IDR_MAINFRAME);
	CXTPPopupBar* pCommandBar = new CXTPRibbonSystemPopupBar();
	pCommandBar->SetCommandBars(pCommandBars);
	pControlFile->SetCommandBar(pCommandBar);
	pCommandBar->InternalRelease();
	pCommandBar->LoadMenu(sysMenu.GetSubMenu(0));
	pCommandBar->SetIconSize(CSize(32, 32));
	
	//test tab
	CXTPRibbonTab* pTab1 = pRibbonBar->AddTab(L"Tab1");
	CXTPRibbonTab* pTab2 = pRibbonBar->AddTab(L"Tab2");

	return true;
}

void CMainFrame::_LoadIcon()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	UINT uCommand = {IDB_GEAR};
	pCommandBars->GetImageManager()->SetIcons(IDB_GEAR, &uCommand, 1, CSize(0, 0), xtpImageNormal);

	UINT uiSystemMenu[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE }; 
	pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEMMENULARGE, uiSystemMenu, _countof(uiSystemMenu), CSize(32, 32));
}

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext )
{
	m_wndView = new CEditorView;
	if (!m_wndView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, lpcs->cx, lpcs->cy), this, AFX_IDW_PANE_FIRST, NULL))
		return FALSE;
	SetActiveView(m_wndView);

	return TRUE;
}

void CMainFrame::OnTimer( UINT_PTR nIDEvent )
{
	assert(nIDEvent == 0);

	CSceneEditApp* app = (CSceneEditApp*)AfxGetApp();
	if(!app->m_app.Update())
		SendMessage(WM_CLOSE);
}

void CMainFrame::CreateDockPane()
{
	m_paneManager.InstallDockingPanes(this);

	// Create docking panes.
	m_paneManager.CreatePane(IDR_Pane_ResourceSelector, CRect(0, 0, 250, 120), xtpPaneDockLeft);
}

LRESULT CMainFrame::_AttachDockPane( WPARAM wParam, LPARAM lParam )
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			switch (pPane->GetID())
			{
			case IDR_Pane_ResourceSelector:
				pPane->Attach(&m_resourceSelector);
				break;
			default: assert(0);
			}
		}
		return 1;
	}

	return 0;
}

bool CMainFrame::CreateMeshPanel( CImageList& imageList, Ogre::StringVectorPtr& meshNames )
{
	if (!m_resourceSelector.Create(WS_CHILD|WS_VISIBLE, CRect(200,100,400,300), this, IDS_ResourceSelector))
		return false;

	m_resourceSelector.SetOwner(this);
	m_resourceSelector.SetIconSize(CSize(MESH_ICON_SIZE, MESH_ICON_SIZE));
	CXTPTaskPanelGroup* pGroup = m_resourceSelector.AddGroup(IDS_ResourceSelector_Mesh);

	int itemCount = imageList.GetImageCount();
 	m_resourceSelector.GetImageManager()->SetImageList(imageList.Detach(), 0);
	
	for (int i=0; i<itemCount; ++i)
	{
		std::wstring meshname(Utility::EngineToUnicode(meshNames->at(i)));
		meshname.erase(meshname.length()-5);
		// Add folder entries
		CXTPTaskPanelGroupItem* pItem = pGroup->AddLinkItem(i, 0);
		pItem->SetIconIndex(i);
		pItem->SetCaption(meshname.c_str());
	}
					
	m_resourceSelector.SetBehaviour(xtpTaskPanelBehaviourList);
	m_resourceSelector.SetSelectItemOnFocus(TRUE);
	m_resourceSelector.SetMultiColumn(TRUE);
	m_resourceSelector.SetColumnWidth(RES_SELECTOR_COLUMN_WIDTH);

	// Select the first folder.
	m_resourceSelector.GetAt(0)->SetExpanded(TRUE);
	m_resourceSelector.AllowDrag(xtpTaskItemAllowDragCopyOutsideControl);

	return true;
}

LRESULT CMainFrame::OnResPanelNotify( WPARAM wParam, LPARAM lParam )
{
	switch(wParam) {
	case XTP_TPN_CLICK:
		{
			CXTPTaskPanelGroupItem* pItem = (CXTPTaskPanelGroupItem*)lParam;
			TRACE(_T("Click Event: pItem.Caption = %s, pItem.ID = %i\n"), pItem->GetCaption(), pItem->GetID());
		}
		break;

	case XTP_TPN_RCLICK:
		{
			CXTPTaskPanelItem* pItem = (CXTPTaskPanelItem*)lParam;
			TRACE(_T("RClick Event: pItem.Caption = %s, pItem.ID = %i\n"), pItem->GetCaption(), pItem->GetID());

		}
		break;
	}

	return 0;
}
