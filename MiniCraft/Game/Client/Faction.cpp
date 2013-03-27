#include "stdafx.h"
#include "Faction.h"


Faction::Faction(eGameRace race)
:m_race(race)
,m_mineral(0)
,m_base(nullptr)
,m_teamColor(COLOR::White)
,m_pEnemy(nullptr)
{

}
