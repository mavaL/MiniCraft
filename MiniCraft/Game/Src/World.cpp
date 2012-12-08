#include "stdafx.h"
#include "World.h"
#include "GameDefine.h"
#include "Unit.h"



World::World()
:m_pRecast(nullptr)
,m_pDetourTileCache(nullptr)
,m_pDetourCrowd(nullptr)
,m_pSceneMgr(nullptr)
,m_pGold(nullptr)
{

}

void World::Init()
{
	using namespace Ogre;

	m_pSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, SCENE_MANAGER_NAME);
	m_pSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

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
	pHouseNode->scale(3, 3, 3);
	vecNavEnt.push_back(ent);

	SceneNode* pMineNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(RES_POS);
	pMineNode->scale(0.1f, 0.1f, 0.1f);
	m_pGold = m_pSceneMgr->createEntity("银矿脉.mesh");
	pMineNode->attachObject(m_pGold);
	vecNavEnt.push_back(m_pGold);

	//初始化Recast库,全局config非常重要,参见默认值
	m_pRecast = new OgreRecast(m_pSceneMgr, OgreRecastConfigParams());
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
	// 	m_pCharacter = new TestCharacter("TestAgent", m_pSceneMgr, m_pDetourCrowd, Vector3(20,0,20));
	// 	m_pCharacter->updateDestination(Vector3(-15,0,-15));
	// 
	// 	int ret = m_pRecast->FindPath(Vector3(20,0,20), Vector3(-15,0,-15), 1, 1) ;
	// 	if( ret >= 0 )
	// 		m_pRecast->CreateRecastPathLine(1) ; // Draw a line showing path at specified slot
}

void World::Shutdown()
{
	SAFE_DELETE(m_pDetourCrowd);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);

	Ogre::Root::getSingleton().destroySceneManager(m_pSceneMgr);
	m_pSceneMgr = nullptr;
}

void World::Update(float dt)
{
	m_pDetourCrowd->updateTick(dt);

	for(size_t i=0; i<m_vecUnits.size(); ++i)
		m_vecUnits[i]->Update(dt);
}

Unit* World::CreateUnit()
{
	Ogre::Entity* pEnt = m_pSceneMgr->createEntity("Sinbad.mesh");
	Ogre::SceneNode* pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode
		("UnitNode", Ogre::Vector3(10,0,10), Ogre::Quaternion(Ogre::Radian(Ogre::Math::PI), Ogre::Vector3::UNIT_Y));

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
