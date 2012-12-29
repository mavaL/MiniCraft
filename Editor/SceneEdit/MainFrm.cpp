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
#include "UI/TerrainPropertyPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CXTPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CXTPFrameWnd)
	ON_WM_CREATE()
	ON_XTP_CREATECONTROL()
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
	ON_UPDATE_COMMAND_UI(IDC_Terrain_Deform, OnUpdateUI_BtnTerrainDeform)
	ON_COMMAND(IDC_Terrain_Deform, OnBtnTerrainDeform)
	ON_UPDATE_COMMAND_UI(IDC_Terrain_Splat, OnUpdateUI_BtnTerrainSplat)
	ON_COMMAND(IDC_Terrain_Splat, OnBtnTerrainSplat)
	ON_UPDATE_COMMAND_UI(IDC_Terrain_LayerTexture, OnUpdateUI_GalleryLayerTex)
	ON_XTP_EXECUTE(IDC_Terrain_LayerTexture, OnSelectLayerTex)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_Terrain_Splat_Layer0, IDC_Terrain_Splat_Layer4, OnUpdateUI_SplatSelectLayer)
	ON_COMMAND_RANGE(IDC_Terrain_Splat_Layer0, IDC_Terrain_Splat_Layer4, OnSplatSelectLayer)
END_MESSAGE_MAP()


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
:m_wndView(NULL)
,m_propertyTerrain(new PropertyPaneTerrain)
,m_terrainTexGallery(NULL)
{
}

CMainFrame::~CMainFrame()
{
	ManipulatorSystem.RemoveCallback(this);
	SAFE_DELETE(m_propertyTerrain);
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

	//RibbonHome - GroupTerrainModify - Deform
	pGroup->Add(xtpControlButton, IDC_Terrain_Deform);

	//RibbonHome - GroupTerrainModify - Splat
	CXTPControlPopup* pBtn = dynamic_cast<CXTPControlPopup*>(pGroup->Add(xtpControlSplitButtonPopup, IDC_Terrain_Splat));
	pBtn->GetCommandBar()->GetControls()->Add(xtpControlButton, IDC_Terrain_Splat_Layer0);
	pBtn->GetCommandBar()->GetControls()->Add(xtpControlButton, IDC_Terrain_Splat_Layer1);
	pBtn->GetCommandBar()->GetControls()->Add(xtpControlButton, IDC_Terrain_Splat_Layer2);
	pBtn->GetCommandBar()->GetControls()->Add(xtpControlButton, IDC_Terrain_Splat_Layer3);
	pBtn->GetCommandBar()->GetControls()->Add(xtpControlButton, IDC_Terrain_Splat_Layer4);
	
	//RibbonHome - GroupTerrainBrush
	pGroup = pTab->AddGroup(L"Terrain Brush");

	//RibbonHome - GroupTerrainBrush - EditBrushSize
	CXTPControlEdit* pControl = (CXTPControlEdit*)pGroup->Add(xtpControlEdit, ID_Terrain_BrushSize1);
	pControl->ShowSpinButtons(TRUE);

	pControl = (CXTPControlEdit*)pGroup->Add(xtpControlEdit, ID_Terrain_BrushSize2);
	pControl->ShowSpinButtons(TRUE);

	//RibbonHome - GroupTerrainTexture
	pGroup = pTab->AddGroup(L"Layer Texture");
	pGroup->SetControlsCentering(TRUE);

	m_terrainTexGallery = CXTPControlGalleryItems::CreateItems(pCommandBars, IDC_Terrain_LayerTexture);

	CXTPControlGallery* pControlGallery = (CXTPControlGallery*)pGroup->Add(new CXTPControlGallery(), IDC_Terrain_LayerTexture);	
	pControlGallery->SetControlSize(CSize(250, 60));
	//pControlGallery->SetResizable();
	pControlGallery->SetItemsMargin(0, 1, 0, 1);
	pControlGallery->ShowLabels(FALSE);
	pControlGallery->ShowBorders(TRUE);
	pControlGallery->SetItems(m_terrainTexGallery);
	
	_CreateLayerTexIcon();

	return true;
}

void CMainFrame::_LoadIcon()
{
	CXTPCommandBars* pCommandBars = GetCommandBars();

	UINT uCommand = {IDB_GEAR};
	pCommandBars->GetImageManager()->SetIcons(IDB_GEAR, &uCommand, 1, CSize(0, 0), xtpImageNormal);

	UINT uiSystemMenu[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE }; 
	pCommandBars->GetImageManager()->SetIcons(IDB_SYSTEMMENULARGE, uiSystemMenu, _countof(uiSystemMenu), CSize(32, 32));

	UINT iconDeform[] = { IDC_Terrain_Deform };
	pCommandBars->GetImageManager()->SetIcons(IDB_Button, iconDeform, _countof(iconDeform), CSize(32, 32));

	UINT iconSplat[] = { IDC_Terrain_Splat };
	pCommandBars->GetImageManager()->SetIcons(IDB_Button, iconSplat, _countof(iconSplat), CSize(32, 32));
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
	m_paneManager.SetTheme(xtpPaneThemeVisualStudio2010);
	m_paneManager.SetClientMargin(6);
	m_paneManager.SetThemedFloatingFrames(TRUE);
	m_paneManager.SetAlphaDockingContext(TRUE);
	m_paneManager.SetShowDockingContextStickers(TRUE);
	m_paneManager.SetShowContentsWhileDragging(TRUE);
	m_paneManager.SetDefaultPaneOptions(xtpPaneHasMenuButton);

	CXTPDockingPane* paneResSelector = m_paneManager.CreatePane(IDR_Pane_ResourceSelector, CRect(0, 0, 250, 120), xtpPaneDockRight);
	m_paneManager.CreatePane(IDR_Pane_TerrainProperty, CRect(0, 0, 180, 140), xtpPaneDockRight, paneResSelector);
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

			case IDR_Pane_TerrainProperty:
				pPane->Attach(m_propertyTerrain);
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
	m_resourceSelector.GetAt(0)->SetExpanded(TRUE);
	//拖拽支持
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

	m_propertyTerrain->Create(L"STATIC", NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, 0);
	m_propertyTerrain->m_wndPropertyGrid.SetTheme(xtpGridThemeVisualStudio2010);

	_CreateDockPane();

	ManipulatorSystem.AddCallback(this);

	return true;
}

