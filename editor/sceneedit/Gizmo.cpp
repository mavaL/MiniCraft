#include "stdafx.h"
#include "Gizmo.h"
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
