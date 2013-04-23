#include "stdafx.h"
#include "ManipulatorEffect.h"
#include "OgreManager.h"
#include "ManipulatorScene.h"
#include "Utility.h"
#include "Effect/EffectController.h"
#include "Effect/EffectBase.h"

ManipulatorEffect::ManipulatorEffect()
:m_bIsPlayAnim(false)
,m_curAnim(Ogre::StringUtil::BLANK)
,m_curEffect(Ogre::StringUtil::BLANK)
,m_effectNameID(0)
{
	ManipulatorScene::GetSingleton().AddCallback(this);
}

ManipulatorEffect::~ManipulatorEffect()
{
	ManipulatorScene::GetSingleton().RemoveCallback(this);
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

void ManipulatorEffect::SetShadowParam( const std::string& name, const std::wstring& value )
{
	Kratos::ShadowParams& params = RenderManager.GetShadowParams();
	params[name] = Utility::UnicodeToEngine(value);
	RenderManager.ApplyShadowParams();
}

void ManipulatorEffect::SetSsaoParam( const std::string& name, const std::wstring& value )
{
	Kratos::SsaoParams& params = RenderManager.GetSsaoParams();
	params[name] = Utility::UnicodeToEngine(value);
	RenderManager.ApplySsaoParams();
}

const std::string& ManipulatorEffect::GetShadowParam( const std::string& name ) const
{
	const Kratos::ShadowParams& params = RenderManager.GetShadowParams();
	auto iter = params.find(name);
	assert(iter != params.end());
	return iter->second;
}

const std::string& ManipulatorEffect::GetSsaoParam( const std::string& name ) const
{
	const Kratos::SsaoParams& params = RenderManager.GetSsaoParams();
	auto iter = params.find(name);
	assert(iter != params.end());
	return iter->second;
}

void ManipulatorEffect::Serialize( rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode )
{
	using namespace Ogre;
	const Kratos::SEffectConfig& cfg = RenderManager.GetEffectConfig();
	SceneManager* sm = RenderManager.m_pSceneMgr;

	const String strAmbient = StringConverter::toString(sm->getAmbientLight());
	const String strShadowOn = StringConverter::toString(cfg.bShadow);

	//shadow param
	rapidxml::xml_node<>* shadowNode = doc->allocate_node(rapidxml::node_element, "shadow");

	shadowNode->append_attribute(doc->allocate_attribute("AmbientLight", doc->allocate_string(strAmbient.c_str())));
	shadowNode->append_attribute(doc->allocate_attribute("EnableShadow", doc->allocate_string(strShadowOn.c_str())));
	
	Kratos::ShadowParams& shadowParams = RenderManager.GetShadowParams();
	for(auto iter=shadowParams.begin(); iter!=shadowParams.end(); ++iter)
		shadowNode->append_attribute(doc->allocate_attribute(iter->first.c_str(), iter->second.c_str()));

	XMLNode->append_node(shadowNode);

	//ssao param
	const String strSsaoOn = StringConverter::toString(cfg.bSSAO);
	rapidxml::xml_node<>* ssaoNode = doc->allocate_node(rapidxml::node_element, "ssao");
	ssaoNode->append_attribute(doc->allocate_attribute("EnableSSAO", doc->allocate_string(strSsaoOn.c_str())));
	
	Kratos::SsaoParams& ssaoParams = RenderManager.GetSsaoParams();
	for(auto iter=ssaoParams.begin(); iter!=ssaoParams.end(); ++iter)
		ssaoNode->append_attribute(doc->allocate_attribute(iter->first.c_str(), iter->second.c_str()));

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

				SaveStringInterface(effect, data.params);
	
				unitData->m_effects[itAnim->first].push_back(data);
			}
		}
	}
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
		m_effectTemplates.insert(std::make_pair(unitData.params["meshname"], effectTmpl));

		//per animation
		for (auto itAnim=unitData.m_effects.begin(); itAnim!=unitData.m_effects.end(); ++itAnim)
		{
			auto& slots = itAnim->second;
			//per effect
			for (size_t iEffect=0; iEffect<slots.size(); ++iEffect)
			{
				Ogre::NameValuePairList& info = slots[iEffect].params;
				int type = Ogre::StringConverter::parseInt(info["type"]);
				Kratos::AttachEffectBase* effect = effectTmpl->AddEffect(itAnim->first, (Kratos::eAttachEffect)type);
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

void ManipulatorEffect::OnSceneClose()
{
	for(auto iter=m_effectTemplates.begin(); iter!=m_effectTemplates.end(); ++iter)
		delete iter->second;
	m_effectTemplates.clear();
}




