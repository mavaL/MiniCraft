/********************************************************************
	created:	31:3:2013   21:52
	filename	BehaviorTreeEditorProperty.h
	author:		maval

	purpose:	ÐÐÎªÊ÷±à¼­Æ÷Ä£¿é
*********************************************************************/

#ifndef BehaviorTreeEditorProperty_h__
#define BehaviorTreeEditorProperty_h__

#include "..\PropertiesPane.h"

class PropertyPaneBehaviorTree : public CPropertiesPane
{
public:
	PropertyPaneBehaviorTree():CPropertiesPane() {}
	~PropertyPaneBehaviorTree() {}

protected:
	virtual	bool	_OnCreate();
	virtual int		_GetIDStart() { return -1; }
	virtual int		_GetIDEnd() { return -1; }
	virtual int		_GetIDMutableStart() { return -1; }
	virtual int		_GetIDMutableEnd() { return -1; }
};

class BehaviorTreeEditorProperty : public CDialog	
{
public:
	BehaviorTreeEditorProperty(CXTPDialog* parent);
	~BehaviorTreeEditorProperty() {}

protected:
	virtual BOOL	OnInitDialog();

private:
	DECLARE_MESSAGE_MAP()
	afx_msg	void	OnSize(UINT nType, int cx, int cy);

private:
	CXTPDialog*					m_parent;
	PropertyPaneBehaviorTree	m_propertyBT;
};


#endif // BehaviorTreeEditorProperty_h__














