#include "stdafx.h"
#include "CommandPanel.h"
#include "GUIManager.h"

using namespace CEGUI;

UiCommandPanel::UiCommandPanel()
:m_pLayout(nullptr)
{

}

void UiCommandPanel::Init()
{
	m_pLayout = CGUIManager::GetSingleton().LoadWindowLayout("CommandPanel.layout");
	m_pLayout->setMousePassThroughEnabled(true);
	CGUIManager::GetSingleton().SetGUISheet(m_pLayout);
}

void UiCommandPanel::Destroy()
{
	CGUIManager::GetSingleton().SetGUISheet(nullptr);
	CGUIManager::GetSingleton().UnloadWindowLayout(m_pLayout);
	m_pLayout = nullptr;
}

void UiCommandPanel::CEGUI_SetAbilitySlot( int slotIndex, eSlotState state, const std::string& imgName )
{
	STRING wndNamePrefix("CommandSlot");
	wndNamePrefix += Ogre::StringConverter::toString(slotIndex);
	Window* wndFrame = m_pLayout->getChild(wndNamePrefix + "Frame");
	Window* wndIcon = wndFrame->getChild(wndNamePrefix + "Icon");
	assert(wndFrame && wndIcon);

	switch (state)
	{
	case eSlotState_Empty: 
		{ 
			wndFrame->setProperty("NormalImage", "set:Icon_CommandEmpty image:CommandEmpty");
			wndFrame->setProperty("Alpha", "0.2");
			wndIcon->setProperty("Image", ""); 
		}
		break;
	case eSlotState_Normal: 
		{
			STRING prop("set:");
			prop += imgName;
			prop += " image:full_image";
			wndFrame->setProperty("NormalImage", "set:Icon_TerranFrameNormal image:TerranFrameNormal");
			wndFrame->setProperty("Alpha", "1.0");
			wndIcon->setProperty("Image", prop);
		}
		break;
	default: assert(0);
	}
}
