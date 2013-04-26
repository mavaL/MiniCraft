/********************************************************************
	created:	11:4:2013   23:33
	filename	RagdollComponent.h
	author:		maval

	purpose:	Ragdoll×é¼þ
*********************************************************************/

#ifndef RagdollComponent_h__
#define RagdollComponent_h__

#include "Component.h"
#include "KratosPrerequisites.h"

class AnimatedComponent;
class GizmoAxis;

class RagdollComponent : public Component
{
public:
	RagdollComponent(SelectableObject* pOwner);
	~RagdollComponent();

public:
	virtual void	Update(float dt);
	void			SetRigFile(const STRING& filename);
	void			StartRagdoll();
#ifdef _DEBUG
	void			ShowSkeleton(bool bShow);
#endif

private:
	Kratos::Ragdoll*	m_pRagdoll;
	bool				m_bStart;
	AnimatedComponent*	m_animCp;
#ifdef _DEBUG
	std::vector<GizmoAxis*>	m_boneGizmos;
	bool				m_bShowGizmo;
#endif
};

#endif // RagdollComponent_h__