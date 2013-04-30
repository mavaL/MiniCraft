#include "stdafx.h"
#include "World.h"
#include "GameDefine.h"
#include "OgreManager.h"
#include "GUIManager.h"
#include <SdkCameraMan.h>
#include "ObjectManager.h"
#include "GameDataDef.h"
#include "CommandPanel.h"
#include "InfoPanel.h"
#include "Scene.h"
#include "PortraitPanel.h"
#include "AIFaction.h"
#include "Resource.h"
#include "Building.h"
#include "BehaviorTreeTemplateManager.h"
#include "BlackBoard.h"
#include "PathComponent.h"
#include "RagdollComponent.h"
#include "PhysicManager.h"
#include "BehaviorComponent.h"

SGlobalEnvironment	g_Environment;

//**** Define stuff for the Lua Class ****//
// Define the Lua ClassName
const char World::className[] = "World";

// Define the methods we will expose to Lua
#define method(class, name) {#name, &class::name}
Luna<World>::RegType World::methods[] = 
{
	method(World, GetPlayerUnitNum),
	method(World, SetGlobalBBParam_Int),
	method(World, SetGlobalBBParam_Float),
	method(World, SetGlobalBBParam_Bool),
	{0,0}
};

////////////////////////////////////////////////////////////////
World::World()
:m_cameraMan(nullptr)
,m_bFreeCamMode(false)
,m_pSceneQuery(nullptr)
,m_pRaySceneQuery(nullptr)
,m_pSphereSceneQuery(nullptr)
,m_cmdPanel(new UiCommandPanel)
,m_infoPanel(new UiInfoPanel)
,m_portraitPanel(new UiPortraitPanel)
,m_pScriptSystem(Kratos::ScriptSystem::GetSingletonPtr())
,m_pRenderSystem(Kratos::COgreManager::GetSingletonPtr())
{
	Luna<World>::Register(m_pScriptSystem->GetLuaState());
	m_pScriptSystem->BindObjectToLua<World>("world", this);
}

World::~World()
{
	SAFE_DELETE(m_cmdPanel);
	SAFE_DELETE(m_infoPanel);
	SAFE_DELETE(m_portraitPanel);
}

