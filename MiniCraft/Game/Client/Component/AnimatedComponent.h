/********************************************************************
	created:	11:3:2013   17:07
	filename	AnimatedComponent.h
	author:		maval

	purpose:	动画控制组件.
				note:目前的只有Unit对象能够拥有该组件
*********************************************************************/

#ifndef AnimatedComponent_h__
#define AnimatedComponent_h__

#include "Component.h"
#include "GameDataDef.h"
#include "KratosPrerequisites.h"

class SelectableObject;
class Unit;

class AnimatedComponent : public Component
{
public:
	AnimatedComponent(SelectableObject* pOwner);
	~AnimatedComponent();

public:
	virtual void	Update(float dt);

public:
	void			PlayAnimation(eAnimation type, bool bLoop);
	void			StopAnimation();
	void			SetManuallyControlBones();
	bool			IsAnimationOver();

private:
	Unit*						m_parent;
	Kratos::EffectController*	m_pController;
	STRING						m_curAnim;
};

#endif // AnimatedComponent_h__