/********************************************************************
	created:	20:1:2013   22:41
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Faction.h
	author:		maval
	
	purpose:	Faction代表一个玩家阵营
*********************************************************************/

#ifndef Faction_h__
#define Faction_h__

#include "GameDefine.h"
#include "KratosPrerequisites.h"

class Building;

class Faction
{
public:
	Faction(eGameRace race);
	~Faction() {}

public:
	void			SetBase(Building* pBase) { m_base = pBase; }
	Building*		GetBase()	{ return m_base; }
	void			AddMineral(int num) { m_mineral +=num; }
	void			SetTeamColor(const COLOR& color) { m_teamColor = color; }
	const COLOR&	GetTeamColor() const { return m_teamColor; }

private:
	eGameRace	m_race;
	int			m_mineral;		//金钱数
	Building*	m_base;			//测试基地
	COLOR		m_teamColor;	//队伍颜色
};

#endif // Faction_h__