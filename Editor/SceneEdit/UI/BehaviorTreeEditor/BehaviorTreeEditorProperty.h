/********************************************************************
	created:	31:3:2013   21:52
	filename	BehaviorTreeEditorProperty.h
	author:		maval

	purpose:	行为树编辑器模块
*********************************************************************/

#ifndef BehaviorTreeEditorProperty_h__
#define BehaviorTreeEditorProperty_h__

#include "..\PropertiesPane.h"
#include "Manipulator/ManipulatorGameData.h"
#include "BehaviorTreeEditorView.h"

class PropertyPaneBehaviorTree : public CPropertiesPane
{
public:
	PropertyPaneBehaviorTree();
	~PropertyPaneBehaviorTree() {}

	enum ePropertyID
	{
		////property range [propStart, propEnd)
		propStart = 0,
		////////These are Mutable items
		propMutableItemStart = propStart,
		propConditon = propMutableItemStart,		//条件节点表达式
		propAction,									//行为项
		propBBParamName,							//黑板参数名称
		propBBParamValue,							//黑板参数值
		propBBParamType,							//黑板参数类型
		propMutableItemEnd,
		propEnd = propMutableItemEnd
	};

	enum eCategory
	{
		eCategory_SequenceNode,
		eCategory_ConditionNode,
		eCategory_ActionNode,
		eCategory_Blackboard,
	};

public:
	void			SetView(BehaviorTreeEditorView* pView) { m_pView = pView; }
	void			OnNodeSelected(eFlowGraphNodeType nodeType, int id = -1);

protected:
	virtual	bool	_OnCreate();
	virtual	void	_SetProperty(int nID);
	virtual	void	_UpdateProperty(int nID);
	virtual int		_GetIDStart()		{ return propStart; }
	virtual int		_GetIDEnd()			{ return propEnd; }
	virtual int		_GetIDMutableStart() { return propMutableItemStart; }
	virtual int		_GetIDMutableEnd()	{ return propMutableItemEnd; }

private:
	ManipulatorGameData::Blackboard*	_GetCurBB();

private:
	ManipulatorGameData::BTTemplate::SBTNode*	m_curNode;
	int											m_curBBParamIndex;
	BehaviorTreeEditorView*						m_pView;
	eFlowGraphNodeType							m_curNodeType;
};

class BehaviorTreeEditorProperty : public CDialog	
{
public:
	BehaviorTreeEditorProperty(CXTPDialog* parent);
	~BehaviorTreeEditorProperty() {}

public:
	void			SetView(BehaviorTreeEditorView* pView);
	void			OnNodeSelected(eFlowGraphNodeType nodeType, int id = -1);

protected:
	virtual BOOL	OnInitDialog();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnSize(UINT nType, int cx, int cy);

private:
	PropertyPaneBehaviorTree	m_propertyBT;
};


#endif // BehaviorTreeEditorProperty_h__














