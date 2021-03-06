#include "stdafx.h"
#include "BehaviorComponent.h"
#include "BehaviorTreeTemplate.h"
#include "BehaviorTreeTemplateManager.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "Behavior.h"
#include "SelectableObject.h"
#include "Unit.h"
#include "ScriptSystem.h"
#include "ConcreteBehavior.h"

BehaviorComponent::BehaviorComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_pTemplate(nullptr)
,m_pOwnBB(new Kratos::aiBlackBoard)
,m_pCurBehavior(nullptr)
{
	assert(m_pOwner->GetType() == eObjectType_Unit);
}

BehaviorComponent::~BehaviorComponent()
{
	SAFE_DELETE(m_pOwnBB);
}

void BehaviorComponent::InitBehaviors()
{
	Kratos::aiBehaviorTreeTemplateManager& btMgr = Kratos::aiBehaviorTreeTemplateManager::GetSingleton();
	btMgr.AddBehavior("Idle", new aiBehaviorIdle);
	btMgr.AddBehavior("MoveToEnemyBase", new aiBehaviorMoveToEnemyBase);
	btMgr.AddBehavior("MoveToBase", new aiBehaviorMoveToBase);
	btMgr.AddBehavior("MoveToRes", new aiBehaviorMoveToRes);
	btMgr.AddBehavior("GatherRes", new aiBehaviorGathering);
	btMgr.AddBehavior("RetriveRes", new aiBehaviorRetriveRes);
	btMgr.AddBehavior("ReturnRes", new aiBehaviorReturnRes);
	btMgr.AddBehavior("AttackTarget", new aiBehaviorAttackTarget);
}

void BehaviorComponent::SetTemplate( const STRING& name )
{
	m_pTemplate = Kratos::aiBehaviorTreeTemplateManager::GetSingleton().GetTemplate(name);
	m_pTemplate->CloneBlackBoard(*m_pOwnBB);
}

void BehaviorComponent::Update( float dt )
{
	if(!m_pTemplate)
		return;

	//更新黑板
	SCRIPTNAMAGER.Call(m_pTemplate->GetBBScriptEntry().c_str(), m_pOwner->GetID());

	//更新该单位的行为
	STRING newBehavior("");
	Kratos::eEvalState state = m_pTemplate->GetBT()->Evaluate(m_pOwnBB, newBehavior);
	assert(state == Kratos::eEvalState_Success);

	Ogre::Any any(static_cast<Unit*>(m_pOwner));
	Kratos::aiBehavior* pNewBehavior = Kratos::aiBehaviorTreeTemplateManager::GetSingleton().GetBehavior(newBehavior);

	if (m_pCurBehavior != pNewBehavior)
	{
		if(m_pCurBehavior)
			m_pCurBehavior->Exit(any);
		
		pNewBehavior->Execute(any);
		m_pCurBehavior = pNewBehavior;
	}

	m_pCurBehavior->Update(any, dt);
}

void BehaviorComponent::UpdateBehaviors()
{
	//更新行为树全局黑板
	SCRIPTNAMAGER.Call("GlobalBBUpdate_Terran");
	SCRIPTNAMAGER.Call("GlobalBBUpdate_Zerg");
}


