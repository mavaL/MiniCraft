#include "stdafx.h"
#include "BehaviorTreeEditorProperty.h"
#include "BehaviorTreeEditorDlg.h"
#include "Utility.h"
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

void BehaviorTreeEditorProperty::OnNodeSelected( eFlowGraphNodeType nodeType, int id /*= -1*/ )
{
	m_propertyBT.OnNodeSelected(nodeType, id);
}

void BehaviorTreeEditorProperty::SetView( BehaviorTreeEditorView* pView )
{
	m_propertyBT.SetView(pView);
}

/////////////////////////////////////////////////////////////////////
PropertyPaneBehaviorTree::PropertyPaneBehaviorTree()
:CPropertiesPane()
,m_curNode(nullptr)
,m_pView(nullptr)
,m_curNodeType(eFlowGraphNodeType_None)
,m_curBBParamIndex(-1)
{

}

bool PropertyPaneBehaviorTree::_OnCreate()
{
	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"SequenceNode");
	pCategory->Expand();
	pCategory->SetHidden(TRUE);
	pCategory->SetID(eCategory_SequenceNode);

	pCategory = m_wndPropertyGrid.AddCategory(L"ConditionNode");
	PROPERTY_REG(pCategory,	, L"Expression"		, 			L"",	propConditon);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);
	pCategory->SetID(eCategory_ConditionNode);

	pCategory = m_wndPropertyGrid.AddCategory(L"ActionNode");
	PROPERTY_REG(pCategory,	, L"Action"		, 			L"",	propAction);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);
	pCategory->SetID(eCategory_ActionNode);

	//Behavior列表
	auto& behaviors = ManipulatorSystem.GetGameData().GetAllBehaviorNames();
	CXTPPropertyGridItemConstraints* pList = m_mapItem[propAction]->GetConstraints();
	for (size_t i=0; i<behaviors.size(); ++i)
		pList->AddConstraint(behaviors[i].c_str());

	m_mapItem[propAction]->SetFlags(xtpGridItemHasComboButton);

	pCategory = m_wndPropertyGrid.AddCategory(L"Blackboard");
	PROPERTY_REG(pCategory,	, L"Name"		, 			L"",	propBBParamName);
	PROPERTY_REG(pCategory,	, L"Value"		, 			L"",	propBBParamValue);
	PROPERTY_REG(pCategory,	, L"Type"		, 			L"",	propBBParamType);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);
	pCategory->SetID(eCategory_Blackboard);

	//黑板参数类型列表
	pList = m_mapItem[propBBParamType]->GetConstraints();
	pList->AddConstraint(L"Int");
	pList->AddConstraint(L"Float");
	pList->AddConstraint(L"Bool");
	m_mapItem[propBBParamType]->SetFlags(xtpGridItemHasComboButton);

	EnableMutableProperty(TRUE);

	return true;
}

void PropertyPaneBehaviorTree::_SetProperty( int nID )
{
	ManipulatorGameData& manGameData = ManipulatorSystem.GetGameData();

	switch (nID)
	{
	case propAction:
	case propConditon: m_curNode->txtProperty = m_mapItem[nID]->GetValue(); break;

	case propBBParamName:
		{
			assert(m_curBBParamIndex != -1);
			const std::wstring oldName = _GetCurBB()->at(m_curBBParamIndex);
			const std::wstring newName = m_mapItem[nID]->GetValue();
			manGameData.RenameBlackboardParam(oldName, newName, *m_pView->GetActiveTemplate(), m_curNodeType == eFlowGraphNodeType_OwnBlackboard);
		}
		break;

	case propBBParamValue:
		{
			const std::wstring& paramName = _GetCurBB()->at(m_curBBParamIndex);
			bool bOwnBB = m_curNodeType == eFlowGraphNodeType_OwnBlackboard;
			auto param = manGameData.GetBlackboardParam(paramName, *m_pView->GetActiveTemplate(), bOwnBB);
			param.value = m_mapItem[nID]->GetValue();

			manGameData.SetBlackboardParam(paramName, param, *m_pView->GetActiveTemplate(), bOwnBB);
		}
		break;

	case propBBParamType:
		{
			const std::wstring& paramName = _GetCurBB()->at(m_curBBParamIndex);
			bool bOwnBB = m_curNodeType == eFlowGraphNodeType_OwnBlackboard;
			auto param = manGameData.GetBlackboardParam(paramName, *m_pView->GetActiveTemplate(), bOwnBB);
			param.type = m_mapItem[nID]->GetValue();

			manGameData.SetBlackboardParam(paramName, param, *m_pView->GetActiveTemplate(), bOwnBB);
		}
		break;

	default: assert(0);
	}
	
	m_pView->Refresh();
}

