#include "stdafx.h"
#include "UnitState.h"
#include "Unit.h"
#include "Command.h"
#include "GameDefine.h"


///TODO: 各UnitState逻辑现在都改为依赖Unit来实现了,所以各UnitState可写为单件,避免频繁创建销毁

void IdleState::Enter()
{
	ScriptSystem::GetSingleton().Call("IdleState_Enter");
}

void IdleState::Exit()
{
	ScriptSystem::GetSingleton().Call("IdleState_Exit");
}

void IdleState::Update(float dt)
{
	ScriptSystem::GetSingleton().Call("IdleState_Update", dt);
}

MoveState::MoveState( Unit* pOwner )
:UnitState(pOwner)
{
	m_type = eUnitState_Move;
}

void MoveState::Enter()
{
	ScriptSystem::GetSingleton().Call("MoveState_Enter");
}

void MoveState::Update( float dt )
{
	ScriptSystem::GetSingleton().Call("MoveState_Update", dt);
}

void MoveState::Exit()
{
	ScriptSystem::GetSingleton().Call("MoveState_Exit");
}

CollectResState::CollectResState( Unit* pOwner )
:UnitState(pOwner)
{ 
	m_type = eUnitState_Collect; 
}

void CollectResState::Enter()
{
	ScriptSystem::GetSingleton().Call("CollectResState_Enter");
}

void CollectResState::Update( float dt )
{
	ScriptSystem::GetSingleton().Call("CollectResState_Update", dt);
}

void CollectResState::Exit()
{
	ScriptSystem::GetSingleton().Call("CollectResState_Exit");
}

ReturnResState::ReturnResState( Unit* pOwner )
:UnitState(pOwner)
{ 
	m_type = eUnitState_Return; 
}

void ReturnResState::Enter()
{
	ScriptSystem::GetSingleton().Call("ReturnResState_Enter");
}

void ReturnResState::Update( float dt )
{
	ScriptSystem::GetSingleton().Call("ReturnResState_Update", dt);
}

void ReturnResState::Exit()
{
	ScriptSystem::GetSingleton().Call("ReturnResState_Exit");
}

