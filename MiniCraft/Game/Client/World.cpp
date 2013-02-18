#include "stdafx.h"
#include "World.h"
#include "Unit.h"
#include "GameDefine.h"
#include "OgreManager.h"
#include "GUIManager.h"
#include <SdkCameraMan.h>
#include "ObjectManager.h"
#include "GameDataDef.h"
#include "Faction.h"
#include "CommandPanel.h"
#include "InfoPanel.h"
#include "Scene.h"
#include "Building.h"


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
,m_cmdPanel(new UiCommandPanel)
,m_infoPanel(new UiInfoPanel)
{
	
}

World::~World()
{
	SAFE_DELETE(m_cmdPanel);
	SAFE_DELETE(m_infoPanel);
}

void World::Init()
{
	using namespace Ogre;

	m_pSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, SCENE_MANAGER_NAME);

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
	m_pCamera->setPosition(0, 30, 0);
	m_pCamera->lookAt(0, 0, 10);

	//测试两个CPU玩家
	m_player[eGameRace_Terran] = new Faction(eGameRace_Terran);
	m_player[eGameRace_Zerg] = new Faction(eGameRace_Zerg);

	GameDataDefManager::GetSingleton().LoadAllData();

	//初始化Recast库
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

	//加载测试场景
	m_pTestScene = new Scene(m_pSceneMgr);
	m_pTestScene->Load("MyStarCraft.Scene", "General", this);

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

	m_cmdPanel->Init();
	m_infoPanel->Init();

	CEGUI::WindowManager& wndMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* pRoot = wndMgr.getWindow("Root");
	assert(pRoot);
	CGUIManager::GetSingleton().SetGUISheet(pRoot);
	pRoot->addChildWindow(wndMgr.getWindow("InfoPanelFrame"));
}

void World::Shutdown()
{
	SAFE_DELETE(m_pTestScene);

	SAFE_DELETE(m_player[eGameRace_Terran]);
	SAFE_DELETE(m_player[eGameRace_Zerg]);

	ObjectManager::GetSingleton().DestroyAll();

	SAFE_DELETE(m_pDetourCrowd);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);
	SAFE_DELETE(m_cameraMan);

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

	m_cmdPanel->Destroy();
	m_infoPanel->Destroy();
}

void World::Update(float dt)
{
	m_pDetourCrowd->updateTick(dt);

	ObjectManager::GetSingleton().UpdateAll(dt);

	World::GetSingleton().GetInfoPanel()->Update();
}

bool World::ClampPosToNavMesh( Ogre::Vector3& wPos )
{
	bool ret = m_pRecast->findNearestPointOnNavmesh(wPos, wPos);
	assert(ret);
	return ret;
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
	Ogre::TerrainGroup::RayResult result = m_pTestScene->GetTerrainGroup()->rayIntersects(queryRay);
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
		result = m_pTestScene->GetTerrainGroup()->rayIntersects(queryRay);
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
	POS newPos = camPos + camDir * 1.7f + camRight * -0.9f + camUp * -0.67f;
	m_pUISceneNode1->_setDerivedOrientation(m_pCamera->getRealOrientation());
	m_pUISceneNode1->_setDerivedPosition(newPos);
	m_pUISceneNode2->setPosition(1, -0.02f, 0);
	m_pUISceneNode3->setPosition(1.38f, 0.64f, 0);
	m_pUISceneNode4->setPosition(1.8f, -0.0f, -0.01f);
}

bool World::GetTerrainIntersectPos( const FLOAT2& screenPos, POS& retPt )
{
	Ogre::Ray ray;
	m_pCamera->getCameraToViewportRay(screenPos.x, screenPos.y, &ray);

	auto result = m_pTestScene->GetTerrainGroup()->rayIntersects(ray);

	if (result.hit)
	{
		retPt = result.position;
		return true;
	}

	return false;
}

void World::_LoadObjects( rapidxml::xml_node<>* node )
{
	size_t count = Ogre::StringConverter::parseUnsignedInt(node->first_attribute("count")->value());
	rapidxml::xml_node<>* curObjNode = node->first_node();

	for (size_t i=0; i< count; ++i)
	{
		const STRING strMesh = curObjNode->first_attribute("meshname")->value();
		const STRING strPos = curObjNode->first_attribute("position")->value();
		const STRING strOrient = curObjNode->first_attribute("orientation")->value();
		const STRING strScale = curObjNode->first_attribute("scale")->value();
		const bool bIsBuilding = Ogre::StringConverter::parseBool(curObjNode->first_attribute("isbuilding")->value());
		const bool bIsResource = Ogre::StringConverter::parseBool(curObjNode->first_attribute("isresource")->value());

		if (bIsBuilding || bIsResource)
		{
			Object* pObject = ObjectManager::GetSingleton().CreateObject(bIsBuilding ? eObjectType_Building : eObjectType_Resource);

			if(bIsBuilding)
			{
				const STRING strBuildingName = curObjNode->first_attribute("buildingname")->value();
				Building* pBuilding = static_cast<Building*>(pObject);

				const POS& pos = Ogre::StringConverter::parseVector3(strPos);
				const ORIENT& orient = Ogre::StringConverter::parseQuaternion(strOrient);
				const SCALE& scale = Ogre::StringConverter::parseVector3(strScale);

				pBuilding->Init(strBuildingName, pos, orient, scale);
			}
			else
			{
				pObject->setParameter("meshname", strMesh);
				pObject->setParameter("position", strPos);
				pObject->setParameter("orientation", strOrient);
				pObject->setParameter("scale", strScale);
			}
		}
		else
		{
			const Ogre::Vector3 pos = Ogre::StringConverter::parseVector3(strPos);
			const Ogre::Quaternion orient = Ogre::StringConverter::parseQuaternion(strOrient);
			const Ogre::Vector3 scale = Ogre::StringConverter::parseVector3(strScale);

			//非游戏对象,不纳入逻辑管理,只渲染
			Ogre::Entity* entity = m_pSceneMgr->createEntity(strMesh);
			assert(entity);

			Ogre::SceneNode* pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(pos, orient);
			pNode->setScale(scale);
			pNode->attachObject(entity);
		}	

		curObjNode = curObjNode->next_sibling();
	}
}
