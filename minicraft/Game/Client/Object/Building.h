/********************************************************************
	created:	24:1:2013   0:34
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Object\Building.h
	author:		maval
	
	purpose:	建筑类
*********************************************************************/

#ifndef Building_h__
#define Building_h__

#include "SelectableObject.h"
#include "Command.h"

class Building : public SelectableObject
{
	DECL_PARAM_COMMAND(RallyPoint)
public:
	Building();
	~Building() {}

public:
	virtual eObjectType GetType() const { return eObjectType_Building; }
	virtual void	Update(float dt);
	virtual void	_OnCommandFinished(eCommandType cmd);

public:
	void			Init(const STRING& name, const POS& pos, const ORIENT& orient, const SCALE& scale);
	const STRING&	GetBuildingName() const {return m_buildingName; } 
	float			GetCurProgress() const { return m_fCurProgress; }
	void			SetRallyPoint(const POS& pos) { m_rallyPoint = pos; }
	const POS&		GetRallyPoint() const	{ return m_rallyPoint; }


private:
	STRING			m_buildingName;		//单位名称,如:barrack...
	float			m_fCurProgress;		//若当前正在生产状态,已生产的时间
	POS				m_rallyPoint;		//集结点
};


#endif // Building_h__