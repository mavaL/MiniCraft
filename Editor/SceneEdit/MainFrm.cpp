String

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SceneEdit.h"
#include "MainFrm.h"
#include "View.h"
#include "EditorDefine.h"
#include "Utility.h"
#include "Manipulator/ManipulatorScene.h"
#include "Manipulator/ManipulatorAction.h"
#include "Action/ActionBase.h"
#include "UI/TerrainPropertyPane.h"
#include "UI/ObjectPropertyPane.h"
#include "UI/DialogGameDataBuilding.h"
#include "UI/EffectPropertyPane.h"
#include "UI/AttachmentPropertyPane.h"


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
	ON_COMMAND(IDC_NavMesh_Generate, OnNavMeshGenerate)
	ON_COMMAND(IDC_NavMesh_Show, OnNavMeshShow)
	ON_COMMAND(IDC_NavMesh_SaveObj, OnNavMeshSaveObj)
	ON_COMMAND(IDC_NavMesh_SaveNavMesh, OnNavMeshSaveNavMesh)
	ON_UPDATE_COMMAND_UI(IDC_NavMesh_Show, OnUpdateUI_NavMeshShow)
	ON_UPDATE_COMMAND_UI(IDC_NavMesh_Generate, OnUpdateUI_NavMeshGenerate)
	ON_UPDATE_COMMAND_UI(IDC_NavMesh_SaveObj, OnUpdateUI_NavMeshSaveObj)
	ON_UPDATE_COMMAND_UI(IDC_NavMesh_SaveNavMesh, OnUpdateUI_NavMeshSaveNavMesh)
	ON_COMMAND(IDC_Object_Move, OnObjectEdit<ManipulatorObject::eEditMode_Move>)
	ON_COMMAND(IDC_Object_Rotate, OnObjectEdit<ManipulatorObject::eEditMode_Rotate>)
	ON_COMMAND(IDC_Object_Scale, OnObjectEdit<ManipulatorObject::eEditMode_Scale>)
	ON_COMMAND(IDC_Object_Select, OnObjectEdit<ManipulatorObject::eEditMode_Select>)
	ON_UPDATE_COMMAND_UI(IDC_Object_Move, OnUpdateUI_ObjectEdit<ManipulatorObject::eEditMode_Move>)
	ON_UPDATE_COMMAND_UI(IDC_Object_Rotate, OnUpdateUI_ObjectEdit<ManipulatorObject::eEditMode_Rotate>)
	ON_UPDATE_COMMAND_UI(IDC_Object_Scale, OnUpdateUI_ObjectEdit<ManipulatorObject::eEditMode_Scale>)
	ON_UPDATE_COMMAND_UI(IDC_Object_Select, OnUpdateUI_ObjectEdit<ManipulatorObject::eEditMode_Select>)
	ON_UPDATE_COMMAND_UI(IDC_Animation_Names, OnUpdateUI_AnimNames)
	ON_NOTIFY(CBN_SELCHANGE, IDC_Animation_Names, OnAnimSelectChange)
	ON_COMMAND(IDC_GameData_Building, OnDataBuilding)
	ON_UPDATE_COMMAND_UI(IDC_GameData_Building, OnUpdateUI_DataBuilding)
	ON_COMMAND(IDC_Effect_Shadow, OnShadowOnOff)
	ON_UPDATE_COMMAND_UI(IDC_Effect_Shadow, OnUpdateUI_ShadowOnOff)
	ON_COMMAND(IDC_Effect_SSAO, OnSSAOOnOff)
	ON_UPDATE_COMMAND_UI(IDC_Effect_SSAO, OnUpdateUI_SSAOOnOff)
	ON_COMMAND(IDC_Animation_Play, OnAnimPlay)
	ON_COMMAND(IDC_Animation_Stop, OnAnimStop)
	ON_UPDATE_COMMAND_UI(IDC_Animation_Play, OnUpdateUI_AnimPlay)
	ON_UPDATE_COMMAND_UI(IDC_Animation_Stop, OnUpdateUI_AnimStop)
	ON_COMMAND(IDC_Animation_EffectAdd, OnAnimEffectAdd)
	ON_COMMAND(IDC_Animation_EffectRemove, OnAnimEffectRemove)
	ON_UPDATE_COMMAND_UI(IDC_Animation_EffectAdd, OnUpdateUI_AnimEffectAdd)
	ON_UPDATE_COMMAND_UI(IDC_Animation_EffectRemove, OnUpdateUI_AnimEffectRemove)
	ON_UPDATE_COMMAND_UI(IDC_Animation_EffectList, OnUpdateUI_AnimEffectList)
	ON_NOTIFY(CBN_SELCHANGE, IDC_Animation_EffectList, OnAnimEffectSelectChange)
	ON_UPDATE_COMMAND_UI(IDC_Object_Remove, OnUpdateUI_ObjectRemove)
	ON_COMMAND(IDC_Object_Remove, OnObjectRemove)
	ON_UPDATE_COMMAND_UI(IDC_Effect_Sharpen, OnUpdateUI_SharpenOnOff)
	ON_UPDATE_COMMAND_UI(IDC_Effect_FXAA, OnUpdateUI_FXAAOnOff)
	ON_COMMAND(IDC_Effect_Sharpen, OnSharpenOnOff)
	ON_COMMAND(IDC_Effect_FXAA, OnFXAAOnOff)

