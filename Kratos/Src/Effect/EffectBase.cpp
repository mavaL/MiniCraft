#include "stdafx.h"
#include "EffectBase.h"
#include "OgreManager.h"

namespace Kratos
{
	IMPL_PARAM_COMMAND_STR(AttachEffectBase, Name)
	IMPL_PARAM_COMMAND_STR(AttachEffectBase, Locator)
	IMPL_PARAM_COMMAND(AttachEffectBase, StartTime, Real)
	IMPL_PARAM_COMMAND(AttachEffectBase, LifeTime, Real)
	IMPL_PARAM_COMMAND(AttachEffectBase, Type, Int)

	AttachEffectBase::AttachEffectBase(Ogre::Entity* parent)
	:m_name(Ogre::StringUtil::BLANK)
	,m_parent(parent)
	,m_fStartTime(0)
	,m_fLifeTime(0)
	,m_fCurTime(0)
	,m_state(eEffectState_Uninit)
	,m_locator(Ogre::StringUtil::BLANK)
	{
	}

	AttachEffectBase::~AttachEffectBase()
	{
	}

	void AttachEffectBase::Stop()
	{
		m_state = eEffectState_Stoped;
	}

	void AttachEffectBase::Pause()
	{
		m_state = eEffectState_Paused;
	}

	void AttachEffectBase::Resume()
	{
		m_state = eEffectState_Started;
	}

	void AttachEffectBase::_SetParent( Ogre::Entity* ent )
	{
		m_parent = ent;
		Destroy();
	}

	void AttachEffectBase::Destroy()
	{
		m_state = eEffectState_Uninit;
	}

	void AttachEffectBase::Start()
	{
		m_fCurTime = 0;
		m_state = eEffectState_Prepared;
	}

	bool AttachEffectBase::InitParamDict(const STRING& name)
	{
		STRING dictName("AttachEffectDict_");
		dictName += name;

		if (createParamDictionary(dictName))
		{
			Ogre::ParamDictionary* dict = getParamDictionary();
			dict->addParameter(Ogre::ParameterDef("type", "type of this effect", Ogre::PT_INT), &m_sCmdType);
			dict->addParameter(Ogre::ParameterDef("name", "name of this effect", Ogre::PT_STRING), &m_sCmdName);
			dict->addParameter(Ogre::ParameterDef("locator", "bone name to which this particle attach", Ogre::PT_STRING), &m_sCmdLocator);
			dict->addParameter(Ogre::ParameterDef("starttime", "effect start time relative to animation", Ogre::PT_REAL), &m_sCmdStartTime);
			dict->addParameter(Ogre::ParameterDef("lifetime", "effect's life time", Ogre::PT_REAL), &m_sCmdLifeTime);
			return true;
		}
		return false;
	}
}