void PropertyPaneBehaviorTree::_UpdateProperty( int nID )
{
	ManipulatorGameData& manGameData = ManipulatorSystem.GetGameData();

	switch (nID)
	{
	case propAction:
	case propConditon: m_mapItem[nID]->SetValue(m_curNode->txtProperty.c_str()); break;
	case propBBParamName:
		{
			assert(m_curBBParamIndex != -1);
			m_mapItem[nID]->SetValue(_GetCurBB()->at(m_curBBParamIndex).c_str());
		}
		break;

	case propBBParamValue:
		{
			assert(m_curBBParamIndex != -1);
			const std::wstring& name = _GetCurBB()->at(m_curBBParamIndex);
			auto param = manGameData.GetBlackboardParam(name, *m_pView->GetActiveTemplate(), m_curNodeType == eFlowGraphNodeType_OwnBlackboard);
			m_mapItem[nID]->SetValue(param.value.c_str());
		}
		break;

	case propBBParamType:
		{
			assert(m_curBBParamIndex != -1);
			const std::wstring& name = _GetCurBB()->at(m_curBBParamIndex);
			auto param = manGameData.GetBlackboardParam(name, *m_pView->GetActiveTemplate(), m_curNodeType == eFlowGraphNodeType_OwnBlackboard);
			m_mapItem[nID]->SetValue(param.type.c_str());
		}
		break;

	default: assert(0);
	}
}

void PropertyPaneBehaviorTree::OnNodeSelected( eFlowGraphNodeType nodeType, int id /*= -1*/ )
{
	m_curNode = nullptr;
	m_curNodeType = nodeType;
	m_curBBParamIndex = -1;

	auto categories = m_wndPropertyGrid.GetCategories();
	int count = categories->GetCount();
	for(int i=0; i<count; ++i)
		categories->GetAt(i)->SetHidden(TRUE);

	ManipulatorGameData::BTTemplate* pTmpl = m_pView->GetActiveTemplate();
	if(!pTmpl)
		return;

	switch (nodeType)
	{
	case eFlowGraphNodeType_TreeNode:
		{
			auto iter = std::find_if(pTmpl->m_nodeList.begin(), pTmpl->m_nodeList.end(), 
				[&](const ManipulatorGameData::BTTemplate::SBTNode* pNode)
			{
				return pNode->flowGraphNodeID == id;
			});
			assert(iter != pTmpl->m_nodeList.end());
			m_curNode = *iter;

			eCategory category;
			if(m_curNode->type == L"Sequence")
				category = eCategory_SequenceNode;
			else if(m_curNode->type == L"Condition")
				category = eCategory_ConditionNode;
			else if(m_curNode->type == L"Action")
				category = eCategory_ActionNode;

			categories->GetAt(category)->SetHidden(FALSE);
			UpdateCategoryProperty(category);
		}
		break;

	case eFlowGraphNodeType_OwnBlackboard: 
	case eFlowGraphNodeType_GlobalBlackboard: 
		{
			m_curBBParamIndex = id;
			categories->GetAt(eCategory_Blackboard)->SetHidden(FALSE);
			UpdateCategoryProperty(eCategory_Blackboard);
		}
		break;
	}
}

ManipulatorGameData::Blackboard* PropertyPaneBehaviorTree::_GetCurBB()
{
	assert(m_curNodeType == eFlowGraphNodeType_OwnBlackboard || m_curNodeType == eFlowGraphNodeType_GlobalBlackboard);

	if(m_curNodeType == eFlowGraphNodeType_OwnBlackboard)
		return &m_pView->GetActiveTemplate()->m_ownBB;
	else
		return m_pView->GetActiveTemplate()->m_raceBB;	
}
