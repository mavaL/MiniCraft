/********************************************************************
	created:	31:3:2013   21:52
	filename	BehaviorTreeEditorProperty.h
	author:		maval

	purpose:	ÐÐÎªÊ÷±à¼­Æ÷Ä£¿é
*********************************************************************/

#ifndef BehaviorTreeEditorProperty_h__
#define BehaviorTreeEditorProperty_h__

#include "..\PropertiesPane.h"
#include "Manipulator/ManipulatorGameData.h"

class BehaviorTreeEditorView;

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
		propConditon = propMutableItemStart,
		propAction,
		propMutableItemEnd,
		propEnd = propMutableItemEnd
	};

public:
	void			SetView(BehaviorTreeEditorView* pView) { m_pView = pView; }
	void			SetCurNode(ManipulatorGameData::BTTemplate::SBTNode* node);

protected:
	virtual	bool	_OnCreate();
	virtual	void	_SetProperty(int nID);
	virtual	void	_UpdateProperty(int nID);
	virtual int		_GetIDStart()		{ return propStart; }
	virtual int		_GetIDEnd()			{ return propEnd; }
	virtual int		_GetIDMutableStart() { return propMutableItemStart; }
	virtual int		_GetIDMutableEnd()	{ return propMutableItemEnd; }

private:
	ManipulatorGameData::BTTemplate::SBTNode*	m_curNode;
	BehaviorTreeEditorView*						m_pView;
};

class BehaviorTreeEditorProperty : public CDialog	
{
public:
	BehaviorTreeEditorProperty(CXTPDialog* parent);
	~BehaviorTreeEditorProperty() {}

public:
	void			SetView(BehaviorTreeEditorView* pView);
	void			OnNodeSelected(bool bSelected, int id = -1);

protected:
	virtual BOOL	OnInitDialog();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnSize(UINT nType, int cx, int cy);

private:
	BehaviorTreeEditorView*		m_pView;
	PropertyPaneBehaviorTree	m_propertyBT;
};


#endif // BehaviorTreeEditorProperty_h__














