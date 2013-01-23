#include "stdafx.h"
#include "ObjectBase.h"
#include "World.h"

Object::Object()
{
	static int nextID = 0;
	m_ID = nextID++;
}

///////////////////////////////////////////////////////////////////////
RenderableObject::RenderableObject()
:Object()
,m_pEntity(nullptr)
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

void RenderableObject::DestroyRenderInstance()
{
	if (m_pSceneNode)
	{
		g_Environment.m_pSceneMgr->destroySceneNode(m_pSceneNode);
		m_pSceneNode = nullptr;
	}
	if (m_pEntity)
	{
		g_Environment.m_pSceneMgr->destroyEntity(m_pEntity);
		m_pEntity = nullptr;
	}

	m_bRenderableReady = false;
}


