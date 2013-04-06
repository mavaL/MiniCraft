#include "stdafx.h"
#include "Unit.h"
#include "GameDefine.h"
#include "World.h"
#include "Command.h"
#include "AIComponent.h"
#include "PathComponent.h"
#include "HarvestComponent.h"
#include "AnimatedComponent.h"
#include "BehaviorComponent.h"
#include "OgreManager.h"
#include "GameDataDef.h"
#include "AI/BehaviorTree/BehaviorTreeTemplate.h"
#include "AI/BehaviorTree/BlackBoard.h"

IMPL_PARAM_COMMAND_STR(Unit, Name)
IMPL_PARAM_COMMAND(Unit, Race, Int)
IMPL_PARAM_COMMAND(Unit, ProduceTime, Real)
IMPL_PARAM_COMMAND_STR(Unit, PortraitName)

//**** Define stuff for the Lua Class ****//
// Define the Lua ClassName
const char Unit::className[] = "Unit";

// Define the methods we will expose to Lua
#define method(class, name) {#name, &class::name}
Luna<Unit>::RegType Unit::methods[] = 
{
	method(Unit, SetBlackboardParamInt),
	method(Unit, SetBlackboardParamFloat),
	method(Unit, SetBlackboardParamBool),
	method(Unit, GetHarvestStage),
	method(Unit, GetGatheringTime),
	{0,0}
};

const STRING Unit::UNIT_TABLE_NAME	=	"UnitTable";
std::unordered_map<STRING, Ogre::Entity*>			Unit::m_portraitCache;
std::unordered_map<STRING, Ogre::AnimationState*>	Unit::m_portraitAnimCache;

Unit::Unit()
:SelectableObject()
,m_unitName(Ogre::StringUtil::BLANK)
,m_fStopTime(0)
,m_data(nullptr)
,m_pHealthBar(nullptr)
,m_pScriptSystem(Kratos::ScriptSystem::GetSingletonPtr())
{
	if(InitParamDict("Unit"))
	{
		Ogre::ParamDictionary* dict = getParamDictionary();
		dict->addParameter(Ogre::ParameterDef("name", "Unit name", Ogre::PT_STRING), &m_sCmdName);
		dict->addParameter(Ogre::ParameterDef("race", "Race", Ogre::PT_INT), &m_sCmdRace);
		dict->addParameter(Ogre::ParameterDef("portrait", "Portrait mesh name", Ogre::PT_STRING), &m_sCmdPortraitName);
		dict->addParameter(Ogre::ParameterDef("timecost", "Time cost to produce this unit", Ogre::PT_REAL), &m_sCmdProduceTime);
	}

	//将对象绑定到lua
	m_pScriptSystem->BindObjectToLua<Unit>(UNIT_TABLE_NAME, GetID(), this);
}

Unit::~Unit()
{
	DestroyRenderInstance();
}

void Unit::_OnCommandFinished( eCommandType cmd )
{
	if(cmd == eCommandType_Move)
		GetPath()->StopMove();

	__super::_OnCommandFinished(cmd);
}

void Unit::Init()
{
	assert(m_data);

	//创建渲染实例
	setParameter("meshname", m_data->params["meshname"]);

	//血条
// 	m_pHealthBar = RenderManager.m_pSceneMgr->createBillboardSet(2);
// 	//m_pHealthBar->createBillboard(0,0,0, COLOR::Black)->setDimensions(0.5f, 0.1f);
// 	m_pHealthBar->setBillboardOrigin(Ogre::BBO_CENTER_LEFT);
// 	m_pHealthBar->createBillboard(0,0,0.01f, COLOR::Red)->setDimensions(0.5f, 0.1f);
// 	m_pHealthBar->setMaterialName("HPBar");
// 	m_pSceneNode->createChildSceneNode(POS(0,1.5f,0))->attachObject(m_pHealthBar);

	//初始化技能
	for (int iAbil=0; iAbil<MAX_ABILITY_SLOT; ++iAbil)
	{
		const std::string& strAbil = m_data->m_vecAbilities[iAbil];
		if (strAbil != "")
		{
			const SAbilityData& pAbil = GameDataDefManager::GetSingleton().m_abilityData.find(strAbil)->second;
			SetAbility(iAbil, &pAbil);

			//采集组件
			if(pAbil.m_type == eCommandType_Gather)
				AddComponent(eComponentType_Harvest, new HarvestComponent(this));
		}
	}

	if(GetEntity()->hasSkeleton())
		AddComponent(eComponentType_Animated, new AnimatedComponent(this));

	AddComponent(eComponentType_AI, new AiComponent(this));
	eGameRace race = (eGameRace)GetRace();
	GetAi()->SetFaction(World::GetSingleton().GetFaction(race));
}

