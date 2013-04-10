#include "stdafx.h"
#include "BehaviorTreeEditorView.h"
#include "Manipulator/ManipulatorScene.h"
#include "BehaviorTreeEditorProperty.h"
#include "resource.h"
#include "BehaviorTreeEditorDlg.h"
#include "Utility.h"
#include "BehaviorTreeEditorExplorer.h"
#include "SubclassHelper.h"

BEGIN_MESSAGE_MAP(BehaviorTreeEditorView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(XTP_FGN_CONNECTIONCHANGING, IDC_BTEditor_FlowGraph, _OnXtpConnectionChanging)
	ON_NOTIFY(XTP_FGN_CONNECTIONCHANGED, IDC_BTEditor_FlowGraph, _OnXtpConnectionChanged)
END_MESSAGE_MAP()

const int OWN_BLACKBOARD_NODE_ID	=	-1;
const int GLOBAL_BLACKBOARD_NODE_ID	=	-2;

BehaviorTreeEditorView::BehaviorTreeEditorView(CXTPDialog* parent)
:m_page(nullptr)
,m_curTmpl(nullptr)
,m_ownBBNode(nullptr)
,m_globalBBNode(nullptr)
,m_pExplorer(nullptr)
{
}

BehaviorTreeEditorView::~BehaviorTreeEditorView()
{
	m_page->GetNodes()->RemoveAll();
}

BOOL BehaviorTreeEditorView::PreCreateWindow( CREATESTRUCT& cs )
{
	if(!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style |= WS_CLIPSIBLINGS;

	return TRUE;
}

bool BehaviorTreeEditorView::FlowGraphProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam, LRESULT& result )
{
	if (uMsg == WM_LBUTTONDOWN)
	{
		CPoint point;
		GetCursorPos(&point);
		m_wndControl.ScreenToClient(&point);

		CXTPFlowGraphNode* pNode = m_wndControl.HitTestNode(point);
		CXTPFlowGraphConnection* pConne = m_wndControl.HitTestConnection(point);
		eBTSelectionType type;
		CXTPFlowGraphElement* curElement = nullptr;

		if (pNode)
		{
			if(pNode == m_ownBBNode)
			{
				CXTPFlowGraphConnectionPoint* pItem = m_wndControl.HitTestConnectionArea(point);
				if(pItem)
				{
					type = eBTSelectionType_OwnBlackboard;
					curElement = pItem;
				}
				else
				{
					type = eBTSelectionType_BT;
				}
			}
			else if(pNode == m_globalBBNode)
			{
				CXTPFlowGraphConnectionPoint* pItem = m_wndControl.HitTestConnectionArea(point);
				if(pItem)
				{
					type = eBTSelectionType_GlobalBlackboard;
					curElement = pItem;
				}
				else
				{
					type = eBTSelectionType_BT;
				}
			}
			else
			{
				type = eBTSelectionType_TreeNode;
				curElement = pNode;
			}
		}
		else if (pConne)
		{
			type = eBTSelectionType_Connection;
			curElement = pConne;
		}
		else
		{
			type = eBTSelectionType_BT;
		}

		m_pProp->GetPropPane().OnFgElementSelected(type, curElement);

		return true;
		result = 0;
	}
	
	return false;
}

int BehaviorTreeEditorView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndControl.Create(WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), 
		this, IDC_BTEditor_FlowGraph);

	WndSubclassHelper<BehaviorTreeEditorView>::GetSingleton().Subclass(
		m_wndControl.GetSafeHwnd(),this, &BehaviorTreeEditorView::FlowGraphProc);

	m_wndControl.SetPaintManager(new CXTPFlowGraphMetallicTheme());
	m_wndControl.SetSmoothingMode(xtpFlowGraphSmoothingModeHighQuality);
	m_wndControl.GetPages()->RemoveAll();
	m_page = m_wndControl.GetPages()->AddPage(new CXTPFlowGraphPage());
	m_wndControl.SetActivePage(m_page);
	m_wndControl.SetZoomRange(0.5, 5.0);

	return 0;
}

void BehaviorTreeEditorView::OnSize( UINT nType, int cx, int cy )
{
	CRect rcClient;
	GetClientRect(rcClient);

	HDWP hdwp = BeginDeferWindowPos(1);
	DeferWindowPos(hdwp, m_wndControl.GetSafeHwnd(), nullptr, 0, 0, rcClient.Width(), rcClient.Height(), SWP_NOZORDER);
	EndDeferWindowPos(hdwp);
}

