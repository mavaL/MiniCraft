#include "stdafx.h"
#include "Building.h"
#include "AIComponent.h"
#include "GUIManager.h"


Building::Building()
:SelectableObject()
,m_buildingName("")
,m_fCurProgress(0)
{
	
}

void Building::Update( float dt )
{
	if(GetAiComponent()->GetCurState() == eObjectState_Produce)
		m_fCurProgress += dt;

	__super::Update(dt);
}

void Building::_OnCommandFinished(eCommandType cmd)
{
	__super::_OnCommandFinished(cmd);
	if(cmd == eCommandType_Produce)
		m_fCurProgress = 0;
}

