#include "stdafx.h"
#include "RagdollComponent.h"
#include "SelectableObject.h"
#include "Physics/Ragdoll.h"
#include "AnimatedComponent.h"
#include "OgreManager.h"
#ifdef _DEBUG
#include "BoneGizmo.h"
#endif

#if USE_PHYSICS	==	1

RagdollComponent::RagdollComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_bStart(false)
#ifdef _DEBUG
,m_bShowGizmo(false)
,m_pRagdoll(nullptr)
#endif
{
	assert(m_pOwner->GetType() == eObjectType_Unit);
	m_animCp = m_pOwner->GetAnim();
}

RagdollComponent::~RagdollComponent()
{
	SAFE_DELETE(m_pRagdoll);
#ifdef _DEBUG
	for(size_t i=0; i<m_boneGizmos.size(); ++i)
		SAFE_DELETE(m_boneGizmos[i]);
	m_boneGizmos.clear();
#endif
}

void RagdollComponent::SetRigFile( const STRING& filename )
{
	m_pRagdoll = new Kratos::Ragdoll(m_pOwner->GetEntity(), m_pOwner->GetSceneNode(), filename);
}

void RagdollComponent::StartRagdoll()
{
	assert(m_pRagdoll);
	m_animCp->SetManuallyControlBones();
	m_animCp->StopAnimation();
	m_pRagdoll->StartRagdoll();
	m_bStart = true;
}

void RagdollComponent::Update( float dt )
{
	if(m_bStart)
		m_pRagdoll->UpdateRagdoll();

#ifdef _DEBUG
	if (m_bShowGizmo)
	{
		for(size_t i=0; i<m_boneGizmos.size(); ++i)
			m_boneGizmos[i]->Update();
	}
#endif
}

#ifdef _DEBUG
void RagdollComponent::ShowSkeleton( bool bShow )
{
	//TODO: 渲染批次太多了,instancing
	if (!m_bShowGizmo)
	{
		Ogre::SkeletonInstance* pSkel = m_pOwner->GetEntity()->getSkeleton();
		for (unsigned short b = 0; b < pSkel->getNumBones()/4; ++b)
		{
			Ogre::Bone* bone = pSkel->getBone(b);
			GizmoAxis* gizmo = new GizmoAxis;
			gizmo->Attach(bone);

			m_boneGizmos.push_back(gizmo);
		}
		m_bShowGizmo = true;
	}
	
	for(size_t i=0; i<m_boneGizmos.size(); ++i)
		m_boneGizmos[i]->Show(bShow);
}

#endif

#endif