END_MESSAGE_MAP()


CMainFrame::CMainFrame()
:m_wndView(NULL)
,m_propertyTerrain(new PropertyPaneTerrain)
,m_terrainTexGallery(NULL)
,m_paneResSelector(NULL)
,m_paneTerrain(NULL)
,m_propertyObject(new PropertyPaneObject)
,m_propertyEffect(new PropertyPaneEffect)
,m_propertyAttachment(new PropertyPaneAttachment)
,m_paneObject(NULL)
,m_paneEffect(NULL)
,m_paneAttachment(NULL)
,m_animTab(NULL)
,m_animList(NULL)
,m_effectList(NULL)
{
}

CMainFrame::~CMainFrame()
{
	SAFE_DELETE(m_propertyTerrain);
	SAFE_DELETE(m_wndView);
	SAFE_DELETE(m_propertyObject);
	SAFE_DELETE(m_propertyEffect);
	SAFE_DELETE(m_propertyAttachment);
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
	ManipulatorSystem.GetObject().RemoveCallback(this);
	ManipulatorScene::GetSingleton().RemoveCallback(this);

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

	//RibbonHome - GroupObject
	CXTPRibbonGroup* pGroup = pTab->AddGroup(L"Object");
	//RibbonHome - GroupObject - Select
	pGroup->Add(xtpControlButton, IDC_Object_Select);
	//RibbonHome - GroupObject - Move
	pGroup->Add(xtpControlButton, IDC_Object_Move);
	//RibbonHome - GroupObject - Rotate
	pGroup->Add(xtpControlButton, IDC_Object_Rotate);
	//RibbonHome - GroupObject - Scale
	pGroup->Add(xtpControlButton, IDC_Object_Scale);
	//RibbonHome - GroupObject - Rmove
	pGroup->Add(xtpControlButton, IDC_Object_Remove);

	///RibbonHome - GroupTerrainModify
	pGroup = pTab->AddGroup(L"Terrain Modify");

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

	//RibbonHome - GroupNavMesh
	pGroup = pTab->AddGroup(L"Navigation Mesh");
	//RibbonHome - GroupNavMesh - Generate
	pGroup->Add(xtpControlButton, IDC_NavMesh_Generate);
	//RibbonHome - GroupNavMesh - Show
	pGroup->Add(xtpControlButton, IDC_NavMesh_Show);
	//RibbonHome - GroupNavMesh - Save
	pGroup->Add(xtpControlButton, IDC_NavMesh_SaveObj);
	pGroup->Add(xtpControlButton, IDC_NavMesh_SaveNavMesh);

	///RibbonGameData
	pTab = pRibbonBar->AddTab(L"Game Data");

	//RibbonGameData - GroupData
	pGroup = pTab->AddGroup(L"Data");
	//RibbonGameData - GroupData - Building
	pGroup->Add(xtpControlButton, IDC_GameData_Building);

	///RibbonEffect
	pTab = pRibbonBar->AddTab(L"Effect");

	//RibbonEffect - GroupSwitch
	pGroup = pTab->AddGroup(L"Switch");
	//RibbonEffect - GroupSwitch - Shadow
	pGroup->Add(xtpControlButton, IDC_Effect_Shadow);
	//RibbonEffect - GroupSwitch - SSAO
	pGroup->Add(xtpControlButton, IDC_Effect_SSAO);
	//RibbonEffect - GroupSwitch - Sharpen
	pGroup->Add(xtpControlButton, IDC_Effect_Sharpen);
	//RibbonEffect - GroupSwitch - FXAA
	pGroup->Add(xtpControlButton, IDC_Effect_FXAA);

	///RibbonAnimation
	m_animTab = pRibbonBar->InsertTab(-1, L"Animation");
	m_animTab->SetContextTab(xtpRibbonTabContextColorRed, L"Animation");
	//选中了带动画物体才显示
	m_animTab->SetVisible(FALSE);

	//RibbonAnimation - GroupAnim
	pGroup = m_animTab->AddGroup(L"Anim");

	//RibbonAnimation - GroupAnim - AnimList
	m_animList = dynamic_cast<CXTPControlComboBox*>(pGroup->Add(xtpControlComboBox, IDC_Animation_Names));
	m_animList->SetDropDownListStyle();
	m_animList->SetWidth(150);

	//RibbonAnimation - GroupAnim - Play
	pGroup->Add(xtpControlButton, IDC_Animation_Play);

	//RibbonAnimation - GroupAnim - Stop
	pGroup->Add(xtpControlButton, IDC_Animation_Stop);

	//RibbonAnimation - GroupEffect
	pGroup = m_animTab->AddGroup(L"Effect");

	//RibbonAnimation - GroupEffect - EffectList
	m_effectList = dynamic_cast<CXTPControlComboBox*>(pGroup->Add(xtpControlComboBox, IDC_Animation_EffectList));
	m_effectList->SetDropDownListStyle();
	m_effectList->SetWidth(150);

	//RibbonAnimation - GroupEffect - EffectAdd
	pGroup->Add(xtpControlButton, IDC_Animation_EffectAdd);

	//RibbonAnimation - GroupEffect - EffectRemove
	pGroup->Add(xtpControlButton, IDC_Animation_EffectRemove);
	
	return true;
}

