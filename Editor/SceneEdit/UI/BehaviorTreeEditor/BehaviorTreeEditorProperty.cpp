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

void BehaviorTreeEditorProperty::SetView( BehaviorTreeEditorView* pView )
{
	m_propertyBT.SetView(pView);
}

/////////////////////////////////////////////////////////////////////
PropertyPaneBehaviorTree::PropertyPaneBehaviorTree()
:CPropertiesPane()
,m_pView(nullptr)
,m_curNodeType(eBTSelectionType_None)
,m_curFgElement(nullptr)
{

}

bool PropertyPaneBehaviorTree::_OnCreate()
{
	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"Behavior Tree Template");
	PROPERTY_REG(pCategory,	, L"Name"			, 		L"",	propBTName);
	PROPERTY_REG(pCategory,	, L"Race"			, 		L"",	propBTRace);
	PROPERTY_REG(pCategory,	, L"Script File"	, 		L"",	propBTScriptName);
	PROPERTY_REG(pCategory,	, L"Script Entry"	, 		L"",	propBTScriptEntry);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);

	//种族列表
	CXTPPropertyGridItemConstraints* pList = m_mapItem[propBTRace]->GetConstraints();
	pList->AddConstraint(L"Terran");
	pList->AddConstraint(L"Zerg");
	m_mapItem[propBTRace]->SetFlags(xtpGridItemHasComboButton);

	pCategory = m_wndPropertyGrid.AddCategory(L"Name");
	PROPERTY_REG(pCategory,	, L"Element Name"		, 		L"",	propFgElementName);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);

	pCategory = m_wndPropertyGrid.AddCategory(L"Priority");
	PROPERTY_REG(pCategory,	Number, L"Priority"		, 		0,	propPriority);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);

	pCategory = m_wndPropertyGrid.AddCategory(L"SequenceNode");
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

	//Behavior列表
	auto& behaviors = ManipulatorSystem.GetGameData().GetAllBehaviorNames();
	pList = m_mapItem[propAction]->GetConstraints();
	for (size_t i=0; i<behaviors.size(); ++i)
		pList->AddConstraint(behaviors[i].c_str());

	m_mapItem[propAction]->SetFlags(xtpGridItemHasComboButton);

	pCategory = m_wndPropertyGrid.AddCategory(L"Blackboard");
	PROPERTY_REG(pCategory,	, L"Name"		, 			L"",	propBBParamName);
	PROPERTY_REG(pCategory,	, L"Value"		, 			L"",	propBBParamValue);
	PROPERTY_REG(pCategory,	, L"Type"		, 			L"",	propBBParamType);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);

	//黑板参数类型列表
	pList = m_mapItem[propBBParamType]->GetConstraints();
	pList->AddConstraint(L"Int");
	pList->AddConstraint(L"Float");
	pList->AddConstraint(L"Bool");
	m_mapItem[propBBParamType]->SetFlags(xtpGridItemHasComboButton);

	pCategory = m_wndPropertyGrid.AddCategory(L"Connection");
	PROPERTY_REG(pCategory,	, L"From"	, 			L"",	propConnectionOutput);
	PROPERTY_REG(pCategory,	, L"To"		, 			L"",	propConnectionInput);
	pCategory->Expand();
	pCategory->SetHidden(TRUE);

	EnableMutableProperty(TRUE);

	return true;
}

