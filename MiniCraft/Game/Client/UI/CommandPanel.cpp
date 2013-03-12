#include "stdafx.h"
#include "CommandPanel.h"
#include "GUIManager.h"
#include "GameDataDef.h"
#include "SelectableObject.h"
#include "Command.h"
#include "AIComponent.h"

using namespace CEGUI;

UiCommandPanel::UiCommandPanel()
:m_pLayout(nullptr)
,m_pActiveObj(nullptr)
{

}

void UiCommandPanel::Init()
{
	m_pLayout = GUIMANAGER.LoadWindowLayout("CommandPanel.layout");
	assert(m_pLayout);

	for (int i=0; i<MAX_ABILITY_SLOT; ++i)
		CEGUI_SetAbilitySlot(i, false);

	//¹Ò½ÓUIÏìÓ¦
	m_pLayout->getChild(0)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<0>, this));
	m_pLayout->getChild(1)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<1>, this));
	m_pLayout->getChild(2)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<2>, this));
	m_pLayout->getChild(3)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<3>, this));
	m_pLayout->getChild(4)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<4>, this));
	m_pLayout->getChild(5)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<5>, this));
	m_pLayout->getChild(6)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<6>, this));
	m_pLayout->getChild(7)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<7>, this));
	m_pLayout->getChild(8)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<8>, this));
	m_pLayout->getChild(9)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<9>, this));
	m_pLayout->getChild(10)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<10>, this));
	m_pLayout->getChild(11)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<11>, this));
	m_pLayout->getChild(12)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<12>, this));
	m_pLayout->getChild(13)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<13>, this));
	m_pLayout->getChild(14)->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&UiCommandPanel::CEGUI_OnCommandBtnClicked<14>, this));
}

void UiCommandPanel::Destroy()
{
	GUIMANAGER.SetGUISheet(nullptr);
	GUIMANAGER.UnloadWindowLayout(m_pLayout);
	m_pLayout = nullptr;
}

void UiCommandPanel::CEGUI_SetAbilitySlot( int slotIndex, bool bEnable, const std::string& imgSetName, const std::string& imgName )
{
	Window* wndFrame = m_pLayout->getChild(slotIndex);
	Window* wndIcon = wndFrame->getChild(0);
	wndFrame->setEnabled(bEnable);
	if (bEnable)
	{
		STRING prop("set:");
		prop += imgSetName;
		prop += " image:";
		prop += imgName;
		wndIcon->setProperty("Image", prop);
		wndFrame->setProperty("Alpha", "1.0");
	}
	else
	{
		wndFrame->setProperty("Alpha", "0.2");
		wndIcon->setProperty("Image", "");
	}
}

template<int Slot>
bool UiCommandPanel::CEGUI_OnCommandBtnClicked( const CEGUI::EventArgs& e )
{
	assert(m_pActiveObj);
	m_pActiveObj->SetActiveAbility(Slot);
	const SAbilityData* pData = m_pActiveObj->GetActiveAbility();
	
	if(pData->m_param == TARGETING_AND_FORCE_EXECUTE)
	{
		m_pActiveObj->GetAi()->SetParallelState(eObjectState_Targeting);
	}
	else
	{
		m_pActiveObj->GetAi()->GiveCommand(Command(pData->m_type, m_pActiveObj), pData->m_param == FORCE_EXECUTE);
	}

	return true;
}
