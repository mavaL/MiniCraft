#include "stdafx.h"
#include "GameDefine.h"
#include <SdkCameraMan.h>
#include "BattleState.h"
#include "OgreManager.h"
#include "Unit.h"
#include "Command.h"
#include "UnitState.h"
#include "World.h"
#include "Building.h"
#include "Resource.h"


std::string CBattleState::StateName = "BattleState";

CBattleState::CBattleState()
:m_bCamMoveLeft(false)
,m_bCamMoveRight(false)
,m_bCamMoveUp(false)
,m_bCamMoveDown(false)
,m_bLBDown(false)
,m_LBDownPos(-1, -1, -1)
,m_LBDownScreenPos(-1, -1)
,m_pSelectionQuad(nullptr)
,m_pQuadNode(nullptr)
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

	//TODO: 测试代码


// 	Unit* pTestUnit = world.CreateUnit(/*Ogre::Vector3(10, 0, -10)*/Ogre::Vector3::ZERO);
// 	pTestUnit->SetState(eUnitState_Idle);
// 
// // 	for(int i=0; i<9; ++i)
// // 	{
// // 		const Ogre::Vector3 randPos = world.GetRandomPositionOnNavmesh();
// // 		Unit* pTestUnit = world.CreateUnit(randPos);
// // 		pTestUnit->SetState(eUnitState_Idle);
// // 	}

	m_pSelectionQuad = new Ogre::Rectangle2D(true);
	m_pSelectionQuad->setMaterial("SelectionQuad");
	(const_cast<Ogre::AxisAlignedBox&>(m_pSelectionQuad->getBoundingBox())).setInfinite();
	m_pQuadNode = g_Environment.m_pSceneMgr->getRootSceneNode()->createChildSceneNode("SelectionQuadNode");
	m_pQuadNode->attachObject(m_pSelectionQuad);
	m_pQuadNode->setVisible(false);
	
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
	SAFE_DELETE(m_pSelectionQuad);
	m_pQuadNode = nullptr;
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

	world.UpdateConsoleUITransform(timeSinceLastFrame);
}

bool CBattleState::OnInputSys_MousePressed( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	World& world = World::GetSingleton();

	float screenX = arg.state.X.abs / (float)arg.state.width;
	float screenY = arg.state.Y.abs / (float)arg.state.height;
	Ogre::Ray ray;
	world.GetCamera()->getCameraToViewportRay(screenX, screenY, &ray);

	Ogre::Plane floor(Ogre::Vector3::UNIT_Y, 0);
	auto result = ray.intersects(floor);
	//检测拾取射线是否与游戏区域相交
	if (!result.first)
		return true;

	if(id == OIS::MB_Left)
	{
		//坐标空间转换
		float x = screenX * 2 - 1;
		float y = 1 - 2 * screenY;

		m_pSelectionQuad->setCorners(x, y, x, y, false);
		m_pQuadNode->setVisible(true);

		m_LBDownScreenPos = Ogre::Vector2(screenX, screenY);
		m_LBDownPos = ray.getPoint(result.second);
		m_bLBDown = true;
	}
// 	else if (id == OIS::MB_Right)
// 	{
// 		const SelectedContainer& selectedUnits = world.GetSelectedObjects();
// 		for (size_t i=0; i<selectedUnits.size(); ++i)
// 		{
// 			CommandBase* pCmd = _ComputeCommand(selectedUnits[i], ray.getPoint(result.second));
// 			if(pCmd)
// 				selectedUnits[i]->GiveCommand(*pCmd);
// 			delete pCmd;
// 		}
// 	}        

	return true;
}

