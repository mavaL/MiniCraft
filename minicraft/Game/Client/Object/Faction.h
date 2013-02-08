/********************************************************************
	created:	20:1:2013   22:41
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Faction.h
	author:		maval
	
	purpose:	Faction代表一个玩家阵营
*********************************************************************/

#ifndef Faction_h__
#define Faction_h__

#include "ObjectBase.h"
#include "GameDataDef.h"


class Faction : public Object
{
public:
	Faction();
	~Faction() {}

	static const STRING sNamePrefix;
	
public:
	virtual eObjectType GetType() const { return eObjectType_Faction; }
	virtual const STRING& GetNamePrefix() const { return sNamePrefix; }
	virtual	void		Update(float dt) {}

public:
	void				SetRace(eGameRace race) { m_race = race; }
	void				SetTeamColor(const COLOR& color) { m_teamColor = color; }

private:
	eGameRace			m_race;
	COLOR				m_teamColor;
};

#endif // Faction_h__