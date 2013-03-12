#include "stdafx.h"
#include "AnimatedComponent.h"
#include "Effect/EffectController.h"
#include "SelectableObject.h"

AnimatedComponent::AnimatedComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_pController(new Kratos::EffectController(pOwner->GetEntity()))
{
	//创建该模型的所有挂接特效
	//TODO: cache!cache!cache!
	const STRING& unitName = m_pOwner->getParameter("unitname");
	const auto& effectSlots = GameDataDefManager::GetSingleton().m_unitData[unitName].m_effects;

	for (size_t i=0; i<effectInfo.size(); ++i)
	{
		const SEffectData& effectInfo = effectSlots[i];
		Kratos::ParticleEffect* newEffect = new Kratos::ParticleEffect(pOwner->GetEntity());
		m_pController->AddEffect(effectInfo.m_anim, effectInfo.m_locator, newEffect);

		//自足,反射
		const Ogre::ParameterList& attributes = newEffect->getParameters();
		for (size_t i=0; i<attributes.size(); ++i)
		{
			const Ogre::ParameterDef& def = attributes[i];
			newEffect->setParameter(def.name, )
		}
	}
}

AnimatedComponent::~AnimatedComponent()
{
	SAFE_DELETE(m_pController);
}

void AnimatedComponent::PlayAnimation( eAnimation type, bool bLoop )
{
	const STRING& unitName = m_pOwner->getParameter("unitname");
	const SUnitData& unitData = GameDataDefManager::GetSingleton().m_unitData[unitName];
	const STRING& animName = (unitData.m_anims)[type];

	m_pController->PlayAnimation(animName, bLoop);
}

void AnimatedComponent::StopAnimation()
{
	m_pController->StopAnimation();
}

void AnimatedComponent::Update( float dt )
{
	m_pController->Update(dt);
}


