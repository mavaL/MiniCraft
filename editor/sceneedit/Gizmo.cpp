#include "stdafx.h"
#include "Gizmo.h"
#include "EditorDefine.h"
#include "Manipulator/ManipulatorScene.h"

using namespace Ogre;


GizmoRectangle::~GizmoRectangle()
{
	DestroyRenderable();
}

void GizmoRectangle::InitRenderable(float w, float h)
{
	DestroyRenderable();

	//矩形长宽上分布点的个数
	const int ptCountOfWidth = (int)std::ceil(w * POINT_DENSITY);
	const int ptCountOfHeight = (int)std::ceil(h * POINT_DENSITY);
	const int totalCount = (ptCountOfWidth + ptCountOfHeight) * 2 + 1 - 4;

	mRenderOp.vertexData = new VertexData();

	mRenderOp.indexData = 0;
	mRenderOp.vertexData->vertexCount = totalCount;
	mRenderOp.vertexData->vertexStart = 0; 
	mRenderOp.operationType = RenderOperation::OT_LINE_STRIP; 
	mRenderOp.useIndexes = false; 
	mRenderOp.useGlobalInstancingVertexBufferIsAvailable = false;

	VertexDeclaration* decl = mRenderOp.vertexData->vertexDeclaration;
	VertexBufferBinding* bind = mRenderOp.vertexData->vertexBufferBinding;

	size_t offset = 0;
	decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
	offset += VertexElement::getTypeSize(VET_FLOAT3);
	decl->addElement(0, offset, VET_FLOAT3, VES_DIFFUSE);
	offset += VertexElement::getTypeSize(VET_FLOAT3);

	HardwareVertexBufferSharedPtr vbuf = 
		HardwareBufferManager::getSingleton().createVertexBuffer(
		offset,
		mRenderOp.vertexData->vertexCount,
		HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);

	// Bind buffer
	bind->setBinding(0, vbuf);

	//总是可见
	(const_cast<Ogre::AxisAlignedBox&>(getBoundingBox())).setInfinite();

	setMaterial("BaseWhiteNoLighting");
}

void GizmoRectangle::getWorldTransforms( Matrix4* xform ) const
{
	// return identity matrix to prevent parent transforms
	*xform = Matrix4::IDENTITY;
}

void GizmoRectangle::UpdatePosition( const Ogre::Vector3& pos, float w, float h )
{
	ManipulatorTerrain& terrain = ManipulatorSystem.GetTerrain();
	//矩形长宽上分布点的个数
	const int ptCountOfWidth = (int)std::ceil(w * POINT_DENSITY);
	const int ptCountOfHeight = (int)std::ceil(h * POINT_DENSITY);

	const float halfW = w / 2, halfH = h / 2;
	const float ptDist = 1 / POINT_DENSITY;

	HardwareVertexBufferSharedPtr vbuf = 
		mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	float* pFloat = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
	
	const float yOffset = 0.01f;
	//上边
	for (int i=0; i<ptCountOfWidth-1; ++i)
	{
		//position
		float x = pos.x - halfW + i * ptDist;
		float z = pos.z - halfH;
		float y = terrain.GetHeightAt(Ogre::Vector2(x, z));

		*pFloat++ = x;
		*pFloat++ = y + yOffset;
		*pFloat++ = z;

		//color
		*pFloat++ = 0.0f;
		*pFloat++ = 0.0f;
		*pFloat++ = 1.0f;
	}

	//右边
	for (int i=0; i<ptCountOfHeight-1; ++i)
	{
		//position
		float x = pos.x + halfW;
		float z = pos.z - halfH + i * ptDist;
		float y = terrain.GetHeightAt(Ogre::Vector2(x, z));

		*pFloat++ = x;
		*pFloat++ = y + yOffset;
		*pFloat++ = z;

		//color
		*pFloat++ = 0.0f;
		*pFloat++ = 0.0f;
		*pFloat++ = 1.0f;
	}

	//下边
	for (int i=0; i<ptCountOfWidth-1; ++i)
	{
		//position
		float x = pos.x + halfW - i * ptDist;
		float z = pos.z + halfH;
		float y = terrain.GetHeightAt(Ogre::Vector2(x, z));

		*pFloat++ = x;
		*pFloat++ = y + yOffset;
		*pFloat++ = z;

		//color
		*pFloat++ = 0.0f;
		*pFloat++ = 0.0f;
		*pFloat++ = 1.0f;
	}

	//左边
	for (int i=0; i<ptCountOfHeight-1; ++i)
	{
		//position
		float x = pos.x - halfW;
		float z = pos.z + halfH - i * ptDist;
		float y = terrain.GetHeightAt(Ogre::Vector2(x, z));

		*pFloat++ = x;
		*pFloat++ = y + yOffset;
		*pFloat++ = z;

		//color
		*pFloat++ = 0.0f;
		*pFloat++ = 0.0f;
		*pFloat++ = 1.0f;
	}

	//最后一个点,起点
	float x = pos.x - halfW;
	float z = pos.z - halfH;
	float y = terrain.GetHeightAt(Ogre::Vector2(x, z));

	*pFloat++ = x;
	*pFloat++ = y + yOffset;
	*pFloat++ = z;

	//color
	*pFloat++ = 0.0f;
	*pFloat++ = 0.0f;
	*pFloat++ = 1.0f;

	vbuf->unlock();
}

