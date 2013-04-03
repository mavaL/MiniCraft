#include "stdafx.h"
#include "BehaviorTreeEditorProperty.h"

BEGIN_MESSAGE_MAP(BehaviorTreeEditorProperty, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP() 

BehaviorTreeEditorProperty::BehaviorTreeEditorProperty( CXTPDialog* parent )
:CDialog()
,m_parent(parent)
{
	Create(IDD_DlgChildEmpty, parent);
}

void BehaviorTreeEditorProperty::OnSize( UINT nType, int cx, int cy )
{
	CRect rcClient;
	GetClientRect(rcClient);

	HDWP hdwp = BeginDeferWindowPos(1);
	DeferWindowPos(hdwp, m_propertyBT.GetSafeHwnd(), nullptr, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER);
	EndDeferWindowPos(hdwp);
}

BOOL BehaviorTreeEditorProperty::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;

	if(!m_propertyBT.Create(L"STATIC", NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
		CXTPEmptyRect(), this, IDR_Pane_BT_Property))
		return FALSE;
	m_propertyBT.m_wndPropertyGrid.SetTheme(xtpGridThemeVisualStudio2010);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////
bool PropertyPaneBehaviorTree::_OnCreate()
{
	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"General");

	return true;
}
