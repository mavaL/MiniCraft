#include "stdafx.h"
#include "BehaviorTreeEditorExplorer.h"
#include "resource.h"
#include "Manipulator/ManipulatorScene.h"

BEGIN_MESSAGE_MAP(BehaviorTreeEditorExplorer, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_BTEditor_List, OnDoubleClick)
END_MESSAGE_MAP() 

BehaviorTreeEditorExplorer::BehaviorTreeEditorExplorer( CXTPDialog* parent )
:CDialog()
,m_parent(parent)
{
	Create(IDD_DlgBTEditor_Explorer, parent);
}

BOOL BehaviorTreeEditorExplorer::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

 	m_hList = ::GetDlgItem(m_hWnd, IDC_BTEditor_List);
	int style = LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT;
	ListView_SetExtendedListViewStyleEx(m_hList, style, style);
	SetWindowTheme(m_hList, L"Explorer", nullptr);

 	LVCOLUMN col;
	col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	col.cx = 150;
	col.iSubItem = 0;
	col.pszText = L"Template";
	ListView_InsertColumn(m_hList,0, &col);

	LVITEM vitem;  
	vitem.mask = LVIF_TEXT;
	auto names = ManipulatorSystem.GetGameData().GetAllBehaviorTreeTemplateNames();
	for (size_t i=0; i<names.size(); ++i)
	{
		vitem.pszText = (LPWSTR)names[i].c_str();  
		vitem.iItem = i; 
		vitem.iSubItem = 0;
		ListView_InsertItem(m_hList, &vitem);
	}

	return TRUE;
}

void BehaviorTreeEditorExplorer::OnSize( UINT nType, int cx, int cy )
{
	CRect rcClient;
	GetClientRect(rcClient);

	HDWP hdwp = BeginDeferWindowPos(1);
	DeferWindowPos(hdwp, m_hList, nullptr, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER);
	EndDeferWindowPos(hdwp);
}

void BehaviorTreeEditorExplorer::OnDoubleClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		int i = 0;
	}
	*pResult = 0;
}

