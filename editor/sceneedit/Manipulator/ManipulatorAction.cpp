#include "stdafx.h"
#include "ManipulatorAction.h"
#include "Action/ActionBase.h"
#include "Action/ActionNone.h"
#include "Action/ActionTerrainDeform.h"
#include "Action/ActionTerrainSplat.h"


ManipulatorAction::ManipulatorAction()
{
	m_mapAction.insert(std::make_pair(eActionType_None, new ActionNone));
	m_mapAction.insert(std::make_pair(eActionType_TerrainDeform, new ActionTerrainDeform));
	m_mapAction.insert(std::make_pair(eActionType_TerrainSplat, new ActionTerrainSplat));

	//初始化编辑器状态
	m_pActiveAction = m_mapAction[eActionType_None];
}

ManipulatorAction::~ManipulatorAction()
{
	for(auto iter=m_mapAction.begin(); iter!=m_mapAction.end(); ++iter)
		delete iter->second;
	m_mapAction.clear();
}

void ManipulatorAction::SetActiveAction( eActionType type )
{
	//反激活当前状态
	if (m_pActiveAction)
	{
		m_pActiveAction->Leave();
		m_pActiveAction = nullptr;
	}

	auto iter = m_mapAction.find(type);
	assert(iter != m_mapAction.end() && "Invalid ActionType!");

	(iter->second)->Enter();
	m_pActiveAction = iter->second;
}
