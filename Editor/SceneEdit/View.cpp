
// View.cpp : CEditorView 类的实现
//

#include "stdafx.h"
#include "SceneEdit.h"
#include "View.h"
#include "Manipulator/ManipulatorScene.h"
#include "Manipulator/ManipulatorObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEditorView

CEditorView::CEditorView()
{
	m_pResDropHelper = new COleDropTarget;
}

CEditorView::~CEditorView()
{
	SAFE_DELETE(m_pResDropHelper);
}

IMPLEMENT_DYNCREATE(CEditorView, CView)

BEGIN_MESSAGE_MAP(CEditorView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CEditorView 消息处理程序

BOOL CEditorView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CEditorView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码
	
	// 不要为绘制消息而调用 CWnd::OnPaint()
}

void CEditorView::OnSize( UINT nType, int cx, int cy )
{
	CView::OnSize(nType, cx, cy);
	((CSceneEditApp*)AfxGetApp())->m_app.OnViewportResized();
}

int CEditorView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if(!m_pResDropHelper->Register(this))
		return -1;

	return 0;
}

BOOL CEditorView::OnDrop( COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
	//取出拖拽项
	CXTPTaskPanelItem* pItemDrop = CXTPTaskPanelItem::CreateFromOleData(pDataObject);
	assert(pItemDrop);

	//获取资源名称
	std::wstring meshname = pItemDrop->GetCaption();
	meshname += L".mesh";

	Ogre::Vector2 pos((float)point.x, (float)point.y);
	RECT rect;
	GetClientRect(&rect);
	pos.x /= (rect.right-rect.left);
	pos.y /= (rect.bottom-rect.top);

	//创建物体
	if(!ManipulatorScene::GetSingleton().GetObject().AddEntity(Utility::UnicodeToEngine(meshname), pos))
		return FALSE;

	return TRUE;
}

DROPEFFECT CEditorView::OnDragOver( COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
	return DROPEFFECT_COPY;
}

