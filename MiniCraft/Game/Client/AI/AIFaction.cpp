#include "stdafx.h"
#include "AIFaction.h"
#include "ObjectManager.h"
#include "Unit.h"
#include "SelectableObject.h"
#include "Building.h"
#include "PathComponent.h"
#include "BehaviorComponent.h"
#include "BehaviorTreeTemplateManager.h"
#include "AIComponent.h"

FactionAI::FactionAI( eGameRace race )
:Faction(race)
,m_fPastTime(0)
{
	
}

void FactionAI::Init()
{
	POS rallyPos = m_base->GetRallyPoint();
	rallyPos.z -= 1;

	for (int i=0; i<3; ++i)
	{
		for (int j=0; j<3; ++j)
		{
			STRING unitName;
			if(m_race == eGameRace_Terran)
				unitName = "Marine";
			else if(m_race == eGameRace_Zerg)
				unitName = "Zergling";

			Unit* pUnit = static_cast<Unit*>(ObjectManager::GetSingleton().CreateObject(eObjectType_Unit));
			pUnit->setParameter("name", unitName);
			pUnit->Init();
			pUnit->GetAi()->SetCpuControl(true);
			pUnit->SetPosition(rallyPos);
			pUnit->AddComponent(eComponentType_Path, new PathComponent(pUnit));
			pUnit->AddComponent(eComponentType_Behevior, new BehaviorComponent(pUnit));
			pUnit->GetBehavior()->SetTemplate(unitName);

			rallyPos.x -= 1;
		}
		rallyPos.x = m_base->GetRallyPoint().x;
		rallyPos.z -= 1;
	}
}

void FactionAI::Update( float dt )
{
	
}


