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
	default: assert(0);
	}
}