bool CBattleState::OnInputSys_MouseReleased( const OIS::MouseEvent& arg, OIS::MouseButtonID id )
{
	if (id == OIS::MB_Left)
	{
		World& world = World::GetSingleton();
		world.ClearSelectedState();
		m_pQuadNode->setVisible(false);

		float screenX = arg.state.X.abs / (float)arg.state.width;
		float screenY = arg.state.Y.abs / (float)arg.state.height;
		Ogre::Ray ray;
		world.GetCamera()->getCameraToViewportRay(screenX, screenY, &ray);

		Ogre::Plane floor(Ogre::Vector3::UNIT_Y, 0);
		auto result = ray.intersects(floor);
		//检测拾取射线是否与游戏区域相交
		if (!result.first)
		{
			m_bLBDown = false;
			return true;
		}
		
		std::vector<Ogre::MovableObject*> vecResult;
		//鼠标拖动小于阕值时,进行射线查询,使得结果更准确
		Ogre::Vector3 mouseUpPos(ray.getPoint(result.second));
		if(mouseUpPos.positionEquals(m_LBDownPos, 0.1f))
		{
			Ogre::MovableObject* pObject = world.GetRaySceneQueryResult(ray, eQueryType_SelectableObject);
			if (pObject)
				vecResult.push_back(pObject);
		}
		else
		{
			//建立查询包围盒
			Ogre::AxisAlignedBox aabb;
			aabb.setMinimum(std::min(m_LBDownPos.x, mouseUpPos.x), std::min(m_LBDownPos.y, mouseUpPos.y), 
				std::min(m_LBDownPos.z, mouseUpPos.z));
			aabb.setMaximum(std::max(m_LBDownPos.x, mouseUpPos.x), std::max(m_LBDownPos.y + 100, mouseUpPos.y + 100),
				std::max(m_LBDownPos.z, mouseUpPos.z));

			world.GetAABBSceneQueryResult(aabb, vecResult, eQueryType_SelectableObject);
		}

		//单位设置为选中状态
		for(size_t i=0; i<vecResult.size(); ++i)
		{
			Ogre::Entity* pEntity = dynamic_cast<Ogre::Entity*>(vecResult[i]);
			int unitID = _GetIDFromName(pEntity);
			world.SetObjectSelected(unitID);
		}
	}

	return true;
}

bool CBattleState::OnInputSys_MouseMove( const OIS::MouseEvent& arg )
{

	if (World::GetSingleton().IsFreeCameraEnabled())
		World::GetSingleton().GetCameraMan()->injectMouseMove(arg);

	if(m_bLBDown)
	{
		Ogre::Vector2 newScreenPos((float)arg.state.X.abs/arg.state.width, (float)arg.state.Y.abs/arg.state.height);
		float left		= std::min(m_LBDownScreenPos.x, newScreenPos.x);
		float bottom	= std::max(m_LBDownScreenPos.y, newScreenPos.y);
		float right		= std::max(m_LBDownScreenPos.x, newScreenPos.x);
		float top		= std::min(m_LBDownScreenPos.y, newScreenPos.y);

		left = 2 * left - 1;
		right = 2 * right - 1;
		bottom = 1 - 2 * bottom;
		top	= 1 - 2 * top;

		m_pSelectionQuad->setCorners(left, top, right, bottom, false);
	}

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

// 	//检测是否目标点超出了游戏地图
// 	//TODO:后面很有可能加入Map类,相关数据和操作封装进去
// 	if(targetPos.x < -50 || targetPos.x > 50 || targetPos.z < -50 || targetPos.z > 50)
// 		return nullptr;
// 
// 	static Ogre::Vector3 goldArea = World::GetSingleton().GetResAABB().getHalfSize();
// 	if (targetPos.x >= -goldArea.x && 
// 		targetPos.x <= goldArea.x &&
// 		targetPos.y >= -goldArea.z &&
// 		targetPos.y <= goldArea.z)
// 	{
// 		//采集资源
// 		pCmd = new HarvestCommand(pUnit);
// 	}
// 	else
// 	{
// 		//将目标点调整至有效
// 		Ogre::Vector3 adjustPos(targetPos);
// 		World::GetSingleton().ClampPosToNavMesh(adjustPos);
// 		//执行移动命令
// 		pCmd = new MoveCommand(pUnit, adjustPos);
// 	}

	return pCmd;
}

int CBattleState::_GetIDFromName( Ogre::Entity* pEntity ) const
{
	static int UNIT_NAME_PREFIX_LEN = strlen(Unit::sNamePrefix.c_str());
	static int BUILDING_NAME_PREFIX_LEN = strlen(Building::sNamePrefix.c_str());
	static int RESOURCE_NAME_PREFIX_LEN = strlen(Resource::sNamePrefix.c_str());

	STRING strID;
	const STRING& entName = pEntity->getName();
	if(entName.substr(0, UNIT_NAME_PREFIX_LEN) == Unit::sNamePrefix)
		strID = entName.substr(UNIT_NAME_PREFIX_LEN);
	else if(entName.substr(0, BUILDING_NAME_PREFIX_LEN) == Building::sNamePrefix)
		strID = entName.substr(BUILDING_NAME_PREFIX_LEN);
	else if(entName.substr(0, RESOURCE_NAME_PREFIX_LEN) == Resource::sNamePrefix)
		strID = entName.substr(RESOURCE_NAME_PREFIX_LEN);

	return Ogre::StringConverter::parseInt(strID, -1);
}
