#include "stdafx.h"
#include "AIFaction.h"
#include "ObjectManager.h"
#include "Unit.h"
#include "SelectableObject.h"
#include "Building.h"
#include "PathComponent.h"
#include "BehaviorComponent.h"

FactionAI::FactionAI( eGameRace race )
:Faction(race)
,m_fPastTime(0)
{

}

void FactionAI::Update( float dt )
{
	m_fPastTime += dt;
	///测试.每隔一段时间出生一个单位
	static const float BORN_INTERNAL = 5;
	if (m_fPastTime >= BORN_INTERNAL)
	{
		//限制10个
		if (m_unitNum <= 10)
		{
			STRING unitName;
			if(m_race == eGameRace_Terran)
				unitName = "Marine";
			else if(m_race == eGameRace_Zerg)
				unitName = "Zergling";

			SelectableObject* pNewObj = static_cast<SelectableObject*>(ObjectManager::GetSingleton().CreateObject(eObjectType_Unit));
			pNewObj->setParameter("name", unitName);
			static_cast<Unit*>(pNewObj)->Init();
			pNewObj->SetPosition(m_base->GetRallyPoint());
			pNewObj->AddComponent(eComponentType_Path, new PathComponent(pNewObj));

			//设置行为树
			pNewObj->AddComponent(eComponentType_Behevior, new BehaviorComponent(pNewObj));
			pNewObj->GetBehavior()->SetTempalte(unitName);

			++m_unitNum;
		}
		m_fPastTime = 0;
	}
}
