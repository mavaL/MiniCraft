#include "stdafx.h"
#include "Faction.h"


Faction::Faction(eGameRace race)
:m_race(race)
,m_mineral(0)
,m_base(nullptr)
,m_teamColor(COLOR::White)
,m_pEnemy(nullptr)
,m_unitNum(0)
{

}

void Faction::SetEnemy( Faction* enemy )
{
	assert(enemy != this && "Don't treat self as enemy..");
	m_pEnemy = enemy;
}
