/********************************************************************
	created:	4:3:2013   23:02
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorEffect.h
	author:		maval
	
	purpose:	图形效果相关管理
*********************************************************************/

#ifndef ManipulatorEffect_h__
#define ManipulatorEffect_h__

namespace Kratos
{
	class EffectController;
	class ParticleEffect;
}

class ManipulatorEffect
{
public:
	ManipulatorEffect();
	~ManipulatorEffect();

public:
	void	OnFrameMove(float dt);	
	void	Serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode);

	void	SetShadowEnable(bool bEnable);
	bool	GetShadowEnable() const;
	void	SetSSAOEnable(bool bEnable);
	bool	GetSSAOEnable() const;

	void	SetShadowFarDist(float dist);
	void	SetShadowSplitPadding(float pad);
	void	SetShadowOptimalAdjustFactor(int layer, float factor);
	void	SetShadowUseSimpleOptimalAdjust(bool b);
	void	SetShadowCameraLightDirectionThreshold(int degree);
	void	SetShadowMapSize(int layer, int size);
	void	SetShadowSelfShadow(bool b);
	void	SetShadowCasterRenderBackFaces(bool b);
	void	SetShadowPssmLambda(float f);
	void	SetShadowDirectionalLightExtrusionDistance(float f);
	void	SetSSAOSampleLength(float f);
	void	SetSSAOOffsetScale(float f);
	void	SetSSAOEdgeHighlight(float f);
	void	SetSSAODefaultAccessibility(float f);

	float	GetShadowFarDist() const;
	float	GetShadowSplitPadding() const;
	Ogre::Vector3	GetShadowOptimalAdjustFactor() const;
	bool	GetShadowUseSimpleOptimalAdjust() const;
	int		GetShadowCameraLightDirectionThreshold() const;
	Ogre::Vector3	GetShadowMapSize() const;
	bool	GetShadowSelfShadow() const;
	bool	GetShadowCasterRenderBackFaces() const;
	float	GetShadowPssmLambda() const;
	float	GetShadowDirectionalLightExtrusionDistance() const;
	float	GetSSAOSampleLength() const;
	float	GetSSAOOffsetScale() const;
	float	GetSSAOEdgeHighlight() const;
	float	GetSSAODefaultAccessibility() const;

	std::vector<std::wstring>	GetAnimationNames() const;
	void						PlayAnimation(int animIndex, bool bPlayOrStop);
	const std::string			GetCurAnimationName();
	void						SetCurAnimationName(const std::string& anim);
	bool						GetIsPlayingAnim() const;
	std::vector<std::wstring>	GetLocatorNames() const;
	//获取PU所有粒子模板名字
	std::vector<std::wstring>	GetParticleTmpNames() const;
	//设置当前正在编辑的特效
	void	SetActiveEffect(const std::string& animName, const std::string& locator);
	//设置/获取特效参数,返回false表示设置失败
	bool						SetEffectParam(const std::string& param, const std::string& value);
	const std::string			GetEffectParam(const std::string& param) const;

private:
	float			m_pssmLambda;
	float			m_ssaoSampleLength;
	float			m_ssaoOffsetScale;
	float			m_ssaoDefaultAccessibility;
	float			m_ssaoEdgeHighlight;

	typedef std::unordered_map<Ogre::Entity*, Kratos::EffectController*> EffectMap;
	EffectMap				m_effects;			//所有模型的相关特效
	Kratos::ParticleEffect*	m_curEditEffect;	//当前编辑特效
	std::string				m_curAnim;			//当前选择动画
	bool					m_bIsPlayAnim;
};


#endif // ManipulatorEffect_h__