void World::Init()
{
	using namespace Ogre;

	SceneManager* sm = m_pRenderSystem->m_pSceneMgr;
	Camera* cam = m_pRenderSystem->m_pMainCamera;

	m_pSceneQuery = sm->createAABBQuery(AxisAlignedBox());
	m_pRaySceneQuery = sm->createRayQuery(Ray());
	m_pRaySceneQuery->setSortByDistance(true);
	m_pSphereSceneQuery = sm->createSphereQuery(Sphere());

	Ogre::MovableObject::setDefaultQueryFlags(eQueryType_Default);

	m_cameraMan = new OgreBites::SdkCameraMan(cam);
	m_cameraMan->setStyle(OgreBites::CS_FREELOOK);
	m_cameraMan->setTopSpeed(50);

	//RTS锁死视角
	cam->setPosition(0, 24, 0);
	cam->lookAt(0, 0, 8);
	//cam->setFOVy(Degree(30));

	GameDataDefManager::GetSingleton().LoadAllData();

	//初始化行为库
	BehaviorComponent::InitBehaviors();

	//测试两个AI
	m_player[eGameRace_Terran] = new FactionAI(eGameRace_Terran);
	m_player[eGameRace_Zerg] = new FactionAI(eGameRace_Zerg);
	m_player[eGameRace_Terran]->SetEnemy(m_player[eGameRace_Zerg]);
	m_player[eGameRace_Zerg]->SetEnemy(m_player[eGameRace_Terran]);
	m_player[eGameRace_Terran]->SetTeamColor(COLOR::Blue);
	m_player[eGameRace_Zerg]->SetTeamColor(COLOR::Red);

	const STRING sceneName("MyStarCraft.Scene");
	//初始化Recast库
	PathComponent::InitRecastLib(sceneName);

	//加载测试场景
	m_pTestScene = new Kratos::Scene();
	m_pTestScene->Load(sceneName, "General", this);

	dynamic_cast<FactionAI*>(m_player[eGameRace_Zerg])->Init();
	dynamic_cast<FactionAI*>(m_player[eGameRace_Terran])->Init();

	//UI for test
	Ogre::Entity* pEntConsole = m_pRenderSystem->CreateEntityWithTangent("ConsoleTerran_0.mesh", sm);
	pEntConsole->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2);
	pEntConsole->setCastShadows(false);
	m_pUISceneNode1 = sm->getRootSceneNode()->createChildSceneNode("UIConsoleNode");
	m_pUISceneNode1->attachObject(pEntConsole);
	m_pConsoleAnim1 = pEntConsole->getAnimationState("Birth");
	assert(m_pConsoleAnim1);
	(const_cast<Ogre::AxisAlignedBox&>(pEntConsole->getMesh()->getBounds())).setInfinite();

	pEntConsole = m_pRenderSystem->CreateEntityWithTangent("ConsoleTerran_1.mesh", sm);
	pEntConsole->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2);
	pEntConsole->setCastShadows(false);
	m_pUISceneNode2 = m_pUISceneNode1->createChildSceneNode("InfoPanelNode");
	m_pUISceneNode2->attachObject(pEntConsole);
	m_pConsoleAnim2 = pEntConsole->getAnimationState("Birth");
	assert(m_pConsoleAnim2);
	(const_cast<Ogre::AxisAlignedBox&>(pEntConsole->getMesh()->getBounds())).setInfinite();

	pEntConsole = m_pRenderSystem->CreateEntityWithTangent("ConsoleTerran_2.mesh", sm);
	pEntConsole->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2);
	pEntConsole->setCastShadows(false);
	m_pUISceneNode3 = m_pUISceneNode1->createChildSceneNode("CmdPanelNode");
	m_pUISceneNode3->attachObject(pEntConsole);
	(const_cast<Ogre::AxisAlignedBox&>(pEntConsole->getMesh()->getBounds())).setInfinite();

	pEntConsole = m_pRenderSystem->CreateEntityWithTangent("ConsoleProtoss_6.mesh", sm);
	pEntConsole->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_2);
	pEntConsole->setCastShadows(false);
	m_pUISceneNode4 = m_pUISceneNode1->createChildSceneNode("PortraitPanelNode");
	m_pUISceneNode4->attachObject(pEntConsole);
	(const_cast<Ogre::AxisAlignedBox&>(pEntConsole->getMesh()->getBounds())).setInfinite();

	m_cmdPanel->Init();
	m_infoPanel->Init();
	m_portraitPanel->Init();

	CEGUI::WindowManager& wndMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* pRoot = wndMgr.getWindow("Root");
	assert(pRoot);
	GUIMANAGER.SetGUISheet(pRoot);
	pRoot->addChildWindow(wndMgr.getWindow("InfoPanelFrame"));
}

void World::Shutdown()
{
	SAFE_DELETE(m_pTestScene);

	SAFE_DELETE(m_player[eGameRace_Terran]);
	SAFE_DELETE(m_player[eGameRace_Zerg]);

	ObjectManager::GetSingleton().DestroyAll();
	PathComponent::DestroyRecastLib();
	SAFE_DELETE(m_cameraMan);

	m_pRenderSystem->m_pSceneMgr->destroyQuery(m_pSceneQuery);
	m_pRenderSystem->m_pSceneMgr->destroyQuery(m_pRaySceneQuery);
	m_pRenderSystem->m_pSceneMgr->destroyQuery(m_pSphereSceneQuery);
	m_pSceneQuery = nullptr;
	m_pRaySceneQuery = nullptr;
	m_pSphereSceneQuery = nullptr;

	SelectableObject::ReleaseMeshCache();

	m_vecSelectUnis.clear();

	g_Environment.Reset();

	m_cmdPanel->Destroy();
	m_infoPanel->Destroy();
	m_portraitPanel->Destroy();
}