void CMainFrame::_LoadIcon()
{
	CXTPImageManager* pImageMgr = GetCommandBars()->GetImageManager();

	UINT uiSystemMenu[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE };	
	pImageMgr->SetIcons(IDB_SYSTEMMENULARGE, uiSystemMenu, _countof(uiSystemMenu), CSize(32, 32));

	UINT uCommand = {IDB_GEAR};				pImageMgr->SetIcons(IDB_GEAR, &uCommand, 1, CSize(0, 0), xtpImageNormal);
	UINT icon[1] = { IDC_Terrain_Deform };	pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Terrain_Splat;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_NavMesh_Generate;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_NavMesh_Show;				pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_NavMesh_SaveObj;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_NavMesh_SaveNavMesh;		pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Object_Move;				pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Object_Rotate;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Object_Scale;				pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Object_Select;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Object_Remove;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_GameData_Building;		pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Effect_Shadow;				pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Effect_SSAO;				pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Effect_Sharpen;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Effect_FXAA;				pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Animation_Play;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
	icon[0] = IDC_Animation_Stop;			pImageMgr->SetIcons(IDB_Button, icon, _countof(icon), CSize(32, 32));
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

	//之所以放在这里是因为不能随便放
	static bool bInitLayout = false;
	if (!bInitLayout)
	{
		m_paneManager.AttachPane(m_paneTerrain, m_paneResSelector);
		m_paneManager.AttachPane(m_paneObject, m_paneResSelector);
		m_paneManager.AttachPane(m_paneEffect, m_paneResSelector);
		m_paneManager.AttachPane(m_paneAttachment, m_paneResSelector);
		bInitLayout = true;
	}

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
	m_paneManager.SetDefaultPaneOptions(xtpPaneNoHideable);

	m_paneResSelector	= m_paneManager.CreatePane(IDR_Pane_ResourceSelector, CRect(0, 0, 250, 120), xtpPaneDockRight);
	m_paneTerrain		= m_paneManager.CreatePane(IDR_Pane_TerrainProperty, CRect(0, 0, 250, 120), xtpPaneDockRight);
	m_paneObject		= m_paneManager.CreatePane(IDR_Pane_ObjectProperty, CRect(0, 0, 250, 120), xtpPaneDockRight);
	m_paneEffect		= m_paneManager.CreatePane(IDR_Pane_EffectProperty, CRect(0, 0, 250, 120), xtpPaneDockRight);
	m_paneAttachment	= m_paneManager.CreatePane(IDR_Pane_AttachmentProperty, CRect(0, 0, 250, 120), xtpPaneDockRight);
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

			case IDR_Pane_ObjectProperty:
				pPane->Attach(m_propertyObject);
				break;

			case IDR_Pane_EffectProperty:
				pPane->Attach(m_propertyEffect);
				break;

			case IDR_Pane_AttachmentProperty:
				pPane->Attach(m_propertyAttachment);
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
	ManipulatorSystem.GetResource().RenderAllMeshIcons(iconList, meshNames);

	if(!_CreateMeshPanel(iconList, meshNames))
		return FALSE;

	m_propertyTerrain->Create(L"STATIC", NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, IDR_Pane_TerrainProperty);
	m_propertyObject->Create(L"STATIC", NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, IDR_Pane_ObjectProperty);
	m_propertyEffect->Create(L"STATIC", NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, IDR_Pane_EffectProperty);
	m_propertyAttachment->Create(L"STATIC", NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CXTPEmptyRect(), this, IDR_Pane_AttachmentProperty);

	m_propertyTerrain->m_wndPropertyGrid.SetTheme(xtpGridThemeVisualStudio2010);
	m_propertyObject->m_wndPropertyGrid.SetTheme(xtpGridThemeVisualStudio2010);
	m_propertyEffect->m_wndPropertyGrid.SetTheme(xtpGridThemeVisualStudio2010);
	m_propertyAttachment->m_wndPropertyGrid.SetTheme(xtpGridThemeVisualStudio2010);

	_CreateDockPane();

	ManipulatorSystem.GetResource().PrepareAllIcons();

	ManipulatorScene::GetSingleton().AddCallback(this);
	ManipulatorSystem.GetObject().AddCallback(this);

	return true;
}

