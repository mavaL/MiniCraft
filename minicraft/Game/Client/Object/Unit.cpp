#include "stdafx.h"
#include "Unit.h"
#include "GameDefine.h"
#include "World.h"
#include "Command.h"
#include "AIComponent.h"
#include "PathComponent.h"
#include "HarvestComponent.h"

IMPL_PARAM_COMMAND(Unit, ClampPos, Vector3)
IMPL_PARAM_COMMAND_STR(Unit, UnitName)

//**** Define stuff for the Lua Class ****//
// Define the Lua ClassName
const char Unit::className[] = "Unit";

// Define the methods we will expose to Lua
#define method(class, name) {#name, &class::name}
Luna<Unit>::RegType Unit::methods[] = 
{
	method(Unit, PlayAnimation),
	method(Unit, StopAnimation),
	method(Unit, FindPath),
	method(Unit, SetState),
	method(Unit, SetDestPosition),
	method(Unit, GetDestPosition),
	method(Unit, GetCurCommandType),
	method(Unit, UpdatePathFinding),
	method(Unit, AttachRes),
	method(Unit, DetachRes),
	{0,0}
};

const STRING Unit::UNIT_TABLE_NAME	=	"UnitTable";

Unit::Unit()
:SelectableObject()
,m_pAnimState(nullptr)
,m_unitName(Ogre::StringUtil::BLANK)
,m_fStopTime(0)
{
	Ogre::ParamDictionary* dict = getParamDictionary();
	dict->addParameter(Ogre::ParameterDef("clamppos", "clamp position of the object", Ogre::PT_VECTOR3), &m_sCmdClampPos);
	dict->addParameter(Ogre::ParameterDef("unitName", "logic representation of the unit", Ogre::PT_STRING), &m_sCmdUnitName);

	//将对象绑定到lua
	//ScriptSystem::GetSingleton().BindObjectToLua<Unit>(UNIT_TABLE_NAME, GetID(), this);
}

Unit::~Unit()
{
	DestroyRenderInstance();
}

void Unit::Update( float dt )
{
	if(m_pAnimState)
		m_pAnimState->addTime(dt);

	__super::Update(dt);
}

void Unit::PlayAnimation( eAnimation type, bool bLoop )
{
	//停止当前动画
	StopAnimation();

	SUnitData* pUnitData = &GameDataDefManager::GetSingleton().m_unitData[m_unitName];
	const STRING& animName = (pUnitData->m_anims)[type];

	m_pAnimState = m_pEntity->getAnimationState(animName);
	assert(m_pAnimState);
	m_pAnimState->setEnabled(true);
	m_pAnimState->setTimePosition(0);
	m_pAnimState->setLoop(bLoop);
}

void Unit::StopAnimation()
{
	if(m_pAnimState)
	{
		m_pAnimState->setEnabled(false);
		m_pAnimState = nullptr;
	}
}

int Unit::PlayAnimation( lua_State* L )
{
// 	bool bLoop = ScriptSystem::GetSingleton().Get_Bool(-1);
// 	const STRING topAnimName = ScriptSystem::GetSingleton().Get_String(-2);
// 	PlayAnimation(topAnimName, bLoop);

	return 0;
}

int Unit::StopAnimation( lua_State* L )
{
	StopAnimation();
	return 0;
}

int Unit::SetState( lua_State* L )
{
// 	const eUnitState state = static_cast<eUnitState>(ScriptSystem::GetSingleton().Get_Int(-1));
// 	m_pAI->SetState(state);

	return 0;
}

int Unit::FindPath( lua_State* L )
{
// 	ScriptSystem& system = ScriptSystem::GetSingleton();
// 	float x = system.Get_Float(-3);
// 	float y = system.Get_Float(-2);
// 	float z = system.Get_Float(-1);
// 
// 	if(m_pAI->FindPath(Ogre::Vector3(x, y, z)))
// 	{
// 		system.Push_Bool(true);
// 	}
// 	else
// 	{
// 		system.Push_Bool(false);
// 	}

	return 1;
}

