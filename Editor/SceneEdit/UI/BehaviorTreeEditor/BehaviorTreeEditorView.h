/********************************************************************
	created:	31:3:2013   21:53
	filename	BehaviorTreeEditorView.h
	author:		maval

	purpose:	ÐÐÎªÊ÷±à¼­Æ÷Ä£¿é
*********************************************************************/

#ifndef BehaviorTreeEditorView_h__
#define BehaviorTreeEditorView_h__

class BehaviorTreeEditorView : public CWnd
{
public:
	BehaviorTreeEditorView() {}
	~BehaviorTreeEditorView() {}

protected:
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);

private:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg	void	OnSize(UINT nType, int cx, int cy);

	CXTPFlowGraphControl	m_wndControl;
};


#endif // BehaviorTreeEditorView_h__














