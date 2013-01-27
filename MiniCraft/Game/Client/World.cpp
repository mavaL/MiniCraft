#include "stdafx.h"
#include "World.h"
#include "GameDefine.h"
#include "Unit.h"
#include "OgreManager.h"
#include <SdkCameraMan.h>
#include "DotSceneLoader.h"
#include "ObjectManager.h"


SGlobalEnvironment	g_Environment;

////////////////////////////////////////////////////////////////
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

	g_Environment.m_pSceneMgr = m_pSceneMgr;

	m_pSceneQuery = m_pSceneMgr->createAABBQuery(AxisAlignedBox());
	m_pRaySceneQuery = m_pSceneMgr->createRayQuery(Ray());
	m_pRaySceneQuery->setSortByDistance(true);

	Ogre::MovableObject::setDefaultQueryFlags(eQueryType_Default);

	m_pCamera = m_pSceneMgr->createCamera("GodViewCam");
	m_pCamera->setNearClipDistance(0.1f);
	COgreManager::GetSingleton().GetViewport()->setCamera(m_pCamera);

	m_cameraMan = new OgreBites::SdkCameraMan(m_pCamera);
	m_cameraMan->setStyle(OgreBites::CS_FREELOOK);

	//RTS锁死视角
	m_pCamera->setPosition(0, 35, 0);
	m_pCamera->lookAt(0, 0, 8.75f);

	//场景中参与构建NavMesh的物体
	std::vector<Entity*> vecNavEnt;	

	//加载测试场景Test.Scene
	DotSceneLoader sceneLoader;
	sceneLoader.parseDotScene("MyStarCraft.Scene", "General", 
		m_pSceneMgr, m_pSceneMgr->getRootSceneNode()->createChildSceneNode("SceneNode"));

	//初始化Recast库
	OgreRecastConfigParams recastParams = OgreRecastConfigParams();
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

 	//m_pDetourTileCache->drawNavMesh();

	//初始化Detour寻路库
	m_pDetourCrowd = new OgreDetourCrowd(m_pRecast);

	g_Environment.m_pRecast = m_pRecast;
	g_Environment.m_pCrowd = m_pDetourCrowd;

	//UI for test
	Ogre::Entity* pEntConsole = m_pSceneMgr->createEntity("ConsoleTerran_0.mesh");
	pEntConsole->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	m_pUISceneNode1 = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("UIConsoleNode");
	m_pUISceneNode1->attachObject(pEntConsole);
	m_pConsoleAnim1 = pEntConsole->getAnimationState("Birth");
	assert(m_pConsoleAnim1);
	(const_cast<Ogre::AxisAlignedBox&>(pEntConsole->getMesh()->getBounds())).setInfinite();

	pEntConsole = m_pSceneMgr->createEntity("ConsoleTerran_1.mesh");
	pEntConsole->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	m_pUISceneNode2 = m_pUISceneNode1->createChildSceneNode("InfoPanelNode");
	m_pUISceneNode2->attachObject(pEntConsole);
	m_pConsoleAnim2 = pEntConsole->getAnimationState("Birth");
	assert(m_pConsoleAnim2);
	(const_cast<Ogre::AxisAlignedBox&>(pEntConsole->getMesh()->getBounds())).setInfinite();

	pEntConsole = m_pSceneMgr->createEntity("ConsoleTerran_2.mesh");
	pEntConsole->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	m_pUISceneNode3 = m_pUISceneNode1->createChildSceneNode("CmdPanelNode");
	m_pUISceneNode3->attachObject(pEntConsole);
	(const_cast<Ogre::AxisAlignedBox&>(pEntConsole->getMesh()->getBounds())).setInfinite();

	pEntConsole = m_pSceneMgr->createEntity("ConsoleProtoss_6.mesh");
	pEntConsole->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	m_pUISceneNode4 = m_pUISceneNode1->createChildSceneNode("PortraitPanelNode");
	m_pUISceneNode4->attachObject(pEntConsole);
	(const_cast<Ogre::AxisAlignedBox&>(pEntConsole->getMesh()->getBounds())).setInfinite();
}

