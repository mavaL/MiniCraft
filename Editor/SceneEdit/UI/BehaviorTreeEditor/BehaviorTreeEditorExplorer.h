/********************************************************************
	created:	31:3:2013   22:11
	filename	BehaviorTreeEditorExplorer.h
	author:		maval

	purpose:	ÐÐÎªÊ÷±à¼­Æ÷Ä£¿é
*********************************************************************/

#ifndef BehaviorTreeEditorExplorer_h__
#define BehaviorTreeEditorExplorer_h__

class BehaviorTreeEditorView;

class BehaviorTreeEditorExplorer : public CDialog	
{
public:
	BehaviorTreeEditorExplorer(CXTPDialog* parent);
	~BehaviorTreeEditorExplorer() {}

public:
	void			SetView(BehaviorTreeEditorView* pView) { m_pView = pView; }

protected:
	virtual BOOL	OnInitDialog();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);

private:
	HWND					m_hList;
	BehaviorTreeEditorView*	m_pView;
};


#endif // BehaviorTreeEditorExplorer_h__














