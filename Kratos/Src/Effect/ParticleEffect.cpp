#include "ParticleEffect.h"
#include <ParticleUniverseSystemManager.h>
#include "OgreManager.h"

using namespace ParticleUniverse;

namespace Kratos
{
	IMPL_PARAM_COMMAND_STR(ParticleEffect, ParticleTemplate)

	ParticleEffect::ParticleEffect(Ogre::Entity* parent)
	:AttachEffectBase(parent)
	,m_particle(nullptr)
	,m_template(Ogre::StringUtil::BLANK)
	{
		Ogre::ParamDictionary* dict = getParamDictionary();
		dict->addParameter(Ogre::ParameterDef("template", "template name of this particle", Ogre::PT_STRING), &m_sCmdParticleTemplate);
		m_type = eAttachEffect_Particle;
	}

	ParticleEffect::~ParticleEffect()
	{
	}

	void ParticleEffect::SetParticleTemplate( const STRING& name )
	{
		if(m_particle)
		{
			ParticleSystemManager::getSingleton().destroyParticleSystem(m_particle, RenderManager.m_pSceneMgr);
			m_particle = nullptr;
		}
		m_state = eEffectState_Uninit;
		m_template = name;
	}

	void ParticleEffect::Start()
	{
		if (!m_particle && !m_template.empty())
		{
			assert(m_state == eEffectState_Uninit);
			m_particle = ParticleSystemManager::getSingleton().createParticleSystem(m_name, m_template, RenderManager.m_pSceneMgr);
			assert(m_particle);
		}
		else if (m_state == eEffectState_Uninit)
		{
			m_parent->attachObjectToBone(m_locator, m_particle);
		}

		__super::Start();
	}

	void ParticleEffect::Stop()
	{
		if(m_particle)
			m_particle->stop();
		__super::Stop();
	}

	void ParticleEffect::Pause()
	{
		m_particle->pause();
		__super::Pause();
	}

	void ParticleEffect::Resume()
	{
		m_particle->resume();
		__super::Resume();
	}

	void ParticleEffect::Update( float dt )
	{
		m_fCurTime += dt;
		if (m_state == eEffectState_Prepared)
		{
			if (m_fCurTime >= m_fStartTime)
			{
				//真正开始播放
				m_particle->start();
				m_state = eEffectState_Started;
			}
		}
		else if (m_state == eEffectState_Started)
		{
			//LifeTime为0则使用粒子本身的长度
			bool bLifeEnd = m_fLifeTime > 0 && m_fCurTime >= m_fLifeTime;
			if (m_particle->getState() == ParticleSystem::PSS_STOPPED || bLifeEnd)
				Stop();
		}
	}

	void ParticleEffect::Destroy()
	{
		if(m_particle)
		{
			Stop();
			if(m_particle->getParentNode())
				m_particle->detachFromParent();
			ParticleSystemManager::getSingleton().destroyParticleSystem(m_particle, RenderManager.m_pSceneMgr);
			m_particle = nullptr;
		}
		__super::Destroy();
	}
}