void BehaviorTreeEditorView::SetActiveItem( const std::wstring& name )
{
	if (name.empty())
	{
		m_pProp->GetPropPane().OnFgElementSelected(eBTSelectionType_None);
		m_page->GetNodes()->RemoveAll();
	}
	else
	{
		m_curTmpl = &ManipulatorSystem.GetGameData().GetBTTemplate(name);
		m_pProp->GetPropPane().OnFgElementSelected(eBTSelectionType_BT);
		RefreshAll();
	}
}

void BehaviorTreeEditorView::RefreshAll()
{
	m_page->GetNodes()->RemoveAll();

	//创建节点
	for (auto itNode=m_curTmpl->m_nodeList.begin(); itNode!=m_curTmpl->m_nodeList.end(); ++itNode)
	{
		auto& node = *itNode;
		CXTPFlowGraphNode* pNode = m_page->GetNodes()->AddNode(new CXTPFlowGraphNode());
		node->fgElementName = _OnFgElementAdded(pNode);

		pNode->SetCaption(node->type.c_str());
		pNode->SetColor(node->color);
		pNode->GetConnectionPoints()->GetAt(0)->SetCaption(node->txtProperty.c_str());

		int id = Utility::GenGUID();
		pNode->SetID(id);
		node->flowGraphNodeID = id;
	}

	//连接节点
	for (auto itNode=m_curTmpl->m_nodeList.begin(); itNode!=m_curTmpl->m_nodeList.end(); ++itNode)
	{
		_ConnectFgNodes(*itNode);
	}

	///独有黑板
	m_ownBBNode = m_page->GetNodes()->AddNode(new CXTPFlowGraphNode());
	m_ownBBNode->SetCaption(L"OwnBlackBoard");
	m_ownBBNode->SetColor(0xc0c0c0ff);
	m_ownBBNode->SetID(OWN_BLACKBOARD_NODE_ID);
	RefreshBlackboard(true);
	
	///种族全局黑板
	m_globalBBNode = m_page->GetNodes()->AddNode(new CXTPFlowGraphNode());
	m_globalBBNode->SetCaption(L"RaceBlackBoard");
	m_globalBBNode->SetColor(0xc0c0c0ff);
	m_globalBBNode->SetID(GLOBAL_BLACKBOARD_NODE_ID);
	RefreshBlackboard(false);
}

void BehaviorTreeEditorView::_ConnectFgNodes( ManipulatorGameData::BTTemplate::SBTNode* node )
{
	std::sort(node->childs.begin(), node->childs.end()
		, [](const ManipulatorGameData::BTTemplate::SBTNode* lhs, const ManipulatorGameData::BTTemplate::SBTNode* rhs)
	{
		return lhs->priority < rhs->priority;
	});

	CXTPFlowGraphNodes* nodes = m_page->GetNodes();
	//连接到子节点
	for (size_t iChild=0; iChild<node->childs.size(); ++iChild)
	{
		if(!node->childs[iChild])
			continue;

		CXTPFlowGraphConnection* pConnection;
		pConnection = m_page->GetConnections()->AddConnection(new CXTPFlowGraphConnection());
		_OnFgElementAdded(pConnection);

		CXTPFlowGraphNode* pFrom = FindFgNodeByID(node->flowGraphNodeID);
		CXTPFlowGraphNode* pTo = FindFgNodeByID(node->childs[iChild]->flowGraphNodeID);

		pConnection->SetOutputPoint(pFrom->GetConnectionPoints()->GetAt(0));
		pConnection->SetInputPoint(pTo->GetConnectionPoints()->GetAt(0));
	}
}

void BehaviorTreeEditorView::Arrange()
{
	m_wndControl.Arrange();
}

void BehaviorTreeEditorView::SetPropertyDlg( BehaviorTreeEditorProperty* pProp )
{
	m_pProp = pProp;
}

void BehaviorTreeEditorView::AddNewNode( ManipulatorGameData::eBTNodeType type )
{
	CXTPFlowGraphNode* pNode = m_page->GetNodes()->AddNode(new CXTPFlowGraphNode());
	auto node = ManipulatorSystem.GetGameData().AddBTNode(*m_curTmpl, type);
	node->fgElementName = _OnFgElementAdded(pNode);

	pNode->SetCaption(node->type.c_str());
	pNode->SetColor(node->color);

	int id = Utility::GenGUID();
	pNode->SetID(id);
	node->flowGraphNodeID = id;
}