void PropertyPaneBehaviorTree::_SetProperty( int nID )
{
	ManipulatorGameData& manGameData = ManipulatorSystem.GetGameData();

	switch (nID)
	{
	case propBTRace:
		{
			const std::wstring race = m_mapItem[nID]->GetValue();
			eGameRace erace;
			if(race == L"Terran") erace = eGameRace_Terran;
			else if(race == L"Zerg") erace = eGameRace_Zerg;
			else assert(0);

			m_pView->GetActiveTemplate()->race = erace;
		}
		break;

	case propBTScriptName: m_pView->GetActiveTemplate()->m_scriptName = m_mapItem[nID]->GetValue(); break;
	case propBTScriptEntry: m_pView->GetActiveTemplate()->m_scriptEntry = m_mapItem[nID]->GetValue(); break;

	case propAction:
	case propConditon: 
		{
			CXTPFlowGraphNode* pFgNode = dynamic_cast<CXTPFlowGraphNode*>(m_curFgElement);
			auto node = m_pView->FindNodeByID(pFgNode->GetID());
			node->txtProperty = m_mapItem[nID]->GetValue();
			m_pView->RefreshTreeNode(node);
		}
		break;

	case propBBParamName:
		{
			CXTPFlowGraphConnectionPoint* pConnePt = dynamic_cast<CXTPFlowGraphConnectionPoint*>(m_curFgElement);
			bool bOwnBB = m_curNodeType == eBTSelectionType_OwnBlackboard;
			const std::wstring newName = m_mapItem[nID]->GetValue();
			manGameData.RenameBlackboardParam(pConnePt->GetID(), newName, *m_pView->GetActiveTemplate(), bOwnBB);
			pConnePt->SetCaption(newName.c_str());
		}
		break;

	case propBBParamValue:
		{
			CXTPFlowGraphConnectionPoint* pConnePt = dynamic_cast<CXTPFlowGraphConnectionPoint*>(m_curFgElement);
			bool bOwnBB = m_curNodeType == eBTSelectionType_OwnBlackboard;
			auto param = manGameData.GetBlackboardParam(pConnePt->GetID(), *m_pView->GetActiveTemplate(), bOwnBB);
			param.value = m_mapItem[nID]->GetValue();

			manGameData.SetBlackboardParam(pConnePt->GetID(), param, *m_pView->GetActiveTemplate(), bOwnBB);
		}
		break;

	case propBBParamType:
		{
			CXTPFlowGraphConnectionPoint* pConnePt = dynamic_cast<CXTPFlowGraphConnectionPoint*>(m_curFgElement);
			bool bOwnBB = m_curNodeType == eBTSelectionType_OwnBlackboard;
			auto param = manGameData.GetBlackboardParam(pConnePt->GetID(), *m_pView->GetActiveTemplate(), bOwnBB);
			param.type = m_mapItem[nID]->GetValue();

			manGameData.SetBlackboardParam(pConnePt->GetID(), param, *m_pView->GetActiveTemplate(), bOwnBB);
		}
		break;

	case propPriority:
		{
			CXTPFlowGraphNode* pFgNode = dynamic_cast<CXTPFlowGraphNode*>(m_curFgElement);
			auto node = m_pView->FindNodeByID(pFgNode->GetID());
			CXTPPropertyGridItemNumber* pItem = dynamic_cast<CXTPPropertyGridItemNumber*>(m_mapItem[propPriority]);
			node->priority = pItem->GetNumber();
		}
		break;

	default: assert(0);
	}
}

void PropertyPaneBehaviorTree::_UpdateProperty( int nID )
{
	ManipulatorGameData& manGameData = ManipulatorSystem.GetGameData();

	switch (nID)
	{
	case propBTName: m_mapItem[nID]->SetValue(m_pView->GetActiveTemplate()->m_name.c_str()); break;

	case propBTRace:
		{
			std::wstring race;
			eGameRace erace = (eGameRace)m_pView->GetActiveTemplate()->race;
			if(erace == eGameRace_Terran) race = L"Terrain";
			else if(erace == eGameRace_Zerg) race = L"Zerg";
			else assert(0);

			m_mapItem[nID]->SetValue(race.c_str());
		}
		break;

	case propBTScriptName: m_mapItem[nID]->SetValue(m_pView->GetActiveTemplate()->m_scriptName.c_str()); break;
	case propBTScriptEntry: m_mapItem[nID]->SetValue(m_pView->GetActiveTemplate()->m_scriptEntry.c_str()); break;

	case propFgElementName:
		{
			const std::wstring name = m_pView->GetFgElementName(m_curNodeType, m_curFgElement);
			m_mapItem[nID]->SetValue(name.c_str());
		}
		break;

	case propAction:
	case propConditon: 
		{
			CXTPFlowGraphNode* pFgNode = dynamic_cast<CXTPFlowGraphNode*>(m_curFgElement);
			auto node = m_pView->FindNodeByID(pFgNode->GetID());
			m_mapItem[nID]->SetValue(node->txtProperty.c_str()); 
		}
		break;

	case propBBParamName:
		{
			CXTPFlowGraphConnectionPoint* pConnePt = dynamic_cast<CXTPFlowGraphConnectionPoint*>(m_curFgElement);
			auto param = manGameData.FindBBParam(*_GetCurBB(), pConnePt->GetID());
			m_mapItem[nID]->SetValue(param->name.c_str());
		}
		break;

	case propBBParamValue:
		{
			CXTPFlowGraphConnectionPoint* pConnePt = dynamic_cast<CXTPFlowGraphConnectionPoint*>(m_curFgElement);
			auto param = manGameData.GetBlackboardParam(pConnePt->GetID(), *m_pView->GetActiveTemplate(), m_curNodeType == eBTSelectionType_OwnBlackboard);
			m_mapItem[nID]->SetValue(param.value.c_str());
		}
		break;

	case propBBParamType:
		{
			CXTPFlowGraphConnectionPoint* pConnePt = dynamic_cast<CXTPFlowGraphConnectionPoint*>(m_curFgElement);
			auto param = manGameData.GetBlackboardParam(pConnePt->GetID(), *m_pView->GetActiveTemplate(), m_curNodeType == eBTSelectionType_OwnBlackboard);
			m_mapItem[nID]->SetValue(param.type.c_str());
		}
		break;

	case propPriority:
		{
			CXTPFlowGraphNode* pFgNode = dynamic_cast<CXTPFlowGraphNode*>(m_curFgElement);
			auto node = m_pView->FindNodeByID(pFgNode->GetID());
			CXTPPropertyGridItemNumber* pItem = dynamic_cast<CXTPPropertyGridItemNumber*>(m_mapItem[propPriority]);
			pItem->SetNumber(node->priority);
		}
		break;

	case propConnectionInput:
	case propConnectionOutput:
		{
			CXTPFlowGraphConnection* pConne = dynamic_cast<CXTPFlowGraphConnection*>(m_curFgElement);
			auto inputNode = m_pView->FindNodeByID(pConne->GetInputNode()->GetID());
			auto outputNode = m_pView->FindNodeByID(pConne->GetOutputNode()->GetID());

			m_mapItem[propConnectionInput]->SetValue(inputNode->fgElementName.c_str());
			m_mapItem[propConnectionOutput]->SetValue(outputNode->fgElementName.c_str());
		}
		break;

	default: assert(0);
	}
}

