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
class BehaviorTreeEditorExplorer;

enum eBTSelectionType
{
	eBTSelectionType_BT,
	eBTSelectionType_TreeNode,
	eBTSelectionType_OwnBlackboard,
	eBTSelectionType_GlobalBlackboard,
	eBTSelectionType_Connection,
	eBTSelectionType_None
};

class BehaviorTreeEditorView : public CWnd
{
public:
	BehaviorTreeEditorView(CXTPDialog* parent);
	~BehaviorTreeEditorView();

	void			SetPropertyDlg(BehaviorTreeEditorProperty* pProp);
	void			SetExplorer(BehaviorTreeEditorExplorer* pExp)	{ m_pExplorer = pExp; }

public:
	void			NewBT(const std::wstring& name);
	void			SetActiveItem(const std::wstring& name);
	void			Arrange();
	void			Sync();
	void			RefreshAll();
	void			RefreshTreeNode(ManipulatorGameData::BTTemplate::SBTNode* pNode);
	void			RefreshBlackboard(bool bOwnBB);
	ManipulatorGameData::BTTemplate* 
					GetActiveTemplate() { return m_curTmpl; }
	void			AddNewNode(ManipulatorGameData::eBTNodeType type);
	void			AddBlackboardParam(bool bOwnBB);
	ManipulatorGameData::BTTemplate::SBTNode*
					FindNodeByID(int id);
	CXTPFlowGraphNode* FindFgNodeByID(int id);
	std::wstring	GetFgElementName(eBTSelectionType type, CXTPFlowGraphElement* element);
	void			DeleteCurElement();

protected:
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);

private:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg	void	OnSize(UINT nType, int cx, int cy);
	void			_ConnectFgNodes(ManipulatorGameData::BTTemplate::SBTNode* node);
	std::wstring	_OnFgElementAdded(CXTPFlowGraphElement* element);
	void			_OnXtpConnectionChanging(NMHDR* pNMHDR, LRESULT* pResult);
	void			_OnXtpConnectionChanged(NMHDR* pNMHDR, LRESULT* pResult);
	void			_OnFgElememtRemoved(const std::wstring& name);
	void			_OnFgConnectionRemoved(CXTPFlowGraphConnection* pConne);
	bool			_IsChildOf(ManipulatorGameData::BTTemplate::SBTNode* parent, ManipulatorGameData::BTTemplate::SBTNode* self);

	BehaviorTreeEditorProperty*			m_pProp;
	BehaviorTreeEditorExplorer*			m_pExplorer;
	CXTPFlowGraphControl				m_wndControl;
	CXTPFlowGraphPage*					m_page;
	ManipulatorGameData::BTTemplate*	m_curTmpl;
	CXTPFlowGraphNode*					m_ownBBNode;
	CXTPFlowGraphNode*					m_globalBBNode;

	typedef std::unordered_map<std::wstring, CXTPFlowGraphElement*>	FgElementContainer;
	FgElementContainer					m_elements;
};


#endif // BehaviorTreeEditorView_h__














