#include "stdafx.h"
#include "World.h"
#include "GameDefine.h"
#include "Unit.h"
#include "OgreManager.h"
#include <SdkCameraMan.h>
#include "DotSceneLoader.h"


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
,m_terrainGroup(nullptr)
,m_terrainOption(nullptr)
,m_pTerrain(nullptr)
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

	//加载测试场景Test.Scene
	DotSceneLoader sceneLoader;
	sceneLoader.parseDotScene("TestScene.Scene", "General", 
		m_pSceneMgr, m_pSceneMgr->getRootSceneNode()->createChildSceneNode("SceneNode"));

	//初始化Recast库
	OgreRecastConfigParams recastParams = OgreRecastConfigParams();
// 	recastParams.setCellSize(1);
// 	recastParams.setCellHeight(0.16f);
// 	recastParams.setAgentMaxSlope(45);
// 	recastParams.setAgentHeight(1.5f);
// 	recastParams.setAgentMaxClimb(1.5f);
// 	recastParams.setAgentRadius(0.6f);
// 	recastParams.setEdgeMaxLen(2);
// 	recastParams.setEdgeMaxError(1.3f);
// 	recastParams.setVertsPerPoly(6);
// 	recastParams.setRegionMinSize(2);
// 	recastParams.setRegionMergeSize(3);
// 	recastParams.setDetailSampleDist(6);
// 	recastParams.setDetailSampleMaxError(1);
	recastParams.setCellSize(1);
	recastParams.setCellHeight(0.16f);
	recastParams.setAgentMaxSlope(15);
	recastParams.setAgentHeight(1.5f);
	recastParams.setAgentMaxClimb(0.5f);
	recastParams.setAgentRadius(0.6f);
	recastParams.setEdgeMaxLen(2);
	recastParams.setEdgeMaxError(1.3f);
	recastParams.setVertsPerPoly(6);
	recastParams.setRegionMinSize(2);
	recastParams.setRegionMergeSize(3);
	recastParams.setDetailSampleDist(6);
	recastParams.setDetailSampleMaxError(1);

	m_pRecast = new OgreRecast(m_pSceneMgr, recastParams);
	m_pDetourTileCache = new OgreDetourTileCache(m_pRecast);

	//加载编辑器导出的导航网格数据
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(
		"NavMesh.Bin", "General", false);
	assert(m_pDetourTileCache->loadAll(stream));

 	m_pDetourTileCache->drawNavMesh();

	//初始化Detour寻路库
	m_pDetourCrowd = new OgreDetourCrowd(m_pRecast);
}

void World::Shutdown()
{
	SAFE_DELETE(m_terrainGroup);
	SAFE_DELETE(m_terrainOption);
	SAFE_DELETE(m_pDetourCrowd);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);
	SAFE_DELETE(m_cameraMan);
	m_pTerrain = nullptr;

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

void World::LoadTerrain( rapidxml::xml_node<>* XMLNode )
{
	m_terrainOption = new Ogre::TerrainGlobalOptions;

	Ogre::Real worldSize = DotSceneLoader::getAttribReal(XMLNode, "worldSize");
	int mapSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("mapSize")->value());
	//bool colourmapEnabled = DotSceneLoader::getAttribBool(XMLNode, "colourmapEnabled");
	//int colourMapTextureSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("colourMapTextureSize")->value());
	//int compositeMapDistance = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningCompositeMapDistance")->value());
	int maxPixelError = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningMaxPixelError")->value());

	//     Ogre::Vector3 lightdir(0, -0.3, 0.75);
	//     lightdir.normalise();
	//     Ogre::Light* l = mSceneMgr->createLight("tstLight");
	//     l->setType(Ogre::Light::LT_DIRECTIONAL);
	//     l->setDirection(lightdir);
	//     l->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
	//     l->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	m_terrainOption->setMaxPixelError((Ogre::Real)maxPixelError);
	//m_terrainOption->setCompositeMapDistance((Ogre::Real)compositeMapDistance);
	// mTerrainGlobalOptions->setLightMapDirection(lightdir);
	m_terrainOption->setCompositeMapAmbient(m_pSceneMgr->getAmbientLight());
	//mTerrainGlobalOptions->setCompositeMapDiffuse(l->getDiffuseColour());

	//mSceneMgr->destroyLight("tstLight");

	m_terrainGroup = new Ogre::TerrainGroup(m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, mapSize, worldSize);
	m_terrainGroup->setOrigin(Ogre::Vector3::ZERO);
	m_terrainGroup->setResourceGroup("General");

	//加载地形数据
	m_terrainGroup->defineTerrain(0, 0, "terrain.dat");
	m_terrainGroup->loadTerrain(0, 0);
	m_terrainGroup->freeTemporaryResources();
	m_pTerrain = m_terrainGroup->getTerrain(0, 0);
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
	ClampToTerrain(adjustPos);
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

void World::ClampToTerrain(Ogre::Vector3& pos)
{
	// Setup the scene query
	Ogre::Ray queryRay(pos, Ogre::Vector3::NEGATIVE_UNIT_Y);

	// Perform the scene query
	Ogre::TerrainGroup::RayResult result = m_terrainGroup->rayIntersects(queryRay);
	if(result.hit) 
	{
		Ogre::Real terrainHeight = result.position.y;
		pos.y = terrainHeight;
	} 
	else
	{
		// Try querying terrain above character
		queryRay.setOrigin(pos);
		queryRay.setDirection(Ogre::Vector3::UNIT_Y);

		// Perform scene query again
		result = m_terrainGroup->rayIntersects(queryRay);
		if(result.hit) 
		{
			Ogre::Real terrainHeight = result.position.y;
			pos.y = terrainHeight;
		}
	}
}