void BehaviorTreeEditorView::RefreshTreeNode( ManipulatorGameData::BTTemplate::SBTNode* pNode )
{
	FindFgNodeByID(pNode->flowGraphNodeID)->GetConnectionPoints()->GetAt(0)->SetCaption(pNode->txtProperty.c_str());
}

void BehaviorTreeEditorView::RefreshBlackboard( bool bOwnBB )
{
	CXTPFlowGraphNode* pNode = bOwnBB ? m_ownBBNode : m_globalBBNode;
	ManipulatorGameData::Blackboard* pBB = bOwnBB ? &m_curTmpl->m_ownBB : m_curTmpl->m_raceBB;
	pNode->GetConnectionPoints()->RemoveAll();

	for (size_t i=0; i<pBB->size(); ++i)
	{
		ManipulatorGameData::SBBParam& param = pBB->at(i);
		CXTPFlowGraphConnectionPoint* pConnectionPoint;
		pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint());
		pConnectionPoint->SetCaption(param.name.c_str());
		pConnectionPoint->SetType(xtpFlowGraphPointOutput);

		int id = Utility::GenGUID();
		pConnectionPoint->SetID(id);
		param.fgID = id;
	}
}

void BehaviorTreeEditorView::AddBlackboardParam( bool bOwnBB )
{
	CXTPFlowGraphNode* pNode = bOwnBB ? m_ownBBNode : m_globalBBNode;
	ManipulatorGameData::Blackboard* pBB = bOwnBB ? &m_curTmpl->m_ownBB : m_curTmpl->m_raceBB;
	const std::string paramName = ManipulatorSystem.GetGameData().DefineBlackboardParam(bOwnBB, *m_curTmpl);

	CXTPFlowGraphConnectionPoint* pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint);
	pConnectionPoint->SetCaption(Utility::EngineToUnicode(paramName).c_str());
	pConnectionPoint->SetType(xtpFlowGraphPointOutput);

	int id = Utility::GenGUID();
	pConnectionPoint->SetID(id);
	pBB->at(pBB->size()-1).fgID = id;
}

ManipulatorGameData::BTTemplate::SBTNode* BehaviorTreeEditorView::FindNodeByID( int id )
{
	auto iter = std::find_if(m_curTmpl->m_nodeList.begin(), m_curTmpl->m_nodeList.end(), 
		[&](const ManipulatorGameData::BTTemplate::SBTNode* pNode)
	{
		return pNode->flowGraphNodeID == id;
	});
	assert(iter != m_curTmpl->m_nodeList.end());

	return *iter;
}

CXTPFlowGraphNode* BehaviorTreeEditorView::FindFgNodeByID( int id )
{
	CXTPFlowGraphNodes* pNodes = m_page->GetNodes();
	CXTPFlowGraphNode* pFgNode = nullptr;
	for (int i=0; i<pNodes->GetCount(); ++i)
	{
		if (pNodes->GetAt(i)->GetID() == id)
		{
			pFgNode = pNodes->GetAt(i);
			break;
		}
	}
	assert(pFgNode != nullptr);

	return pFgNode;
}