void CMainFrame::OnSceneNew()
{
	m_propertyTerrain->UpdateAllFromEngine();
	m_propertyTerrain->EnableMutableProperty(TRUE);
}

void CMainFrame::OnSceneOpen()
{
	m_propertyTerrain->UpdateAllFromEngine();
	m_propertyTerrain->EnableMutableProperty(TRUE);
}

void CMainFrame::OnSceneClose()
{
	m_propertyTerrain->EnableMutableProperty(FALSE);
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

void CMainFrame::OnUpdateUI_BtnTerrainDeform( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	bool bActive = ManipulatorSystem.GetTerrain().GetTerrainDeformEnabled();
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bActive);
}

void CMainFrame::OnBtnTerrainDeform()
{
	bool bEnable = !ManipulatorSystem.GetTerrain().GetTerrainDeformEnabled();
	eActionType action = bEnable ? eActionType_TerrainDeform : eActionType_None;
	ManipulatorAction::GetSingleton().SetActiveAction(action);
}

void CMainFrame::OnUpdateUI_BtnTerrainSplat( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	bool bActive = ManipulatorSystem.GetTerrain().GetTerrainSplatEnabled();
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bActive);
}

void CMainFrame::OnBtnTerrainSplat()
{
	bool bEnable = !ManipulatorSystem.GetTerrain().GetTerrainSplatEnabled();
	eActionType action = bEnable ? eActionType_TerrainSplat : eActionType_None;
	ManipulatorAction::GetSingleton().SetActiveAction(action);
}

int CMainFrame::OnCreateControl( LPCREATECONTROLSTRUCT lpCreateControl )
{
	return FALSE;
}

void CMainFrame::_CreateLayerTexIcon()
{
	const Ogre::StringVector& vecNames = ManipulatorSystem.GetTerrain().GetAllLayerTexThumbnailNames();
	Gdiplus::Bitmap bmFinal(vecNames.size()*50, 50);
	Gdiplus::Graphics g(&bmFinal);

	for (size_t i=0; i<vecNames.size(); ++i)
	{
		const std::wstring path = Utility::EngineToUnicode(vecNames[i]);
		Gdiplus::Bitmap bmTex(path.c_str());
		//每个纹理缩略图以64*64绘在图像上
		g.DrawImage(&bmTex, Gdiplus::Rect(i*50,0,50,50), 
			0, 0, bmTex.GetWidth(), bmTex.GetHeight(), Gdiplus::UnitPixel);
	}

	HBITMAP hBm;
	bmFinal.GetHBITMAP(Gdiplus::Color::Black, &hBm);
	//设置到UI上
	m_terrainTexGallery->GetImageManager()->SetIcons(*CBitmap::FromHandle(hBm), 0, 0, CSize(50, 50));
	m_terrainTexGallery->SetItemSize(CSize(55, 55));

	for (size_t i = 0; i<vecNames.size(); ++i)
		m_terrainTexGallery->AddItem(i, i);
}

void CMainFrame::OnUpdateUI_GalleryLayerTex( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
 	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnSelectLayerTex( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
	CXTPControlGallery* pGallery = DYNAMIC_DOWNCAST(CXTPControlGallery, tagNMCONTROL->pControl);
	if (pGallery)
	{
		ManipulatorTerrain& manTerrain = ManipulatorSystem.GetTerrain();
		CXTPControlGalleryItem* pItem = pGallery->GetItem(pGallery->GetSelectedItem());
		if (pItem)
		{
			const int nCurLayer = manTerrain.GetCurEditLayer();
			manTerrain.SetLayerTexture(nCurLayer, pItem->GetID());

			//更新property控件
			m_propertyTerrain->UpdateProperty(PropertyPaneTerrain::propLayerDiffuseMap0 + nCurLayer);
			m_propertyTerrain->UpdateProperty(PropertyPaneTerrain::propLayerNormalMap0 + nCurLayer);
		}
		*pResult = TRUE; // Handled
	}
}

void CMainFrame::OnUpdateUI_SplatSelectLayer( CCmdUI* pCmdUI )
{
	bool bEnable = (pCmdUI->m_nID - IDC_Terrain_Splat_Layer0) == ManipulatorSystem.GetTerrain().GetCurEditLayer();
	pCmdUI->SetCheck(bEnable);
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnSplatSelectLayer( UINT nID )
{
	ManipulatorSystem.GetTerrain().SetCurEditLayer(nID - IDC_Terrain_Splat_Layer0);
}
