#include "stdafx.h"
#include "AnimatedComponent.h"
#include "Effect/EffectController.h"
#include "Effect/ParticleEffect.h"
#include "SelectableObject.h"

AnimatedComponent::AnimatedComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_pController(new Kratos::EffectController(pOwner->GetEntity()))
{
	//创建该模型的所有挂接特效
	//TODO: cache!cache!cache!
	const STRING& unitName = m_pOwner->getParameter("unitname");
	const auto& effects = GameDataDefManager::GetSingleton().m_unitData[unitName].m_effects;

	//per animation
	for (auto itAnim=effects.begin(); itAnim!=effects.end(); ++itAnim)
	{
		const auto& slots = itAnim->second;
		//per effect
		for (size_t iEffect=0; iEffect<slots.size(); ++iEffect)
		{
			const Ogre::NameValuePairList& info = slots[iEffect].params;
			Kratos::ParticleEffect* effect = m_pController->AddEffect(itAnim->first);
			effect->setParameterList(info);
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
	SUnitData& unitData = GameDataDefManager::GetSingleton().m_unitData[unitName];
	const STRING& animName = unitData.m_anims[type];

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


