#include "stdafx.h"
#include "BattleState.h"
#include <SdkCameraMan.h>
#include "OgreManager.h"
#include "OgreRecast.h"
#include "OgreDetourTileCache.h"
#include "OgreDetourCrowd.h"
#include "TestCharacter.h"




std::string CBattleState::StateName = "BattleState";

CBattleState::CBattleState()
:m_pCamera(nullptr)
,m_pSceneMgr(nullptr)
,m_pCameraMan(nullptr)
,m_pRecast(nullptr)
,m_pDetourTileCache(nullptr)
,m_pDetourCrowd(nullptr)
,m_pCharacter(nullptr)
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
	m_pCamera->setPosition(0, 100, 0);
	m_pCamera->lookAt(0, 0, -100);
	m_pCameraMan = new OgreBites::SdkCameraMan(m_pCamera);
	m_pCameraMan->setStyle(OgreBites::CS_FREELOOK);
	m_pCameraMan->setTopSpeed(50);

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

	m_pRecast = new OgreRecast(m_pSceneMgr);
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
	m_pCharacter = new TestCharacter("TestAgent", m_pSceneMgr, m_pDetourCrowd, Vector3(20,0,20));
	m_pCharacter->updateDestination(Vector3(-15,0,-15));

	int ret = m_pRecast->FindPath(Vector3(20,0,20), Vector3(-15,0,-15), 1, 1) ;
	if( ret >= 0 )
		m_pRecast->CreateRecastPathLine(1) ; // Draw a line showing path at specified slot

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
	SAFE_DELETE(m_pCharacter);
	SAFE_DELETE(m_pDetourCrowd);
	SAFE_DELETE(m_pDetourTileCache);
	SAFE_DELETE(m_pRecast);

	SAFE_DELETE(m_pCameraMan);
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

	Ogre::FrameEvent evt;
	evt.timeSinceLastFrame = 0.03f;
	m_pCameraMan->frameRenderingQueued(evt);

	m_pDetourCrowd->updateTick(timeSinceLastFrame);
	m_pCharacter->update(timeSinceLastFrame);
}

bool CBattleState::OnInputSys_MousePressed( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	if(id == OIS::MB_Left)
	{

	}
	else
	{
		m_pCameraMan->injectMouseDown(arg, id);
	}

	return true;
}

bool CBattleState::OnInputSys_MouseReleased( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	m_pCameraMan->injectMouseUp(arg, id);
	return true;
}

bool CBattleState::OnInputSys_MouseMove( const OIS::MouseEvent& arg )
{
	m_pCameraMan->injectMouseMove(arg);
	return true;
}

bool CBattleState::OnInputSys_KeyPressed( const OIS::KeyEvent& arg )
{
	m_pCameraMan->injectKeyDown(arg);
	return true;
}

bool CBattleState::OnInputSys_KeyReleased( const OIS::KeyEvent& arg )
{
	if(arg.key == OIS::KC_ESCAPE)
		m_bQuit = true;
	else
		m_pCameraMan->injectKeyUp(arg);

	return true;
}

void CBattleState::_QueryCursorPos( Ogre::Vector3& retPos )
{

}