void Unit::Update( float dt )
{
	if(m_bSelected)
		m_portraitAnimCache[m_unitName]->addTime(dt);

	/////测试更新血条
// 	Ogre::Billboard* bb = m_pHealthBar->getBillboard(0);
// 	float oldWidth = bb->getOwnWidth();
// 	bb->setDimensions(oldWidth + dt * 0.02f, 0.1f);

	__super::Update(dt);
}

void Unit::StopAction()
{
	GetAi()->CancelAllCommand();
	GetAnim()->StopAnimation();
	SetStopTime(0);
}

Ogre::Entity* Unit::GetPortrait(Ogre::SceneManager* sm, Ogre::Light* light)
{
	auto iter = m_portraitCache.find(m_unitName);
	if(iter == m_portraitCache.end())
	{
		Ogre::Entity* ent = RenderManager.CreateEntityWithTangent(m_data->params["portrait"], sm);
		Ogre::AnimationState* anim = ent->getAnimationState("Portrait");
		assert(anim);
		anim->setEnabled(false);
		anim->setLoop(true);

		auto params = ent->getSubEntity(0)->getMaterial()->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
		params->setNamedConstant("LightColor", light->getDiffuseColour());

		iter = m_portraitCache.insert(std::make_pair(m_unitName, ent)).first;
		m_portraitAnimCache.insert(std::make_pair(m_unitName, anim));
	}

	return iter->second;
}

void Unit::_OnSelected( bool bSelected )
{
	__super::_OnSelected(bSelected);

	Ogre::AnimationState* anim = m_portraitAnimCache[m_unitName];
	if (bSelected)
	{
		anim->setEnabled(true);
		anim->setTimePosition(0);
	}
	else
	{
		anim->setEnabled(false);
	}
}

const STRING& Unit::GetPortraitName() const
{
	return m_data->params["portrait"];
}

void Unit::SetName( const STRING& name )
{
	GameDataDefManager& dataMgr = GameDataDefManager::GetSingleton();
	auto iter = dataMgr.m_unitData.find(name);
	assert(iter != dataMgr.m_unitData.end());
	m_data = &iter->second;

	m_unitName = name;
}

int Unit::GetRace() const
{
	return Ogre::StringConverter::parseInt(m_data->params["race"]);
}

float Unit::GetProduceTime() const
{
	return Ogre::StringConverter::parseReal(m_data->params["timecost"]);
}

int Unit::SetBlackboardParamInt( lua_State* L )
{
	const STRING paramName = m_pScriptSystem->Get_String(-2);
	const int iValue = m_pScriptSystem->Get_Int(-1);
	const STRING value = Ogre::StringConverter::toString(iValue);

	m_pBehavior->GetBlackboard()->SetParam(paramName, value);

	return 0;
}

int Unit::SetBlackboardParamFloat( lua_State* L )
{
	const STRING paramName = m_pScriptSystem->Get_String(-2);
	const float fValue = m_pScriptSystem->Get_Float(-1);
	const STRING value = Ogre::StringConverter::toString(fValue);

	m_pBehavior->GetBlackboard()->SetParam(paramName, value);

	return 0;
}

int Unit::SetBlackboardParamBool( lua_State* L )
{
	const STRING paramName = m_pScriptSystem->Get_String(-2);
	const bool bValue = m_pScriptSystem->Get_Bool(-1);
	const STRING value = Ogre::StringConverter::toString(bValue);

	m_pBehavior->GetBlackboard()->SetParam(paramName, value);

	return 0;
}

int Unit::GetHarvestStage( lua_State* L )
{
	int iStage = m_pGather->GetCurStage();
	m_pScriptSystem->Push_Int(iStage);

	return 1;
}

int Unit::GetGatheringTime( lua_State* L )
{
	float fTime = m_pGather->GetGatherTime();
	m_pScriptSystem->Push_Float(fTime);

	return 1;
}
