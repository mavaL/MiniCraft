#include "stdafx.h"
#include "BehaviorTreeEditorDlg.h"
#include "BehaviorTreeEditorView.h"
#include "BehaviorTreeEditorExplorer.h"
#include "BehaviorTreeEditorProperty.h"

BEGIN_MESSAGE_MAP(DialogBehaviorTreeEditor, CXTPDialog)
	ON_WM_SIZE()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, _AttachDockPane)
END_MESSAGE_MAP()

DialogBehaviorTreeEditor::DialogBehaviorTreeEditor( CWnd* pParent /*= NULL*/ )
:CXTPDialog()
,m_pView(new BehaviorTreeEditorView)
,m_pExplorer(new BehaviorTreeEditorExplorer(this))
,m_property(new BehaviorTreeEditorProperty(this))
,m_paneExplorer(NULL)
,m_paneProperty(NULL)
{
	Create(IDD_DlgChildEmpty, pParent);
}

DialogBehaviorTreeEditor::~DialogBehaviorTreeEditor()
{
	SAFE_DELETE(m_pView);
	SAFE_DELETE(m_pExplorer);
	SAFE_DELETE(m_property);
}

BOOL DialogBehaviorTreeEditor::OnInitDialog()
{
	if (!m_pView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0), this, AFX_IDW_PANE_FIRST, NULL))
		return FALSE;

	if (!InitCommandBars())
		return FALSE;

	_CreateRibbon();
	_CreateDockPane();

	return TRUE;
}

void DialogBehaviorTreeEditor::OnSize( UINT nType, int cx, int cy )
{
	CRect rcClient;
	GetClientRect(rcClient);

	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposDefault, 0, rcClient);
	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, rcClient, rcClient);

	HDWP hdwp = BeginDeferWindowPos(1);
	DeferWindowPos(hdwp, m_pView->GetSafeHwnd(), nullptr, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), SWP_NOZORDER);
	EndDeferWindowPos(hdwp);
}

void DialogBehaviorTreeEditor::_CreateRibbon()
{
	CXTPPaintManager::SetTheme(xtpThemeRibbon);
	 
	///Ribbon
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)GetCommandBars()->Add(_T("Ribbon"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
	pRibbonBar->ShowCaptionAlways(FALSE);
	pRibbonBar->ShowQuickAccess(FALSE);

	///RibbonHome
	CXTPRibbonTab* pTab = pRibbonBar->AddTab(L"Home");
	//RibbonHome - Groupxxx
	CXTPRibbonGroup* pGroup = pTab->AddGroup(L"xxxx");
	//RibbonHome - Groupxxx - AddNode
	pGroup->Add(xtpControlButton, IDC_BTEditor_AddNode);
}

void DialogBehaviorTreeEditor::_CreateDockPane()
{
	m_dockMgr.InstallDockingPanes(this);
	m_dockMgr.SetTheme(xtpPaneThemeVisualStudio2010);
	m_dockMgr.SetClientMargin(6);
	m_dockMgr.SetThemedFloatingFrames(TRUE);
	m_dockMgr.SetAlphaDockingContext(TRUE);
	m_dockMgr.SetShowDockingContextStickers(TRUE);
	m_dockMgr.SetShowContentsWhileDragging(TRUE);
	m_dockMgr.SetDefaultPaneOptions(xtpPaneNoHideable);

	m_paneExplorer = m_dockMgr.CreatePane(IDR_Pane_BT_Explorer, CRect(0, 0, 250, 0), xtpPaneDockLeft);
	m_paneExplorer->SetMinTrackSize(100);
	m_paneExplorer->Attach(m_pExplorer);

	m_paneProperty = m_dockMgr.CreatePane(IDR_Pane_BT_Property, CRect(0, 0, 250, 0), xtpPaneDockRight);
	m_paneProperty->SetMinTrackSize(100);
	m_paneProperty->Attach(m_property);
}

LRESULT DialogBehaviorTreeEditor::_AttachDockPane( WPARAM wParam, LPARAM lParam )
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			switch (pPane->GetID())
			{
			case IDR_Pane_BT_Explorer: pPane->Attach(m_pExplorer); break;
			case IDR_Pane_BT_Property: pPane->Attach(m_property); break;
			default: assert(0);
			}
		}
		return 1;
	}

	return 0;
}
