#include "stdafx.h"
#include "RagdollComponent.h"
#include "SelectableObject.h"
#include "Physics/Ragdoll.h"
#include "AnimatedComponent.h"

RagdollComponent::RagdollComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_bStart(false)
{
	assert(m_pOwner->GetType() == eObjectType_Unit);
	const STRING filename = "zombie_dismemberment_ragdoll_rot.hkt";
	m_pRagdoll = new Kratos::Ragdoll(m_pOwner->GetEntity(), m_pOwner->GetSceneNode(), filename);

	m_animCp = m_pOwner->GetAnim();
	m_animCp->PlayAnimation(eAnimation_Move, true);
}

RagdollComponent::~RagdollComponent()
{
	SAFE_DELETE(m_pRagdoll);
}

void RagdollComponent::StartRagdoll()
{
	m_animCp->SetManuallyControlBones();
	m_animCp->StopAnimation();
	m_pRagdoll->StartRagdoll();
	m_bStart = true;
}

void RagdollComponent::Update( float dt )
{
	if(m_bStart)
		m_pRagdoll->UpdateRagdoll();
}