void GizmoRectangle::DestroyRenderable()
{
	SAFE_DELETE(mRenderOp.vertexData);
}

/////////////////////////////////////////////////////////////////////////////
void GizmoCircle::InitRenderable()
{

}

GizmoCircle::~GizmoCircle()
{
	DestroyRenderable();
}

/////////////////////////////////////////////////////////////////////////////
GizmoAxis::GizmoAxis()
:m_pObjGizmoNode(nullptr)
,m_pAttachNode(nullptr)
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
		m_pAxis[i] = ManipulatorSystem.m_pSceneMgr->createEntity("Arrow1m.mesh");
		m_pAxis[i]->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	}

	m_pAxis[0]->setMaterialName("RedEmissive");
	m_pAxis[1]->setMaterialName("GreenEmissive");
	m_pAxis[2]->setMaterialName("BlueEmissive");

	//设置查询掩码
	m_pAxis[0]->setQueryFlags(eQueryMask_GizmoAxisX);
	m_pAxis[1]->setQueryFlags(eQueryMask_GizmoAxisY);
	m_pAxis[2]->setQueryFlags(eQueryMask_GizmoAxisZ);
}

void GizmoAxis::Attach( Ogre::SceneNode* pNode )
{
	if (m_pAttachNode)
	{
		m_pAttachNode->removeChild(m_pObjGizmoNode);
		m_pAttachNode = nullptr;
	}
	m_pAttachNode = pNode;
	m_pAttachNode->addChild(m_pObjGizmoNode);

	m_pAttachNode->_update(true, false);

	Node* xNode = m_pObjGizmoNode->getChild("GizmoXAixsNode");
	Node* yNode = m_pObjGizmoNode->getChild("GizmoYAixsNode");
	Node* zNode = m_pObjGizmoNode->getChild("GizmoZAixsNode");

	xNode->resetToInitialState();
	yNode->resetToInitialState();
	zNode->resetToInitialState();

	Vector3 scale(0.5f, 0.5f, 3);
	//Scale
	xNode->setScale(scale);
	yNode->setScale(scale);
	zNode->setScale(scale);
	//Rotate
	xNode->yaw(Ogre::Degree(90));
	yNode->pitch(Ogre::Degree(-90));
	float offset = Math::Abs(m_pAxis[0]->getBoundingBox().getMinimum().z);
	//Translate
	xNode->translate(Vector3(0,0,offset*scale.z), Node::TS_LOCAL);
	yNode->translate(Vector3(0,0,offset*scale.z), Node::TS_LOCAL);
	zNode->translate(Vector3(0,0,offset*scale.z), Node::TS_LOCAL);
}

void GizmoAxis::Show( bool bShow )
{
	m_pObjGizmoNode->setVisible(bShow);
}

void GizmoAxis::_Destroy()
{
	m_pAxis[0] = m_pAxis[1] = m_pAxis[2] = nullptr;
}

void GizmoAxis::Reset()
{
	_Destroy();
	_Init();
}

void GizmoAxis::OnGizmoNodeReset()
{
	//Gizmo Node
	m_pObjGizmoNode = ManipulatorSystem.m_pSceneMgr->getRootSceneNode()->createChildSceneNode("ObjectGizmoNode");

	Ogre::SceneNode* pXNode = m_pObjGizmoNode->createChildSceneNode("GizmoXAixsNode");
	Ogre::SceneNode* pYNode = m_pObjGizmoNode->createChildSceneNode("GizmoYAixsNode");
	Ogre::SceneNode* pZNode = m_pObjGizmoNode->createChildSceneNode("GizmoZAixsNode");

	pXNode->setInitialState();
	pYNode->setInitialState();
	pZNode->setInitialState();

	pXNode->attachObject(m_pAxis[0]);
	pYNode->attachObject(m_pAxis[1]);
	pZNode->attachObject(m_pAxis[2]);

	m_pObjGizmoNode->setVisible(false);

	ManipulatorSystem.m_pSceneMgr->getRootSceneNode()->removeChild(m_pObjGizmoNode);

	m_pAttachNode = nullptr;
}

void GizmoAxis::HighlightAxis( bool bHighlight, eAxis axis )
{
	assert(axis != eAxis_None);
	if(bHighlight)
	{
		m_pAxis[axis]->setMaterialName("YellowEmissive");
	}
	else
	{
		switch (axis)
		{
		case 0: m_pAxis[axis]->setMaterialName("RedEmissive"); break;
		case 1: m_pAxis[axis]->setMaterialName("GreenEmissive"); break;
		case 2: m_pAxis[axis]->setMaterialName("BlueEmissive"); break;
		}
	}
}
