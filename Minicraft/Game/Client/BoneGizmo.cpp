#include "stdafx.h"
#include "BoneGizmo.h"
#include "OgreManager.h"
#include "KratosPrerequisites.h"

using namespace Ogre;

GizmoAxis::GizmoAxis()
:m_pObjGizmoNode(nullptr)
,m_pAttachNode(nullptr)
,m_pXAxis(nullptr)
,m_pYAxis(nullptr)
,m_pZAxis(nullptr)
{
	_Init();
}

GizmoAxis::~GizmoAxis()
{
	_Destroy();
}

void GizmoAxis::_Init()
{
	for (int i=0; i<3; ++i)
	{
		m_pAxisMove[i] = RenderManager.m_pSceneMgr->createEntity("Arrow1m.mesh");
		m_pAxisMove[i]->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
		m_pAxisMove[i]->setCastShadows(false);
	}

	m_pAxisMove[0]->setMaterialName("RedEmissive");
	m_pAxisMove[1]->setMaterialName("GreenEmissive");
	m_pAxisMove[2]->setMaterialName("BlueEmissive");

	//Gizmo Node
	m_pObjGizmoNode = RenderManager.m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pObjGizmoNode->setInheritScale(false);

	//×ø±êÖá
	m_pXAxis = m_pObjGizmoNode->createChildSceneNode();
	m_pYAxis = m_pObjGizmoNode->createChildSceneNode();
	m_pZAxis = m_pObjGizmoNode->createChildSceneNode();

	m_pXAxis->setInitialState();
	m_pYAxis->setInitialState();
	m_pZAxis->setInitialState();

	m_pXAxis->attachObject(m_pAxisMove[0]);
	m_pYAxis->attachObject(m_pAxisMove[1]);
	m_pZAxis->attachObject(m_pAxisMove[2]);

	m_pObjGizmoNode->setVisible(false);
}

void GizmoAxis::Attach( Ogre::Node* pNode )
{
	assert(pNode);
	m_pAttachNode = pNode;

	m_pXAxis->resetToInitialState();
	m_pYAxis->resetToInitialState();
	m_pZAxis->resetToInitialState();

	Vector3 scale(0.02f, 0.02f, 0.12f);
	//Scale
	m_pXAxis->setScale(scale);
	m_pYAxis->setScale(scale);
	m_pZAxis->setScale(scale);
	//Rotate
	m_pXAxis->yaw(Ogre::Degree(90));
	m_pYAxis->pitch(Ogre::Degree(-90));
	float offset = Math::Abs(m_pAxisMove[0]->getBoundingBox().getMinimum().z);
	//Translate
	m_pXAxis->translate(Vector3(0,0,offset*scale.z), Node::TS_LOCAL);
	m_pYAxis->translate(Vector3(0,0,offset*scale.z), Node::TS_LOCAL);
	m_pZAxis->translate(Vector3(0,0,offset*scale.z), Node::TS_LOCAL);
}

void GizmoAxis::Detach()
{
	m_pAttachNode = nullptr;
}

void GizmoAxis::Show( bool bShow )
{
	m_pObjGizmoNode->setVisible(bShow);
}

void GizmoAxis::_Destroy()
{
	SceneManager* sm = RenderManager.m_pSceneMgr; 
	sm->destroyEntity(m_pAxisMove[0]);
	sm->destroyEntity(m_pAxisMove[1]);
	sm->destroyEntity(m_pAxisMove[2]);
	m_pAxisMove[0] = m_pAxisMove[1] = m_pAxisMove[2] = nullptr;
}

void GizmoAxis::Update()
{
	if (m_pAttachNode)
	{
		m_pObjGizmoNode->setScale(SCALE(m_pAttachNode->_getDerivedScale()));
		m_pObjGizmoNode->_setDerivedOrientation(m_pAttachNode->_getDerivedOrientation());
		m_pObjGizmoNode->_setDerivedPosition(m_pAttachNode->_getDerivedPosition());
	}
}
