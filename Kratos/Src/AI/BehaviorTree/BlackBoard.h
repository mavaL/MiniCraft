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

class aiBlackBoard
{
public:
	aiBlackBoard();
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
	//加入新参数到字典中
	void			DefineParam(const STRING& name, const STRING& value, eVarType type);
	void			Clone(aiBlackBoard& toClone);
	const aiBlackBoard::SValue&	GetParam(const STRING& name) const;
	void			SetParam(const STRING& name, const STRING& value);

private:
	aiBlackBoard(const aiBlackBoard&);
	aiBlackBoard& operator= (const aiBlackBoard&);

	typedef HashMap<STRING, SValue> ParamMap;
	ParamMap	m_params;
};

#endif // BlackBoard_h__