void World::Update(float dt)
{
	//更新行为树全局黑板
	m_pScriptSystem->Call("GlobalBBUpdate_Terran");
	m_pScriptSystem->Call("GlobalBBUpdate_Zerg");

	m_player[eGameRace_Terran]->Update(dt);
	m_player[eGameRace_Zerg]->Update(dt);

	PathComponent::UpdateRecastLib(dt);

	ObjectManager::GetSingleton().UpdateAll(dt);

	m_infoPanel->Update();
	m_portraitPanel->Update();
}

void World::EnableFreeCamera( bool bEnable )
{
	assert(m_pRenderSystem->m_pMainCamera && m_cameraMan);

	if(!bEnable)
	{
		Ogre::Camera* cam = m_pRenderSystem->m_pMainCamera;
		const Ogre::Vector3 pos = cam->getPosition();
		cam->setPosition(0, 24, 0);
		cam->lookAt(0, 0, 8);
	}

	m_bFreeCamMode = bEnable;
}

void World::GetAABBSceneQueryResult(const Ogre::AxisAlignedBox& box, 
	std::vector<Ogre::MovableObject*>& result, int mask)
{
	m_pSceneQuery->setBox(box);
	m_pSceneQuery->setQueryMask(mask);
	Ogre::SceneQueryResult& queryResults = m_pSceneQuery->execute();

	auto movableList = queryResults.movables;
	for (auto iter=movableList.begin(); iter!=movableList.end(); ++iter)
		result.push_back(*iter);
}

Ogre::MovableObject* World::GetRaySceneQueryResult( const OIS::MouseEvent& arg, int mask, POS* retIntersect )
{
	float screenX = arg.state.X.abs / (float)arg.state.width;
	float screenY = arg.state.Y.abs / (float)arg.state.height;

	if(retIntersect && !GetTerrainIntersectPos(FLOAT2(screenX, screenY), *retIntersect))
		return nullptr;

	Ogre::Ray ray;
	m_pRenderSystem->m_pMainCamera->getCameraToViewportRay(screenX, screenY, &ray);

	m_pRaySceneQuery->setRay(ray);
	m_pRaySceneQuery->setQueryMask(mask);
	const Ogre::RaySceneQueryResult& result = m_pRaySceneQuery->execute();

	if(result.empty())
		return nullptr;

	return result[0].movable;
}

void World::GetSphereSceneQueryResult( const Ogre::Sphere& s, std::vector<Ogre::MovableObject*>& result, int mask /*= eQueryType_All*/ )
{
	m_pSphereSceneQuery->setSphere(s);
	m_pSphereSceneQuery->setQueryMask(mask);
	Ogre::SceneQueryResult& queryResults = m_pSphereSceneQuery->execute();

	auto movableList = queryResults.movables;
	for (auto iter=movableList.begin(); iter!=movableList.end(); ++iter)
		result.push_back(*iter);
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

	Ogre::Camera* cam = m_pRenderSystem->m_pMainCamera;

	const FLOAT3& camRight = cam->getRealRight();
	const FLOAT3& camUp = cam->getRealUp();
	const POS& camPos = cam->getRealPosition();
	const FLOAT3& camDir = cam->getRealDirection();

	//TODO: 硬编码设置UI位置,以后要加入UI layout
	POS newPos = camPos + camDir * 1.7f + camRight * -1.1f + camUp * -0.67f;
	m_pUISceneNode1->_setDerivedOrientation(cam->getRealOrientation());
	m_pUISceneNode1->_setDerivedPosition(newPos);
	m_pUISceneNode2->setPosition(1.2f, -0.02f, 0);
	m_pUISceneNode3->setPosition(1.76f, 0.64f, 0);
	m_pUISceneNode4->setPosition(2.17f, -0.0f, -0.01f);
}

