#include "stdafx.h"
#include "ManipulatorObject.h"
#include "../EditorDefine.h"
#include "ManipulatorScene.h"
#include "ManipulatorAction.h"
#include "Gizmo.h"
#include <OgreWireBoundingBox.h>


ManipulatorObject::ManipulatorObject()
:m_curEditMode(eEditMode_None)
,m_pSelectEntity(nullptr)
{
	ManipulatorScene::GetSingleton().AddCallback(this);
	m_pGizmoAixs = new GizmoAxis;
	m_pRaySceneQuery = ManipulatorSystem.m_pSceneMgr->createRayQuery(Ogre::Ray());
	m_pAABBSceneQuery = ManipulatorSystem.m_pSceneMgr->createAABBQuery(Ogre::AxisAlignedBox());
	m_pRaySceneQuery->setSortByDistance(true);
}

ManipulatorObject::~ManipulatorObject()
{
	ManipulatorSystem.m_pSceneMgr->destroyQuery(m_pRaySceneQuery);
	ManipulatorSystem.m_pSceneMgr->destroyQuery(m_pAABBSceneQuery);
	m_pRaySceneQuery = nullptr;
	m_pAABBSceneQuery = nullptr;
	SAFE_DELETE(m_pGizmoAixs);
	ManipulatorScene::GetSingleton().RemoveCallback(this);
}

void ManipulatorObject::OnGizmoNodeReset()
{
	m_pGizmoAixs->OnGizmoNodeReset();
}

void ManipulatorObject::OnSceneClose()
{
	m_curEditMode = eEditMode_None; 
	m_pSelectEntity = nullptr;
}

void ManipulatorObject::OnSceneNew()
{
	m_pGizmoAixs->Reset();
}

void ManipulatorObject::OnSceneOpen()
{
	m_pGizmoAixs->Reset();
}

bool ManipulatorObject::AddEntity( const Ogre::String& meshname, const Ogre::Vector3& worldPos )
{
	static int counter = 0;

	Ogre::String entName("Entity_");
	entName += Ogre::StringConverter::toString(counter++);

	Ogre::Entity* newEntity = ManipulatorSystem.m_pSceneMgr->createEntity(entName, meshname);
	if(!newEntity)
		return false;

	Ogre::SceneNode* pNode = ManipulatorSystem.m_pSceneMgr->getRootSceneNode()->createChildSceneNode(worldPos);
	pNode->attachObject(newEntity);

	//每个Entity创建一个包围盒节点
	Ogre::WireBoundingBox* aabb = new Ogre::WireBoundingBox;
	aabb->setMaterial("BaseWhiteNoLighting");
	aabb->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	Ogre::SceneNode* aabbNode = pNode->createChildSceneNode(entName);
	aabbNode->attachObject(aabb);
	aabbNode->setVisible(false);

	//设置查询掩码
	newEntity->setQueryFlags(eQueryMask_Entity);

	//激活物体移动状态
	SetCurEditMode(eEditMode_Move);
	SetSelection(newEntity);
	ManipulatorAction::GetSingleton().SetActiveAction(eActionType_ObjectEdit);
	
	return true;
}

bool ManipulatorObject::AddEntity( const Ogre::String& meshname, const Ogre::Vector2& screenPos )
{
	Ogre::Ray ray = ManipulatorSystem.m_pMainCamera->getCameraToViewportRay(screenPos.x, screenPos.y);

	Vector3 pt;
	if (ManipulatorSystem.GetTerrain().GetRayIntersectPoint(ray, pt))
		return AddEntity(meshname, pt);

	return true;
}

void ManipulatorObject::SetSelection( Ogre::Entity* pEnt )
{
	ClearSelection();
	
	assert(pEnt && pEnt->getParentNode());
	ShowEntityGizmo(pEnt, true, m_curEditMode, false);
	m_pSelectEntity = pEnt;
}

void ManipulatorObject::ShowEntityGizmo(Ogre::Entity* pEntity, bool bShow, eEditMode mode, bool bDrift/* = false*/)
{
	assert(pEntity);
	assert(mode != eEditMode_None);

	if (mode == eEditMode_Select)
	{
		Ogre::SceneNode* aabbNode = dynamic_cast<Ogre::SceneNode*>(pEntity->getParentSceneNode()->getChild(pEntity->getName()));
		aabbNode->setVisible(bShow);
		if (bShow)
		{
			//显示包围盒
			Ogre::WireBoundingBox* pAABB = GetEntityAABBGizmo(pEntity);
			if(bDrift)		//显示为红色
				pAABB->setMaterial("RedEmissive_ZCheck");
			else			//显示为绿色
				pAABB->setMaterial("GreenEmissive_ZCheck");
		}
	}
	else
	{
		//显示坐标轴
		m_pGizmoAixs->Show(bShow, mode == eEditMode_Move || mode == eEditMode_Scale);
		if (bShow)
			m_pGizmoAixs->Attach(pEntity->getParentSceneNode());
	}
}

Ogre::MovableObject* ManipulatorObject::DoRaySceneQuery( const Ogre::Ray& ray, int queryMask /*= eQueryMask_Default*/ )
{
	m_pRaySceneQuery->setRay(ray);
	m_pRaySceneQuery->setQueryMask(queryMask);
	const Ogre::RaySceneQueryResult& result = m_pRaySceneQuery->execute();

	if(result.empty())
		return nullptr;

	return result[0].movable;
}

void ManipulatorObject::DoAABBSceneQuery( const Ogre::AxisAlignedBox& aabb, int queryMask /*= 0xffffffff*/ )
{
	m_pAABBSceneQuery->setBox(aabb);
	m_pAABBSceneQuery->setQueryMask(queryMask);
	Ogre::SceneQueryResult& queryResults = m_pAABBSceneQuery->execute();

	auto movableList = queryResults.movables;
}

void ManipulatorObject::SelectionMove( const Ogre::Vector3& vecMove )
{
	assert(m_pSelectEntity);
	m_pSelectEntity->getParentSceneNode()->translate(vecMove, Ogre::Node::TS_LOCAL);
}

void ManipulatorObject::ClearSelection()
{
	if (m_pSelectEntity)
	{
		//隐藏所有gizmo
		ShowEntityGizmo(m_pSelectEntity, false, eEditMode_Select);
		ShowEntityGizmo(m_pSelectEntity, false, eEditMode_Move);
		m_pSelectEntity = nullptr;
	}
}

void ManipulatorObject::OnFrameMove( float dt )
{
	//更新选中物体的包围盒
	if (m_pSelectEntity)
	{
		Ogre::SceneNode* aabbNode = dynamic_cast<Ogre::SceneNode*>(
			m_pSelectEntity->getParentSceneNode()->getChild(m_pSelectEntity->getName()));
		Ogre::WireBoundingBox* pAABB = GetEntityAABBGizmo(m_pSelectEntity);
		pAABB->setupBoundingBox(m_pSelectEntity->getWorldBoundingBox(true));
		//避免被裁减
		(const_cast<Ogre::AxisAlignedBox&>(pAABB->getBoundingBox())).setInfinite();
		aabbNode->_updateBounds();
	}
}

Ogre::WireBoundingBox* ManipulatorObject::GetEntityAABBGizmo(Ogre::Entity* pEntity)
{
	assert(pEntity);
	Ogre::SceneNode* aabbNode = dynamic_cast<Ogre::SceneNode*>(pEntity->getParentSceneNode()->getChild(pEntity->getName()));
	return dynamic_cast<Ogre::WireBoundingBox*>(aabbNode->getAttachedObject(0));
}


