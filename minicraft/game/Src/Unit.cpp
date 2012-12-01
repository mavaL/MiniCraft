#include "stdafx.h"
#include "Unit.h"
#include "OgreDetourCrowd.h"
#include "UnitState.h"
#include "Command.h"



Unit::Unit( Ogre::Entity* pEnt, Ogre::SceneNode* pNode, OgreRecast* pRecast, OgreDetourCrowd* pDetour )
:m_pCurCommand(nullptr)
,m_pEntity(pEnt)
,m_pRecast(pRecast)
,m_pDetour(pDetour)
,m_pNode(pNode)
,m_pCurState(nullptr)
{
	m_pNode->attachObject(m_pEntity);
	m_pNode->translate(0, 5, 0, Ogre::Node::TS_WORLD);
	//将单位缩放至适于Recast库
	float scale = m_pDetour->getAgentRadius() / pEnt->getBoundingRadius();
	m_pNode->setScale(scale, scale, scale);

	//创建寻路对象
	m_agentID = m_pDetour->addAgent(pNode->_getDerivedPosition());
	m_pAgent = const_cast<dtCrowdAgent*>(m_pDetour->getAgent(m_agentID));
	assert(m_pAgent);

	// TODO:搞清楚这个有什么用
	//m_pEntity->setQueryFlags(DEFAULT_MASK);
}

void Unit::GiveCommand( const CommandBase& cmd )
{
	//停止当前命令
	if (m_pCurCommand)
		CancelCurCommand();

	//创建新命令
	switch (cmd.GetType())
	{
	case eCommandType_Move:		m_pCurCommand = new MoveCommand; break;
	case eCommandType_Attack:	m_pCurCommand = new AttackCommand; break;
	case eCommandType_Build:	m_pCurCommand = new BuildCommand; break;
	case eCommandType_Harvest:	m_pCurCommand = new HarvestCommand; break;
	}
	m_pCurCommand->Clone(cmd);

	//执行新命令
	m_pCurCommand->Excute();
}

void Unit::CancelCurCommand()
{
	if(!m_pCurCommand)
		return;

	m_pCurCommand->Cancel();
	delete m_pCurCommand;
	m_pCurCommand = nullptr;
}

void Unit::SetState( const UnitState& newState )
{
	//退出当前状态
	if(m_pCurState)
	{
		m_pCurState->Exit();
		delete m_pCurState;
	}
	//创建新状态
	switch (newState.GetType())
	{
	case eUnitState_Idle:		m_pCurState = new IdleState(this); break;
	case eUnitState_Move:		m_pCurState = new MoveState(this); break;
	case eUnitState_Attack:		m_pCurState = new AttackState(this); break;
	case eUnitState_Build:		m_pCurState = new BuildState(this); break;
	case eUnitState_Collect:	m_pCurState = new CollectResState(this); break;
	case eUnitState_Return:		m_pCurState = new ReturnResState(this); break;
	}
	
	m_pCurState->Enter();
}

void Unit::Update( float dt )
{
	m_pCurState->Update(dt);
}

bool Unit::FindPath( const Ogre::Vector3& destPos )
{
	int ret = m_pRecast->FindPath(m_pNode->_getDerivedPosition(), destPos, 1, 1);
	
	if(ret >= 0)
	{
		m_pDetour->setMoveTarget(m_agentID, destPos, false);
		//绘制路径线
		m_pRecast->CreateRecastPathLine(1);
	}

	return ret >= 0;
}

void Unit::MoveTo( const Ogre::Vector3& destPos )
{
	//sinbad.mesh的建模Z轴是反的
	m_pNode->lookAt(destPos, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
	m_pNode->_setDerivedPosition(destPos);
}
