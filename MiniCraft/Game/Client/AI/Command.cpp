#include "stdafx.h"
#include "Command.h"
#include "SelectableObject.h"
#include "AIComponent.h"


Command::Command(eCommandType type, SelectableObject* pOwner)
:m_type(type)
,m_pOwner(pOwner)
{

}

void Command::Excute()
{
	switch (m_type)
	{
	case eCommandType_Produce: m_pOwner->GetAiComponent()->SetCurState(eObjectState_Produce); break;

	case eCommandType_Move: 
		{
			bool bNeedMove = Ogre::StringConverter::parseBool(m_pOwner->getParameter("needmove"));
			if(bNeedMove)
				m_pOwner->GetAiComponent()->SetCurState(eObjectState_Move);
			else
				m_pOwner->GetAiComponent()->SetCurState(eObjectState_Targeting);
		}
		break;
	default: assert(0);
	}
}