int Unit::GetDestPosition( lua_State* L )
{
// 	ScriptSystem& system = ScriptSystem::GetSingleton();
// 	system.Push_Float(m_destPos.x);
// 	system.Push_Float(m_destPos.y);
// 	system.Push_Float(m_destPos.z);

	return 3;
}

int Unit::GetCurCommandType( lua_State* L )
{
// 	ScriptSystem::GetSingleton().Push_Int((int)m_pAI->GetCurCommand()->GetType());

	return 1;
}

int Unit::UpdatePathFinding( lua_State* L )
{
// 	const float dt = ScriptSystem::GetSingleton().Get_Float(-1);
// 	bool bArrived = false;
// 
// 	POS curPos = m_pAI->GetAgentPos();
// 	World::GetSingleton().ClampToTerrain(curPos);
// 
// 	m_pSceneNode->lookAt(curPos, Ogre::Node::TS_WORLD/*, Ogre::Vector3::UNIT_Z*/);
// 	SetPosition(curPos);
// 
// 	if (curPos.positionEquals(m_destPos, 0.1f))
// 		bArrived = true;
// 
// 	ScriptSystem::GetSingleton().Push_Bool(bArrived);

	return 1;
}

int Unit::AttachRes( lua_State* L )
{
	ScriptSystem& system = ScriptSystem::GetSingleton();
	float x = system.Get_Float(-4);
	float y = system.Get_Float(-3);
	float z = system.Get_Float(-2);
	float fScale = system.Get_Float(-1);
	const STRING meshname = system.Get_String(-5);

// 	//创建背负资源
// 	if(m_pResEnt == nullptr)
// 	{
// 		m_pResEnt = Ogre::Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->createEntity(meshname.c_str());
// 		m_pResNode = m_pNode->createChildSceneNode(Ogre::Vector3(x, y, z));
// 		m_pResNode->attachObject(m_pResEnt);
// 		m_pResNode->scale(fScale, fScale, fScale);
// 	}
// 	m_pResNode->setVisible(true);

	return 0;
}

int Unit::DetachRes( lua_State* L )
{
	//m_pResNode->setVisible(false);

	return 0;
}

int Unit::SetDestPosition( lua_State* L )
{
// 	ScriptSystem& system = ScriptSystem::GetSingleton();
// 	float x = system.Get_Float(-3);
// 	float y = system.Get_Float(-2);
// 	float z = system.Get_Float(-1);
// 
// 	m_destPos = Ogre::Vector3(x, y, z);

	return 0;
}

void Unit::SetClampPos( const POS& pos )
{
	Ogre::Vector3 adjustPos = pos;
	bool bSucceed = World::GetSingleton().ClampPosToNavMesh(adjustPos);
	assert(bSucceed);
	World::GetSingleton().ClampToTerrain(adjustPos);

	__super::SetPosition(adjustPos);
}

void Unit::_OnCommandFinished( eCommandType cmd )
{
	if(cmd == eCommandType_Move)
		GetPath()->StopMove();

	__super::_OnCommandFinished(cmd);
}

void Unit::SetUnitName( const STRING& name )
{
	m_unitName = name;

	GameDataDefManager& dataMgr = GameDataDefManager::GetSingleton();
	auto iter = dataMgr.m_unitData.find(m_unitName);
	assert(iter != dataMgr.m_unitData.end());
	const SUnitData& data = iter->second;
	
	//创建渲染实例
	setParameter("meshname", data.m_meshname);

	//初始化技能
	for (int iAbil=0; iAbil<MAX_ABILITY_SLOT; ++iAbil)
	{
		const std::string& strAbil = data.m_vecAbilities[iAbil];
		if (strAbil != "")
		{
			const SAbilityData& pAbil = dataMgr.m_abilityData.find(strAbil)->second;
			SetAbility(iAbil, &pAbil);

			//采集组件
			if(pAbil.m_type == eCommandType_Gather)
				AddComponent(eComponentType_Harvest, new HarvestComponent(this));
		}
	}	
}

void Unit::StopAction()
{
	GetAi()->CancelAllCommand();
	StopAnimation();
	SetStopTime(0);
}
