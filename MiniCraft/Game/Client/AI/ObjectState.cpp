#include "stdafx.h"
#include "ObjectState.h"
#include "GUIManager.h"
#include "SelectableObject.h"
#include "AIComponent.h"
#include "Building.h"

void StateProduce::Enter(SelectableObject* pOwner)
{
	UiInfoPanel* panel = CGUIManager::GetSingleton().GetInfoPanel();
	panel->ShowInfoIcon(true, pOwner->GetActiveAbility()->m_iconName);
	panel->ShowProgressQueue(true);
}

void StateProduce::Update( float dt, SelectableObject* pOwner )
{
	Building* pObj = dynamic_cast<Building*>(pOwner);
	float fProgress = pObj->GetCurProgress();
	float fTotalTime = pOwner->GetActiveAbility()->m_fTimeCost;

	if (fProgress > fTotalTime)
	{
		//Éú²úÍê±Ï
		pOwner->GetAiComponent()->SetCurState(eObjectState_Idle);
	}

}

void StateProduce::Exit(SelectableObject* pOwner)
{
	pOwner->_OnCommandFinished(eCommandType_Produce);
	UiInfoPanel* panel = CGUIManager::GetSingleton().GetInfoPanel();
	panel->SetVisible(false);
}
