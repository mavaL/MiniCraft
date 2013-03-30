#include "stdafx.h"
#include "PathComponent.h"
#include "OgreDetourCrowd.h"
#include "World.h"
#include "SelectableObject.h"

PathComponent::PathComponent( SelectableObject* pOwner )
:Component(pOwner)
,m_pRecast(g_Environment.m_pRecast)
,m_pDetour(g_Environment.m_pCrowd)
,m_pAgent(nullptr)
,m_agentID(-1)
,m_bIsMoving(false)
{
	//确定渲染实例已创建
	assert(pOwner->IsRenderableReady());

	//创建寻路对象
	m_agentID = m_pDetour->addAgent(pOwner->GetPosition());
	m_pAgent = const_cast<dtCrowdAgent*>(m_pDetour->getAgent(m_agentID));
	assert(m_pAgent);
}

PathComponent::~PathComponent()
{
	if(m_pAgent)
	{
		m_pDetour->removeAgent(m_agentID);
		m_pAgent = nullptr;
	}
}

bool PathComponent::FindPath( POS& destPos, bool bJustTry )
{
	POS beginPos(m_pOwner->GetPosition());
	World::GetSingleton().ClampPosToNavMesh(beginPos);

	World::GetSingleton().ClampPosToNavMesh(destPos);

	int ret = m_pRecast->FindPath(beginPos, destPos, 1, 1);
	if(ret >= 0 && !bJustTry)
	{
		m_pDetour->setMoveTarget(m_agentID, destPos, false);
		//绘制路径线
		//m_pRecast->CreateRecastPathLine(1);
		m_bIsMoving = true;
	}

	return ret >= 0;
}

const POS PathComponent::GetAgentPos() const
{
	assert(m_pAgent->active);
	POS agentPos;
	OgreRecast::FloatAToOgreVect3(m_pAgent->npos, agentPos);

	return agentPos;
}

bool PathComponent::StopMove()
{
	bool ret = m_pDetour->stopAgent(m_agentID);
	assert(ret);
	m_bIsMoving = false;

	return ret;
}

void PathComponent::SetDestPos( const POS& destPos )
{
	POS adjustPos(destPos);
	World::GetSingleton().ClampPosToNavMesh(adjustPos);

	m_destPos = adjustPos;
}

bool PathComponent::_UpdatePathFinding( float dt )
{
	POS curPos = GetAgentPos();
	World::GetSingleton().ClampToTerrain(curPos);
	m_pOwner->GetSceneNode()->lookAt(curPos, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
	m_pOwner->SetPosition(curPos);

	if (curPos.positionEquals(m_destPos, 0.1f))
	{
		StopMove();
		return true;
	}

	return false;
}

void PathComponent::EnableObstcleAvoidance( bool bEnable )
{
	if (bEnable)
		m_pAgent->params.updateFlags = DT_CROWD_ANTICIPATE_TURNS | 
		DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_OBSTACLE_AVOIDANCE;
	else
		m_pAgent->params.updateFlags = 0;
}
