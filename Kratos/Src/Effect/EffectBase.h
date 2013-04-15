/********************************************************************
	created:	21:3:2013   23:32
	filename	EffectBase.h
	author:		maval

	purpose:	角色挂接特效基类
*********************************************************************/

#ifndef EffectBase_h__
#define EffectBase_h__

#include "KratosPrerequisites.h"

namespace Kratos
{
	enum eAttachEffect
	{
		eAttachEffect_Particle,
		eAttachEffect_DLight
	};

	enum eEffectState
	{
		eEffectState_Uninit,
		eEffectState_Init,
		eEffectState_Prepared,
		eEffectState_Started,
		eEffectState_Stoped,
		eEffectState_Paused,
	};

	class AttachEffectBase : public Ogre::StringInterface
	{
		DECL_PARAM_COMMAND(Name)
		DECL_PARAM_COMMAND(Type)
		DECL_PARAM_COMMAND(Locator)
		DECL_PARAM_COMMAND(StartTime)
		DECL_PARAM_COMMAND(LifeTime)

	public:
		AttachEffectBase(Ogre::Entity* parent);
		virtual ~AttachEffectBase();

		///NO VIRTUAL!!
		bool			InitParamDict(const STRING& name);

	public:
		virtual void	Start();
		virtual void	Stop();
		virtual void	Pause();
		virtual void	Resume();
		virtual void	Update(float dt) = 0;
		virtual void	Destroy();

		void			SetStartTime(float fStartTime)		{ m_fStartTime = fStartTime; }
		float			GetStartTime() const				{ return m_fStartTime; }
		void			SetLifeTime(float fLifeTime)		{ m_fLifeTime = fLifeTime; }
		float			GetLifeTime() const					{ return m_fLifeTime; }
		void			SetLocator(const STRING& locator)	{ m_locator = locator; m_state = eEffectState_Uninit; }
		const STRING&	GetLocator() const					{ return m_locator; }
		void			SetName(const STRING& name)			{ m_name = name; }
		const STRING&	GetName() const						{ return m_name; }
		void			SetType(int type)					{ m_type = (eAttachEffect)type; }
		int				GetType() const						{ return m_type; }

		//编辑器用
		void			_SetParent(Ogre::Entity* ent);

	protected:
		Ogre::Entity*	m_parent;
		eAttachEffect	m_type;
		STRING			m_name;
		STRING			m_locator;
		float			m_fStartTime;
		float			m_fLifeTime;
		float			m_fCurTime;
		eEffectState	m_state;
	};
}

#endif // EffectBase_h__