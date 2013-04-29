#include "stdafx.h"
#include "EffectController.h"
#include "OgreManager.h"
#include "ParticleEffect.h"
#include "DeferredLightEffect.h"

namespace Kratos
{

	EffectController::EffectController( Ogre::Entity* parent )
	:m_parent(parent)
	,m_anim(nullptr)
	,m_bEnd(false)
	{

	}

	EffectController::~EffectController()
	{
		for(auto iter=m_effects.begin(); iter!=m_effects.end(); ++iter)
		{
			EffectSlots& slots = iter->second;
			for(size_t i=0; i<slots.size(); ++i)
				delete slots[i];
		}
		m_effects.clear();
	}

	AttachEffectBase* EffectController::AddEffect( const STRING& animName, eAttachEffect type )
	{
		AttachEffectBase* newEffect = nullptr;
		switch (type)
		{
		case eAttachEffect_Particle: newEffect = new ParticleEffect(m_parent); break;
		case eAttachEffect_DLight: newEffect = new DLightEffect(m_parent); break;
		}

		m_effects[animName].push_back(newEffect);
		return newEffect;
	}

	void EffectController::RemoveEffect( const STRING& name )
	{
		for (auto iter=m_effects.begin(); iter!=m_effects.end(); ++iter)
		{
			EffectSlots& slots = iter->second;
			for (auto itEffect=slots.begin(); itEffect!=slots.end(); ++itEffect)
			{
				AttachEffectBase* effect = *itEffect;
				if (effect->GetName() == name)
				{
					effect->Destroy();
					delete effect;
					slots.erase(itEffect);

					return;
				}
			}
		}
		assert(0 && "There's no effect with this name!");
	}

	void EffectController::PlayAnimation( const STRING& animName, bool bLoop )
	{
		assert(m_parent->hasAnimationState(animName));

		//停止当前动画
		StopAnimation();

		//播放动画
		m_anim = m_parent->getAnimationState(animName);
		m_anim->setEnabled(true);
		m_anim->setTimePosition(0);
		m_anim->setLoop(bLoop);

		//播放特效
		auto iter = m_effects.find(animName);
		if (iter != m_effects.end())
		{
			EffectSlots& slots = iter->second;
			for (auto itEffect=slots.begin(); itEffect!=slots.end(); ++itEffect)
			{
				AttachEffectBase* effect = *itEffect;
				effect->Start();
			}
		}

		m_bEnd = false;
	}

	void EffectController::StopAnimation()
	{
		if(m_anim)
		{
			//停止特效
			auto iter = m_effects.find(m_anim->getAnimationName());
			if (iter != m_effects.end())
			{
				EffectSlots& slots = iter->second;
				for (auto itEffect=slots.begin(); itEffect!=slots.end(); ++itEffect)
					(*itEffect)->Stop();
			}

			//停止动画
			m_anim->setEnabled(false);
			m_anim = nullptr;
		}

		m_bEnd = true;
	}

	void EffectController::Update( float dt )
	{
		if(!m_anim)
			return;

		//更新动画
		static float fLastTimePos = m_anim->getTimePosition();
		m_anim->addTime(dt);

		if(m_anim->hasEnded())
		{
			StopAnimation();
			m_anim = nullptr;
		}
		else
		{
			bool bLooped = false;
			float fCurTimePos = m_anim->getTimePosition();
			//hack a little..
			//说明动画已经循环了一次
			if(fCurTimePos < fLastTimePos)
				bLooped = true;
			fLastTimePos = fCurTimePos;

			//更新特效
			auto iter = m_effects.find(m_anim->getAnimationName());
			if(iter != m_effects.end())
			{
				EffectSlots& slots = iter->second;
				for (auto itEffect=slots.begin(); itEffect!=slots.end(); ++itEffect)
				{
					AttachEffectBase* effect = *itEffect;
					if(bLooped)
						effect->Start();
					else
						effect->Update(dt);
				}
			}
		}
	}

	AttachEffectBase* EffectController::GetEffect( const STRING& name )
	{
		for (auto iter=m_effects.begin(); iter!=m_effects.end(); ++iter)
		{
			EffectSlots& slots = iter->second;
			for (auto itEffect=slots.begin(); itEffect!=slots.end(); ++itEffect)
			{
				AttachEffectBase* effect = *itEffect;
				if (effect->GetName() == name)
					return effect;
			}
		}

		return nullptr;
	}

	void EffectController::_SetParent( Ogre::Entity* ent )
	{
		for(auto itSlot=m_effects.begin(); itSlot!=m_effects.end(); ++itSlot)
		{
			EffectSlots& slots = itSlot->second;
			for (auto itEffect=slots.begin(); itEffect!=slots.end(); ++itEffect)
				(*itEffect)->_SetParent(ent);
		}
		m_parent = ent; 
		m_anim = nullptr;
	}

}

