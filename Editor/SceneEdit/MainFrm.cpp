String

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SceneEdit.h"
#include "MainFrm.h"
#include "View.h"
#include "EditorDefine.h"
#include "Manipulator/ManipulatorScene.h"
#include "Manipulator/ManipulatorAction.h"
#include "Action/ActionBase.h"

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
	ON_UPDATE_COMMAND_UI(ID_Terrain_BrushSize1, OnUpdateUI_TerrainBrushSize1)
	ON_UPDATE_COMMAND_UI(ID_Terrain_BrushSize2, OnUpdateUI_TerrainBrushSize2)
	ON_XTP_EXECUTE(ID_Terrain_BrushSize1, OnTerrainBrushSize1)
	ON_NOTIFY(XTP_FN_SPINUP, ID_Terrain_BrushSize1, OnTerrainBrushSize1Spin)
	ON_NOTIFY(XTP_FN_SPINDOWN, ID_Terrain_BrushSize1, OnTerrainBrushSize1Spin)
	ON_XTP_EXECUTE(ID_Terrain_BrushSize2, OnTerrainBrushSize2)
	ON_NOTIFY(XTP_FN_SPINUP, ID_Terrain_BrushSize2, OnTerrainBrushSize2Spin)
	ON_NOTIFY(XTP_FN_SPINDOWN, ID_Terrain_BrushSize2, OnTerrainBrushSize2Spin)
	ON_UPDATE_COMMAND_UI(IDC_Terrain_Modify, OnUpdateUI_BtnTerrainModify)
	ON_COMMAND(IDC_Terrain_Modify, OnBtnTerrainModify)
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

	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars == NULL)
		return false;

	CXTPPaintManager::SetTheme(xtpThemeRibbon);

	//关闭默认菜单
	::SetMenu(m_hWnd, nullptr);
	m_hMenuDefault = nullptr;
	m_dwMenuBarState = AFX_MBS_HIDDEN;
	
	//初始化图标
	_LoadIcon();

	///Ribbon
	CXTPRibbonBar* pRibbonBar = (CXTPRibbonBar*)pCommandBars->Add(_T("Ribbon"), xtpBarTop, RUNTIME_CLASS(CXTPRibbonBar));
	if (!pRibbonBar)
		return false;
	pRibbonBar->EnableDocking(0);
	//pRibbonBar->GetTabPaintManager()->m_bSelectOnButtonDown = FALSE;

	///创建系统按钮
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

	///RibbonHome
	CXTPRibbonTab* pTab = pRibbonBar->AddTab(L"Home");

	///RibbonHome - GroupTerrainModify
	CXTPRibbonGroup* pGroup = pTab->AddGroup(L"Terrain Modify");

	//RibbonHome - GroupTerrainModify - BtnTerrainModify
	pGroup->Add(xtpControlButton, IDC_Terrain_Modify);
	
	//RibbonHome - GroupTerrainBrush
	pGroup = pTab->AddGroup(L"Terrain Brush");

	//RibbonHome - GroupTerrainBrush - EditBrushSize
	CXTPControlEdit* pControl = (CXTPControlEdit*)pGroup->Add(xtpControlEdit, ID_Terrain_BrushSize1);
	pControl->ShowSpinButtons(TRUE);

	pControl = (CXTPControlEdit*)pGroup->Add(xtpControlEdit, ID_Terrain_BrushSize2);
	pControl->ShowSpinButtons(TRUE);

	return true;
}

void CMainFrame::_LoadIcon()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	UINT uCommand = {IDB_GEAR};
	pCommandBars->GetImageManager()->SetIcons(IDB_GEAR, &uCommand, 1, CSize(0, 0), xtpImageNormal);

	UINT uiSystemMenu[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE }; 
	pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEMMENULARGE, uiSystemMenu, _countof(uiSystemMenu), CSize(32, 32));

	UINT uiTerrainModifyBtn[] = { IDC_Terrain_Modify };
	pCommandBars->GetImageManager()->SetIcons(IDB_Button, uiTerrainModifyBtn, _countof(uiTerrainModifyBtn), CSize(32, 32));
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

void CMainFrame::_CreateDockPane()
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

