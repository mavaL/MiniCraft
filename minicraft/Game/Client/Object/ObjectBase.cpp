#include "stdafx.h"
#include "ObjectBase.h"
#include "World.h"

Object::Object()
{
	static int nextID = 0;
	m_ID = nextID++;
}

///////////////////////////////////////////////////////////////////////
IMPL_PARAM_COMMAND(RenderableObject, Position, Vector3)
IMPL_PARAM_COMMAND(RenderableObject, Orientation, Quaternion)

RenderableObject::RenderableObject()
:Object()
,m_pEntity(nullptr)
,m_pSceneNode(nullptr)
,m_bRenderableReady(false)
{
	if (createParamDictionary("RenderableObject"))
	{
		Ogre::ParamDictionary* dict = getParamDictionary();
		dict->addParameter(Ogre::ParameterDef("position", "position of the object", Ogre::PT_VECTOR3), &m_sCmdPosition);
	}
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

void RenderableObject::CreateRenderInstance( const STRING& meshname )
{
	DestroyRenderInstance();

	m_pSceneNode = g_Environment.m_pSceneMgr->getRootSceneNode()->createChildSceneNode();

	//Éè¶¨Ãû×Ö
	STRING entName = GetNamePrefix();	
	entName += Ogre::StringConverter::toString(GetID());

	m_pEntity = g_Environment.m_pSceneMgr->createEntity(entName, meshname);
	m_pEntity->setQueryFlags(eQueryType_SelectableObject);
	m_pSceneNode->attachObject(m_pEntity);

	m_bRenderableReady = true;
}


