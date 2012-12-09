#include "stdafx.h"
#include "GameDefine.h"
#include "BattleState.h"
#include "OgreManager.h"
#include "Unit.h"
#include "Command.h"
#include "UnitState.h"
#include "World.h"
#include <SdkCameraMan.h>




std::string CBattleState::StateName = "BattleState";

CBattleState::CBattleState()
:m_bCamMoveLeft(false)
,m_bCamMoveRight(false)
,m_bCamMoveUp(false)
,m_bCamMoveDown(false)
{
}

CBattleState::~CBattleState()
{
}

void CBattleState::enter()
{
	using namespace Ogre;

	World& world = World::GetSingleton();
	world.Init();

	//创建测试单位
	Unit* pTestUnit = world.CreateUnit(Ogre::Vector3(10, 0, -10));
	pTestUnit->SetState(eUnitState_Idle);

	for(int i=0; i<9; ++i)
	{
		const Ogre::Vector3 randPos = world.GetRandomPositionOnNavmesh();
		Unit* pTestUnit = world.CreateUnit(randPos);
		pTestUnit->SetState(eUnitState_Idle);
	}

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
	World::GetSingleton().Shutdown();
}

void CBattleState::update(float timeSinceLastFrame)
{
	if(m_bQuit)
	{
		m_pParent->shutdown();
		return;
	}

	World& world = World::GetSingleton();
	world.Update(timeSinceLastFrame);

	if(world.IsFreeCameraEnabled())
	{
		Ogre::FrameEvent evt;
		evt.timeSinceLastFrame = timeSinceLastFrame;
		world.GetCameraMan()->frameRenderingQueued(evt);
	}
	else
	{
		Ogre::Camera* pCam = world.GetCamera();
		//更新摄像机运动
		if(m_bCamMoveLeft)	pCam->move(Ogre::Vector3(timeSinceLastFrame*40, 0, 0));
		if(m_bCamMoveRight) pCam->move(Ogre::Vector3(-timeSinceLastFrame*40, 0, 0));
		if(m_bCamMoveUp)	pCam->move(Ogre::Vector3(0, 0, timeSinceLastFrame*40));
		if(m_bCamMoveDown)	pCam->move(Ogre::Vector3(0, 0, -timeSinceLastFrame*40));
	}
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
		World::GetSingleton().GetCamera()->getCameraToViewportRay(screenX, screenY, &ray);
		Ogre::Plane floor(Ogre::Vector3::UNIT_Y, 0);
		auto result = ray.intersects(floor);
		if (result.first)
		{
			Unit* pUnit = World::GetSingleton().GetUnitFromID(0);
			CommandBase* pCmd = _ComputeCommand(pUnit, ray.getPoint(result.second));
			if(pCmd)
				pUnit->GiveCommand(*pCmd);
			delete pCmd;
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

	if (World::GetSingleton().IsFreeCameraEnabled())
		World::GetSingleton().GetCameraMan()->injectMouseMove(arg);

	//获取屏幕尺寸
	Ogre::RenderWindow* pRenderWnd = COgreManager::GetSingleton().GetRenderWindow();
	int w = (int)pRenderWnd->getWidth(), h = (int)pRenderWnd->getHeight();
	//获取当前鼠标绝对位置
	int absX = arg.state.X.abs, absY = arg.state.Y.abs;

	if		(absX == w	&& arg.state.X.rel	> 0 ) m_bCamMoveRight = true;
	else if	(absX == 0	&& arg.state.X.rel < 0)	m_bCamMoveLeft = true;
	if		(absY == h	&& arg.state.Y.rel > 0) m_bCamMoveDown = true;
	else if	(absY == 0	&& arg.state.Y.rel < 0) m_bCamMoveUp = true;

	if (absX > 0 && absX < w)
		m_bCamMoveRight = m_bCamMoveLeft = false;
	if (absY > 0 && absY < h)
		m_bCamMoveUp = m_bCamMoveDown = false;

	return true;
}

bool CBattleState::OnInputSys_KeyPressed( const OIS::KeyEvent& arg )
{
	if (World::GetSingleton().IsFreeCameraEnabled())
		World::GetSingleton().GetCameraMan()->injectKeyDown(arg);

	return true;
}

bool CBattleState::OnInputSys_KeyReleased( const OIS::KeyEvent& arg )
{
	if(arg.key == OIS::KC_ESCAPE)
		m_bQuit = true;
	else if(arg.key == OIS::KC_F1)
		World::GetSingleton().EnableFreeCamera(!World::GetSingleton().IsFreeCameraEnabled());
	else if (World::GetSingleton().IsFreeCameraEnabled())
		World::GetSingleton().GetCameraMan()->injectKeyUp(arg);

	return true;
}

CommandBase* CBattleState::_ComputeCommand( Unit* pUnit, const Ogre::Vector3& targetPos )
{
	CommandBase* pCmd = nullptr;

	//检测是否目标点超出了游戏地图
	//TODO:后面很有可能加入Map类,相关数据和操作封装进去
	if(targetPos.x < -50 || targetPos.x > 50 || targetPos.z < -50 || targetPos.z > 50)
		return nullptr;

	static Ogre::Vector3 goldArea = World::GetSingleton().GetResAABB().getHalfSize();
	if (targetPos.x >= -goldArea.x && 
		targetPos.x <= goldArea.x &&
		targetPos.y >= -goldArea.z &&
		targetPos.y <= goldArea.z)
	{
		//采集资源
		pCmd = new HarvestCommand(pUnit);
	}
	else
	{
		//将目标点调整至有效
		Ogre::Vector3 adjustPos(targetPos);
		World::GetSingleton().ClampPosToNavMesh(adjustPos);
		//执行移动命令
		pCmd = new MoveCommand(pUnit, adjustPos);
	}

	return pCmd;
}
