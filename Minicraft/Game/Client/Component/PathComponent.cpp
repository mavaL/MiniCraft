#include "stdafx.h"
#include "PathComponent.h"
#include "OgreDetourCrowd.h"
#include "World.h"
#include "SelectableObject.h"
#include "OgreManager.h"
#include "Scene.h"

OgreDetourCrowd* PathComponent::m_pDetour = nullptr;
OgreRecast* PathComponent::m_pRecast = nullptr;
OgreDetourTileCache* PathComponent::m_pDetourTileCache = nullptr;

PathComponent::PathComponent( SelectableObject* pOwner )
:Component(pOwner)
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

void PathComponent::InitRecastLib(const STRING& sceneName)
{
	OgreRecastConfigParams recastParams = OgreRecastConfigParams();
	recastParams.setCellSize(1);
	recastParams.setCellHeight(0.16f);
	recastParams.setAgentMaxSlope(15);
	recastParams.setAgentHeight(1.5f);
	recastParams.setAgentMaxClimb(0.5f);
	recastParams.setAgentRadius(0.4f);
	recastParams.setEdgeMaxLen(2);
	recastParams.setEdgeMaxError(1.3f);
	recastParams.setVertsPerPoly(6);
	recastParams.setRegionMinSize(2);
	recastParams.setRegionMergeSize(3);
	recastParams.setDetailSampleDist(6);
	recastParams.setDetailSampleMaxError(1);

	m_pRecast = new OgreRecast(RenderManager.m_pSceneMgr, recastParams);
	m_pDetourTileCache = new OgreDetourTileCache(m_pRecast);

	STRING basename, extname;
	Ogre::StringUtil::splitBaseFilename(sceneName, basename, extname);
	STRING navMeshFilename = basename + "_NavMesh.Bin";
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(navMeshFilename, "General", false);
	m_pDetourTileCache->loadAll(stream);
	//m_pDetourTileCache->drawNavMesh();

	m_pDetour = new OgreDetourCrowd(m_pRecast);

	g_Environment.m_pCrowd = m_pDetour;
	g_Environment.m_pRecast = m_pRecast;
	g_Environment.m_pDetourTileCache = m_pDetourTileCache;
}

void PathComponent::DestroyRecastLib()
{
	SAFE_DELETE(m_pDetour);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);
}

void PathComponent::UpdateRecastLib(float dt)
{
	m_pDetour->updateTick(dt);
}

bool PathComponent::FindPath( POS& destPos, bool bJustTry )
{
	POS beginPos(m_pOwner->GetPosition());
	ClampPosToNavMesh(beginPos);
	ClampPosToNavMesh(destPos);

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
	ClampPosToNavMesh(adjustPos);

	m_destPos = adjustPos;
}

bool PathComponent::UpdatePathFinding( float dt )
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

POS PathComponent::GetRandomPositionOnNavmesh()
{
	TileSelection bound = m_pDetourTileCache->getBounds();
	int tx = (int)(Ogre::Math::RangeRandom(0, 1) * bound.maxTx);
	int ty = (int)(Ogre::Math::RangeRandom(0, 1) * bound.maxTy);

	Ogre::AxisAlignedBox tileBounds = m_pDetourTileCache->getTileBounds(tx, ty);
	Ogre::Vector3 center = tileBounds.getCenter();  // Center of the specified tile
	//center.y = tileBounds.getMinimum().y;   // Place on the ground
	// TODO centering probably has the biggest change of the point clipping to the navmesh

	// Get random point in tile (in circle in the middle of the tile with radius of tilesize/2)
	Ogre::Real radius = m_pDetourTileCache->getTileSize()/2;
	return m_pRecast->getRandomNavMeshPointInCircle(center, radius-1);   // TODO I could also make RADIUS_EPSILON be a fraction of the tileSize
}

bool PathComponent::ClampPosToNavMesh( POS& wPos )
{
	bool ret = m_pRecast->findNearestPointOnNavmesh(wPos, wPos);
	assert(ret);
	return ret;
}

void PathComponent::StopAgent()
{
	m_pDetour->stopAgent(m_agentID);
}




