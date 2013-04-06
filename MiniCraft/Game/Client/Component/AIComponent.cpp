#include "stdafx.h"
#include "AIComponent.h"
#include "SelectableObject.h"
#include "World.h"
#include "Unit.h"
#include "PathComponent.h"
#include "HarvestComponent.h"
#include "Resource.h"
#include "OgreManager.h"
#include "Faction.h"
#include "GameDataDef.h"

AiComponent::AiComponent(SelectableObject* pOwner)
:Component(pOwner)
,m_bExecuting(false)
,m_curState(eObjectState_Idle)
,m_parallelState(nullptr)
,m_player(nullptr)
,m_attkTarget(nullptr)
,m_bCpuControl(false)
{
	m_states.push_back(new StateIdle);
	m_states.push_back(new StateProduce);
	m_states.push_back(new StateMove);
	m_states.push_back(new StateTargeting);
	m_states.push_back(new StateStop);
	m_states.push_back(new StateGather);
	m_states.push_back(new StateAttack);
}

AiComponent::~AiComponent()
{
	std::for_each(m_states.begin(), m_states.end(), std::default_delete<ObjectState>());
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
	if(m_bCpuControl)
		return;

	m_states[m_curState]->Update(dt, m_pOwner);
	//更新并行状态
	if(m_parallelState) 
		m_parallelState->Update(dt, m_pOwner);
}

void AiComponent::_OnCommandFinished()
{
	//将该命令从命令队列中移除
	if(!m_cmdQueue.empty())
		m_cmdQueue.pop_front();
}

void AiComponent::GiveCommand( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	World& world = World::GetSingleton();

	//是否选中了其他物体
	POS intersectPos;
	Ogre::MovableObject* pMovable = world.GetRaySceneQueryResult(arg, eQueryType_SelectableObject, &intersectPos);
	SelectableObject* pHitObj = nullptr;
	if(pMovable)
	{
		pHitObj = Ogre::any_cast<SelectableObject*>(pMovable->getUserAny());
		assert(pHitObj);
	}

	const SAbilityData* pData = m_pOwner->GetActiveAbility();
	if (pHitObj)	//选中了物体
	{
		if(pHitObj->GetType() == eObjectType_Resource)	//选中了资源
		{
			//无效指令..
			//TODO: 应该弹出UI提示,并回到选择目标状态
			if(id == OIS::MB_Left && pData->m_type != eCommandType_Gather)
				return;

			//该单位没有采集技能
			if(id == OIS::MB_Right && !m_pOwner->HasAbility(eCommandType_Gather))
				return;
	
			//设置采集目标
			m_pOwner->GetGather()->SetTarget(static_cast<Resource*>(pHitObj));
			//执行采集命令
			m_pOwner->GetAi()->GiveCommand(Command(eCommandType_Gather, m_pOwner), true);
		}
		else	
		{
			if(id == OIS::MB_Left && pData->m_type == eCommandType_Attack)
			{
				assert(m_pOwner->HasAbility(eCommandType_Attack));
				m_pOwner->GetAi()->SetAttackTarget(pHitObj);
				//执行攻击命令
				m_pOwner->GetAi()->GiveCommand(Command(eCommandType_Attack, m_pOwner), true);
			}
		}
	}
	else	//点击地面
	{
		//没选中物体,并且不是移动,视为无效指令
		//TODO: 应该弹出UI提示,并回到选择目标状态
		if(id == OIS::MB_Left && pData->m_type != eCommandType_Move)
			return;

		//进行寻路尝试
		if(m_pOwner->GetPath()->FindPath(intersectPos, true))
		{
			m_pOwner->GetPath()->SetDestPos(intersectPos);
			//执行寻路命令
			m_pOwner->GetAi()->GiveCommand(Command(eCommandType_Move, m_pOwner), true);
		}
	}
}

eCommandType AiComponent::GetCurCommand() const
{
	return m_cmdQueue.front().GetType();
}

void AiComponent::CancelAllCommand()
{
	CancelCurCommand();
	m_cmdQueue.clear();
}

void AiComponent::SetParallelState( eObjectState state )
{
	assert(!m_parallelState);

	m_states[state]->Enter(m_pOwner);
	m_parallelState = m_states[state];
}

void AiComponent::ClearParallelState()
{
	if (m_parallelState)
	{
		m_parallelState->Exit(m_pOwner);
		m_parallelState = nullptr;
	}
}

bool AiComponent::IsAlly( SelectableObject* obj )
{
	return m_player == obj->GetAi()->GetFaction();
}

void AiComponent::SetFaction( Faction* player )
{
	m_pOwner->InitTeamColor(player->GetTeamColor());
	m_player = player;
}