bool CMainFrame::_CreateMeshPanel( CImageList& imageList, Ogre::StringVectorPtr& meshNames )
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

bool CMainFrame::CreateEditorMainUI()
{
	if(!_OnCreateRibbon())
		return false;

	CImageList iconList;
	Ogre::StringVectorPtr meshNames;
	(dynamic_cast<CSceneEditApp*>(AfxGetApp()))->m_app.RenderAllMeshIcons(iconList, meshNames);

	if(!_CreateMeshPanel(iconList, meshNames))
		return FALSE;

	_CreateDockPane();

	return true;
}

void CMainFrame::OnUpdateUI_TerrainBrushSize1( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, CXTPControl::FromUI(pCmdUI));
	if (pControl && !pControl->HasFocus())
	{
		float width = ManipulatorSystem.GetTerrain().GetSquareBrushWidth();
		std::wstring strWidth = Utility::EngineToUnicode(Ogre::StringConverter::toString(width));
		pControl->SetEditText(strWidth.c_str());
	}
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateUI_TerrainBrushSize2( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, CXTPControl::FromUI(pCmdUI));
	if (pControl && !pControl->HasFocus())
	{
		float height = ManipulatorSystem.GetTerrain().GetSquareBrushHeight();
		std::wstring strHeight = Utility::EngineToUnicode(Ogre::StringConverter::toString(height));
		pControl->SetEditText(strHeight.c_str());
	}
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnTerrainBrushSize1( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, tagNMCONTROL->pControl);

	if (pControl)
	{
		String strWidth = Utility::UnicodeToEngine(pControl->GetEditText());
		float width = Ogre::StringConverter::parseReal(strWidth);
		ManipulatorSystem.GetTerrain().SetSquareBrushWidth(width);

		*pResult = TRUE;
	}
}

void CMainFrame::OnTerrainBrushSize1Spin( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMXTPUPDOWN* tagNMCONTROL = (NMXTPUPDOWN*)pNMHDR;
	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, tagNMCONTROL->pControl);

	if (pControl)
	{
		String strWidth = Utility::UnicodeToEngine(pControl->GetEditText());
		float width = Ogre::StringConverter::parseReal(strWidth) + tagNMCONTROL->iDelta;
		ManipulatorSystem.GetTerrain().SetSquareBrushWidth(width);

		std::wstring strNewWidth = Utility::EngineToUnicode(Ogre::StringConverter::toString(width));
		pControl->SetEditText(strNewWidth.c_str());
	}
	*pResult = 1;
}

void CMainFrame::OnTerrainBrushSize2( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, tagNMCONTROL->pControl);

	if (pControl)
	{
		String strHeight = Utility::UnicodeToEngine(pControl->GetEditText());
		float height = Ogre::StringConverter::parseReal(strHeight);
		ManipulatorSystem.GetTerrain().SetSquareBrushHeight(height);

		*pResult = TRUE;
	}
}

void CMainFrame::OnTerrainBrushSize2Spin( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMXTPUPDOWN* tagNMCONTROL = (NMXTPUPDOWN*)pNMHDR;
	CXTPControlEdit* pControl = DYNAMIC_DOWNCAST(CXTPControlEdit, tagNMCONTROL->pControl);

	if (pControl)
	{
		String strHeight = Utility::UnicodeToEngine(pControl->GetEditText());
		float height = Ogre::StringConverter::parseReal(strHeight) + tagNMCONTROL->iDelta;
		ManipulatorSystem.GetTerrain().SetSquareBrushHeight(height);

		std::wstring strNewHeight = Utility::EngineToUnicode(Ogre::StringConverter::toString(height));
		pControl->SetEditText(strNewHeight.c_str());
	}
	*pResult = 1;
}

void CMainFrame::OnUpdateUI_BtnTerrainModify( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	bool bActive = ManipulatorSystem.GetTerrain().GetTerrainModifyEnabled();
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bActive);
}

void CMainFrame::OnBtnTerrainModify()
{
	bool bEnable = !ManipulatorSystem.GetTerrain().GetTerrainModifyEnabled();
	eActionType action = bEnable ? eActionType_TerrainDeform : eActionType_None;
	ManipulatorAction::GetSingleton().SetActiveAction(action);
}
