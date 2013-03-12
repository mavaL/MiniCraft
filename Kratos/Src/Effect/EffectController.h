/********************************************************************
	created:	11:3:2013   13:44
	author:		maval
	
	purpose:	粒子等特效管理器.每个带动画的模型对象拥有一个
*********************************************************************/

#ifndef EffectController_h__
#define EffectController_h__

#include "KratosPrerequisites.h"

namespace Kratos
{
	class ParticleEffect;

	class EffectController
	{
	public:
		EffectController(Ogre::Entity* parent);
		~EffectController();

		typedef std::map<STRING, ParticleEffect*>		EffectSlots;		//一个动画的所有挂接点对应的特效
		typedef std::unordered_map<STRING, EffectSlots>	EffectContainer;	//所有动画对应的特效

	public:
		//增加一个挂接特效
		void			AddEffect(const STRING& animName, const STRING& locator, ParticleEffect* pEffect);
		//删除挂接特效
		void			RemoveEffect(const STRING& animName, const STRING& locator);
		//获取模型对应特效
		ParticleEffect*	GetEffect(const STRING& animName, const STRING& locator);
		//播放动画,自动管理特效的播放
		void			PlayAnimation(const STRING& animName, bool bLoop);
		void			StopAnimation();
		//更新该控制器
		void			Update(float dt);
		//编辑器用
		const EffectContainer&	_GetEffects() const { return m_effects; }

	private:
		Ogre::Entity*			m_parent;
		EffectContainer			m_effects;
		Ogre::AnimationState*	m_anim;			//当前播放动画
	};

}

#endif // EffectController_h__