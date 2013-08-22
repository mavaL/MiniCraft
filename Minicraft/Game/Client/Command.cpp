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
	case eCommandType_Produce:	m_pOwner->GetAi()->SetCurState(eObjectState_Produce); break;
	case eCommandType_Move: 	m_pOwner->GetAi()->SetCurState(eObjectState_Move); break;
	case eCommandType_Stop:		m_pOwner->GetAi()->SetCurState(eObjectState_Stop); break;
	case eCommandType_Gather:	m_pOwner->GetAi()->SetCurState(eObjectState_Gather); break;
	case eCommandType_Attack:	m_pOwner->GetAi()->SetCurState(eObjectState_Attack); break;
	default: assert(0);
	}
}

