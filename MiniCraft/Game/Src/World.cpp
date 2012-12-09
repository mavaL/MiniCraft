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
{

}

void World::Init()
{
	using namespace Ogre;

	m_pSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, SCENE_MANAGER_NAME);
	m_pSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

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
	Ogre::SceneNode* node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("GroundNode");
	node->attachObject(ent);
	vecNavEnt.push_back(ent);

	ent = m_pSceneMgr->createEntity("highlanderhouse.01.mesh");
	SceneNode* pHouseNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(BASE_POS);
	pHouseNode->attachObject(ent);
	vecNavEnt.push_back(ent);

	SceneNode* pMineNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(RES_POS);
	pMineNode->scale(0.04f, 0.04f, 0.04f);
	m_pGold = m_pSceneMgr->createEntity("银矿脉.mesh");
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

	Ogre::Root::getSingleton().destroySceneManager(m_pSceneMgr);
	m_pSceneMgr = nullptr;

	COgreManager::GetSingleton().GetViewport()->setCamera(nullptr);
	m_pCamera = nullptr;
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

	Ogre::Entity* pEnt = m_pSceneMgr->createEntity("Sinbad.mesh");
	Ogre::SceneNode* pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(adjustPos);

	Unit* pNewUnit = new Unit((int)m_vecUnits.size(), pEnt, pNode, m_pRecast, m_pDetourCrowd);
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
