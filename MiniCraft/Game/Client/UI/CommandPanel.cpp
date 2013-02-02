#include "stdafx.h"
#include "CommandPanel.h"
#include "GUIManager.h"


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
