#include "stdafx.h"
#include "UnitState.h"
#include "Unit.h"
#include "Command.h"



void IdleState::Enter()
{
	//播放休闲动画
	m_pIdleAnim = m_pOwner->GetEntity()->getAnimationState("IdleTop");
	assert(m_pIdleAnim);
	m_pIdleAnim->setEnabled(true);
	m_pIdleAnim->setTimePosition(0);
	m_pIdleAnim->setLoop(true);
}

void IdleState::Exit()
{
	m_pIdleAnim->setEnabled(false);
}

void IdleState::Update(float dt)
{
	m_pIdleAnim->addTime(dt);
}

MoveState::MoveState( Unit* pOwner )
:UnitState(pOwner)
,m_destPos(-1,-1,-1)
,m_pRunBaseAnim(nullptr)
,m_pRunTopAnim(nullptr)
{
	m_type = eUnitState_Move;
}

void MoveState::Enter()
{
	/// TODO:单位状态应该不依赖Command,因为状态变化不一定是由Command引起的
	MoveCommand* pCmd = dynamic_cast<MoveCommand*>(m_pOwner->GetCurCommand());
	assert(pCmd);
	m_destPos = pCmd->GetDestPos();

	if(!m_pOwner->FindPath(m_destPos))
	{
		//寻路失败,转入空闲状态
		m_pOwner->SetState(IdleState(m_pOwner));
		return;
	}

	//打开跑动画
	m_pRunBaseAnim = m_pOwner->GetEntity()->getAnimationState("RunBase");
	m_pRunTopAnim = m_pOwner->GetEntity()->getAnimationState("RunTop");
	assert(m_pRunBaseAnim && m_pRunTopAnim);

	m_pRunBaseAnim->setEnabled(true);
	m_pRunBaseAnim->setTimePosition(0);
	m_pRunBaseAnim->setLoop(true);

	m_pRunTopAnim->setEnabled(true);
	m_pRunTopAnim->setTimePosition(0);
	m_pRunTopAnim->setLoop(true);
}

void MoveState::Update( float dt )
{
	//更新动画
	m_pRunBaseAnim->addTime(dt);
	m_pRunTopAnim->addTime(dt);

	if(m_pOwner->GetAgent()->active) 
	{
		Ogre::Vector3 agentPos;
		OgreRecast::FloatAToOgreVect3(m_pOwner->GetAgent()->npos, agentPos);

		agentPos.y = m_pOwner->GetSceneNode()->_getDerivedPosition().y;
		//更新寻路
		m_pOwner->MoveTo(agentPos);

		//是否已到达目的点
		if (agentPos.positionEquals(m_destPos, 1))
			m_pOwner->SetState(IdleState(m_pOwner));
	}
}

void MoveState::Exit()
{
	if(m_pRunBaseAnim)
		m_pRunBaseAnim->setEnabled(false);
	if(m_pRunTopAnim)
		m_pRunTopAnim->setEnabled(false);
}

