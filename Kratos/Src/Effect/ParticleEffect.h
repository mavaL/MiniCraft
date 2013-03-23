/********************************************************************
	created:	11:3:2013   13:35
	author:		maval
	
	purpose:	粒子特效类
*********************************************************************/

#ifndef ParticleEffect_h__
#define ParticleEffect_h__

#include "EffectBase.h"

namespace ParticleUniverse
{
	class ParticleSystem;
}

namespace Kratos
{
	class ParticleEffect : public AttachEffectBase
	{
		DECL_PARAM_COMMAND(ParticleTemplate)

	public:
		ParticleEffect(Ogre::Entity* parent);
		~ParticleEffect();

	public:
		virtual void	Start();
		virtual void	Stop();
		virtual void	Pause();
		virtual void	Resume();
		virtual void	Update(float dt);
		virtual void	Destroy();

		void			SetParticleTemplate(const STRING& name);
		const STRING&	GetParticleTemplate() const			{ return m_template; }

	private:
		STRING			m_template;			//粒子模板
		ParticleUniverse::ParticleSystem*	m_particle;
	};
}

#endif // ParticleEffect_h__