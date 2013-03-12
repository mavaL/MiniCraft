#pragma once

#include "Manipulator/ManipulatorEventCallback.h"

class CEditorView;
class PropertyPaneTerrain;
class PropertyPaneObject;
class PropertyPaneEffect;
class PropertyPaneAttachment;

class CMainFrame : public CXTPFrameWnd, public ManipulatorSceneEventCallback, public ManipulatorObjectEventCallback
{
	
public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL	OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	/////////////////////////////////////////////////////////
	///事件回调
	virtual	void	OnSceneNew();
	virtual	void	OnSceneOpen();
	virtual void	OnSceneClose();

	virtual void	OnObjectSetSelection(Ogre::Entity* pObject);
	virtual void	OnObjectClearSelection(Ogre::Entity* pObject);
	virtual void	OnObjectPropertyChanged(Ogre::Entity*);
	
#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

	//初始化编辑器界面
	bool			CreateEditorMainUI();

protected:
	DECLARE_DYNAMIC(CMainFrame)
	virtual BOOL	OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	afx_msg int		OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

private:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSetFocus(CWnd *pOldWnd);
	afx_msg void	OnClose();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnResPanelNotify(WPARAM wParam, LPARAM lParam);

	void			_LoadIcon();
	bool			_OnCreateRibbon();
	void			_CreateLayerTexIcon();
	void			_CreateDockPane();
	LRESULT			_AttachDockPane(WPARAM wParam, LPARAM lParam);
	//创建mesh资源选择面板
	bool			_CreateMeshPanel(CImageList& imageList, Ogre::StringVectorPtr& meshNames);

	///Terrain
	void			OnUpdateUI_TerrainBrushSize1(CCmdUI* pCmdUI);
	void			OnUpdateUI_TerrainBrushSize2(CCmdUI* pCmdUI);
	void			OnTerrainBrushSize1(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnTerrainBrushSize1Spin(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnTerrainBrushSize2(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnTerrainBrushSize2Spin(NMHDR* pNMHDR, LRESULT* pResult);
	//deform
	void			OnUpdateUI_BtnTerrainDeform(CCmdUI* pCmdUI);
	void			OnBtnTerrainDeform();
	//splat
	void			OnUpdateUI_BtnTerrainSplat(CCmdUI* pCmdUI);
	void			OnBtnTerrainSplat();
	//LayerTexture
	void			OnUpdateUI_GalleryLayerTex(CCmdUI* pCmdUI);
	void			OnSelectLayerTex(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnUpdateUI_SplatSelectLayer(CCmdUI* pCmdUI);
	void			OnSplatSelectLayer(UINT nID);
	///NavMesh
	void			OnNavMeshGenerate();
	void			OnNavMeshShow();
	void			OnNavMeshSaveObj();
	void			OnNavMeshSaveNavMesh();
	void			OnUpdateUI_NavMeshShow(CCmdUI* pCmdUI);
	void			OnUpdateUI_NavMeshGenerate(CCmdUI* pCmdUI);
	void			OnUpdateUI_NavMeshSaveObj(CCmdUI* pCmdUI);
	void			OnUpdateUI_NavMeshSaveNavMesh(CCmdUI* pCmdUI);
	///Object
	template<int mode> void OnObjectEdit();
	template<int mode> void	OnUpdateUI_ObjectEdit(CCmdUI* pCmdUI);
	///Animation
	void			OnUpdateUI_AnimNames(CCmdUI* pCmdUI);
	void			OnAnimSelectChange(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnUpdateUI_AnimPlay(CCmdUI* pCmdUI);
	void			OnUpdateUI_AnimStop(CCmdUI* pCmdUI);
	void			OnAnimPlay();
	void			OnAnimStop();

	///GameData
	void			OnUpdateUI_DataBuilding(CCmdUI* pCmdUI);
	void			OnDataBuilding();		
	///Effect
	void			OnUpdateUI_ShadowOnOff(CCmdUI* pCmdUI);
	void			OnShadowOnOff();
	void			OnUpdateUI_SSAOOnOff(CCmdUI* pCmdUI);
	void			OnSSAOOnOff();

private:
	CXTPDockingPaneManager	m_paneManager;
	CEditorView*			m_wndView;
	CXTPTaskPanel			m_resourceSelector;		//mesh选择控件
	PropertyPaneTerrain*	m_propertyTerrain;		//地形属性面板
	CXTPControlGalleryItems* m_terrainTexGallery;	//地形纹理控件
	PropertyPaneObject*		m_propertyObject;		//物体属性面板
	PropertyPaneEffect*		m_propertyEffect;		//特效属性面板
	PropertyPaneAttachment*	m_propertyAttachment;	//挂接物属性面板

	CXTPRibbonTab*			m_animTab;
	CXTPControlComboBox*	m_animList;

	CXTPDockingPane*		m_paneResSelector;		
	CXTPDockingPane*		m_paneTerrain;
	CXTPDockingPane*		m_paneObject;
	CXTPDockingPane*		m_paneEffect;
	CXTPDockingPane*		m_paneAttachment;

	typedef std::unordered_map<int, bool> DialogVisibleFlags;
	DialogVisibleFlags		m_dlgVisibleFlags;
};


