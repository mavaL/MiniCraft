#include "stdafx.h"
#include "ManipulatorEffect.h"
#include "OgreManager.h"
#include "ManipulatorScene.h"

ManipulatorEffect::ManipulatorEffect()
:m_pssmLambda(0.75f)
,m_ssaoSampleLength(20)
,m_ssaoOffsetScale(1)
,m_ssaoDefaultAccessibility(0.5f)
,m_ssaoEdgeHighlight(1.99f)
{

}

ManipulatorEffect::~ManipulatorEffect()
{
}

void ManipulatorEffect::SetShadowEnable( bool bEnable )
{
	RenderManager.EnableShadow(bEnable);
}

bool ManipulatorEffect::GetShadowEnable() const
{
	return RenderManager.GetEffectConfig().bShadow;
}

void ManipulatorEffect::SetSSAOEnable( bool bEnable )
{
	RenderManager.EnableSSAO(bEnable);
}

bool ManipulatorEffect::GetSSAOEnable() const
{
	return RenderManager.GetEffectConfig().bSSAO;
}

void ManipulatorEffect::SetShadowFarDist( float dist )
{
	RenderManager.m_pSceneMgr->setShadowFarDistance(dist);
}

void ManipulatorEffect::SetShadowSplitPadding( float pad )
{
	RenderManager.GetShadowCameraSetup()->setSplitPadding(pad);
}

void ManipulatorEffect::SetShadowOptimalAdjustFactor( int layer, float factor )
{
	RenderManager.GetShadowCameraSetup()->setOptimalAdjustFactor(layer, factor);
}

void ManipulatorEffect::SetShadowUseSimpleOptimalAdjust( bool b )
{
	RenderManager.GetShadowCameraSetup()->setUseSimpleOptimalAdjust(b);
}

void ManipulatorEffect::SetShadowCameraLightDirectionThreshold( int degree )
{
	RenderManager.GetShadowCameraSetup()->setCameraLightDirectionThreshold(Ogre::Degree((float)degree));
}

void ManipulatorEffect::SetShadowMapSize( int layer, int size )
{
	RenderManager.m_pSceneMgr->setShadowTextureConfig(layer, size, size, Ogre::PF_FLOAT32_R);
}

void ManipulatorEffect::SetShadowSelfShadow( bool b )
{
	RenderManager.m_pSceneMgr->setShadowTextureSelfShadow(b);
}

void ManipulatorEffect::SetShadowCasterRenderBackFaces( bool b )
{
	RenderManager.m_pSceneMgr->setShadowCasterRenderBackFaces(b);
}

void ManipulatorEffect::SetShadowPssmLambda( float f )
{
	m_pssmLambda = f;
	float n = RenderManager.m_pMainCamera->getNearClipDistance();
	float farDist = RenderManager.m_pSceneMgr->getShadowFarDistance();
	RenderManager.GetShadowCameraSetup()->calculateSplitPoints(3, n, farDist, m_pssmLambda); 
}

void ManipulatorEffect::SetShadowDirectionalLightExtrusionDistance( float f )
{
	RenderManager.m_pSceneMgr->setShadowDirectionalLightExtrusionDistance(f);
}

float ManipulatorEffect::GetShadowFarDist() const
{
	return RenderManager.m_pSceneMgr->getShadowFarDistance();
}

float ManipulatorEffect::GetShadowSplitPadding() const
{
	return RenderManager.GetShadowCameraSetup()->getSplitPadding();
}

Ogre::Vector3 ManipulatorEffect::GetShadowOptimalAdjustFactor() const
{
	Ogre::PSSMShadowCameraSetup* cam = RenderManager.GetShadowCameraSetup();
	return Ogre::Vector3(cam->getOptimalAdjustFactor(0), cam->getOptimalAdjustFactor(1), cam->getOptimalAdjustFactor(2));
}

bool ManipulatorEffect::GetShadowUseSimpleOptimalAdjust() const
{
	return RenderManager.GetShadowCameraSetup()->getUseSimpleOptimalAdjust();
}

int ManipulatorEffect::GetShadowCameraLightDirectionThreshold() const
{
	return (int)RenderManager.GetShadowCameraSetup()->getCameraLightDirectionThreshold().valueDegrees();
}

Ogre::Vector3 ManipulatorEffect::GetShadowMapSize() const
{
	Ogre::Vector3 ret;
	auto iter = RenderManager.m_pSceneMgr->getShadowTextureConfigIterator();
	ret.x = (float)iter.getNext().width;
	ret.y = (float)iter.getNext().width;
	ret.z = (float)iter.getNext().width;

	return ret;
}

bool ManipulatorEffect::GetShadowSelfShadow() const
{
	return RenderManager.m_pSceneMgr->getShadowTextureSelfShadow();
}

bool ManipulatorEffect::GetShadowCasterRenderBackFaces() const
{
	return RenderManager.m_pSceneMgr->getShadowCasterRenderBackFaces();
}

float ManipulatorEffect::GetShadowPssmLambda() const
{
	return m_pssmLambda;
}