void World::Shutdown()
{
	ObjectManager::GetSingleton().DestroyAll();

	SAFE_DELETE(m_terrainGroup);
	SAFE_DELETE(m_terrainOption);
	SAFE_DELETE(m_pDetourCrowd);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);
	SAFE_DELETE(m_cameraMan);
	m_pTerrain = nullptr;

	m_pSceneMgr->destroyQuery(m_pSceneQuery);
	m_pSceneMgr->destroyQuery(m_pRaySceneQuery);
	m_pSceneQuery = nullptr;
	m_pRaySceneQuery = nullptr;

	Ogre::Root::getSingleton().destroySceneManager(m_pSceneMgr);
	m_pSceneMgr = nullptr;

	SelectableObject::ReleaseMeshCache();

	COgreManager::GetSingleton().GetViewport()->setCamera(nullptr);
	m_pCamera = nullptr;

	m_vecSelectUnis.clear();

	g_Environment.Reset();
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
	m_terrainOption->setCompositeMapDistance(3000);
	m_terrainOption->setUseRayBoxDistanceCalculation(true);
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

	ObjectManager::GetSingleton().UpdateAll(dt);
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
		m_pCamera->setPosition(pos.x, 35, pos.z);
		m_pCamera->lookAt(pos.x, 0, pos.z + 8.75f);
	}

	m_bFreeCamMode = bEnable;
}

void World::GetAABBSceneQueryResult(const Ogre::AxisAlignedBox& box, 
	std::vector<Ogre::MovableObject*>& result, int queryMask)
{
	assert(m_pSceneQuery);

	m_pSceneQuery->setBox(box);
	m_pSceneQuery->setQueryMask(queryMask);
	Ogre::SceneQueryResult& queryResults = m_pSceneQuery->execute();

	auto movableList = queryResults.movables;
	for (auto iter=movableList.begin(); iter!=movableList.end(); ++iter)
		result.push_back(*iter);
}

Ogre::MovableObject* World::GetRaySceneQueryResult( const Ogre::Ray& ray, int queryMask /*= 0xffffffff*/ )
{
	assert(m_pRaySceneQuery);

	m_pRaySceneQuery->setRay(ray);
	m_pRaySceneQuery->setQueryMask(queryMask);
	const Ogre::RaySceneQueryResult& result = m_pRaySceneQuery->execute();

	if(result.empty())
		return nullptr;

	return result[0].movable;
}

void World::SetObjectSelected( int ID )
{
	SelectableObject* pObject = dynamic_cast<SelectableObject*>(ObjectManager::GetSingleton().GetObject(ID));
	pObject->SetSelected(true);
	m_vecSelectUnis.push_back(pObject);
}

void World::ClearSelectedState()
{
	for (size_t i=0; i<m_vecSelectUnis.size(); ++i)
	{
		SelectableObject* pObject = dynamic_cast<SelectableObject*>(m_vecSelectUnis[i]);
		pObject->SetSelected(false);
	}

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

void World::UpdateConsoleUITransform(float dt)
{
	static bool bPlayed = false;
	static float sWaitTime = 0;
	if (!bPlayed)
	{
		sWaitTime += dt;
		if (sWaitTime > 2.0f)
		{
			m_pConsoleAnim1->setEnabled(true);
			m_pConsoleAnim2->setEnabled(true);
			m_pConsoleAnim1->setLoop(false);
			m_pConsoleAnim2->setLoop(false);
			bPlayed = true;
		}
	}
	m_pConsoleAnim1->addTime(dt);
	m_pConsoleAnim2->addTime(dt);

	const FLOAT3& camRight = m_pCamera->getRealRight();
	const FLOAT3& camUp = m_pCamera->getRealUp();
	const POS& camPos = m_pCamera->getRealPosition();
	const FLOAT3& camDir = m_pCamera->getRealDirection();
	// 	Ogre::Degree halfFov(m_pCamera->getFOVy().valueDegrees() / 2);
	// 	float fOffset = 3 * Ogre::Math::Tan(halfFov.valueRadians());
	//TODO: 硬编码设置UI位置,以后要加入UI layout
	POS newPos = camPos + camDir * 2 + camRight * -1.07f + camUp * -0.8f;
	m_pUISceneNode1->_setDerivedOrientation(m_pCamera->getRealOrientation());
	m_pUISceneNode1->_setDerivedPosition(newPos);
	m_pUISceneNode2->setPosition(1, -0.02f, 0);
	m_pUISceneNode3->setPosition(1.8f, 0.64f, 0);
	m_pUISceneNode4->setPosition(2.25f, -0.03f, -0.1f);
}
