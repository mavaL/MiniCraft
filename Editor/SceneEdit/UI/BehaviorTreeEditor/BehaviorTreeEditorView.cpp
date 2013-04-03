#include "stdafx.h"
#include "BehaviorTreeEditorView.h"


BEGIN_MESSAGE_MAP(BehaviorTreeEditorView, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL BehaviorTreeEditorView::PreCreateWindow( CREATESTRUCT& cs )
{
	if(!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style |= WS_CLIPSIBLINGS;

	return TRUE;
}

int BehaviorTreeEditorView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndControl.Create(WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), 
		this, IDC_STATIC);

	m_wndControl.SetPaintManager(new CXTPFlowGraphMetallicTheme());

	m_wndControl.GetPages()->RemoveAll();

	//m_wndControl.SetPaintManager(new CXTPFlowGraphMetallicTheme());

	CXTPFlowGraphPage* pPage = m_wndControl.GetPages()->AddPage(new CXTPFlowGraphPage());
	int i;

	for (i = 1; i <= 10; i++)
	{
		CXTPFlowGraphNode* pNode = pPage->GetNodes()->AddNode(new CXTPFlowGraphNode());

		CString strCaption;
		strCaption.Format(_T("Node %d"), i);
		pNode->SetCaption(strCaption);
		pNode->SetLocation(CPoint(rand() % 5000, rand() % 3000));

		CXTPFlowGraphConnectionPoint* pConnectionPoint;
		pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint());
		pConnectionPoint->SetCaption(_T("Item 1"));
		pConnectionPoint->SetType(xtpFlowGraphPointOutput);

		pConnectionPoint = pNode->GetConnectionPoints()->AddConnectionPoint(new CXTPFlowGraphConnectionPoint());
		pConnectionPoint->SetCaption(_T("Item 2"));
		pConnectionPoint->SetType(xtpFlowGraphPointInput);
	}

	m_wndControl.SetActivePage(pPage);

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
