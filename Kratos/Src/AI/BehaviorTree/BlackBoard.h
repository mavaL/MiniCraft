/********************************************************************
	created:	26:3:2013   22:34
	filename	BlackBoard.h
	author:		maval

	purpose:	一棵行为树的参数字典.数据驱动
*********************************************************************/

#ifndef BlackBoard_h__
#define BlackBoard_h__

#include "KratosPrerequisites.h"
#include <OgreCommon.h>
#include <rapidxml.hpp>

class aiBehaviorTree;

class aiBlackBoard
{
public:
	aiBlackBoard();
	aiBlackBoard(aiBehaviorTree* parent);
	~aiBlackBoard() {}

	enum eVarType
	{
		eVarType_Int,
		eVarType_Float,
		eVarType_Bool
	};

	struct SValue
	{
		SValue() {}
		SValue(const STRING& val, eVarType type):m_value(val),m_type(type) {}
		STRING		m_value;
		eVarType	m_type;
	};

public:
	void			LoadParams(rapidxml::xml_node<>* node);
	//加入新参数到字典中
	void			DefineParam(const STRING& name, const STRING& value, eVarType type);
	void			Clone(aiBlackBoard& toClone);
	const aiBlackBoard::SValue&	GetParam(const STRING& name) const;
	bool			IsParamExists(const STRING& name) const;
	void			SetParam(const STRING& name, const STRING& value);
	aiBehaviorTree*	GetParent() const { return m_parent; }
	//判断参数是否不存在于该黑板,但存在于种族全局黑板
	bool			IsGlobalParam(const STRING& name) const;

private:
	aiBlackBoard(const aiBlackBoard&);
	aiBlackBoard& operator= (const aiBlackBoard&);

	typedef HashMap<STRING, SValue> ParamMap;
	ParamMap		m_params;

	aiBehaviorTree* m_parent;
};

#endif // BlackBoard_h__