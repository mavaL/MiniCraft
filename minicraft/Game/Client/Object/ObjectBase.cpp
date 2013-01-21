#include "stdafx.h"
#include "ObjectBase.h"

Object::Object()
{
	static int nextID = 0;
	m_ID = nextID++;
}

///////////////////////////////////////////////////////////////////////
RenderableObject::RenderableObject()
:m_pEntity(nullptr)
,m_pSceneNode(nullptr)
,m_bRenderableReady(false)
{

}

void RenderableObject::SetPosition( const POS& pos )
{
	m_pSceneNode->_setDerivedPosition(pos);
}

void RenderableObject::SetOrientation( const ORIENT& orient )
{
	m_pSceneNode->_setDerivedOrientation(orient);
}

void RenderableObject::SetScale( const SCALE& scale )
{
	m_pSceneNode->setScale(scale);
}

const POS& RenderableObject::GetPosition() const
{
	return m_pSceneNode->_getDerivedPosition();
}

const ORIENT& RenderableObject::GetOrientation() const
{
	return m_pSceneNode->_getDerivedOrientation();
}

const SCALE& RenderableObject::GetScale() const
{
	return m_pSceneNode->_getDerivedScale();
}


