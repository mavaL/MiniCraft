#include "stdafx.h"
#include "HarvestComponent.h"
#include "PathComponent.h"
#include "SelectableObject.h"
#include "Resource.h"
#include "Unit.h"
#include "World.h"
#include "Faction.h"
#include "Building.h"

HarvestComponent::HarvestComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_pTarget(nullptr)
,m_bCarried(false)
,m_curStage(eHarvestStage_None)
,m_fGatherTime(0)
{
	assert(m_pOwner->IsRenderableReady());

	m_pCrystal = g_Environment.m_pSceneMgr->createEntity("Crystal_0.mesh");
	assert(m_pCrystal);
	Ogre::SceneNode* pNode = m_pOwner->GetSceneNode()->createChildSceneNode(POS(0, 0, 0.3f));
	pNode->setScale(2,2,2);
	pNode->attachObject(m_pCrystal);
	m_pCrystal->setVisible(false);
}

void HarvestComponent::Update( float dt )
{
	if(m_curStage == eHarvestStage_None)
		return;

	assert(m_pTarget);

	if (m_bCarried && m_curStage != eHarvestStage_Return)
		m_curStage = eHarvestStage_Return;

	//逻辑处理
	PathComponent* path = m_pOwner->GetPath();
	if (m_curStage == eHarvestStage_ToRes)
	{
		if (!path->IsMoving())
		{
			//向资源移动
			const POS& destPos = m_pTarget->GetPosition();
			path->SetDestPos(destPos);

			StateMove tmpState;
			tmpState.Enter(m_pOwner);
		}
		else if (path->_UpdatePathFinding(dt))
		{
			//到达目的点
			Unit* pUnit = dynamic_cast<Unit*>(m_pOwner);
			pUnit->PlayAnimation(eAnimation_Gather, true);
			m_fGatherTime = 0;
			SetCurStage(eHarvestStage_Gather);
		}
	}
	else if (m_curStage == eHarvestStage_Gather)
	{
		//更新采集阶段
		m_fGatherTime += dt;
		const float GATHER_TIME = 4;
		if (m_fGatherTime > GATHER_TIME)
		{
			//减少目标资源剩余数量
			m_pTarget->DecreaseRes(m_pTarget->GetGatherOnceNum());

			m_pCrystal->setVisible(true);
			m_bCarried = true;
			SetCurStage(eHarvestStage_Return);
		}
	}
	else/* if(m_curStage == eHarvestStage_Return)*/
	{
		const STRING& unitName = m_pOwner->getParameter("unitName");
		SUnitData* unitData = &GameDataDefManager::GetSingleton().m_unitData[unitName];
		Faction* player = World::GetSingleton().GetFaction(unitData->m_race);

		if(!path->IsMoving())
		{
			//向基地移动
			POS destPos = player->GetBase()->GetPosition();
			World::GetSingleton().ClampPosToNavMesh(destPos);
			path->SetDestPos(destPos);

			StateMove tmpState;
			tmpState.Enter(m_pOwner);
		}
		else if(path->_UpdatePathFinding(dt))	//到达目的点
		{
			//返还资源
			player->AddMineral(m_pTarget->GetGatherOnceNum());

			m_pCrystal->setVisible(false);
			m_bCarried = false;
			//循环状态
			SetCurStage(eHarvestStage_ToRes);
		}
	}
}
