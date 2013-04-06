#include "stdafx.h"
#include "BehaviorTreeEditorView.h"
#include "Manipulator/ManipulatorScene.h"
#include "BehaviorTreeEditorProperty.h"
#include "resource.h"
#include "BehaviorTreeEditorDlg.h"
#include "Utility.h"

BEGIN_MESSAGE_MAP(BehaviorTreeEditorView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

const int OWN_BLACKBOARD_NODE_ID	=	-1;
const int GLOBAL_BLACKBOARD_NODE_ID	=	-2;

BehaviorTreeEditorView::BehaviorTreeEditorView(CXTPDialog* parent)
:m_page(nullptr)
,m_curTmpl(nullptr)
,m_ownBBNode(nullptr)
,m_globalBBNode(nullptr)
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

//TODO: SubclassHelper not finished yet..
WNDPROC	g_proc;
CXTPFlowGraphControl* g_ctrl;
BehaviorTreeEditorProperty* g_prop;
CXTPFlowGraphNode*	g_ownBB;
CXTPFlowGraphNode*	g_globalBB;

LRESULT CALLBACK MyWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam )
{
	if (uMsg == WM_LBUTTONDOWN)
	{
		CPoint point;
		GetCursorPos(&point);
		g_ctrl->ScreenToClient(&point);

		CXTPFlowGraphNode* pNode = g_ctrl->HitTestNode(point);
		if (pNode)
		{
			if(pNode == g_ownBB)
			{
				CXTPFlowGraphConnectionPoint* pItem = g_ctrl->HitTestConnectionArea(point);
				if(pItem)
					g_prop->OnNodeSelected(eFlowGraphNodeType_OwnBlackboard, pItem->GetID());
				else
					g_prop->OnNodeSelected(eFlowGraphNodeType_None);
			}
			else if(pNode == g_globalBB)
			{
				CXTPFlowGraphConnectionPoint* pItem = g_ctrl->HitTestConnectionArea(point);
				if(pItem)
					g_prop->OnNodeSelected(eFlowGraphNodeType_GlobalBlackboard, pItem->GetID());
				else
					g_prop->OnNodeSelected(eFlowGraphNodeType_None);
			}
			else
			{
				g_prop->OnNodeSelected(eFlowGraphNodeType_TreeNode, pNode->GetID());
			}
		}
		else
		{
			g_prop->OnNodeSelected(eFlowGraphNodeType_None);
		}
	}
	return CallWindowProcW(g_proc, hWnd, uMsg, wParam, lParam);
}

int BehaviorTreeEditorView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndControl.Create(WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), 
		this, IDC_BTEditor_FlowGraph);

	g_proc = (WNDPROC)SetWindowLongPtrW(m_wndControl.GetSafeHwnd(), GWLP_WNDPROC, (LONG_PTR)MyWndProc);
	g_ctrl = &m_wndControl;

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
	m_curTmpl = &ManipulatorSystem.GetGameData().GetBTTemplate(name);
	RefreshAll();
}

void BehaviorTreeEditorView::RefreshAll()
{
	m_page->GetNodes()->RemoveAll();

	//创建节点
	int id = 0;
	for (auto itNode=m_curTmpl->m_nodeList.begin(); itNode!=m_curTmpl->m_nodeList.end(); ++itNode,++id)
	{
		auto& node = *itNode;
		CXTPFlowGraphNode* pNode = m_page->GetNodes()->AddNode(new CXTPFlowGraphNode());
		pNode->SetCaption(node->type.c_str());
		pNode->SetID(id);
		pNode->SetColor(node->color);
		node->flowGraphNodeID = id;

		CXTPFlowGraphConnectionPoint* pConnectionPoint;
		pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint());
		pConnectionPoint->SetCaption(node->txtProperty.c_str());
		pConnectionPoint->SetType(xtpFlowGraphPointInputAndOutput);
	}

	//连接节点
	ManipulatorGameData::BTTemplate::SBTNode* root = m_curTmpl->m_nodeList.front();
	while(root->parent)
		root = root->parent;

	_ConnectFgNodes(root);

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

	g_ownBB = m_ownBBNode;
	g_globalBB = m_globalBBNode;
}

