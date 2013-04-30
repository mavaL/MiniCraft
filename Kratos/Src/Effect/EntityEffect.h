/********************************************************************
	created:	30:4:2013   12:25
	filename	EntityEffect.h
	author:		maval

	purpose:	带动画的模型作为挂接特效.
				SC2很多这种Effect.
*********************************************************************/

#ifndef EntityEffect_h__
#define EntityEffect_h__


#include "EffectBase.h"

namespace Kratos
{
	class EntityEffect : public AttachEffectBase
	{
		DECL_PARAM_COMMAND(MeshName)
		DECL_PARAM_COMMAND(AnimName)

	public:
		EntityEffect(Ogre::Entity* parent);
		~EntityEffect();

	public:
		virtual void	Start();
		virtual void	Stop();
		virtual void	Update(float dt);
		virtual void	Destroy();

		void			SetMeshName(const STRING& meshname);
		const STRING	GetMeshName() const { return m_meshname;}
		void			SetAnimName(const STRING& animname);
		const STRING	GetAnimName() const { return m_animname;}

	private:
		STRING					m_meshname;
		STRING					m_animname;
		Ogre::AnimationState*	m_animState;
		Ogre::Entity*			m_pEntity;
	};
}

#endif // EntityEffect_h__