float ManipulatorEffect::GetShadowDirectionalLightExtrusionDistance() const
{
	return RenderManager.m_pSceneMgr->getShadowDirectionalLightExtrusionDistance();
}

void ManipulatorEffect::Serialize( rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode )
{
	using namespace Ogre;
	const SEffectConfig& cfg = RenderManager.GetEffectConfig();
	SceneManager* sm = RenderManager.m_pSceneMgr;

	const String strAmbient = StringConverter::toString(sm->getAmbientLight());
	const String strShadowOn = StringConverter::toString(cfg.bShadow);

	//shadow param
	const String strFarDist			= StringConverter::toString(GetShadowFarDist());
	const String strPad				= StringConverter::toString(GetShadowSplitPadding());
	const String strAdjustFactor	= StringConverter::toString(GetShadowOptimalAdjustFactor());
	const String strUseSimpleAdjust = StringConverter::toString(GetShadowUseSimpleOptimalAdjust());
	const String strThreshold		= StringConverter::toString(GetShadowCameraLightDirectionThreshold());
	const String strTexSize			= StringConverter::toString(GetShadowMapSize());
	const String strSelfShadow		= StringConverter::toString(GetShadowSelfShadow());
	const String strRenderBack		= StringConverter::toString(GetShadowCasterRenderBackFaces());
	const String strPssmLambda		= StringConverter::toString(GetShadowPssmLambda());
	const String strExtrusion		= StringConverter::toString(GetShadowDirectionalLightExtrusionDistance());

	rapidxml::xml_node<>* shadowNode = doc->allocate_node(rapidxml::node_element, "shadow");

	shadowNode->append_attribute(doc->allocate_attribute("AmbientLight", doc->allocate_string(strAmbient.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("EnableShadow", doc->allocate_string(strShadowOn.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("FarDistance",	doc->allocate_string(strFarDist.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("SplitPadding", doc->allocate_string(strPad.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("AdjustFactor", doc->allocate_string(strAdjustFactor.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("UseSimpleAdjust", doc->allocate_string(strUseSimpleAdjust.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("CameraLightDirectionThreshold", doc->allocate_string(strThreshold.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("ShadowMapSize", doc->allocate_string(strTexSize.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("SelfShadow", doc->allocate_string(strSelfShadow.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("RenderBackFace", doc->allocate_string(strRenderBack.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("PssmLambda", doc->allocate_string(strPssmLambda.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("LightExtrusionDistance", doc->allocate_string(strExtrusion.c_str())));

	XMLNode->append_node(shadowNode);

	//ssao param
	const String strSsaoOn = StringConverter::toString(cfg.bSSAO);
	const String strSampleLen				= StringConverter::toString(GetSSAOSampleLength());
	const String strOffsetScale				= StringConverter::toString(GetSSAOOffsetScale());
	const String strDefaultAccessibility	= StringConverter::toString(GetSSAODefaultAccessibility());
	const String strEdgeHighlight			= StringConverter::toString(GetSSAOEdgeHighlight());

	rapidxml::xml_node<>* ssaoNode = doc->allocate_node(rapidxml::node_element, "ssao");

	ssaoNode->append_attribute(doc->allocate_attribute("EnableSSAO", doc->allocate_string(strSsaoOn.c_str())));
	ssaoNode->append_attribute(doc->allocate_attribute("SampleLength", doc->allocate_string(strSampleLen.c_str())));
	ssaoNode->append_attribute(doc->allocate_attribute("OffsetScale", doc->allocate_string(strOffsetScale.c_str())));
	ssaoNode->append_attribute(doc->allocate_attribute("DefaultAccessibility", doc->allocate_string(strDefaultAccessibility.c_str())));
	ssaoNode->append_attribute(doc->allocate_attribute("EdgeHighlight",	doc->allocate_string(strEdgeHighlight.c_str())));

	XMLNode->append_node(ssaoNode);
}

void ManipulatorEffect::SetSSAOSampleLength( float f )
{
	RenderManager.SetSSAOParam("cSampleLengthScreenSpace", f/100);
	m_ssaoSampleLength = f;
}

void ManipulatorEffect::SetSSAOOffsetScale( float f )
{
	RenderManager.SetSSAOParam("cOffsetScale", f/100);
	m_ssaoOffsetScale = f;
}

void ManipulatorEffect::SetSSAOEdgeHighlight( float f )
{
	RenderManager.SetSSAOParam("cEdgeHighlight", f);
	m_ssaoEdgeHighlight = f;
}

void ManipulatorEffect::SetSSAODefaultAccessibility( float f )
{
	RenderManager.SetSSAOParam("cDefaultAccessibility", f);
	m_ssaoDefaultAccessibility = f;
}

float ManipulatorEffect::GetSSAOSampleLength() const
{
	return m_ssaoSampleLength;
}

float ManipulatorEffect::GetSSAOOffsetScale() const
{
	return m_ssaoOffsetScale;
}

float ManipulatorEffect::GetSSAOEdgeHighlight() const
{
	return m_ssaoEdgeHighlight;
}

float ManipulatorEffect::GetSSAODefaultAccessibility() const
{
	return m_ssaoDefaultAccessibility;
}
