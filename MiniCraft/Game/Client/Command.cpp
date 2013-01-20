#include "stdafx.h"
#include "Command.h"
#include "Unit.h"
#include "UnitState.h"
#include "GameDefine.h"


HarvestCommand::HarvestCommand(Unit* pOwner)
{
	m_pOwner = pOwner;
	m_type = eCommandType_Harvest;
}

void HarvestCommand::Clone( const CommandBase& cmd )
{
	CommandBase::Clone(cmd);
	const HarvestCommand& harvestCmd = dynamic_cast<const HarvestCommand&>(cmd);
}

void HarvestCommand::Excute()
{
	//首先向资源移动
	m_pOwner->SetDestPos(RES_COLLECT_POS);
	m_pOwner->SetState(eUnitState_Move);
}

BuildCommand::BuildCommand( Unit* pOwner, const Ogre::Vector3& pos )
:m_buildPos(pos)
{
	m_pOwner = pOwner;
	m_type = eCommandType_Build;
}

void BuildCommand::Clone( const CommandBase& cmd )
{
	CommandBase::Clone(cmd);
	const BuildCommand& buildCmd = dynamic_cast<const BuildCommand&>(cmd);
	m_buildPos = buildCmd.GetBuildPos();
}

AttackCommand::AttackCommand( Unit* pOwner, Unit* pTarget )
:m_pTarget(pTarget)
{
	m_pOwner = pOwner;
	m_type = eCommandType_Attack;
}

void AttackCommand::Clone( const CommandBase& cmd )
{
	CommandBase::Clone(cmd);
	const AttackCommand& attackCmd = dynamic_cast<const AttackCommand&>(cmd);
	m_pTarget = attackCmd.GetTarget();
}

MoveCommand::MoveCommand( Unit* pOwner, const Ogre::Vector3& DestPos )
:m_destPos(DestPos)
{
	m_pOwner = pOwner;
	m_type = eCommandType_Move;
}

void MoveCommand::Clone( const CommandBase& cmd )
{
	CommandBase::Clone(cmd);
	const MoveCommand& moveCmd = dynamic_cast<const MoveCommand&>(cmd);
	m_destPos = moveCmd.GetDestPos();
}

void MoveCommand::Excute()
{
	m_pOwner->SetDestPos(m_destPos);
	m_pOwner->SetState(eUnitState_Move);
}

void CommandBase::Cancel()
{
	m_pOwner->SetState(eUnitState_Idle);
}
