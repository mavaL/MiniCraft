#include "stdafx.h"
#include "UnitState.h"
#include "Unit.h"
#include "Command.h"
#include "GameDefine.h"


///TODO: 各UnitState逻辑现在都改为依赖Unit来实现了,所以各UnitState可写为单件,避免频繁创建销毁

void IdleState::Enter()
{
	ScriptSystem::GetSingleton().Call("IdleState_Enter", m_pOwner->GetID());
}

void IdleState::Exit()
{
	ScriptSystem::GetSingleton().Call("IdleState_Exit", m_pOwner->GetID());
}

void IdleState::Update(float dt)
{
	ScriptSystem::GetSingleton().Call("IdleState_Update", m_pOwner->GetID(), dt);
}

MoveState::MoveState( Unit* pOwner )
:UnitState(pOwner)
{
	m_type = eUnitState_Move;
}

void MoveState::Enter()
{
	ScriptSystem::GetSingleton().Call("MoveState_Enter", m_pOwner->GetID());
}

void MoveState::Update( float dt )
{
	ScriptSystem::GetSingleton().Call("MoveState_Update", m_pOwner->GetID(), dt);
}

void MoveState::Exit()
{
	ScriptSystem::GetSingleton().Call("MoveState_Exit", m_pOwner->GetID());
}

CollectResState::CollectResState( Unit* pOwner )
:UnitState(pOwner)
{ 
	m_type = eUnitState_Collect; 
}

void CollectResState::Enter()
{
	ScriptSystem::GetSingleton().Call("CollectResState_Enter", m_pOwner->GetID());
}

void CollectResState::Update( float dt )
{
	ScriptSystem::GetSingleton().Call("CollectResState_Update", m_pOwner->GetID(), dt);
}

void CollectResState::Exit()
{
	ScriptSystem::GetSingleton().Call("CollectResState_Exit", m_pOwner->GetID());
}

ReturnResState::ReturnResState( Unit* pOwner )
:UnitState(pOwner)
{ 
	m_type = eUnitState_Return; 
}

void ReturnResState::Enter()
{
	ScriptSystem::GetSingleton().Call("ReturnResState_Enter", m_pOwner->GetID());
}

void ReturnResState::Update( float dt )
{
	ScriptSystem::GetSingleton().Call("ReturnResState_Update", m_pOwner->GetID(), dt);
}

void ReturnResState::Exit()
{
	ScriptSystem::GetSingleton().Call("ReturnResState_Exit", m_pOwner->GetID());
}

