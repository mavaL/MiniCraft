/********************************************************************
	created:	4:3:2013   23:02
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorEffect.h
	author:		maval
	
	purpose:	图形效果相关管理
*********************************************************************/

#ifndef ManipulatorEffect_h__
#define ManipulatorEffect_h__

class ManipulatorEffect
{
public:
	ManipulatorEffect();
	~ManipulatorEffect();

public:
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

private:
	float			m_pssmLambda;
	float			m_ssaoSampleLength;
	float			m_ssaoOffsetScale;
	float			m_ssaoDefaultAccessibility;
	float			m_ssaoEdgeHighlight;
};


#endif // ManipulatorEffect_h__







