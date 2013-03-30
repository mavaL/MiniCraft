#include "stdafx.h"
#include "ConcreteBehavior.h"
#include "Unit.h"
#include "AIComponent.h"
#include "PathComponent.h"
#include "Faction.h"
#include "Building.h"

void aiBehaviorIdle::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAi()->SetCurState(eObjectState_Idle);
}

/////////////////////////////////////////////////////////
void aiBehaviorMoveToEnemyBase::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	AiComponent* pAi = pUnit->GetAi();
	PathComponent* path = pUnit->GetPath();

	POS target = pAi->GetFaction()->GetEnemy()->GetBase()->GetPosition();

	if(path->FindPath(target, true))
	{
		path->SetDestPos(target);
		//Ö´ÐÐÑ°Â·ÃüÁî
		pAi->GiveCommand(Command(eCommandType_Move, pUnit), true);
	}
}

