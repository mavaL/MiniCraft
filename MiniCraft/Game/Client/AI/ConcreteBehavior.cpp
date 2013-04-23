#include "stdafx.h"
#include "ConcreteBehavior.h"
#include "Unit.h"
#include "AIComponent.h"
#include "PathComponent.h"
#include "Faction.h"
#include "Building.h"
#include "HarvestComponent.h"
#include "AnimatedComponent.h"
#include "Resource.h"

void aiBehaviorIdle::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAnim()->PlayAnimation(eAnimation_Idle, true);
}

void aiBehaviorIdle::Exit( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAnim()->StopAnimation();
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
		bool bSucceed = path->FindPath(target, false);
		assert(bSucceed);
		pUnit->GetAnim()->PlayAnimation(eAnimation_Move, true);
	}
}

void aiBehaviorMoveToEnemyBase::Update( Ogre::Any& owner, float dt )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetPath()->_UpdatePathFinding(dt);
}

void aiBehaviorMoveToEnemyBase::Exit( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAnim()->StopAnimation();
}

/////////////////////////////////////////////////////////
void aiBehaviorMoveToBase::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	AiComponent* pAi = pUnit->GetAi();
	PathComponent* path = pUnit->GetPath();

	POS target = pAi->GetFaction()->GetBase()->GetPosition();

	if(path->FindPath(target, true))
	{
		path->SetDestPos(target);
		bool bSucceed = path->FindPath(target, false);
		assert(bSucceed);
		pUnit->GetAnim()->PlayAnimation(eAnimation_Move, true);
	}
}

void aiBehaviorMoveToBase::Update( Ogre::Any& owner, float dt )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	if(pUnit->GetPath()->_UpdatePathFinding(dt))
		pUnit->GetGather()->SetCurStage(HarvestComponent::eHarvestStage_NearBase);
}

void aiBehaviorMoveToBase::Exit( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAnim()->StopAnimation();
}

/////////////////////////////////////////////////////////
void aiBehaviorMoveToRes::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	AiComponent* pAi = pUnit->GetAi();
	PathComponent* path = pUnit->GetPath();
	HarvestComponent* gather = pUnit->GetGather();

	POS target = gather->GetTarget()->GetPosition();

	if(path->FindPath(target, true))
	{
		path->SetDestPos(target);
		bool bSucceed = path->FindPath(target, false);
		assert(bSucceed);
		pUnit->GetAnim()->PlayAnimation(eAnimation_Move, true);
	}
}

void aiBehaviorMoveToRes::Update( Ogre::Any& owner, float dt )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	if(pUnit->GetPath()->_UpdatePathFinding(dt))
		pUnit->GetGather()->SetCurStage(HarvestComponent::eHarvestStage_NearRes);
}

void aiBehaviorMoveToRes::Exit( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAnim()->StopAnimation();
}

/////////////////////////////////////////////////////////
void aiBehaviorGathering::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	HarvestComponent* gather = pUnit->GetGather();

	gather->SetGatherTime(0);
	gather->SetCurStage(HarvestComponent::eHarvestStage_Gather);
	pUnit->GetAnim()->PlayAnimation(eAnimation_Gather, true);
}

void aiBehaviorGathering::Update( Ogre::Any& owner, float dt )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	HarvestComponent* gather = pUnit->GetGather();

	float fTime = gather->GetGatherTime() + dt;
	gather->SetGatherTime(fTime);
}

void aiBehaviorGathering::Exit( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAnim()->StopAnimation();
}

/////////////////////////////////////////////////////////
void aiBehaviorRetriveRes::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetGather()->SetResVisible(true);
	pUnit->GetGather()->SetCurStage(HarvestComponent::eHarvestStage_Return);
}

/////////////////////////////////////////////////////////
void aiBehaviorReturnRes::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	HarvestComponent* gather = pUnit->GetGather();

	gather->SetResVisible(false);
	gather->SetCurStage(HarvestComponent::eHarvestStage_None);
}

/////////////////////////////////////////////////////////
void aiBehaviorAttackTarget::Execute( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAnim()->PlayAnimation(eAnimation_Attack, true);
}

void aiBehaviorAttackTarget::Update( Ogre::Any& owner, float dt )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
}

void aiBehaviorAttackTarget::Exit( Ogre::Any& owner )
{
	Unit* pUnit = Ogre::any_cast<Unit*>(owner);
	pUnit->GetAnim()->StopAnimation();
}
