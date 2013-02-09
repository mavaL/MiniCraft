#include "stdafx.h"
#include "Command.h"
#include "GameDataDef.h"
#include "GUIManager.h"

CommandBase::CommandBase(Object* pOwner, const SAbilityData* param)
:m_pOwner(pOwner)
,m_pParam(param)
{

}

//////////////////////////////////////////////////////////////////////
CmdOperation::CmdOperation(Object* pOwner, const SAbilityData* param)
:CommandBase(pOwner, param)
{	
}

//////////////////////////////////////////////////////////////////////
CmdProduce::CmdProduce(Object* pOwner, const SAbilityData* param)
:CommandBase(pOwner, param)
{
}

void CmdProduce::Excute() const
{
	CGUIManager::GetSingleton().GetInfoPanel()->SetInfoIcon(m_pParam->m_iconName);
}
