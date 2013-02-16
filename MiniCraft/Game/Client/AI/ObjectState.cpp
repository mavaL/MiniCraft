#include "stdafx.h"
#include "ObjectState.h"
#include "GUIManager.h"
#include "SelectableObject.h"
#include "AIComponent.h"
#include "PathComponent.h"
#include "Building.h"
#include "ObjectManager.h"
#include "Unit.h"
#include "World.h"
#include "HarvestComponent.h"
#include "InfoPanel.h"

void StateIdle::Enter( SelectableObject* pOwner )
{
	if(pOwner->GetType() == eObjectType_Unit)
	{
		Unit* pUnit = static_cast<Unit*>(pOwner);
		//播放休闲动画
		pUnit->PlayAnimation(eAnimation_Idle, true);
	}
}

void StateIdle::Exit( SelectableObject* pOwner )
{
	if(pOwner->GetType() == eObjectType_Unit)
	{
		Unit* pUnit = static_cast<Unit*>(pOwner);
		//停止播放休闲动画
		pUnit->StopAnimation();
	}
}

///////////////////////////////////////////////////////////////
void StateMove::Enter( SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Unit);

	PathComponent* path = pOwner->GetPath();
	bool bSucceed = path->FindPath(path->GetDestPos(), false);
	assert(bSucceed);

	//播放移动动画
	Unit* pUnit = static_cast<Unit*>(pOwner);
	pUnit->PlayAnimation(eAnimation_Move, true);
}

void StateMove::Update( float dt, SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Unit);

	if(pOwner->GetPath()->_UpdatePathFinding(dt))
	{
		//已到达目的地,进入空闲状态
		pOwner->GetAi()->SetCurState(eObjectState_Idle);
	}
}

void StateMove::Exit( SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Unit);
	Unit* pUnit = static_cast<Unit*>(pOwner);
	//停止播放移动动画
	pUnit->StopAnimation();

	pOwner->_OnCommandFinished(eCommandType_Move);
}

///////////////////////////////////////////////////////////////
void StateProduce::Enter(SelectableObject* pOwner)
{
	UiInfoPanel* panel = World::GetSingleton().GetInfoPanel();
	panel->ShowInfoIcon(true, pOwner->GetActiveAbility()->m_iconName, "full_image");
	panel->ShowProgressQueue(true);
}

void StateProduce::Update( float dt, SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Building);

	Building* pObj = static_cast<Building*>(pOwner);
	float fProgress = pObj->GetCurProgress();
	const STRING& unitName = pOwner->GetActiveAbility()->m_param;
	SUnitData* unitData = &GameDataDefManager::GetSingleton().m_unitData[unitName];

	if (fProgress > unitData->m_fTimeCost)
	{
		//生产完毕
		pOwner->GetAi()->SetCurState(eObjectState_Idle);

		//鲜活的单位出炉了
		SelectableObject* pNewObj = static_cast<SelectableObject*>(ObjectManager::GetSingleton().CreateObject(eObjectType_Unit));
		pNewObj->setParameter("unitName", unitName);
		pNewObj->setParameter("position", pOwner->getParameter("rallypoint"));

		//新单位进入空闲状态
		pNewObj->AddComponent(eComponentType_AI, new AiComponent(pNewObj));
		pNewObj->AddComponent(eComponentType_Path, new PathComponent(pNewObj));
		pNewObj->GetAi()->SetCurState(eObjectState_Idle);
	}
}

void StateProduce::Exit(SelectableObject* pOwner)
{
	pOwner->_OnCommandFinished(eCommandType_Produce);
	UiInfoPanel* panel = World::GetSingleton().GetInfoPanel();
	panel->SetVisible(false);
}

///////////////////////////////////////////////////////////////
void StateTargeting::Enter( SelectableObject* pOwner )
{
	CInputManager& inputMgr = CInputManager::GetSingleton();
	inputMgr.BindMouseRelease(boost::bind(&StateTargeting::OnInputSys_MouseReleased, this, _1, _2), eInputEventPriority_Targeting);
	inputMgr.BlockMousePressed(eInputEventPriority_default, true);
}

void StateTargeting::Exit( SelectableObject* pOwner )
{
	CInputManager& inputMgr = CInputManager::GetSingleton();
	inputMgr.UnbindMouseRelease(eInputEventPriority_Targeting);
	inputMgr.BlockMousePressed(eInputEventPriority_default, false);
}

bool StateTargeting::OnInputSys_MouseReleased( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	if (id == OIS::MB_Left)
	{
		SelectableObject* pObj = World::GetSingleton().GetSelectedObjects().at(0);
		pObj->GetAi()->GiveCommand(arg, id);
		//退出并行状态
		pObj->GetAi()->ClearParallelState();
	}

	return true;
}

///////////////////////////////////////////////////////////////
void StateStop::Enter( SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Unit);

	Unit* pUnit = static_cast<Unit*>(pOwner);
	pUnit->StopAction();
}

void StateStop::Update( float dt, SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Unit);

	Unit* pUnit = static_cast<Unit*>(pOwner);
	float fPastTime = pUnit->GetStopTime() + dt;
	pUnit->SetStopTime(fPastTime);

	const float STOP_TIME = 0.2f;
	if (fPastTime > STOP_TIME)
	{
		//进入空闲状态
		pUnit->GetAi()->SetCurState(eObjectState_Idle);
	}
}

///////////////////////////////////////////////////////////////
void StateGather::Enter( SelectableObject* pOwner )
{
	HarvestComponent* pCo = QueryComponent(pOwner, eComponentType_Harvest, HarvestComponent);
	pCo->SetCurStage(eHarvestStage_ToRes);
	//禁止单位间阻挡
	pOwner->GetPath()->EnableObstcleAvoidance(false);
}

void StateGather::Update( float dt, SelectableObject* pOwner )
{
	HarvestComponent* pCo = QueryComponent(pOwner, eComponentType_Harvest, HarvestComponent);
	pCo->Update(dt);
}

void StateGather::Exit( SelectableObject* pOwner )
{
	HarvestComponent* pCo = QueryComponent(pOwner, eComponentType_Harvest, HarvestComponent);
	pCo->SetCurStage(eHarvestStage_None);
	//恢复单位间阻挡
	pOwner->GetPath()->EnableObstcleAvoidance(true);
}
