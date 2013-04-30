/********************************************************************
	created:	4:3:2013   23:02
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorEffect.h
	author:		maval
	
	purpose:	图形效果相关管理
*********************************************************************/

#ifndef ManipulatorEffect_h__
#define ManipulatorEffect_h__

#include "ManipulatorEventCallback.h"

namespace Kratos
{
	class EffectController;
	class ParticleEffect;
}

class ManipulatorEffect : public ManipulatorSceneEventCallback
{
public:
	ManipulatorEffect();
	~ManipulatorEffect();

	//////////////事件回调
	virtual void	OnSceneClose();

public:
	void	OnFrameMove(float dt);	
	void	Serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode);
	void	LoadEffect(rapidxml::xml_node<>* node);

	void	SetShadowEnable(bool bEnable);
	bool	GetShadowEnable() const;
	void	SetSSAOEnable(bool bEnable);
	bool	GetSSAOEnable() const;
	void	SetSharpenEnable(bool bEnable);
	bool	GetSharpenEnable() const;
	void	SetFXAAEnable(bool bEnable);
	bool	GetFXAAEnable() const;

	void	SetShadowParam(const std::string& name, const std::wstring& value);
	void	SetSsaoParam(const std::string& name, const std::wstring& value);
	const std::string& GetShadowParam(const std::string& name) const;
	const std::string& GetSsaoParam(const std::string& name) const;

	std::vector<std::wstring>	GetMeshAnimNames(const std::string& meshname) const;
	void						PlayAnimation(Ogre::Entity* ent, int animIndex, bool bPlayOrStop);
	const std::string			GetCurAnimationName();
	void						OnAnimSelectChange(const std::string& anim);
	void						OnAttachEffectSelChange(const std::string& effect);
	bool						GetIsPlayingAnim() const;
	std::vector<std::wstring>	GetLocatorNames() const;
	void						BindEntityToEffectTemplate(Ogre::Entity* ent);
	std::vector<std::wstring>	GetAttachEffectNames();
	int							GetAttachEffectType(const std::string& name);
	//增加/去除一个绑定特效
	std::wstring				AddEffect(int type);
	void						RemoveEffect(const std::string& name);
	//获取PU所有粒子模板名字
	std::vector<std::wstring>	GetParticleTmpNames() const;
	//设置/获取挂接特效属性
	void						SetEffectParam(const std::string& param, const std::string& value);
	const std::string			GetEffectParam(const std::string& param);
	std::vector<std::wstring>	GetAttachEffectMeshNames();

private:
	Kratos::EffectController*	_GetEffectController(Ogre::Entity* ent);

private:
	typedef std::unordered_map<std::string, Kratos::EffectController*> EffectTemplate;
	EffectTemplate		m_effectTemplates;	//所有模型的挂接特效模板.编辑器对它们编辑
	bool				m_bIsPlayAnim;
	std::string			m_curAnim;
	std::string			m_curEffect;
};


#endif // ManipulatorEffect_h__







