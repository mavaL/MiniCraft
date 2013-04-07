/********************************************************************
	created:	31:3:2013   21:48
	filename	BehaviorTreeEditorDlg.h
	author:		maval

	purpose:	ÐÐÎªÊ÷±à¼­Æ÷Ä£¿é
*********************************************************************/

#ifndef BehaviorTreeEditorDlg_h__
#define BehaviorTreeEditorDlg_h__

#include "resource.h"

class BehaviorTreeEditorView;
class BehaviorTreeEditorExplorer;
class BehaviorTreeEditorProperty;

class DialogBehaviorTreeEditor : public CXTPDialog
{
public:
	DialogBehaviorTreeEditor(CWnd* pParent = NULL);
	~DialogBehaviorTreeEditor();

public:
	virtual BOOL				OnInitDialog();
	BehaviorTreeEditorView*		GetView()	{ return m_pView; }
	BehaviorTreeEditorProperty*	GetProp()	{ return m_property; }
	BehaviorTreeEditorExplorer*	GetExplorer()	{ return m_pExplorer; }

private:
	DECLARE_MESSAGE_MAP()
	afx_msg	void				OnSize(UINT nType, int cx, int cy);
	void						OnUpdateUI_Btn(CCmdUI* pCmdUI);
	void						OnBtnArrange();
	void						OnBtnValidate();
	void						OnBtnSave();
	void						OnBtnAddOwnParam();
	void						OnBtnAddRaceParam();
	template<int type> void		OnBtnAddNode();
	void						OnBtnSync();
	void						OnUpdateUI_BtnDelete(CCmdUI* pCmdUI);
	void						OnBtnDelete();
	void						OnBtnRefresh();
	void						OnUpdateUI_BtnNewBT(CCmdUI* pCmdUI);
	void						OnBtnNewBT();

	
	void						_CreateRibbon();
	void						_CreateDockPane();
	LRESULT						_AttachDockPane(WPARAM wParam, LPARAM lParam);
	void						_LoadIcon();

private:
	CXTPDockingPaneManager		m_dockMgr;
	BehaviorTreeEditorView*		m_pView;
	BehaviorTreeEditorExplorer*	m_pExplorer;
	BehaviorTreeEditorProperty*	m_property;

	CXTPDockingPane*			m_paneExplorer;
	CXTPDockingPane*			m_paneProperty;
};


#endif // BehaviorTreeEditorDlg_h__














