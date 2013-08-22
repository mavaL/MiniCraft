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
#include "AnimatedComponent.h"
#include "BehaviorComponent.h"
#include "InfoPanel.h"

void StateIdle::Enter( SelectableObject* pOwner )
{
	//�������ж���
	AnimatedComponent* anim = pOwner->GetAnim();
	if(anim)
		anim->PlayAnimation(eAnimation_Idle, true);
}

void StateIdle::Exit( SelectableObject* pOwner )
{
	//ֹͣ�������ж���
	AnimatedComponent* anim = pOwner->GetAnim();
	if(anim)
		anim->StopAnimation();
}

///////////////////////////////////////////////////////////////
void StateMove::Enter( SelectableObject* pOwner )
{
	PathComponent* path = pOwner->GetPath();
	bool bSucceed = path->FindPath(path->GetDestPos(), false);
	assert(bSucceed);

	//�����ƶ�����
	pOwner->GetAnim()->PlayAnimation(eAnimation_Move, true);
}

void StateMove::Update( float dt, SelectableObject* pOwner )
{
	if(pOwner->GetPath()->UpdatePathFinding(dt))
	{
		//�ѵ���Ŀ�ĵ�,�������״̬
		pOwner->GetAi()->SetCurState(eObjectState_Idle);
	}
}

void StateMove::Exit( SelectableObject* pOwner )
{
	//ֹͣ�����ƶ�����
	pOwner->GetAnim()->StopAnimation();
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
	float timeCost = Ogre::StringConverter::parseReal(unitData->params["timecost"]);

	if (fProgress > timeCost)
	{
		//�������
		pOwner->GetAi()->SetCurState(eObjectState_Idle);

		//�ʻ�ĵ�λ��¯��
		Unit* pUnit = static_cast<Unit*>(ObjectManager::GetSingleton().CreateObject(eObjectType_Unit));
		pUnit->setParameter("name", unitName);
		pUnit->Init();
		pUnit->SetPosition(pObj->GetRallyPoint());
		pUnit->AddComponent(eComponentType_Path, new PathComponent(pUnit));

		//�µ�λ�������״̬
		pUnit->GetAi()->SetCurState(eObjectState_Idle);
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
	Kratos::CInputManager& inputMgr = INPUTMANAGER;
	inputMgr.BindMouseRelease(boost::bind(&StateTargeting::OnInputSys_MouseReleased, this, _1, _2), Kratos::eInputEventPriority_Targeting);
	inputMgr.BindMouseMove(boost::bind(&StateTargeting::OnInputSys_MouseMove, this, _1), Kratos::eInputEventPriority_Targeting);
	inputMgr.BlockMousePressed(Kratos::eInputEventPriority_default, true);
	
	GUIMANAGER.SetCursorMode(Kratos::eCursorMode_TargetNone);
}

void StateTargeting::Exit( SelectableObject* pOwner )
{
	Kratos::CInputManager& inputMgr = INPUTMANAGER;
	inputMgr.UnbindMouseRelease(Kratos::eInputEventPriority_Targeting);
	inputMgr.UnbindMouseMove(Kratos::eInputEventPriority_Targeting);
	inputMgr.BlockMousePressed(Kratos::eInputEventPriority_default, false);

	GUIMANAGER.SetCursorMode(Kratos::eCursorMode_Normal);
}

bool StateTargeting::OnInputSys_MouseReleased( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	SelectableObject* pObj = World::GetSingleton().GetSelectedObjects().at(0);

	if (id == OIS::MB_Left)
	{
		pObj->GetAi()->GiveCommand(arg, id);
		//�˳�����״̬
		pObj->GetAi()->ClearParallelState();
	}
	else if (id == OIS::MB_Right)
	{
		//�˳�����״̬
		pObj->GetAi()->ClearParallelState();
	}

	return true;
}

bool StateTargeting::OnInputSys_MouseMove( const OIS::MouseEvent &arg )
{
	SelectableObject* pObj = World::GetSingleton().GetSelectedObjects().at(0);
	eCommandType cmd = pObj->GetActiveAbility()->m_type;

	//��ȡ��ǰ���hover�µ�����
	Ogre::MovableObject* pMovable = World::GetSingleton().GetRaySceneQueryResult(arg, QueryTypeSelectableObject);
	SelectableObject* pHoverObj = nullptr;
	if(pMovable)
	{
		pHoverObj = Ogre::any_cast<SelectableObject*>(pMovable->getUserAny());
		assert(pHoverObj);
	}

	//���ݵ�ǰ��������ȷ�������ʽ
	Kratos::eCursorMode mode;
	switch (cmd)
	{
	case eCommandType_Move:
		{
			//�޷��ƶ�..
			if(pHoverObj)
				mode = Kratos::eCursorMode_TargetInvalid;
			else
				mode = Kratos::eCursorMode_TargetNone;
		}
		break;

	case eCommandType_Gather:
		{
			if(pHoverObj && pHoverObj->GetType() == eObjectType_Resource)
				mode = Kratos::eCursorMode_TargetAllied;
			else
				mode = Kratos::eCursorMode_TargetInvalid;
		}
		break;

	case eCommandType_Attack:
		{
			if (pHoverObj)
			{
				if(pHoverObj->GetType() == eObjectType_Resource)
					mode = Kratos::eCursorMode_TargetInvalid;
				else if(pObj->GetAi()->IsAlly(pHoverObj))
					mode = Kratos::eCursorMode_TargetAllied;
				else
					mode = Kratos::eCursorMode_TargetEnemy;
			}
			else
			{
				mode = Kratos::eCursorMode_TargetNone;
			}
		}
		break;

	default: assert(0);
	}

	GUIMANAGER.SetCursorMode(mode);

	//������handler������Ӧ
	return false;
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
		//�������״̬
		pUnit->GetAi()->SetCurState(eObjectState_Idle);
	}
}

///////////////////////////////////////////////////////////////
void StateGather::Enter( SelectableObject* pOwner )
{
	pOwner->GetGather()->SetCurStage(HarvestComponent::eHarvestStage_ToRes);
	//��ֹ��λ���赲
	pOwner->GetPath()->EnableObstcleAvoidance(false);
}

void StateGather::Update( float dt, SelectableObject* pOwner )
{
	pOwner->GetGather()->Update(dt);
}

void StateGather::Exit( SelectableObject* pOwner )
{
	pOwner->GetGather()->SetCurStage(HarvestComponent::eHarvestStage_None);
	//�ָ���λ���赲
	pOwner->GetPath()->EnableObstcleAvoidance(true);
}

void StateDeath::Enter( SelectableObject* pOwner )
{
	pOwner->GetAnim()->PlayAnimation(eAnimation_Death, false);
	//ʹ�øõ�λ�����ٱ�ѡ��
	pOwner->GetEntity()->setQueryFlags(0);
	pOwner->GetAi()->SetCpuControl(false);
}

void StateDeath::Update( float dt, SelectableObject* pOwner )
{
	if(pOwner->GetAnim()->IsAnimationOver())
		Exit(pOwner);
}

void StateDeath::Exit( SelectableObject* pOwner )
{
	ObjectManager::GetSingleton().DestroyObject(pOwner);
}