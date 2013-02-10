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
IMPL_PARAM_COMMAND(RenderableObject, Scale, Vector3)
IMPL_PARAM_COMMAND_STR(RenderableObject, MeshName)

RenderableObject::RenderableObject()
:Object()
,m_pEntity(nullptr)
,m_pSceneNode(nullptr)
,m_bRenderableReady(false)
,m_meshname("")
{
	if (createParamDictionary("RenderableObject"))
	{
		Ogre::ParamDictionary* dict = getParamDictionary();
		dict->addParameter(Ogre::ParameterDef("position", "position of the object", Ogre::PT_VECTOR3), &m_sCmdPosition);
		dict->addParameter(Ogre::ParameterDef("orientation", "orientation of the object", Ogre::PT_QUATERNION), &m_sCmdOrientation);
		dict->addParameter(Ogre::ParameterDef("scale", "scale of the object", Ogre::PT_VECTOR3), &m_sCmdScale);
		dict->addParameter(Ogre::ParameterDef("meshname", "mesh file name of the object", Ogre::PT_STRING), &m_sCmdMeshName);
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

void RenderableObject::CreateRenderInstance()
{
	assert(!m_meshname.empty());

	DestroyRenderInstance();

	m_pSceneNode = g_Environment.m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pEntity = g_Environment.m_pSceneMgr->createEntity(m_meshname);
	m_pEntity->setQueryFlags(eQueryType_SelectableObject);
	m_pSceneNode->attachObject(m_pEntity);

	//渲染对象与逻辑对象绑定,方便取出通信
	m_pEntity->setUserAny(Ogre::Any(static_cast<Object*>(this)));

	m_bRenderableReady = true;
}

void RenderableObject::SetMeshName( const STRING& meshname )
{
	m_meshname = meshname;
	CreateRenderInstance();
}