void PropertyPaneBehaviorTree::OnFgElementSelected( eBTSelectionType nodeType, CXTPFlowGraphElement* element )
{
	m_curFgElement = element;
	m_curNodeType = nodeType;

	auto categories = m_wndPropertyGrid.GetCategories();
	int count = categories->GetCount();
	for(int i=0; i<count; ++i)
		categories->GetAt(i)->SetHidden(TRUE);

	ManipulatorGameData::BTTemplate* pTmpl = m_pView->GetActiveTemplate();
	if(!pTmpl)
		return;

	switch (nodeType)
	{
	case eBTSelectionType_BT:
		{
			categories->GetAt(eCategory_BehaviorTree)->SetHidden(FALSE);
			UpdateCategoryProperty(eCategory_BehaviorTree);
		}
		break;

	case eBTSelectionType_TreeNode:
		{
			CXTPFlowGraphNode* pFgNode = dynamic_cast<CXTPFlowGraphNode*>(element);
			auto node = m_pView->FindNodeByID(pFgNode->GetID());

			eCategory category;
			if(node->type == L"Sequence")		category = eCategory_SequenceNode;
			else if(node->type == L"Condition") category = eCategory_ConditionNode;
			else if(node->type == L"Action")	category = eCategory_ActionNode;
			else								assert(0);

			categories->GetAt(eCategory_Node)->SetHidden(FALSE);
			categories->GetAt(eCategory_Name)->SetHidden(FALSE);
			categories->GetAt(category)->SetHidden(FALSE);
			UpdateCategoryProperty(eCategory_Node);
			UpdateCategoryProperty(eCategory_Name);
			UpdateCategoryProperty(category);
		}
		break;

	case eBTSelectionType_OwnBlackboard: 
	case eBTSelectionType_GlobalBlackboard: 
		{
			categories->GetAt(eCategory_Blackboard)->SetHidden(FALSE);
			UpdateCategoryProperty(eCategory_Blackboard);
		}
		break;

	case eBTSelectionType_Connection:
		{
			categories->GetAt(eCategory_Name)->SetHidden(FALSE);
			categories->GetAt(eCategory_Connection)->SetHidden(FALSE);

			UpdateCategoryProperty(eCategory_Name);
			UpdateCategoryProperty(eCategory_Connection);
		}
		break;
	}
}

ManipulatorGameData::Blackboard* PropertyPaneBehaviorTree::_GetCurBB()
{
	assert(m_curNodeType == eBTSelectionType_OwnBlackboard || m_curNodeType == eBTSelectionType_GlobalBlackboard);

	if(m_curNodeType == eBTSelectionType_OwnBlackboard)
		return &m_pView->GetActiveTemplate()->m_ownBB;
	else
		return m_pView->GetActiveTemplate()->m_raceBB;	
}
