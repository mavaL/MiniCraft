#include "stdafx.h"
#include "PathComponent.h"
#include "OgreDetourCrowd.h"
#include "World.h"
#include "SelectableObject.h"

PathComponent::PathComponent( SelectableObject* pOwner )
:m_pOwner(pOwner)
,m_pRecast(g_Environment.m_pRecast)
,m_pDetour(g_Environment.m_pCrowd)
,m_pAgent(nullptr)
,m_agentID(-1)
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

bool PathComponent::FindPath( const Ogre::Vector3& destPos, bool bJustTry )
{
	Ogre::Vector3 beginPos(m_pOwner->GetPosition());
	World::GetSingleton().ClampPosToNavMesh(beginPos);

	Ogre::Vector3 adjustDestPos(destPos);
	World::GetSingleton().ClampPosToNavMesh(adjustDestPos);

	int ret = m_pRecast->FindPath(beginPos, adjustDestPos, 1, 1);
	if(ret >= 0 && !bJustTry)
	{
		m_pDetour->setMoveTarget(m_agentID, adjustDestPos, false);
		//绘制路径线
		//m_pRecast->CreateRecastPathLine(1);
		m_pOwner->setParameter("needmove", "false");
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
	return m_pDetour->stopAgent(m_agentID);
}
