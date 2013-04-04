#include "stdafx.h"
#include "BehaviorTreeEditorView.h"
#include "Manipulator/ManipulatorScene.h"
#include "BehaviorTreeEditorProperty.h"
#include "resource.h"
#include "BehaviorTreeEditorDlg.h"

BEGIN_MESSAGE_MAP(BehaviorTreeEditorView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BehaviorTreeEditorView::BehaviorTreeEditorView(CXTPDialog* parent)
:m_page(nullptr)
,m_curTmpl(nullptr)
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

LRESULT CALLBACK MyWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam )
{
	if (uMsg == WM_LBUTTONDOWN)
	{
		CPoint point;
		GetCursorPos(&point);
		g_ctrl->ScreenToClient(&point);

		CXTPFlowGraphNode* pNode = g_ctrl->HitTestNode(point);
		if (pNode)
			g_prop->OnNodeSelected(true, pNode->GetID());
		else
			g_prop->OnNodeSelected(false);
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
	Refresh();
}

void BehaviorTreeEditorView::Refresh()
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
	for (auto itNode=m_curTmpl->m_nodeList.begin(); itNode!=m_curTmpl->m_nodeList.end(); ++itNode,++id)
	{
		auto& node = *itNode;
		CXTPFlowGraphNodes* nodes = m_page->GetNodes();
		//父节点
		if (node->parent)
		{
			CXTPFlowGraphConnection* pConnection;
			pConnection = m_page->GetConnections()->AddConnection(new CXTPFlowGraphConnection());

			int nFrom = node->parent->flowGraphNodeID;
			int nTo = node->flowGraphNodeID;

			pConnection->SetOutputPoint(nodes->GetAt(nFrom)->GetConnectionPoints()->GetAt(0));
			pConnection->SetInputPoint(nodes->GetAt(nTo)->GetConnectionPoints()->GetAt(0));
		}

		//子节点
		for (size_t iChild=0; iChild<node->childs.size(); ++iChild)
		{
			CXTPFlowGraphConnection* pConnection;
			pConnection = m_page->GetConnections()->AddConnection(new CXTPFlowGraphConnection());

			int nFrom = node->flowGraphNodeID;
			int nTo = node->childs[iChild]->flowGraphNodeID;

			pConnection->SetOutputPoint(nodes->GetAt(nFrom)->GetConnectionPoints()->GetAt(0));
			pConnection->SetInputPoint(nodes->GetAt(nTo)->GetConnectionPoints()->GetAt(0));
		}
	}
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