void CMainFrame::OnSceneNew()
{
	m_propertyTerrain->UpdateAllFromEngine();
	m_propertyEffect->UpdateAllFromEngine();
	m_propertyTerrain->EnableMutableProperty(TRUE);
	m_propertyEffect->EnableMutableProperty(TRUE);
}

void CMainFrame::OnSceneOpen()
{
	m_propertyTerrain->UpdateAllFromEngine();
	m_propertyEffect->UpdateAllFromEngine();
	m_propertyTerrain->EnableMutableProperty(TRUE);
	m_propertyEffect->EnableMutableProperty(TRUE);
}

void CMainFrame::OnSceneClose()
{
	m_propertyTerrain->EnableMutableProperty(FALSE);
	m_propertyObject->EnableMutableProperty(FALSE);
	m_propertyEffect->EnableMutableProperty(FALSE);
	m_propertyAttachment->EnableMutableProperty(FALSE);
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
 	pCmdUI->Enable(ManipulatorSystem.GetIsSceneReady());
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

void CMainFrame::OnNavMeshGenerate()
{
	ManipulatorSystem.GetNavMesh().Generate();
}

void CMainFrame::OnNavMeshShow()
{
	bool bShow = !ManipulatorSystem.GetNavMesh().GetIsShowNavMesh();
	ManipulatorSystem.GetNavMesh().ShowNavMesh(bShow);
}

void CMainFrame::OnUpdateUI_NavMeshShow( CCmdUI* pCmdUI )
{
	if (!ManipulatorSystem.GetNavMesh().HasGenerate())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->SetCheck(ManipulatorSystem.GetNavMesh().GetIsShowNavMesh());
}

void CMainFrame::OnUpdateUI_NavMeshGenerate( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(ManipulatorSystem.GetIsSceneReady());
}

void CMainFrame::OnNavMeshSaveObj()
{
	ManipulatorSystem.GetNavMesh().SaveObj();
}

void CMainFrame::OnUpdateUI_NavMeshSaveObj( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(ManipulatorSystem.GetNavMesh().HasGenerate());
}

void CMainFrame::OnNavMeshSaveNavMesh()
{
	ManipulatorSystem.GetNavMesh().SaveNavMesh();
}

void CMainFrame::OnUpdateUI_NavMeshSaveNavMesh( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(ManipulatorSystem.GetNavMesh().HasGenerate());
}

template<int mode>
void CMainFrame::OnUpdateUI_ObjectEdit( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	if(mode == ManipulatorObject::eEditMode_Select)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(ManipulatorSystem.GetObject().GetSelection() != nullptr);

	bool bActive = ManipulatorSystem.GetObject().GetCurEditMode() == mode;
	pCmdUI->SetCheck(bActive);
}

template<int mode>
void CMainFrame::OnObjectEdit()
{
	bool bActive = ManipulatorSystem.GetObject().GetCurEditMode() == mode;
	bActive = !bActive;

	eActionType action;
	if(mode == ManipulatorObject::eEditMode_Select)
		action = bActive ? eActionType_ObjectSelect : eActionType_None;
	else 
		action = bActive ? eActionType_ObjectEdit : eActionType_None;

	ManipulatorSystem.GetObject().SetCurEditMode((ManipulatorObject::eEditMode)mode);
	ManipulatorAction::GetSingleton().SetActiveAction(action);
}

void CMainFrame::OnObjectPropertyChanged( Ogre::Entity* pEntity )
{
	Ogre::Entity* curSel = ManipulatorSystem.GetObject().GetSelection();
	if(!curSel)
	{
		m_propertyObject->EnableMutableProperty(FALSE);
	}
	else if(curSel == pEntity)
	{
		m_propertyObject->UpdateAllFromEngine();
		m_propertyObject->EnableMutableProperty(TRUE);
	}	
}

void CMainFrame::OnObjectSetSelection( Ogre::Entity* pObject )
{
	assert(pObject);
	m_propertyObject->UpdateAllFromEngine();
	m_propertyObject->EnableMutableProperty(TRUE);

	//如果带骨骼动画,则显示动画Ribbon
	if (pObject->hasSkeleton())
	{
		m_animTab->Select();
		m_animTab->SetVisible(TRUE);

		ManipulatorObject& manObject = ManipulatorSystem.GetObject();
		ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();
		const auto vecNames = manEffect.GetAnimationNames();
		//动画列表
		m_animList->ResetContent();
		for(size_t i=0; i<vecNames.size(); ++i)
			m_animList->AddString(vecNames[i].c_str());
		m_animList->SetCurSel(0);
	}
}

void CMainFrame::OnObjectClearSelection( Ogre::Entity* pObject )
{
	assert(pObject);
	m_propertyObject->UpdateAllFromEngine();
	m_propertyObject->EnableMutableProperty(FALSE);

	if (pObject->getSkeleton())
	{
		//隐藏动画Ribbon
		m_animTab->SetVisible(FALSE);

		//隐藏挂接物编辑面板
		m_propertyAttachment->EnableMutableProperty(FALSE);

		ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();
		manEffect.PlayAnimation(pObject, m_animList->GetCurSel(), false);
		manEffect.OnAnimSelectChange("");
	}
}

void CMainFrame::OnUpdateUI_AnimNames( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnAnimSelectChange( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
	CXTPControlComboBox* pControl = DYNAMIC_DOWNCAST(CXTPControlComboBox, tagNMCONTROL->pControl);
	ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();

	CString animName;
	int curSel = m_animList->GetCurSel();
	m_animList->GetLBText(curSel, animName);
	const std::string anim = Utility::UnicodeToEngine(animName);
	manEffect.OnAnimSelectChange(anim);

	//挂接特效列表
	const auto effectNames = manEffect.GetAttachEffectNames();
	m_effectList->ResetContent();
	m_effectList->AddString(L"");
	for(size_t i=0; i<effectNames.size(); ++i)
		m_effectList->AddString(effectNames[i].c_str());
	m_effectList->SetCurSel(0);

	manEffect.OnAttachEffectSelChange("");
	m_propertyAttachment->EnableMutableProperty(FALSE);
}

void CMainFrame::OnUpdateUI_DataBuilding( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_dlgVisibleFlags[IDD_DlgGameDataBuilding]);
}

void CMainFrame::OnDataBuilding()
{
	static bool bInit = false;
	static DialogGameDataBuilding s_Dlg;
	if (!bInit)
	{
		s_Dlg.Create(IDD_DlgGameDataBuilding);
		s_Dlg.ShowWindow(SW_HIDE);

		//对话框可见标志存入容器
		m_dlgVisibleFlags.insert(std::make_pair(IDD_DlgGameDataBuilding, false));

		bInit = true;
	}
	
	bool bCurVisible = !m_dlgVisibleFlags[IDD_DlgGameDataBuilding];
	s_Dlg.ShowWindow(bCurVisible ? SW_SHOW : SW_HIDE);
	s_Dlg.UpdateWindow();
	m_dlgVisibleFlags[IDD_DlgGameDataBuilding] = bCurVisible;
}

void CMainFrame::OnUpdateUI_ShadowOnOff( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(ManipulatorSystem.GetEffect().GetShadowEnable());
}

void CMainFrame::OnShadowOnOff()
{
	bool bEnable = !ManipulatorSystem.GetEffect().GetShadowEnable();
	ManipulatorSystem.GetEffect().SetShadowEnable(bEnable);
}

void CMainFrame::OnUpdateUI_SSAOOnOff( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(ManipulatorSystem.GetEffect().GetSSAOEnable());
}

void CMainFrame::OnSSAOOnOff()
{
	bool bEnable = !ManipulatorSystem.GetEffect().GetSSAOEnable();
	ManipulatorSystem.GetEffect().SetSSAOEnable(bEnable);
}

void CMainFrame::OnUpdateUI_AnimPlay( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_animTab->IsVisible());
	pCmdUI->SetCheck(ManipulatorSystem.GetEffect().GetIsPlayingAnim());
}

