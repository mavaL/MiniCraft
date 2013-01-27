#include "stdafx.h"
#include "Faction.h"

const STRING Faction::sNamePrefix = "EntFaction";

Faction::Faction()
:m_race((eGameRace)-1)
,m_teamColor(Ogre::ColourValue::White)
{

}
