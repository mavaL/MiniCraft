#include "stdafx.h"
#include "InfoPanel.h"
#include "GUIManager.h"

UiInfoPanel::UiInfoPanel()
:m_pLayout(nullptr)
{

}

void UiInfoPanel::Init()
{
	m_pLayout = CGUIManager::GetSingleton().LoadWindowLayout("InfoPanel.layout");
	assert(m_pLayout);
}

void UiInfoPanel::Destroy()
{
	CGUIManager::GetSingleton().UnloadWindowLayout(m_pLayout);
	m_pLayout = nullptr;
}

void UiInfoPanel::SetInfoIcon( const std::string& imgName )
{
	STRING prop("set:");
	prop += imgName;
	prop += " image:full_image";
	m_pLayout->getChild("InfoIcon")->setProperty("Image", prop);
}
