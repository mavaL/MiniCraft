#include "stdafx.h"
#include "AnimatedComponent.h"
#include "Effect/EffectBase.h"
#include "Effect/EffectController.h"
#include "SelectableObject.h"
#include "Unit.h"

AnimatedComponent::AnimatedComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_pController(new Kratos::EffectController(pOwner->GetEntity()))
,m_curAnim(Ogre::StringUtil::BLANK)
{
	m_parent = dynamic_cast<Unit*>(pOwner);

	//创建该模型的所有挂接特效
	//TODO: cache!cache!cache!
	auto& effects = m_parent->GetUnitData()->m_effects;

	//per animation
	for (auto itAnim=effects.begin(); itAnim!=effects.end(); ++itAnim)
	{
		auto& slots = itAnim->second;
		//per effect
		for (size_t iEffect=0; iEffect<slots.size(); ++iEffect)
		{
			Ogre::NameValuePairList& info = slots[iEffect].params;
			int type = Ogre::StringConverter::parseInt(info["type"]);
			Kratos::AttachEffectBase* effect = m_pController->AddEffect(itAnim->first, (Kratos::eAttachEffect)type);
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
	const STRING& animName = m_parent->GetUnitData()->m_anims[type];

	if (animName != m_curAnim)
	{
		m_pController->PlayAnimation(animName, bLoop);
		m_curAnim = animName;
	}
}

void AnimatedComponent::StopAnimation()
{
	m_pController->StopAnimation();
	m_curAnim = "";
}

void AnimatedComponent::Update( float dt )
{
	m_pController->Update(dt);
}

void AnimatedComponent::SetManuallyControlBones()
{
	Ogre::Skeleton* pSkel = m_pOwner->GetEntity()->getSkeleton();
	Ogre::Skeleton::BoneIterator iter = pSkel->getBoneIterator();
	size_t numBones = pSkel->getNumBones();
	for(size_t i=0; i<numBones; ++i)
	{
		Ogre::Bone* bone = iter.peekNext();
		bone->setManuallyControlled(true);
		iter.moveNext();
	}
	pSkel->getRootBone()->setManuallyControlled(true);
}

bool AnimatedComponent::IsAnimationOver()
{
	return m_pController->IsEnd();
}