void CMainFrame::OnUpdateUI_AnimStop( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_animTab->IsVisible());
	pCmdUI->SetCheck(!ManipulatorSystem.GetEffect().GetIsPlayingAnim());
}

void CMainFrame::OnAnimPlay()
{
	Ogre::Entity* ent = ManipulatorSystem.GetObject().GetSelection();
	ManipulatorSystem.GetEffect().PlayAnimation(ent, m_animList->GetCurSel(), true);
}

void CMainFrame::OnAnimStop()
{
	Ogre::Entity* ent = ManipulatorSystem.GetObject().GetSelection();
	ManipulatorSystem.GetEffect().PlayAnimation(ent, m_animList->GetCurSel(), false);
}

void CMainFrame::OnUpdateUI_AnimEffectAdd( CCmdUI* pCmdUI )
{
	Ogre::Entity* curSel = ManipulatorSystem.GetObject().GetSelection();
	if (curSel && curSel->hasSkeleton())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdateUI_AnimEffectRemove( CCmdUI* pCmdUI )
{
	if(m_effectList->GetCurSel() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnAnimEffectAdd()
{
	const std::wstring name = ManipulatorSystem.GetEffect().AddEffect();
	int index = m_effectList->AddString(name.c_str());
	assert(index != LB_ERR);
	m_effectList->SetCurSel(index);

	m_propertyAttachment->UpdateAllFromEngine();
	m_propertyAttachment->EnableMutableProperty(TRUE);
	m_paneAttachment->Select();
}

void CMainFrame::OnAnimEffectRemove()
{
	CString effectName;
	int curSel = m_effectList->GetCurSel();
	m_effectList->GetLBText(curSel, effectName);
	m_effectList->SetCurSel(0);
	m_propertyAttachment->EnableMutableProperty(FALSE);

	ManipulatorSystem.GetEffect().RemoveEffect(Utility::UnicodeToEngine(effectName));
}

void CMainFrame::OnUpdateUI_AnimEffectList( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnAnimEffectSelectChange( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMXTPCONTROL* tagNMCONTROL = (NMXTPCONTROL*)pNMHDR;
	CXTPControlComboBox* pControl = DYNAMIC_DOWNCAST(CXTPControlComboBox, tagNMCONTROL->pControl);

	CString effectName;
	int curSel = m_effectList->GetCurSel();
	m_effectList->GetLBText(curSel, effectName);

	ManipulatorSystem.GetEffect().OnAttachEffectSelChange(Utility::UnicodeToEngine(effectName));
	
	//显示挂接物编辑面板
	if (curSel != 0)
	{
		m_propertyAttachment->EnableMutableProperty(TRUE);
		m_paneAttachment->Select();
	}

	m_propertyAttachment->UpdateAllFromEngine();
}

void CMainFrame::OnUpdateUI_ObjectRemove( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(ManipulatorSystem.GetObject().GetSelection() != nullptr);
}

void CMainFrame::OnObjectRemove()
{
	ManipulatorObject& manObject = ManipulatorSystem.GetObject();
	manObject.RemoveEntity(manObject.GetSelection());
}

void CMainFrame::OnUpdateUI_SharpenOnOff( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(ManipulatorSystem.GetEffect().GetSharpenEnable());
}

void CMainFrame::OnUpdateUI_FXAAOnOff( CCmdUI* pCmdUI )
{
	if(!ManipulatorSystem.GetIsSceneReady())
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(ManipulatorSystem.GetEffect().GetFXAAEnable());
}

void CMainFrame::OnSharpenOnOff()
{
	bool bEnable = !ManipulatorSystem.GetEffect().GetSharpenEnable();
	ManipulatorSystem.GetEffect().SetSharpenEnable(bEnable);
}

void CMainFrame::OnFXAAOnOff()
{
	bool bEnable = !ManipulatorSystem.GetEffect().GetFXAAEnable();
	ManipulatorSystem.GetEffect().SetFXAAEnable(bEnable);
}
