#include "stdafx.h"
#include "World.h"
#include "GameDefine.h"
#include "Unit.h"
#include "OgreManager.h"
#include <SdkCameraMan.h>



World::World()
:m_pRecast(nullptr)
,m_pDetourTileCache(nullptr)
,m_pDetourCrowd(nullptr)
,m_pSceneMgr(nullptr)
,m_pGold(nullptr)
,m_pCamera(nullptr)
,m_cameraMan(nullptr)
,m_bFreeCamMode(false)
,m_pSceneQuery(nullptr)
,m_pRaySceneQuery(nullptr)
{

}

void World::Init()
{
	using namespace Ogre;

	m_pSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, SCENE_MANAGER_NAME);
	m_pSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

	m_pSceneQuery = m_pSceneMgr->createAABBQuery(AxisAlignedBox());
	m_pRaySceneQuery = m_pSceneMgr->createRayQuery(Ray());
	m_pRaySceneQuery->setSortByDistance(true);

	Ogre::MovableObject::setDefaultQueryFlags(eQueryType_Default);

	m_pCamera = m_pSceneMgr->createCamera("GodViewCam");
	m_pCamera->setNearClipDistance(0.1f);
	COgreManager::GetSingleton().GetViewport()->setCamera(m_pCamera);

	m_cameraMan = new OgreBites::SdkCameraMan(m_pCamera);
	m_cameraMan->setStyle(OgreBites::CS_FREELOOK);

	//45度俯角
	m_pCamera->setPosition(0, 15, 0);
	m_pCamera->lookAt(0, 0, 10);

	//场景中参与构建NavMesh的物体
	std::vector<Entity*> vecNavEnt;	

	Ogre::MeshManager::getSingleton().createPlane("FloorMesh", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::Plane(Ogre::Vector3::UNIT_Y, 0), 100, 100, 10, 10, true, 1, 20, 20, Ogre::Vector3::UNIT_Z);
	Ogre::Entity* ent = m_pSceneMgr->createEntity("entFloor", "FloorMesh");
	ent->setMaterialName("Examples/BumpyMetal");
	ent->setQueryFlags(eQueryType_WorldGeometry);
	Ogre::SceneNode* node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode");
	node->attachObject(ent);
	vecNavEnt.push_back(ent);

	ent = m_pSceneMgr->createEntity("highlanderhouse.01.mesh");
	ent->setQueryFlags(eQueryType_WorldGeometry);
	SceneNode* pHouseNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(BASE_POS);
	pHouseNode->attachObject(ent);
	vecNavEnt.push_back(ent);

	SceneNode* pMineNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(RES_POS);
	pMineNode->scale(0.04f, 0.04f, 0.04f);
	m_pGold = m_pSceneMgr->createEntity("银矿脉.mesh");
	m_pGold->setQueryFlags(eQueryType_WorldGeometry);
	pMineNode->attachObject(m_pGold);
	vecNavEnt.push_back(m_pGold);

	//初始化Recast库,全局config非常重要,参见默认值
	OgreRecastConfigParams initParams;
	//initParams.setAgentRadius(0.2);
	m_pRecast = new OgreRecast(m_pSceneMgr, initParams);

	m_pDetourTileCache = new OgreDetourTileCache(m_pRecast);
	if(m_pDetourTileCache->TileCacheBuild(vecNavEnt)) 
	{
		m_pDetourTileCache->drawNavMesh();
	} 
	else 
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("ERROR: could not generate useable navmesh from mesh using detourTileCache.");
		return;
	}
	//m_pDetourTileCache->saveAll("NavMeshData");

	//初始化Detour寻路库
	m_pDetourCrowd = new OgreDetourCrowd(m_pRecast);
}

