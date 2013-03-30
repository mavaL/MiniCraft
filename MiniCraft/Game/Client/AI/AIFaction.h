/********************************************************************
	created:	27:3:2013   23:56
	filename	AIFaction.h
	author:		maval

	purpose:	代表一个AI阵营
*********************************************************************/

#ifndef AIFaction_h__
#define AIFaction_h__

#include "Faction.h"

class FactionAI : public Faction
{
public:
	FactionAI(eGameRace race);
	~FactionAI() {}

public:
	virtual void	Update(float dt);

private:
	float	m_fPastTime;
};

#endif // AIFaction_h__