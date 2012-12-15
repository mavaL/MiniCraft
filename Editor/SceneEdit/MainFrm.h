
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "View.h"

class CMainFrame : public CXTPFrameWnd
{
	
public:
	CMainFrame();

protected: 
	DECLARE_DYNAMIC(CMainFrame)

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CStatusBar      m_wndStatusBar;
	CEditorView		m_wndView;


private:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSetFocus(CWnd *pOldWnd);
	afx_msg void	OnClose();
	DECLARE_MESSAGE_MAP()

	void			_LoadIcon();
	bool			_OnCreateRibbon();
};


