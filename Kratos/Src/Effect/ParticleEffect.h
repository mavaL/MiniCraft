/********************************************************************
	created:	11:3:2013   13:35
	author:		maval
	
	purpose:	粒子特效类
*********************************************************************/

#ifndef ParticleEffect_h__
#define ParticleEffect_h__

#include <OgreStringInterface.h>
#include "KratosPrerequisites.h"

namespace ParticleUniverse
{
	class ParticleSystem;
}

namespace Kratos
{
	class ParticleEffect : public Ogre::StringInterface
	{
		DECL_PARAM_COMMAND(Name)
		DECL_PARAM_COMMAND(ParticleTemplate)
		DECL_PARAM_COMMAND(Locator)
		DECL_PARAM_COMMAND(StartTime)
		DECL_PARAM_COMMAND(LifeTime)

		enum eEffectState
		{
			eEffectState_Uninit,
			eEffectState_Prepared,
			eEffectState_Started,
			eEffectState_Stoped,
			eEffectState_Paused,
		};

	public:
		ParticleEffect(Ogre::Entity* parent);
		~ParticleEffect();

	public:
		void	Start();
		void	Stop();
		void	Pause();
		void	Resume();
		void	Update(float dt);
		void	Destroy();

		void			SetParticleTemplate(const STRING& name);
		const STRING&	GetParticleTemplate() const			{ return m_template; }
		void			SetStartTime(float fStartTime)		{ m_fStartTime = fStartTime; }
		float			GetStartTime() const				{ return m_fStartTime; }
		void			SetLifeTime(float fLifeTime)		{ m_fLifeTime = fLifeTime; }
		float			GetLifeTime() const					{ return m_fLifeTime; }
		void			SetLocator(const STRING& locator)	{ m_locator = locator; }
		const STRING&	GetLocator() const					{ return m_locator; }
		void			SetName(const STRING& name)			{ m_name = name; }
		const STRING&	GetName() const						{ return m_name; }

		//编辑器用
		void			_SetParent(Ogre::Entity* ent);

	private:
		Ogre::Entity*	m_parent;
		STRING			m_name;
		STRING			m_template;			//粒子模板
		STRING			m_locator;			//挂接点
		ParticleUniverse::ParticleSystem*	m_particle;
		float			m_fStartTime;
		float			m_fLifeTime;
		float			m_fCurTime;
		eEffectState	m_state;
	};
}

#endif // ParticleEffect_h__