bool World::GetTerrainIntersectPos( const FLOAT2& screenPos, POS& retPt )
{
	Ogre::Ray ray;
	m_pRenderSystem->m_pMainCamera->getCameraToViewportRay(screenPos.x, screenPos.y, &ray);

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

		const POS& pos = Ogre::StringConverter::parseVector3(strPos);
		const ORIENT& orient = Ogre::StringConverter::parseQuaternion(strOrient);
		const SCALE& scale = Ogre::StringConverter::parseVector3(strScale);

		if (bIsBuilding || bIsResource)
		{
			Object* pObject = ObjectManager::GetSingleton().CreateObject(bIsBuilding ? eObjectType_Building : eObjectType_Resource);

			if(bIsBuilding)
			{
				const STRING strBuildingName = curObjNode->first_attribute("buildingname")->value();
				Building* pBuilding = static_cast<Building*>(pObject);
				pBuilding->SetName(strBuildingName);
				pBuilding->Init(pos, orient, scale);
			}
			else
			{
				Resource* pRes = static_cast<Resource*>(pObject);
				pRes->SetMeshName(strMesh);
				pRes->SetPosition(pos);
				pRes->SetOrientation(orient);
				pRes->SetScale(scale);
				pRes->GetEntity()->setQueryFlags(eQueryType_Resource);
			}
		}
		else
		{
			//非游戏对象,不纳入逻辑管理,只渲染
			Ogre::Entity* entity = m_pRenderSystem->CreateEntityWithTangent(strMesh, m_pRenderSystem->m_pSceneMgr);
			assert(entity);

			Ogre::SceneNode* pNode = m_pRenderSystem->m_pSceneMgr->getRootSceneNode()->createChildSceneNode(pos, orient);
			pNode->setScale(scale);
			pNode->attachObject(entity);
		}	

		curObjNode = curObjNode->next_sibling();
	}
}

int World::GetPlayerUnitNum( lua_State* L )
{
	const eGameRace race = (eGameRace)m_pScriptSystem->Get_Int(-1);
	int numUnit = m_player[race]->GetUnitNum();
	m_pScriptSystem->Push_Int(numUnit);

	return 1;
}

int World::SetGlobalBBParam_Int( lua_State* L )
{
	const eGameRace race = (eGameRace)m_pScriptSystem->Get_Int(-3);
	const STRING paramName = m_pScriptSystem->Get_String(-2);
	const int iValue = m_pScriptSystem->Get_Int(-1);
	const STRING value = Ogre::StringConverter::toString(iValue);

	Kratos::aiBehaviorTreeTemplateManager::GetSingleton().GetGlobalBB(race)->SetParam(paramName, value);

	return 0;
}

int World::SetGlobalBBParam_Float( lua_State* L )
{
	const eGameRace race = (eGameRace)m_pScriptSystem->Get_Int(-3);
	const STRING paramName = m_pScriptSystem->Get_String(-2);
	const float fValue = m_pScriptSystem->Get_Float(-1);
	const STRING value = Ogre::StringConverter::toString(fValue);

	Kratos::aiBehaviorTreeTemplateManager::GetSingleton().GetGlobalBB(race)->SetParam(paramName, value);

	return 0;
}

int World::SetGlobalBBParam_Bool( lua_State* L )
{
	const eGameRace race = (eGameRace)m_pScriptSystem->Get_Int(-3);
	const STRING paramName = m_pScriptSystem->Get_String(-2);
	const bool bValue = m_pScriptSystem->Get_Bool(-1);
	const STRING value = Ogre::StringConverter::toString(bValue);

	Kratos::aiBehaviorTreeTemplateManager::GetSingleton().GetGlobalBB(race)->SetParam(paramName, value);

	return 0;
}