void World::Shutdown()
{
	SAFE_DELETE(m_pDetourCrowd);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);
	SAFE_DELETE(m_cameraMan);

	//重置脚本系统,为了销毁所有Unit
	ScriptSystem::GetSingleton().Reset();

	m_pSceneMgr->destroyQuery(m_pSceneQuery);
	m_pSceneMgr->destroyQuery(m_pRaySceneQuery);
	m_pSceneQuery = nullptr;
	m_pRaySceneQuery = nullptr;

	Ogre::Root::getSingleton().destroySceneManager(m_pSceneMgr);
	m_pSceneMgr = nullptr;

	COgreManager::GetSingleton().GetViewport()->setCamera(nullptr);
	m_pCamera = nullptr;

	m_vecSelectUnis.clear();
	m_vecUnits.clear();
}

void World::Update(float dt)
{
	m_pDetourCrowd->updateTick(dt);

	for(size_t i=0; i<m_vecUnits.size(); ++i)
		m_vecUnits[i]->Update(dt);
}

Unit* World::CreateUnit(const Ogre::Vector3& pos)
{
	Ogre::Vector3 adjustPos(pos);
	assert(ClampPosToNavMesh(adjustPos));

	int newID = (int)m_vecUnits.size();
	Ogre::String entName(Unit::ENTITY_NAME_PREFIX);
	entName += Ogre::StringConverter::toString(newID);

	Ogre::Entity* pEnt = m_pSceneMgr->createEntity(entName, "Sinbad.mesh");
	Ogre::SceneNode* pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(adjustPos);

	Unit* pNewUnit = new Unit(newID, pEnt, pNode, m_pRecast, m_pDetourCrowd);
	assert(pNewUnit);

	m_vecUnits.push_back(pNewUnit);

	return pNewUnit;
}

Unit* World::GetUnitFromID( int ID )
{
	assert(ID >= 0 && ID < (int)m_vecUnits.size());
	return m_vecUnits[ID];
}

bool World::ClampPosToNavMesh( Ogre::Vector3& wPos )
{
	return m_pRecast->findNearestPointOnNavmesh(wPos, wPos);
}

Ogre::Vector3 World::GetRandomPositionOnNavmesh()
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

void World::EnableFreeCamera( bool bEnable )
{
	assert(m_pCamera && m_cameraMan);

	if(!bEnable)
	{
		const Ogre::Vector3 pos = m_pCamera->getPosition();
		m_pCamera->setPosition(pos.x, 15, pos.z);
		m_pCamera->lookAt(pos.x, 0, pos.z + 10);
	}

	m_bFreeCamMode = bEnable;
}

void World::GetAABBSceneQueryResult(const Ogre::AxisAlignedBox& box, 
	std::vector<Ogre::MovableObject*>& result, int queryMask)
{
	assert(m_pSceneQuery);

	m_pSceneQuery->setBox(box);
	Ogre::SceneQueryResult& queryResults = m_pSceneQuery->execute();

	auto movableList = queryResults.movables;
	for (auto iter=movableList.begin(); iter!=movableList.end(); ++iter)
	{
		if((*iter)->getQueryFlags() & queryMask)
			result.push_back(*iter);
	}
}

Ogre::MovableObject* World::GetRaySceneQueryResult( const Ogre::Ray& ray, int queryMask /*= 0xffffffff*/ )
{
	assert(m_pRaySceneQuery);

	m_pRaySceneQuery->setRay(ray);
	const Ogre::RaySceneQueryResult& result = m_pRaySceneQuery->execute();

	if(result.empty() || result[0].worldFragment)
		return nullptr;

	if(result[0].movable->getQueryFlags() & queryMask)
		return result[0].movable;

	return nullptr;
}

void World::SetUnitSelected( int ID )
{
	m_vecUnits[ID]->SetSelected(true);
	m_vecSelectUnis.push_back(m_vecUnits[ID]);
}

void World::ClearAllUnitSelected()
{
	for (size_t i=0; i<m_vecSelectUnis.size(); ++i)
		m_vecSelectUnis[i]->SetSelected(false);

	m_vecSelectUnis.clear();
}
