#include "EffectController.h"
#include "OgreManager.h"
#include "ParticleEffect.h"

namespace Kratos
{

	EffectController::EffectController( Ogre::Entity* parent )
	:m_parent(parent)
	,m_anim(nullptr)
	{

	}

	EffectController::~EffectController()
	{
		for(auto iter=m_effects.begin(); iter!=m_effects.end(); ++iter)
		{
			EffectSlots& slots = iter->second;
			while(!slots.empty())
				RemoveEffect(iter->first, slots.begin()->first);
		}
		m_effects.clear();
	}

	void EffectController::AddEffect( const STRING& animName, const STRING& locator, ParticleEffect* pEffect )
	{
		m_effects[animName][locator] = pEffect;
		pEffect->SetLocator(locator);
	}

	void EffectController::RemoveEffect( const STRING& animName, const STRING& locator )
	{
		auto iter = m_effects.find(animName);
		assert(iter != m_effects.end());

		EffectSlots& slots = iter->second;
		auto itEffect = slots.find(locator);
		assert(itEffect != slots.end());

		itEffect->second->Destroy();
		delete itEffect->second;
		slots.erase(itEffect);
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
				itEffect->second->Start();
		}
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
					itEffect->second->Stop();
			}

			//停止动画
			m_anim->setEnabled(false);
			m_anim = nullptr;
		}
	}

	void EffectController::Update( float dt )
	{
		if (m_anim)
		{
			//更新特效
			auto iter = m_effects.find(m_anim->getAnimationName());
			if(iter != m_effects.end())
			{
				EffectSlots& slots = iter->second;
				for (auto itEffect=slots.begin(); itEffect!=slots.end(); ++itEffect)
					itEffect->second->Update(dt);
			}

			//更新动画
			m_anim->addTime(dt);

			if(m_anim->hasEnded())
				m_anim = nullptr;
		}
	}

	ParticleEffect* EffectController::GetEffect( const STRING& animName, const STRING& locator )
	{
		auto iter = m_effects.find(animName);
		if(iter != m_effects.end())
		{
			EffectSlots& slots = iter->second;
			auto itEffect = slots.find(locator);
			if(itEffect != slots.end())
				return itEffect->second;
		}

		return nullptr;
	}

}

