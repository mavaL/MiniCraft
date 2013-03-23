#include "stdafx.h"
#include "ManipulatorEffect.h"
#include "OgreManager.h"
#include "ManipulatorScene.h"
#include "Utility.h"
#include "Effect/EffectController.h"
#include "Effect/EffectBase.h"

ManipulatorEffect::ManipulatorEffect()
:m_pssmLambda(0.75f)
,m_ssaoSampleLength(20)
,m_ssaoOffsetScale(1)
,m_ssaoDefaultAccessibility(0.5f)
,m_ssaoEdgeHighlight(1.99f)
,m_bIsPlayAnim(false)
,m_curAnim(Ogre::StringUtil::BLANK)
,m_curEffect(Ogre::StringUtil::BLANK)
,m_effectNameID(0)
{

}

ManipulatorEffect::~ManipulatorEffect()
{
	for(auto iter=m_effectTemplates.begin(); iter!=m_effectTemplates.end(); ++iter)
		delete iter->second;
	m_effectTemplates.clear();
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
	const Kratos::SEffectConfig& cfg = RenderManager.GetEffectConfig();
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

	//sharpen
	rapidxml::xml_node<>* sharpenNode = doc->allocate_node(rapidxml::node_element, "sharpen");
	const String strSharpenEnable = StringConverter::toString(GetSharpenEnable());
	sharpenNode->append_attribute(doc->allocate_attribute("EnableSharpen", doc->allocate_string(strSharpenEnable.c_str())));
	XMLNode->append_node(sharpenNode);

	//fxaa
	rapidxml::xml_node<>* fxaaNode = doc->allocate_node(rapidxml::node_element, "fxaa");
	const String strFXAAEnable = StringConverter::toString(GetFXAAEnable());
	fxaaNode->append_attribute(doc->allocate_attribute("EnableFXAA", doc->allocate_string(strFXAAEnable.c_str())));
	XMLNode->append_node(fxaaNode);

	///保存所有单位的挂接特效信息
	for (auto itTmpl=m_effectTemplates.begin(); itTmpl!=m_effectTemplates.end(); ++itTmpl)
	{
		Kratos::EffectController* pCtrl = itTmpl->second;
		const auto& effects = pCtrl->_GetEffects();

		SUnitData* unitData = ManipulatorSystem.GetGameData().GetUnitData(itTmpl->first);
		unitData->m_effects.clear();

		for (auto itAnim=effects.begin(); itAnim!=effects.end(); ++itAnim)
		{
			const auto& slots = itAnim->second;
			for (auto itEffect=slots.begin(); itEffect!=slots.end(); ++itEffect)
			{
				Kratos::AttachEffectBase* effect = *itEffect;
				const Ogre::ParameterList& attributes = effect->getParameters();
				SEffectData data;

				PARAMS_SAVE(effect, data.params);
	
				unitData->m_effects[itAnim->first].push_back(data);
			}
		}
	}
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

std::vector<std::wstring> ManipulatorEffect::GetParticleTmpNames() const
{
	std::vector<std::wstring> ret;

	Ogre::FileInfoListPtr fileinfo = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo(
		"ParticleUniverse", "*.pu");

	for(auto iter=fileinfo->begin(); iter!=fileinfo->end(); ++iter)
	{
		const Ogre::FileInfo& info = *iter;
		STRING obasename, oextname;
		Ogre::StringUtil::splitBaseFilename(info.basename, obasename, oextname);
		ret.push_back(Utility::EngineToUnicode(obasename));
	}

	return std::move(ret);
}

std::vector<std::wstring> ManipulatorEffect::GetAnimationNames() const
{
	Ogre::Entity* pEntity = ManipulatorSystem.GetObject().GetSelection();
	assert(pEntity);
	std::vector<std::wstring> vecRet;
	Ogre::Skeleton* pSkeleton = pEntity->getSkeleton();

	//没有可用动画
	if(!pSkeleton || pSkeleton->getNumAnimations()==0)
		return std::move(vecRet);

	for(size_t i=0; i<pSkeleton->getNumAnimations(); ++i)
		vecRet.push_back(Utility::EngineToUnicode(pSkeleton->getAnimation(i)->getName()));

	return std::move(vecRet);
}

void ManipulatorEffect::PlayAnimation( Ogre::Entity* ent, int animIndex, bool bPlayOrStop )
{
	Kratos::EffectController* pCtrl = _GetEffectController(ent);
	if (bPlayOrStop)
	{
		const Ogre::String& animName = ent->getSkeleton()->getAnimation(animIndex)->getName();
		pCtrl->PlayAnimation(animName, true);
	}
	else
	{
		pCtrl->StopAnimation();
	}

	m_bIsPlayAnim = bPlayOrStop;
}

void ManipulatorEffect::OnAnimSelectChange( const std::string& anim )
{
	if(m_bIsPlayAnim)
	{
		Kratos::EffectController* pCtrl = _GetEffectController(ManipulatorSystem.GetObject().GetSelection());
		pCtrl->StopAnimation();
		if(!anim.empty())
			pCtrl->PlayAnimation(anim, true);
	}
	m_curAnim = anim;
}

const std::string ManipulatorEffect::GetCurAnimationName()
{
	return m_curAnim;
}

std::vector<std::wstring> ManipulatorEffect::GetLocatorNames() const
{
	Ogre::Entity* ent = ManipulatorSystem.GetObject().GetSelection();
	assert(ent);

	std::vector<std::wstring> ret;
	//M3模型从max导出时locator命名为Ref_xxx形式
	Ogre::SkeletonPtr skel = ent->getMesh()->getSkeleton();
	Ogre::Skeleton::BoneIterator iter = skel->getBoneIterator();

	while (iter.hasMoreElements())
	{
		Ogre::Bone* pBone = iter.peekNext();
		if(pBone->getName().find("Ref_") != Ogre::String::npos)
			ret.push_back(Utility::EngineToUnicode(pBone->getName()));

		iter.getNext();
	}

	return std::move(ret);
}

void ManipulatorEffect::SetEffectParam( const std::string& param, const std::string& value )
{
	if(m_curEffect.empty())
		return;

	Kratos::AttachEffectBase* pEffect = _GetEffectController(ManipulatorSystem.GetObject().GetSelection())->GetEffect(m_curEffect);
	bool ret = pEffect->setParameter(param, value);
	assert(ret);
}

const std::string ManipulatorEffect::GetEffectParam( const std::string& param )
{
	if(m_curEffect.empty())
		return Ogre::StringUtil::BLANK;

	Kratos::AttachEffectBase* pEffect = _GetEffectController(ManipulatorSystem.GetObject().GetSelection())->GetEffect(m_curEffect);
	return pEffect->getParameter(param);
}

void ManipulatorEffect::OnFrameMove( float dt )
{
	Ogre::Entity* ent = ManipulatorSystem.GetObject().GetSelection();
	if (ent && ent->hasSkeleton())
	{
		Kratos::EffectController* controller = m_effectTemplates[ent->getMesh()->getName()];
		controller->Update(dt);
	}
}

bool ManipulatorEffect::GetIsPlayingAnim() const
{
	return m_bIsPlayAnim;
}

void ManipulatorEffect::LoadEffect( rapidxml::xml_node<>* node )
{
	std::string nameID("");
	//per unit
	auto& unitTable = GameDataDefManager::GetSingleton().m_unitData;
	for (auto itUnit=unitTable.begin(); itUnit!=unitTable.end(); ++itUnit)
	{
		SUnitData& unitData = itUnit->second;
		Kratos::EffectController* effectTmpl = new Kratos::EffectController(NULL);
		m_effectTemplates.insert(std::make_pair(unitData.m_meshname, effectTmpl));

		//per animation
		for (auto itAnim=unitData.m_effects.begin(); itAnim!=unitData.m_effects.end(); ++itAnim)
		{
			auto& slots = itAnim->second;
			//per effect
			for (size_t iEffect=0; iEffect<slots.size(); ++iEffect)
			{
				Ogre::NameValuePairList& info = slots[iEffect].params;
				Kratos::eAttachEffect type = Kratos::AttachEffectBase::GetTypeFromString(info["type"]);
				Kratos::AttachEffectBase* effect = effectTmpl->AddEffect(itAnim->first, type);
				effect->setParameterList(info);

				//查找最大名字ID,以后命名新的特效以此为据
				const std::string& name = effect->GetName();
				if(name > nameID)
					nameID = name;
			}
		}
		//特效模板已加载,清空原始数据,因为发生编辑,它们即无效了
		unitData.m_effects.clear();
	}

	if(!nameID.empty())
	{
		int len = strlen("Effect_");
		m_effectNameID = Ogre::StringConverter::parseInt(nameID.substr(len));
		m_effectNameID++;
	}
}

void ManipulatorEffect::BindEntityToEffectTemplate( Ogre::Entity* ent )
{
	Kratos::EffectController* pCtrl = _GetEffectController(ManipulatorSystem.GetObject().GetSelection());
	pCtrl->_SetParent(ent);
}

std::wstring ManipulatorEffect::AddEffect(int type)
{
	assert(!m_curAnim.empty());

	std::string newName("Effect_");
	newName += Ogre::StringConverter::toString(m_effectNameID++);

	Kratos::EffectController* pCtrl = _GetEffectController(ManipulatorSystem.GetObject().GetSelection());
	pCtrl->AddEffect(m_curAnim, (Kratos::eAttachEffect)type)->SetName(newName);
	m_curEffect = newName;

	return Utility::EngineToUnicode(newName);
}

void ManipulatorEffect::RemoveEffect( const std::string& name )
{
	Kratos::EffectController* pCtrl = _GetEffectController(ManipulatorSystem.GetObject().GetSelection());
	Kratos::AttachEffectBase* effect = pCtrl->GetEffect(name);
	assert(effect);
	m_curEffect = "";

	pCtrl->RemoveEffect(name);
	
}

Kratos::EffectController* ManipulatorEffect::_GetEffectController(Ogre::Entity* ent)
{
	auto iter = m_effectTemplates.find(ent->getMesh()->getName());
	assert(iter != m_effectTemplates.end());
	return iter->second;
}

std::vector<std::wstring> ManipulatorEffect::GetAttachEffectNames()
{
	std::vector<std::wstring> ret;
	if(m_curAnim.empty())
		return ret;

	Ogre::Entity* ent = ManipulatorSystem.GetObject().GetSelection();
	const auto& effects = _GetEffectController(ent)->_GetEffects().find(m_curAnim)->second;
	for(size_t i=0; i<effects.size(); ++i)
	{
		const std::string& name = effects[i]->GetName();
		ret.push_back(Utility::EngineToUnicode(name));
	}

	return ret;
}

void ManipulatorEffect::OnAttachEffectSelChange( const std::string& effect )
{
	m_curEffect = effect;
}

void ManipulatorEffect::SetSharpenEnable( bool bEnable )
{
	RenderManager.EnableSharpen(bEnable);
}

bool ManipulatorEffect::GetSharpenEnable() const
{
	return RenderManager.GetEffectConfig().bSharpen;
}

void ManipulatorEffect::SetFXAAEnable( bool bEnable )
{
	RenderManager.EnableFXAA(bEnable);
}

bool ManipulatorEffect::GetFXAAEnable() const
{
	return RenderManager.GetEffectConfig().bFXAA;
}

int ManipulatorEffect::GetAttachEffectType( const std::string& name )
{
	Kratos::EffectController* pCtrl = _GetEffectController(ManipulatorSystem.GetObject().GetSelection());
	Kratos::AttachEffectBase* effect = pCtrl->GetEffect(name);
	return effect->GetType();
}