void BehaviorTreeEditorView::Sync()
{
	//清空目前保存的结构
	for (auto iter=m_curTmpl->m_nodeList.begin(); iter!=m_curTmpl->m_nodeList.end(); ++iter)
	{
		auto treenode = *iter;
		treenode->parent = nullptr;
		//no clear()!
		for(size_t i=0; i<treenode->childs.size(); ++i)
			treenode->childs[i] = nullptr;
	}

	//同步行为树的结构
	CXTPFlowGraphNodes* pNodes = m_page->GetNodes();
	for (int iNode=0; iNode<pNodes->GetCount(); ++iNode)
	{
		CXTPFlowGraphNode* pNode = pNodes->GetAt(iNode);
		if(pNode == m_ownBBNode || pNode == m_globalBBNode)
			continue;
		
		CXTPFlowGraphConnectionPoint* pConnectionPoint = pNode->GetConnectionPoints()->GetAt(0);
		if(pConnectionPoint->GetInputConnectionsCount() > 1)
		{
			::MessageBoxW(0, L"Node has more than one parent!", L"Warning", MB_ICONWARNING);
			continue;
		}

		auto self = FindNodeByID(pNode->GetID());

		//parent
		if (pConnectionPoint->GetInputConnectionsCount() > 0)
		{
			CXTPFlowGraphConnection* pConnection = pConnectionPoint->GetInputConnectionAt(0);
			CXTPFlowGraphNode* ouputNode = pConnection->GetOutputNode();

			if (ouputNode)
			{
				auto parent = FindNodeByID(ouputNode->GetID());
				self->parent = parent;
				if(parent->childs.empty())
					parent->childs.resize(1);
				parent->childs[self->priority] = self;
			}
		}
		
		//childs
		int childCnt = pConnectionPoint->GetOutputConnectionsCount();
		for (int iChild=0; iChild<childCnt; ++iChild)
		{
			CXTPFlowGraphConnection* pConnection = pConnectionPoint->GetOutputConnectionAt(iChild);
			CXTPFlowGraphNode* inputNode = pConnection->GetInputNode();

			if(inputNode)
			{
				auto child = FindNodeByID(inputNode->GetID());
				child->parent = self;
				if(self->childs.empty())
					self->childs.resize(1);
				self->childs[child->priority] = child;
			}
		}
	}

	//清除空的子项
	for (auto iter=m_curTmpl->m_nodeList.begin(); iter!=m_curTmpl->m_nodeList.end(); ++iter)
	{
		auto& childs = (*iter)->childs;
		for(auto itChild=childs.begin(); itChild!=childs.end(); /*++itChild*/)
		{
			if(*itChild == nullptr)
				itChild = childs.erase(itChild);
			else
				++itChild;
		}
	}
}

std::wstring BehaviorTreeEditorView::_OnFgElementAdded( CXTPFlowGraphElement* element )
{
	
	std::string name = "Element_";
	name += Ogre::StringConverter::toString(Utility::GenGUID());
	const std::wstring wname = Utility::EngineToUnicode(name);

	m_elements.insert(std::make_pair(wname, element));

	if(element->IsKindOf(RUNTIME_CLASS(CXTPFlowGraphNode)))
	{
		CXTPFlowGraphNode* pNode = dynamic_cast<CXTPFlowGraphNode*>(element);
		CXTPFlowGraphConnectionPoint* pConnectionPoint;
		pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint());
		pConnectionPoint->SetCaption(L"");
		pConnectionPoint->SetType(xtpFlowGraphPointInputAndOutput);
	}
	else if (element->IsKindOf(RUNTIME_CLASS(CXTPFlowGraphConnection)))
	{
		CXTPFlowGraphConnection* pConne = dynamic_cast<CXTPFlowGraphConnection*>(element);
		pConne->SetCaption(wname.c_str());
	}
	else
	{
		assert(0);
	}

	return std::move(wname);
}

void BehaviorTreeEditorView::_OnFgElememtRemoved( const std::wstring& name )
{
	auto iter = m_elements.find(name);
	assert(iter != m_elements.end());
	m_elements.erase(iter);
}

std::wstring BehaviorTreeEditorView::GetFgElementName(eBTSelectionType type, CXTPFlowGraphElement* element)
{
	std::wstring name;
	if(type == eBTSelectionType_TreeNode)
	{
		CXTPFlowGraphNode* pFgNode = dynamic_cast<CXTPFlowGraphNode*>(element);
		auto node = FindNodeByID(pFgNode->GetID());
		name = node->fgElementName;
	}
	else if (type == eBTSelectionType_Connection)
	{
		CXTPFlowGraphConnection* pConne = dynamic_cast<CXTPFlowGraphConnection*>(element);
		name = pConne->GetCaption();
	}
	else
	{
		assert(0);
	}

	return std::move(name);
}

