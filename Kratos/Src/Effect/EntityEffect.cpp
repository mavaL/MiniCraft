#include "stdafx.h"
#include "EntityEffect.h"
#include "OgreManager.h"

namespace Kratos
{
	IMPL_PARAM_COMMAND_STR(EntityEffect, MeshName)
	IMPL_PARAM_COMMAND_STR(EntityEffect, AnimName)

	EntityEffect::EntityEffect( Ogre::Entity* parent )
	:AttachEffectBase(parent)
	,m_meshname(Ogre::StringUtil::BLANK)
	,m_animname(Ogre::StringUtil::BLANK)
	,m_animState(nullptr)
	,m_pEntity(nullptr)
	{
		m_type = eAttachEffect_Entity;
		if (InitParamDict("EntityEffect"))
		{
			Ogre::ParamDictionary* dict = getParamDictionary();
			dict->addParameter(Ogre::ParameterDef("meshname", "mesh name.", Ogre::PT_STRING), &m_sCmdMeshName);
			dict->addParameter(Ogre::ParameterDef("animname", "animation name to play with.", Ogre::PT_STRING), &m_sCmdAnimName);
		}
	}

	EntityEffect::~EntityEffect()
	{
		Destroy();
	}

	void EntityEffect::Start()
	{
		if (!m_pEntity && !m_meshname.empty())
		{
			assert(m_state == eEffectState_Uninit);
			m_pEntity = RenderManager.m_pSceneMgr->createEntity(m_meshname);
			m_parent->attachObjectToBone(m_locator, m_pEntity);
		}
		if (!m_animState && !m_animname.empty())
		{
			m_animState = m_pEntity->getAnimationState(m_animname);
			m_animState->setLoop(false);
		}

		__super::Start();
	}

	void EntityEffect::Stop()
	{
		if(m_animState)
			m_animState->setEnabled(false);
		m_pEntity->setVisible(false);
		__super::Stop();
	}

	void EntityEffect::Update( float dt )
	{
		m_fCurTime += dt;
		if (m_state == eEffectState_Prepared)
		{
			if (m_fCurTime >= m_fStartTime)
			{
				//真正开始播放
				m_pEntity->setVisible(true);
				m_animState->setEnabled(true);
				m_animState->setTimePosition(0);
				m_state = eEffectState_Started;
			}
		}
		else if (m_state == eEffectState_Started)
		{
			m_animState->addTime(dt);

			if (m_animState->hasEnded())
				Stop();
		}
	}

	void EntityEffect::Destroy()
	{
		if(m_animState)
		{
			m_animState->setEnabled(false);
			m_animState = nullptr;
		}
		if (m_pEntity)
		{
			RenderManager.m_pSceneMgr->destroyEntity(m_pEntity);
			m_pEntity = nullptr;
		}

		__super::Destroy();
	}

	void EntityEffect::SetMeshName( const STRING& meshname )
	{
		Destroy();
		m_meshname = meshname;
	}

	void EntityEffect::SetAnimName( const STRING& animname )
	{
		Destroy();
		m_animname = animname;
	}

}