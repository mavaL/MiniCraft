#include "stdafx.h"
#include "BattleState.h"
#include "OgreManager.h"
#include "Unit.h"
#include "Command.h"
#include "UnitState.h"




std::string CBattleState::StateName = "BattleState";

CBattleState::CBattleState()
:m_pCamera(nullptr)
,m_pSceneMgr(nullptr)
,m_pRecast(nullptr)
,m_pDetourTileCache(nullptr)
,m_pDetourCrowd(nullptr)
,m_bIsSelection(false)
,m_bCamMoveLeft(false)
,m_bCamMoveRight(false)
,m_bCamMoveUp(false)
,m_bCamMoveDown(false)
,m_pTestUnit(nullptr)
{
}

CBattleState::~CBattleState()
{
}

void CBattleState::enter()
{
	using namespace Ogre;

	m_pSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);
	m_pCamera = m_pSceneMgr->createCamera("GodViewCam");
	m_pCamera->setNearClipDistance(1.0f);
	COgreManager::GetSingleton().GetViewport()->setCamera(m_pCamera);

	m_pSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

	//45度俯角
	m_pCamera->setPosition(0, 50, 0);
	m_pCamera->lookAt(0, 0, 50);

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
	SceneNode* pHouseNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0,0,30));
	pHouseNode->attachObject(ent);
	pHouseNode->scale(3, 3, 3);
	vecNavEnt.push_back(ent);

	SceneNode* pMineNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	pMineNode->scale(0.1f, 0.1f, 0.1f);
	ent = m_pSceneMgr->createEntity("银矿脉.mesh");
	pMineNode->attachObject(ent);
	vecNavEnt.push_back(ent);

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

	//测试Unit 
	Entity* pEnt = m_pSceneMgr->createEntity("Sinbad.mesh");
	SceneNode* pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode
		("UnitNode", Vector3(10,0,10), Quaternion(Radian(Math::PI), Vector3::UNIT_Y));
	m_pTestUnit = new Unit(pEnt, pNode, m_pRecast, m_pDetourCrowd);
	m_pTestUnit->SetState(IdleState(m_pTestUnit));

	//绑定输入事件
	CInputManager& inputMgr = CInputManager::GetSingleton();
	inputMgr.BindKeyPressed(boost::bind(&CBattleState::OnInputSys_KeyPressed, this, _1));
	inputMgr.BindKeyReleased(boost::bind(&CBattleState::OnInputSys_KeyReleased, this, _1));
	inputMgr.BindMousePressed(boost::bind(&CBattleState::OnInputSys_MousePressed, this, _1, _2));
	inputMgr.BindMouseRelease(boost::bind(&CBattleState::OnInputSys_MouseReleased, this, _1, _2));
	inputMgr.BindMouseMove(boost::bind(&CBattleState::OnInputSys_MouseMove, this, _1));
	m_bQuit = false;
}

bool CBattleState::pause()
{
    return true;
}

void CBattleState::resume()
{
	
}

void CBattleState::exit()
{
	SAFE_DELETE(m_pDetourCrowd);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);

	COgreManager::GetSingleton().GetViewport()->setCamera(nullptr);
	Ogre::Root::getSingleton().destroySceneManager(m_pSceneMgr);
	
	m_pCamera = nullptr;
	m_pSceneMgr = nullptr;
}

void CBattleState::update(float timeSinceLastFrame)
{
	if(m_bQuit)
	{
		m_pParent->shutdown();
		return;
	}

	//寻路更新加速一下
	m_pDetourCrowd->updateTick(timeSinceLastFrame*3);

	//更新摄像机运动
	if(m_bCamMoveLeft)	m_pCamera->move(Ogre::Vector3(timeSinceLastFrame*40, 0, 0));
	if(m_bCamMoveRight) m_pCamera->move(Ogre::Vector3(-timeSinceLastFrame*40, 0, 0));
	if(m_bCamMoveUp)	m_pCamera->move(Ogre::Vector3(0, 0, timeSinceLastFrame*40));
	if(m_bCamMoveDown)	m_pCamera->move(Ogre::Vector3(0, 0, -timeSinceLastFrame*40));

	m_pTestUnit->Update(timeSinceLastFrame);
}

bool CBattleState::OnInputSys_MousePressed( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	if(id == OIS::MB_Left)
	{

	}
	else if (id == OIS::MB_Right)
	{
		//检测拾取射线是否与游戏区域相交
		float screenX = arg.state.X.abs / (float)arg.state.width;
		float screenY = arg.state.Y.abs / (float)arg.state.height;
		Ogre::Ray ray;
		m_pCamera->getCameraToViewportRay(screenX, screenY, &ray);
		Ogre::Plane floor(Ogre::Vector3::UNIT_Y, 0);
		auto result = ray.intersects(floor);
		if (result.first)
		{
			//检测是否目标点超出了游戏地图
			Ogre::Vector3 destPos(ray.getPoint(result.second));
			if(destPos.x < -50 || destPos.x > 50 || destPos.z < -50 || destPos.z > 50)
				return true;

			//执行移动命令
			destPos.y += 5;
			MoveCommand cmd(m_pTestUnit, destPos);
			m_pTestUnit->GiveCommand(cmd);
		}
	}        

	return true;
}

bool CBattleState::OnInputSys_MouseReleased( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	return true;
}

bool CBattleState::OnInputSys_MouseMove( const OIS::MouseEvent& arg )
{
	//获取屏幕尺寸
	Ogre::RenderWindow* pRenderWnd = COgreManager::GetSingleton().GetRenderWindow();
	int w = (int)pRenderWnd->getWidth(), h = (int)pRenderWnd->getHeight();
	//获取当前鼠标绝对位置
	int absX = arg.state.X.abs, absY = arg.state.Y.abs;

	if	(absX == w		&& arg.state.X.rel	> 0 ) m_bCamMoveRight = true;
	else if(absX == 0	&& arg.state.X.rel < 0)	m_bCamMoveLeft = true;
	if(absY == h	&& arg.state.Y.rel > 0) m_bCamMoveDown = true;
	else if(absY == 0	&& arg.state.Y.rel < 0) m_bCamMoveUp = true;

	if (absX > 0 && absX < w)
		m_bCamMoveRight = m_bCamMoveLeft = false;
	if (absY > 0 && absY < h)
		m_bCamMoveUp = m_bCamMoveDown = false;

	return true;
}

bool CBattleState::OnInputSys_KeyPressed( const OIS::KeyEvent& arg )
{
	return true;
}

bool CBattleState::OnInputSys_KeyReleased( const OIS::KeyEvent& arg )
{
	if(arg.key == OIS::KC_ESCAPE)
		m_bQuit = true;

	return true;
}

void CBattleState::_QueryCursorPos( Ogre::Vector3& retPos )
{

}
