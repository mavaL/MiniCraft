/********************************************************************
	created:	31:3:2013   21:53
	filename	BehaviorTreeEditorView.h
	author:		maval

	purpose:	ÐÐÎªÊ÷±à¼­Æ÷Ä£¿é
*********************************************************************/

#ifndef BehaviorTreeEditorView_h__
#define BehaviorTreeEditorView_h__

#include "Manipulator/ManipulatorGameData.h"

class BehaviorTreeEditorProperty;

class BehaviorTreeEditorView : public CWnd
{
public:
	BehaviorTreeEditorView(CXTPDialog* parent);
	~BehaviorTreeEditorView();

public:
	void			SetPropertyDlg(BehaviorTreeEditorProperty* pProp);
	void			SetActiveItem(const std::wstring& name);
	void			Arrange();
	void			Refresh();
	ManipulatorGameData::BTTemplate* GetActiveTemplate() { return m_curTmpl; }

protected:
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);

private:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg	void	OnSize(UINT nType, int cx, int cy);

	BehaviorTreeEditorProperty*			m_pProp;
	CXTPFlowGraphControl				m_wndControl;
	CXTPFlowGraphPage*					m_page;
	ManipulatorGameData::BTTemplate*	m_curTmpl;
};


#endif // BehaviorTreeEditorView_h__














