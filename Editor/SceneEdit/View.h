
// View.h : CEditorView 类的接口
//


#pragma once


// CEditorView 窗口

class CEditorView : public CView
{
// 构造
public:
	CEditorView();
	DECLARE_DYNCREATE(CEditorView)

// 特性
public:

// 操作
public:

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual	void OnDraw(CDC* dc) {}
	virtual void PostNcDestroy() {}

// 实现
public:
	virtual ~CEditorView();

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	DECLARE_MESSAGE_MAP()
};

