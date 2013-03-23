#include "DeferredLightEffect.h"
#include "OgreManager.h"
#include "DeferredShading/DLight.h"

namespace Kratos
{
	IMPL_PARAM_COMMAND(DLightEffect, LightType, Int)
	IMPL_PARAM_COMMAND(DLightEffect, LightRadius, Real)

	DLightEffect::DLightEffect(Ogre::Entity* parent)
	:AttachEffectBase(parent)
	,m_light(nullptr)
	,m_dlType(eDLightType_None)
	,m_fRadius(0)
	{
		Ogre::ParamDictionary* dict = getParamDictionary();
		dict->addParameter(Ogre::ParameterDef("lighttype", "Deferred light type", Ogre::PT_INT), &m_sCmdLightType);
		dict->addParameter(Ogre::ParameterDef("radius", "Radius of point light", Ogre::PT_REAL), &m_sCmdLightRadius);
		m_type = eAttachEffect_DLight;
		SetLightType(eDLightType_Point);
	}

	DLightEffect::~DLightEffect()
	{
		Destroy();
	}

	void DLightEffect::Start()
	{
		assert(m_light);
		if(m_state == eEffectState_Uninit)
			m_parent->attachObjectToBone(m_locator, m_light->getParentLight());

		__super::Start();
	}

	void DLightEffect::Stop()
	{
		m_light->getParentLight()->setVisible(false);
		__super::Stop();
	}

	void DLightEffect::Pause()
	{
		m_light->getParentLight()->setVisible(false);
		__super::Pause();
	}

	void DLightEffect::Resume()
	{
		m_light->getParentLight()->setVisible(true);
		__super::Resume();
	}

	void DLightEffect::Update( float dt )
	{
		m_fCurTime += dt;
		if (m_state == eEffectState_Prepared)
		{
			if (m_fCurTime >= m_fStartTime)
			{
				//真正开始
				m_light->getParentLight()->setVisible(true);
				m_state = eEffectState_Started;
			}
		}
		else if (m_state == eEffectState_Started)
		{
			bool bLifeEnd = m_fLifeTime > 0 && m_fCurTime >= m_fLifeTime;
			if (bLifeEnd)
				Stop();
		}
	}

	void DLightEffect::Destroy()
	{
		Stop();
		RenderManager.DestroyDLight(m_light);
		m_light = nullptr;
		m_dlType = eDLightType_None;

		__super::Destroy();
	}

	void DLightEffect::SetLightType( int type )
	{
		if(m_dlType == type)
			return;
		Destroy();

		m_light = RenderManager.CreateDLight(type);
		assert(m_light);
		SetLightRadius(m_fRadius);
		m_light->getParentLight()->setVisible(false);
	}

	void DLightEffect::SetLightRadius( float fRadius )
	{
		m_fRadius = fRadius;
		Ogre::Light* lightSrc = m_light->getParentLight();
		lightSrc->setAttenuation(m_fRadius, 1, 0, 0);
		m_light->setAttenuation(lightSrc->getAttenuationConstant(), lightSrc->getAttenuationLinear(), lightSrc->getAttenuationQuadric());	
	}

}