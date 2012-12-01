#include "stdafx.h"
#include "Command.h"
#include "Unit.h"
#include "UnitState.h"


HarvestCommand::HarvestCommand(Unit* pOwner, const Ogre::Vector3& resPos)
:m_resPos(resPos)
{
	m_pOwner = pOwner;
	m_type = eCommandType_Harvest;
}

void HarvestCommand::Clone( const CommandBase& cmd )
{
	CommandBase::Clone(cmd);
	const HarvestCommand& harvestCmd = dynamic_cast<const HarvestCommand&>(cmd);
	m_resPos = harvestCmd.GetResPos();
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
	m_pOwner->SetState(MoveState(m_pOwner));
}

void CommandBase::Cancel()
{
	m_pOwner->SetState(IdleState(m_pOwner));
}
