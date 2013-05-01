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
#include "ObjectManager.h"

IMPL_PARAM_COMMAND_STR(Unit, Name)
IMPL_PARAM_COMMAND(Unit, Race, Int)

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
	method(Unit, GetAttackTargetID),
	method(Unit, SetAttackTargetID),
	method(Unit, FindNearestEnemy),
	{0,0}
};

const STRING Unit::UNIT_TABLE_NAME	=	"UnitTable";
std::unordered_map<STRING, Ogre::Entity*>			Unit::m_portraitCache;
std::unordered_map<STRING, Ogre::AnimationState*>	Unit::m_portraitAnimCache;

const float HP_BAR_WIDTH	=	0.6f;
const float HP_BAR_HEIGHT	=	0.1f;

Unit::Unit()
:SelectableObject()
,m_unitName(Ogre::StringUtil::BLANK)
,m_fStopTime(0)
,m_data(nullptr)
,m_pHealthBar(nullptr)
,m_pScriptSystem(Kratos::ScriptSystem::GetSingletonPtr())
,m_attkTargetID(-1)
,m_fAttkTime(0)
,m_fCurHP(0)
,m_fFullHP(0)
{
	if(InitParamDict("Unit"))
	{
		Ogre::ParamDictionary* dict = getParamDictionary();
		dict->addParameter(Ogre::ParameterDef("name", "Unit name", Ogre::PT_STRING), &m_sCmdName);
		dict->addParameter(Ogre::ParameterDef("race", "Race", Ogre::PT_INT), &m_sCmdRace);

		Luna<Unit>::Register(m_pScriptSystem->GetLuaState());
	}

	//将对象绑定到lua
	m_pScriptSystem->BindObjectToLua<Unit>(UNIT_TABLE_NAME, GetID(), this);
}

Unit::~Unit()
{
	SetAttackTarget(-1);
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
	//TODO: 需要instancing!!
	m_pHealthBar = RenderManager.m_pSceneMgr->createBillboardSet(1);
	//m_pHealthBar->createBillboard(0,0,0, COLOR::Black)->setDimensions(0.5f, 0.1f);
	m_pHealthBar->setBillboardOrigin(Ogre::BBO_CENTER);
	m_pHealthBar->createBillboard(POS::ZERO, COLOR::Green)->setDimensions(HP_BAR_WIDTH, HP_BAR_HEIGHT);
	m_pHealthBar->setMaterialName("HPBar");
	m_pHealthBar->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	m_pSceneNode->createChildSceneNode(POS(0,1.5f,0))->attachObject(m_pHealthBar);

	m_fFullHP = Ogre::StringConverter::parseReal(m_data->m_battleInfo.params["hp"]);
	m_fCurHP = m_fFullHP;

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

	GetEntity()->setQueryFlags(eQueryType_Unit);
}

void Unit::Update( float dt )
{
	if(m_bSelected)
		m_portraitAnimCache[m_unitName]->addTime(dt);

	//更新血条
	{
		float hpBarWidth = (m_fCurHP / m_fFullHP) * HP_BAR_WIDTH;
		Ogre::Billboard* bb = m_pHealthBar->getBillboard(0);
		bb->setDimensions(hpBarWidth, HP_BAR_HEIGHT);
	}

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

float Unit::GetAttackInterval() const
{
	return Ogre::StringConverter::parseReal(m_data->m_battleInfo.params["freq"]);
}

float Unit::GetAttackDamage() const
{
	return Ogre::StringConverter::parseReal(m_data->m_battleInfo.params["damage"]);
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

int Unit::GetAttackTargetID( lua_State* L )
{
	m_pScriptSystem->Push_Int(m_attkTargetID);
	return 1;
}

int Unit::SetAttackTargetID( lua_State* L )
{
	int id = m_pScriptSystem->Get_Int(-1);
	SetAttackTarget(id);

	return 0;
}

void Unit::SetAttackTarget( int ID )
{
	//取消回调
	if (m_attkTargetID != -1)
	{
		Unit* oldTarget = dynamic_cast<Unit*>(ObjectManager::GetSingleton().GetObject(m_attkTargetID));
		oldTarget->RemoveCallback(this);
	}

	//设置回调
	if (ID != -1)
	{
		Unit* newTarget = dynamic_cast<Unit*>(ObjectManager::GetSingleton().GetObject(ID));
		newTarget->AddCallback(this);
	}

	m_attkTargetID = ID;
}

int Unit::FindNearestEnemy( lua_State* L )
{
	//TODO: 轮询效率太低了
	const POS& origin = GetPosition();
	const float ALERT_RADIUS = 2.0f;
	Ogre::Sphere s(origin, ALERT_RADIUS);
	std::vector<Ogre::MovableObject*> queryResult;
	World::GetSingleton().GetSphereSceneQueryResult(s, queryResult, eQueryType_Unit);

	//寻找距离最近单位
	float nearestDist = 10000;
	int findID = -1;
	for (size_t i=0; i<queryResult.size(); ++i)
	{
		float sqDist = queryResult[i]->getParentNode()->_getDerivedPosition().squaredDistance(origin);
		SelectableObject* obj = Ogre::any_cast<SelectableObject*>(queryResult[i]->getUserAny());
		bool bAlly = obj->GetAi()->IsAlly(this);
		Unit* pUnit = dynamic_cast<Unit*>(obj);

		if(	sqDist < nearestDist && !bAlly )
		{
			findID = obj->GetID();
			nearestDist = sqDist;
		}
	}

	m_pScriptSystem->Push_Int(findID);
	return 1;
}

Unit* Unit::GetAttackTarget()
{
	if(m_attkTargetID == -1)
		return nullptr;
	
	return static_cast<Unit*>(ObjectManager::GetSingleton().GetObject(m_attkTargetID));
}

void Unit::_OnAttacked( Unit* attcker )
{
	m_fCurHP -= attcker->GetAttackDamage();

	//嗝儿屁了
	if (m_fCurHP <= 0)
	{
		RemoveComponent(eComponentType_Behevior);
		m_pAi->SetCurState(eObjectState_Death);

		//不再攻击别人
		SetAttackTarget(-1);

		//通知别人也不再攻击自己...
		Excute([&](UnitEventCallback* listener){ listener->OnUnitDeath(this); });
		//Notice:在回调处理中不能调RemoveCallback,会破坏遍历过程,所以回调之后再调
		RemoveAllCallbacks();
	}
}

void Unit::OnUnitDeath(Unit* pUnit)
{
	m_attkTargetID = -1;
}




