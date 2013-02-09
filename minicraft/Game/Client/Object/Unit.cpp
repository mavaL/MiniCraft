#include "stdafx.h"
#include "Unit.h"
#include "GameDefine.h"
#include "World.h"
#include "Command.h"


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
{
	//将对象绑定到lua
	ScriptSystem::GetSingleton().BindObjectToLua<Unit>(UNIT_TABLE_NAME, GetID(), this);
}

Unit::~Unit()
{
	DestroyRenderInstance();
}

void Unit::Update( float dt )
{
}

void Unit::PlayAnimation( const STRING& animName, bool bLoop )
{
	//停止当前动画
	StopAnimation();

	m_pAnimState = m_pEntity->getAnimationState(animName);
	assert(m_pAnimState);
	m_pAnimState->setEnabled(true);
	m_pAnimState->setTimePosition(0);
	m_pAnimState->setLoop(bLoop);
}

void Unit::StopAnimation()
{
	m_pAnimState->setEnabled(false);
	m_pAnimState = nullptr;
}

int Unit::PlayAnimation( lua_State* L )
{
	bool bLoop = ScriptSystem::GetSingleton().Get_Bool(-1);
	const STRING topAnimName = ScriptSystem::GetSingleton().Get_String(-2);
	PlayAnimation(topAnimName, bLoop);

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
	ScriptSystem& system = ScriptSystem::GetSingleton();
	system.Push_Float(m_destPos.x);
	system.Push_Float(m_destPos.y);
	system.Push_Float(m_destPos.z);

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
	ScriptSystem& system = ScriptSystem::GetSingleton();
	float x = system.Get_Float(-3);
	float y = system.Get_Float(-2);
	float z = system.Get_Float(-1);

	m_destPos = Ogre::Vector3(x, y, z);

	return 0;
}

void Unit::SetDestPos( const Ogre::Vector3& destPos )
{
	Ogre::Vector3 adjustPos(destPos);
	World::GetSingleton().ClampPosToNavMesh(adjustPos);
	
	m_destPos = adjustPos;
}

void Unit::SetPosition( const POS& pos )
{
	Ogre::Vector3 adjustPos = pos;
	bool bSucceed = World::GetSingleton().ClampPosToNavMesh(adjustPos);
	assert(bSucceed);
	World::GetSingleton().ClampToTerrain(adjustPos);

	__super::SetPosition(adjustPos);
}