void BehaviorTreeEditorView::_ConnectFgNodes( ManipulatorGameData::BTTemplate::SBTNode* root )
{
	CXTPFlowGraphNodes* nodes = m_page->GetNodes();
	//连接到子节点
	for (size_t iChild=0; iChild<root->childs.size(); ++iChild)
	{
		CXTPFlowGraphConnection* pConnection;
		pConnection = m_page->GetConnections()->AddConnection(new CXTPFlowGraphConnection());

		CXTPFlowGraphNode* pFrom = FindFgNodeByID(root->flowGraphNodeID);
		CXTPFlowGraphNode* pTo = FindFgNodeByID(root->childs[iChild]->flowGraphNodeID);

		pConnection->SetOutputPoint(pFrom->GetConnectionPoints()->GetAt(0));
		pConnection->SetInputPoint(pTo->GetConnectionPoints()->GetAt(0));
	}

	//遍历子节点
	for(size_t i=0; i<root->childs.size(); ++i)
		_ConnectFgNodes(root->childs[i]);
}

void BehaviorTreeEditorView::Arrange()
{
	m_wndControl.Arrange();
}

void BehaviorTreeEditorView::SetPropertyDlg( BehaviorTreeEditorProperty* pProp )
{
	m_pProp = pProp;
	g_prop = m_pProp;
}

void BehaviorTreeEditorView::AddNewNode( ManipulatorGameData::eBTNodeType type )
{
	CXTPFlowGraphNode* pNode = m_page->GetNodes()->AddNode(new CXTPFlowGraphNode());
	CXTPFlowGraphConnectionPoint* pConnectionPoint;
	pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint());
	pConnectionPoint->SetCaption(L"");
	pConnectionPoint->SetType(xtpFlowGraphPointInputAndOutput);

	auto node = ManipulatorSystem.GetGameData().AddBTNode(*m_curTmpl, type);

	pNode->SetCaption(node->type.c_str());
	pNode->SetID(m_curTmpl->m_nodeList.size());
	pNode->SetColor(node->color);
	node->flowGraphNodeID = m_curTmpl->m_nodeList.size();
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
		CXTPFlowGraphConnectionPoint* pConnectionPoint;
		pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint());
		pConnectionPoint->SetCaption(pBB->at(i).c_str());
		pConnectionPoint->SetType(xtpFlowGraphPointOutput);
		pConnectionPoint->SetID(i);
	}
}

void BehaviorTreeEditorView::AddBlackboardParam( bool bOwnBB )
{
	CXTPFlowGraphNode* pNode = bOwnBB ? m_ownBBNode : m_globalBBNode;
	ManipulatorGameData::Blackboard* pBB = bOwnBB ? &m_curTmpl->m_ownBB : m_curTmpl->m_raceBB;
	const std::string paramName = ManipulatorSystem.GetGameData().DefineBlackboardParam(true, *m_curTmpl);

	CXTPFlowGraphConnectionPoint* pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint);
	pConnectionPoint->SetCaption(Utility::EngineToUnicode(paramName).c_str());
	pConnectionPoint->SetType(xtpFlowGraphPointOutput);
	pConnectionPoint->SetID(pBB->size() - 1);
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
			auto parent = FindNodeByID(pConnection->GetOutputNode()->GetID());
			self->parent = parent;
			
			if(parent->childs.size() <= (size_t)self->priority)
				parent->childs.resize(self->priority + 1);
			parent->childs[self->priority] = self;
		}
		
		//childs
		int childCnt = pConnectionPoint->GetOutputConnectionsCount();
		for (int iChild=0; iChild<childCnt; ++iChild)
		{
			CXTPFlowGraphConnection* pConnection = pConnectionPoint->GetOutputConnectionAt(iChild);
			auto child = FindNodeByID(pConnection->GetInputNode()->GetID());

			if(self->childs.size() <= (size_t)child->priority)
				self->childs.resize(child->priority + 1);
			self->childs[child->priority] = child;
			child->parent = self;
		}
	}
}











