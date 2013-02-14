#include "stdafx.h"
#include "AIComponent.h"
#include "SelectableObject.h"
#include "World.h"
#include "Unit.h"
#include "PathComponent.h"


AiComponent::AiComponent(SelectableObject* pOwner)
:m_pOwner(pOwner)
,m_bExecuting(false)
,m_curState(eObjectState_Idle)
{
	m_states.push_back(new StateIdle);
	m_states.push_back(new StateProduce);
	m_states.push_back(new StateMove);
	m_states.push_back(new StateTargeting);
}

AiComponent::~AiComponent()
{
	for(size_t i=0; i<m_states.size(); ++i)
		delete m_states[i];
	m_states.clear();
}

void AiComponent::GiveCommand( Command& cmd, bool bForceExecute )
{
	if (bForceExecute && !m_cmdQueue.empty())
	{
		CancelCurCommand();
		cmd.Excute();
		m_cmdQueue.push_front(cmd);
	}
	else if(m_cmdQueue.size() < MAX_COMMAND_QUEUE_LEN)
	{
		//TODO: 只有MAX_COMMAND_QUEUE_LEN=1时这里逻辑才对
		cmd.Excute();
		m_cmdQueue.push_back(cmd);
	}
}

void AiComponent::CancelCurCommand()
{
	if(!m_cmdQueue.empty())
		m_pOwner->_OnCommandFinished(GetCurCommand());
}

void AiComponent::SetCurState(eObjectState state)
{
 	//退出当前状态
 	m_states[m_curState]->Exit(m_pOwner);
	//进入新状态
 	m_states[state]->Enter(m_pOwner);
	m_curState = state;
}

void AiComponent::Update( float dt )
{
	m_states[m_curState]->Update(dt, m_pOwner);
}

void AiComponent::_OnCommandFinished()
{
	//将该命令从命令队列中移除
	if(!m_cmdQueue.empty())
		m_cmdQueue.pop_front();
}

void AiComponent::GiveCommand( const OIS::MouseEvent& arg )
{
	World& world = World::GetSingleton();

	float screenX = arg.state.X.abs / (float)arg.state.width;
	float screenY = arg.state.Y.abs / (float)arg.state.height;
	
	POS intersectPos;
	if(!world.GetTerrainIntersectPos(FLOAT2(screenX, screenY), intersectPos))
		return;

// 	Ogre::MovableObject* pMovable = world.GetRaySceneQueryResult(ray, eQueryType_SelectableObject);
// 	SelectableObject* pHitObj = nullptr;
// 	if(pMovable)
// 	{
// 		pHitObj = Ogre::any_cast<SelectableObject*>(pMovable->getUserAny());
// 		assert(pHitObj);
// 	}

	if(m_pOwner->GetType() == eObjectType_Unit)
	{
		//进行寻路尝试
		Unit* pUnit = dynamic_cast<Unit*>(m_pOwner);
		World::GetSingleton().ClampPosToNavMesh(intersectPos);

		if(pUnit->GetPathComponent()->FindPath(intersectPos, true))
		{
			pUnit->SetDestPos(intersectPos);
			//执行寻路命令
			pUnit->GetAiComponent()->GiveCommand(Command(eCommandType_Move, pUnit), true);
		}
	}
}

eCommandType AiComponent::GetCurCommand() const
{
	return m_cmdQueue.front().GetType();
}
