#include "stdafx.h"
#include "BehaviorTreeEditorProperty.h"
#include "BehaviorTreeEditorDlg.h"
#include "Utility.h"
#include "BehaviorTreeEditorView.h"
#include "Manipulator/ManipulatorScene.h"

BEGIN_MESSAGE_MAP(BehaviorTreeEditorProperty, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP() 

BehaviorTreeEditorProperty::BehaviorTreeEditorProperty( CXTPDialog* parent )
:CDialog()
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

void BehaviorTreeEditorProperty::OnNodeSelected( bool bSelected, int id /*= -1*/ )
{
	if (bSelected)
	{
		ManipulatorGameData::BTTemplate* pTmpl = m_pView->GetActiveTemplate();
		assert(pTmpl);

		auto iter = std::find_if(pTmpl->m_nodeList.begin(), pTmpl->m_nodeList.end(), 
			[&](const ManipulatorGameData::BTTemplate::SBTNode* pNode)
		{
			return pNode->flowGraphNodeID == id;
		});

		assert(iter != pTmpl->m_nodeList.end());
		m_propertyBT.SetCurNode(*iter);
	}
	else
	{
		m_propertyBT.SetCurNode(NULL);
	}
}

void BehaviorTreeEditorProperty::SetView( BehaviorTreeEditorView* pView )
{
	m_pView = pView; 
	m_propertyBT.SetView(m_pView);
}

/////////////////////////////////////////////////////////////////////
PropertyPaneBehaviorTree::PropertyPaneBehaviorTree()
:CPropertiesPane()
,m_curNode(nullptr)
,m_pView(nullptr)
{

}

bool PropertyPaneBehaviorTree::_OnCreate()
{
	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"SequenceNode");
	pCategory->Expand();
	pCategory->SetHidden(TRUE);

	pCategory = m_wndPropertyGrid.AddCategory(L"ConditionNode");
	PROPERTY_REG(pCategory,	, L"Expression"		, 			L"",	propConditon);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);

	pCategory = m_wndPropertyGrid.AddCategory(L"ActionNode");
	PROPERTY_REG(pCategory,	, L"Action"		, 			L"",	propAction);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);

	//BehaviorÁÐ±í
	auto& behaviors = ManipulatorSystem.GetGameData().GetAllBehaviorNames();
	CXTPPropertyGridItemConstraints* pList = m_mapItem[propAction]->GetConstraints();
	for (size_t i=0; i<behaviors.size(); ++i)
		pList->AddConstraint(behaviors[i].c_str());

	m_mapItem[propAction]->SetFlags(xtpGridItemHasComboButton);

	EnableMutableProperty(TRUE);

	return true;
}

void PropertyPaneBehaviorTree::_SetProperty( int nID )
{
	assert(m_curNode);
	m_curNode->txtProperty = m_mapItem[nID]->GetValue();
	m_pView->Refresh();
}

void PropertyPaneBehaviorTree::_UpdateProperty( int nID )
{
	assert(m_curNode);
	m_mapItem[nID]->SetValue(m_curNode->txtProperty.c_str());
}

void PropertyPaneBehaviorTree::SetCurNode( ManipulatorGameData::BTTemplate::SBTNode* node )
{
	m_curNode = node;
	auto categories = m_wndPropertyGrid.GetCategories();
	int count = categories->GetCount();
	for(int i=0; i<count; ++i)
		categories->GetAt(i)->SetHidden(TRUE);

	if (m_curNode)
	{
		int category = -1;
		if(m_curNode->type == L"Sequence")
			category = 0;
		else if(m_curNode->type == L"Condition")
			category = 1;
		else if(m_curNode->type == L"Action")
			category = 2;

		categories->GetAt(category)->SetHidden(FALSE);
		UpdateAllFromEngine();
	}
}
