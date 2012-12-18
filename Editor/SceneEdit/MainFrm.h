#pragma once


class CEditorView;

class CMainFrame : public CXTPFrameWnd
{
	
public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	//创建mesh资源选择面板
	bool		CreateMeshPanel(CImageList& imageList, Ogre::StringVectorPtr& meshNames);
	void		CreateDockPane();

protected:
	DECLARE_DYNAMIC(CMainFrame)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

private:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSetFocus(CWnd *pOldWnd);
	afx_msg void	OnClose();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);

	void			_LoadIcon();
	bool			_OnCreateRibbon();
	LRESULT			_AttachDockPane(WPARAM wParam, LPARAM lParam);

	CXTPDockingPaneManager	m_paneManager;
	CEditorView*			m_wndView;
	CXTPTaskPanel			m_resourceSelector;
};


