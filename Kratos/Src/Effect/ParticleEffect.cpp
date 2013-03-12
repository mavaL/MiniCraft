#include "ParticleEffect.h"
#include <ParticleUniverseSystemManager.h>
#include "OgreManager.h"

using namespace ParticleUniverse;

namespace Kratos
{
	IMPL_PARAM_COMMAND_STR(ParticleEffect, ParticleTemplate)
	IMPL_PARAM_COMMAND_STR(ParticleEffect, Locator)
	IMPL_PARAM_COMMAND(ParticleEffect, StartTime, Real)
	IMPL_PARAM_COMMAND(ParticleEffect, LifeTime, Real)

	ParticleEffect::ParticleEffect(Ogre::Entity* parent)
	:m_parent(parent)
	,m_particle(nullptr)
	,m_fStartTime(0)
	,m_fLifeTime(0)
	,m_fCurTime(0)
	,m_state(eEffectState_Uninit)
	,m_template(Ogre::StringUtil::BLANK)
	,m_locator(Ogre::StringUtil::BLANK)
	{
		if (createParamDictionary("ParticleEffect"))
		{
			Ogre::ParamDictionary* dict = getParamDictionary();
			dict->addParameter(Ogre::ParameterDef("template", "template name of this particle", Ogre::PT_STRING), &m_sCmdParticleTemplate);
			dict->addParameter(Ogre::ParameterDef("locator", "bone name to which this particle attach", Ogre::PT_STRING), &m_sCmdLocator);
			dict->addParameter(Ogre::ParameterDef("starttime", "effect start time relative to animation", Ogre::PT_REAL), &m_sCmdStartTime);
			dict->addParameter(Ogre::ParameterDef("lifetime", "effect's life time", Ogre::PT_REAL), &m_sCmdLifeTime);
		}
	}

	ParticleEffect::~ParticleEffect()
	{
		Destroy();
	}

	void ParticleEffect::SetParticleTemplate( const STRING& name )
	{
		m_template = name;
		m_state = eEffectState_Uninit;
	}

	void ParticleEffect::Start()
	{
		if (!m_particle || m_state == eEffectState_Uninit)
		{
			m_particle = ParticleSystemManager::getSingleton().createParticleSystem(m_template, RenderManager.m_pSceneMgr);
			assert(m_particle);
			m_parent->attachObjectToBone(m_locator, m_particle);
			Stop();
		}

		assert(m_particle && m_state == eEffectState_Stoped);
		m_fCurTime = 0;
		m_state = eEffectState_Prepared;
	}

	void ParticleEffect::Stop()
	{
		if(m_particle)
			m_particle->stop();
		m_state = eEffectState_Stoped;
	}

	void ParticleEffect::Pause()
	{
		m_particle->pause();
		m_state = eEffectState_Paused;
	}

	void ParticleEffect::Resume()
	{
		m_particle->resume();
		m_state = eEffectState_Started;
	}

	void ParticleEffect::Update( float dt )
	{
		if (m_state == eEffectState_Prepared)
		{
			m_fCurTime += dt;
			if (m_fCurTime >= m_fStartTime)
			{
				//真正开始播放
				m_particle->start();
				m_state = eEffectState_Started;
			}
		}
		else if (m_state == eEffectState_Started)
		{
			if (m_particle->getState() == ParticleSystem::PSS_STOPPED)
			{
				//粒子已经播放结束,同步状态
				Stop();
			}
		}
	}

	void ParticleEffect::Destroy()
	{
		if(m_particle)
		{
			Stop();
			m_particle->detachFromParent();
			ParticleSystemManager::getSingleton().destroyParticleSystem(m_particle, RenderManager.m_pSceneMgr);
			m_particle = nullptr;
		}
		m_state = eEffectState_Uninit;
	}


}