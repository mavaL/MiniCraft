
// View.h : CEditorView 类的接口
//


#pragma once



class CEditorView : public CView
{
public:
	CEditorView();
	virtual ~CEditorView();
	DECLARE_DYNCREATE(CEditorView)

public:
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

protected:
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	virtual	void	OnDraw(CDC* dc) {}
	virtual void	PostNcDestroy() {}

private:
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

private:
	COleDropTarget*	m_pResDropHelper;	//ole拖拽
};

