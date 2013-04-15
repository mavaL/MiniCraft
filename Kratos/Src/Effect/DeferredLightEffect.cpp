#include "stdafx.h"
#include "DeferredLightEffect.h"
#include "OgreManager.h"
#include "DeferredShading/DLight.h"

namespace Kratos
{
	IMPL_PARAM_COMMAND(DLightEffect, LightType, Int)
	IMPL_PARAM_COMMAND(DLightEffect, LightRadius, Real)
	IMPL_PARAM_COMMAND(DLightEffect, PointAttenParam, Vector3)

	DLightEffect::DLightEffect(Ogre::Entity* parent)
	:AttachEffectBase(parent)
	,m_light(nullptr)
	,m_dlType(eDLightType_None)
	,m_fRadius(0)
	,m_dlPointParam(1,0,0)
	{
		m_type = eAttachEffect_DLight;
		if (InitParamDict("DLight"))
		{
			Ogre::ParamDictionary* dict = getParamDictionary();
			dict->addParameter(Ogre::ParameterDef("lighttype", "Deferred light type", Ogre::PT_INT), &m_sCmdLightType);
			dict->addParameter(Ogre::ParameterDef("radius", "Radius of point light", Ogre::PT_REAL), &m_sCmdLightRadius);
			dict->addParameter(Ogre::ParameterDef("PointAtteParam", "Attenuation info of point light", Ogre::PT_VECTOR3), &m_sCmdPointAttenParam);
		}
	}

	DLightEffect::~DLightEffect()
	{
		Destroy();
	}

	void DLightEffect::Start()
	{
		if (!m_light && m_dlType != eDLightType_None)
		{
			assert(m_state == eEffectState_Uninit);
			//创建光源
			m_light = RenderManager.CreateDLight(m_dlType);
			assert(m_light);

			//设置参数
			Ogre::Light* lightSrc = m_light->getParentLight();
			lightSrc->setVisible(false);
			lightSrc->setAttenuation(m_fRadius, m_dlPointParam.x, m_dlPointParam.y, m_dlPointParam.z);
			m_light->setAttenuation(lightSrc->getAttenuationConstant(), lightSrc->getAttenuationLinear(), lightSrc->getAttenuationQuadric());	
		}
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
		if (m_light)
		{
			Stop();
			RenderManager.DestroyDLight(m_light);
			m_light = nullptr;
		}
		
		__super::Destroy();
	}

	void DLightEffect::SetLightType( int type )
	{
		Destroy();
		m_dlType = (eDLightType)type;
	}

	void DLightEffect::SetLightRadius( float fRadius )
	{
		Destroy();
		m_fRadius = fRadius;
	}

	void DLightEffect::SetPointAttenParam( const FLOAT3& param )
	{
		Destroy();
		m_dlPointParam = param;
	}

}