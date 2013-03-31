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

BehaviorComponent::BehaviorComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_pTemplate(nullptr)
,m_pOwnBB(new aiBlackBoard)
,m_curBehavior(Ogre::StringUtil::BLANK)
{
	assert(m_pOwner->GetType() == eObjectType_Unit);
}

BehaviorComponent::~BehaviorComponent()
{
	SAFE_DELETE(m_pOwnBB);
}

void BehaviorComponent::SetTempalte( const STRING& name )
{
	m_pTemplate = aiBehaviorTreeTemplateManager::GetSingleton().GetTemplate(name);
	m_pTemplate->CloneBlackBoard(*m_pOwnBB);
}

void BehaviorComponent::Update( float dt )
{
	//更新黑板
	SCRIPTNAMAGER.Call(m_pTemplate->GetBBScriptEntry().c_str());

	//更新该单位的行为
	STRING newBehavior("");
	eEvalState state = m_pTemplate->GetBT()->Evaluate(m_pOwnBB, newBehavior);
	assert(state == eEvalState_Success);

	if (m_curBehavior != newBehavior)
	{
		Ogre::Any any(static_cast<Unit*>(m_pOwner));
		aiBehaviorTreeTemplateManager::GetSingleton().GetBehavior(newBehavior)->Execute(any);
		m_curBehavior = newBehavior;
	}
}


