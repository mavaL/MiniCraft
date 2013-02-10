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
public:
	Building();
	~Building() {}

public:
	virtual eObjectType GetType() const { return eObjectType_Building; }
	virtual void	Update(float dt);
	virtual void	_OnCommandFinished(eCommandType cmd);

public:
	void			SetBuildingName(const STRING& name) { m_buildingName = name; }
	float			GetCurProgress() const { return m_fCurProgress; }

private:
	STRING			m_buildingName;
	float			m_fCurProgress;		//若当前正在生产状态,已生产的时间
};


#endif // Building_h__