void BehaviorTreeEditorView::DeleteCurElement()
{
	assert(m_curTmpl);
	ManipulatorGameData& manGameData = ManipulatorSystem.GetGameData();
	eBTSelectionType type = m_pProp->GetPropPane().GetCurSelType();
	CXTPFlowGraphElement* curFgElement = m_pProp->GetPropPane().GetCurFgElement();

	switch (type)
	{
	case eBTSelectionType_TreeNode:
		{
			CXTPFlowGraphNode* pFgNode = dynamic_cast<CXTPFlowGraphNode*>(curFgElement);

			if(FindNodeByID(pFgNode->GetID()) == m_curTmpl->rootNode)
			{
				if(IDCANCEL == ::MessageBoxW(0, L"Really remove root node?", L"Warning", MB_ICONWARNING | MB_OKCANCEL))
					return;
			}

			//节点被移除,那么它的所有连接也会被移除
			CXTPFlowGraphConnectionPoint* pConnePt = pFgNode->GetConnectionPoints()->GetAt(0);
			for (int i=0; i<pConnePt->GetInputConnectionsCount(); ++i)
			{
				CXTPFlowGraphConnection* pConne = pConnePt->GetInputConnectionAt(i);
				_OnFgConnectionRemoved(pConne);
			}
			for (int i=0; i<pConnePt->GetOutputConnectionsCount(); ++i)
			{
				CXTPFlowGraphConnection* pConne = pConnePt->GetOutputConnectionAt(i);
				_OnFgConnectionRemoved(pConne);
			}

			const std::wstring name = GetFgElementName(type, curFgElement);
			_OnFgElememtRemoved(name);
			int id = pFgNode->GetID();
			pFgNode->Remove();
			manGameData.DeleteBTNode(*m_curTmpl, id);
		}
		break;

	case eBTSelectionType_Connection:
		{
			CXTPFlowGraphConnection* pConne = dynamic_cast<CXTPFlowGraphConnection*>(curFgElement);
			_OnFgConnectionRemoved(pConne);

			const std::wstring name = GetFgElementName(type, curFgElement);
			_OnFgElememtRemoved(name);
			pConne->Remove();
		}
		break;

	case eBTSelectionType_OwnBlackboard:
		{
			CXTPFlowGraphConnectionPoint* pConnePt = dynamic_cast<CXTPFlowGraphConnectionPoint*>(curFgElement);
			manGameData.DeleteBlackboardParam(pConnePt->GetID(), true, *m_curTmpl);

			pConnePt->Remove();
		}
		break;

	case eBTSelectionType_GlobalBlackboard:
		{
			CXTPFlowGraphConnectionPoint* pConnePt = dynamic_cast<CXTPFlowGraphConnectionPoint*>(curFgElement);
			manGameData.DeleteBlackboardParam(pConnePt->GetID(), false, *m_curTmpl);

			pConnePt->Remove();
		}
		break;

	default: assert(0);
	}

	m_pProp->GetPropPane().OnFgElementSelected(eBTSelectionType_BT);
}

void BehaviorTreeEditorView::_OnFgConnectionRemoved( CXTPFlowGraphConnection* pConne )
{
	auto inputNode = FindNodeByID(pConne->GetInputNode()->GetID());
	auto outputNode = FindNodeByID(pConne->GetOutputNode()->GetID());

	auto itDelete = outputNode->childs.begin();
	std::advance(itDelete, inputNode->priority);
	
	for(auto iter=itDelete; iter!=outputNode->childs.end(); ++iter)
		--((*iter)->priority);
	outputNode->childs.erase(itDelete);

	inputNode->parent = nullptr;
	inputNode->priority = 0;
}

bool BehaviorTreeEditorView::_IsChildOf( ManipulatorGameData::BTTemplate::SBTNode* parent, ManipulatorGameData::BTTemplate::SBTNode* self )
{
	while(self->parent)
	{
		if(self->parent == parent)
			return true;
		self = self->parent;
	}
	return false;
}

void BehaviorTreeEditorView::NewBT( const std::wstring& name )
{
	SetActiveItem(L"");
	m_curTmpl = &ManipulatorSystem.GetGameData().NewBTTemplate(name);
	assert(m_curTmpl);
	RefreshAll();
	m_pExplorer->Refresh();
}

void BehaviorTreeEditorView::_OnXtpConnectionChanging( NMHDR* pNMHDR, LRESULT* pResult )
{
	XTP_NM_FLOWGRAPH_CONNECTIONCHANGING* pInfo = (XTP_NM_FLOWGRAPH_CONNECTIONCHANGING*)pNMHDR;
	if(pInfo->pInputConnectionPoint)
	{
		auto from = FindNodeByID(pInfo->pConnection->GetOutputNode()->GetID());
		auto to	= FindNodeByID(pInfo->pInputConnectionPoint->GetNode()->GetID());

		auto& childs = from->childs;
		size_t priority = childs.size();
		childs.resize(priority + 1);
		childs[priority] = to;

		to->parent = from;
		to->priority = priority;
	}
}

void BehaviorTreeEditorView::_OnXtpConnectionChanged( NMHDR* pNMHDR, LRESULT* pResult )
{
	XTP_NM_FLOWGRAPH_CONNECTIONCHANGED* pInfo = (XTP_NM_FLOWGRAPH_CONNECTIONCHANGED*)pNMHDR;
	if(pInfo->nAction == 1)
		Sync();
}











