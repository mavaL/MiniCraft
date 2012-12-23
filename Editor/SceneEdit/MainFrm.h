#pragma once


class CEditorView;

class CMainFrame : public CXTPFrameWnd
{
	
public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL	OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	
#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

	//初始化编辑器界面
	bool			CreateEditorMainUI();

protected:
	DECLARE_DYNAMIC(CMainFrame)
	virtual BOOL	OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

private:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSetFocus(CWnd *pOldWnd);
	afx_msg void	OnClose();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnResPanelNotify(WPARAM wParam, LPARAM lParam);

	void			_LoadIcon();
	bool			_OnCreateRibbon();
	//创建mesh资源选择面板
	bool			_CreateMeshPanel(CImageList& imageList, Ogre::StringVectorPtr& meshNames);
	void			_CreateDockPane();
	LRESULT			_AttachDockPane(WPARAM wParam, LPARAM lParam);

	void			OnUpdateUI_TerrainBrushSize1(CCmdUI* pCmdUI);
	void			OnUpdateUI_TerrainBrushSize2(CCmdUI* pCmdUI);
	void			OnTerrainBrushSize1(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnTerrainBrushSize1Spin(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnTerrainBrushSize2(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnTerrainBrushSize2Spin(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnUpdateUI_BtnTerrainModify(CCmdUI* pCmdUI);
	void			OnBtnTerrainModify();

	CXTPDockingPaneManager	m_paneManager;
	CEditorView*			m_wndView;
	CXTPTaskPanel			m_resourceSelector;
};


