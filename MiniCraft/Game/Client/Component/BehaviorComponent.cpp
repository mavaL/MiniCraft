#include "stdafx.h"
#include "BehaviorComponent.h"
#include "BehaviorTreeTemplate.h"
#include "BehaviorTreeTemplateManager.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "Behavior.h"
#include "SelectableObject.h"
#include "Unit.h"

BehaviorComponent::BehaviorComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_pTemplate(nullptr)
,m_pOwnParams(new aiBlackBoard)
,m_curBehavior(Ogre::StringUtil::BLANK)
{
	assert(m_pOwner->GetType() == eObjectType_Unit);
}

BehaviorComponent::~BehaviorComponent()
{
	SAFE_DELETE(m_pOwnParams);
}

void BehaviorComponent::SetTempalte( const STRING& name )
{
	m_pTemplate = aiBehaviorTreeTemplateManager::GetSingleton().GetTemplate(name);
	m_pTemplate->CloneBlackBoard(*m_pOwnParams);
}

void BehaviorComponent::Update( float dt )
{
	//更新该单位的行为
	STRING newBehavior("");
	m_pTemplate->GetBT()->Evaluate(m_pOwnParams, newBehavior);

	if (m_curBehavior != newBehavior)
	{
		aiBehaviorTreeTemplateManager::GetSingleton().GetBehavior(newBehavior)->Execute(static_cast<Unit*>(m_pOwner));
		m_curBehavior = newBehavior;
	}
}


