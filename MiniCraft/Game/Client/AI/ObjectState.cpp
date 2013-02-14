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

void StateIdle::Enter( SelectableObject* pOwner )
{
	if(pOwner->GetType() == eObjectType_Unit)
	{
		Unit* pUnit = dynamic_cast<Unit*>(pOwner);
		//播放休闲动画
		pUnit->PlayAnimation(eAnimation_Idle, true);
	}
}

void StateIdle::Update( float dt, SelectableObject* pOwner )
{

}

void StateIdle::Exit( SelectableObject* pOwner )
{
	if(pOwner->GetType() == eObjectType_Unit)
	{
		Unit* pUnit = dynamic_cast<Unit*>(pOwner);
		//停止播放休闲动画
		pUnit->StopAnimation();
	}
}

///////////////////////////////////////////////////////////////
void StateMove::Enter( SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Unit);
	Unit* pUnit = dynamic_cast<Unit*>(pOwner);
	pUnit->GetPathComponent()->FindPath(pUnit->GetDestPos(), false);
	//播放移动动画
	pUnit->PlayAnimation(eAnimation_Move, true);
}

void StateMove::Update( float dt, SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Unit);

	Unit* pUnit = dynamic_cast<Unit*>(pOwner);
	POS curPos = pUnit->GetPathComponent()->GetAgentPos();

	World::GetSingleton().ClampToTerrain(curPos);
	pOwner->GetSceneNode()->lookAt(curPos, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
	pUnit->SetPosition(curPos);

	if (curPos.positionEquals(pUnit->GetDestPos(), 0.1f))
	{
		//已到达目的地,进入空闲状态
		pOwner->GetAiComponent()->SetCurState(eObjectState_Idle);
	}
}

void StateMove::Exit( SelectableObject* pOwner )
{
	assert(pOwner->GetType() == eObjectType_Unit);
	Unit* pUnit = dynamic_cast<Unit*>(pOwner);
	//停止播放移动动画
	pUnit->StopAnimation();

	pOwner->_OnCommandFinished(eCommandType_Move);
}

///////////////////////////////////////////////////////////////
void StateProduce::Enter(SelectableObject* pOwner)
{
	UiInfoPanel* panel = CGUIManager::GetSingleton().GetInfoPanel();
	panel->ShowInfoIcon(true, pOwner->GetActiveAbility()->m_iconName, "full_image");
	panel->ShowProgressQueue(true);
}

void StateProduce::Update( float dt, SelectableObject* pOwner )
{
	Building* pObj = dynamic_cast<Building*>(pOwner);
	float fProgress = pObj->GetCurProgress();
	const STRING& unitName = pOwner->GetActiveAbility()->m_param;
	SUnitData* unitData = &GameDataDefManager::GetSingleton().m_unitData[unitName];

	if (fProgress > unitData->m_fTimeCost)
	{
		//生产完毕
		pOwner->GetAiComponent()->SetCurState(eObjectState_Idle);

		//鲜活的单位出炉了
		Object* pObj = ObjectManager::GetSingleton().CreateObject(eObjectType_Unit);
		pObj->setParameter("unitName", unitName);
		pObj->setParameter("position", pOwner->getParameter("rallypoint"));

		//新单位进入空闲状态
		Unit* pUnit = dynamic_cast<Unit*>(pObj);
		pUnit->SetAiComponent(new AiComponent(pUnit));
		pUnit->SetPathComponent(new PathComponent(pUnit));
		pUnit->GetAiComponent()->SetCurState(eObjectState_Idle);
	}
}

void StateProduce::Exit(SelectableObject* pOwner)
{
	pOwner->_OnCommandFinished(eCommandType_Produce);
	UiInfoPanel* panel = CGUIManager::GetSingleton().GetInfoPanel();
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
		Unit* pUnit = dynamic_cast<Unit*>(pObj);

		FLOAT2 screenPos;
		screenPos.x = arg.state.X.abs / (float)arg.state.width;
		screenPos.y = arg.state.Y.abs / (float)arg.state.height;

		POS intersectPos;
		if(World::GetSingleton().GetTerrainIntersectPos(screenPos, intersectPos))
		{
			if (pUnit->GetPathComponent()->FindPath(intersectPos, true))
			{
				//转入移动状态
				pUnit->SetDestPos(intersectPos);
				pUnit->GetAiComponent()->SetCurState(eObjectState_Move);
			}
		}
	}

	return true;
}
