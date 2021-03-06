#include "stdafx.h"
#include "ObjectBase.h"
#include "World.h"
#include "OgreManager.h"

Object::Object()
{
	static int nextID = 0;
	m_ID = nextID++;
}
Object::~Object()
{
	//销毁所有组件
	//TODO: 回收机制,降低内存碎片
	for(auto iter=m_components.begin(); iter!=m_components.end(); ++iter)
		delete iter->second;
	m_components.clear();
}
void Object::AddComponent( eComponentType type, Component* pCo )
{
	assert(m_components.find(type) == m_components.end() && "Component of this type had already added!");
	m_components.insert(std::make_pair(type, pCo));
}

Component* Object::GetComponent( eComponentType type )
{
	auto iter = m_components.find(type);
	assert(iter != m_components.end() && "There is no component of this type in this object!");
	return iter->second;
}

void Object::UpdateAllComponent( float dt )
{
	for(auto iter=m_components.begin(); iter!=m_components.end(); ++iter)
		iter->second->Update(dt);
}

void Object::RemoveComponent( eComponentType type )
{
	auto iter = m_components.find(type);
	assert(iter != m_components.end() && "There is no component of this type in this object!");
	m_components.erase(iter);
}

///////////////////////////////////////////////////////////////////////
IMPL_PARAM_COMMAND_STR(RenderableObject, MeshName)

RenderableObject::RenderableObject()
:Object()
,m_pEntity(nullptr)
,m_pSceneNode(nullptr)
,m_bRenderableReady(false)
,m_meshname("")
{
}

bool RenderableObject::InitParamDict(const STRING& name)
{
	STRING dictName("ObjectDict_");
	dictName += name;

	if (createParamDictionary(dictName))
	{
		Ogre::ParamDictionary* dict = getParamDictionary();
		dict->addParameter(Ogre::ParameterDef("meshname", "mesh file name of the object", Ogre::PT_STRING), &m_sCmdMeshName);
		return true;
	}
	return false;
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
		RenderManager.m_pSceneMgr->destroySceneNode(m_pSceneNode);
		m_pSceneNode = nullptr;
	}
	if (m_pEntity)
	{
		RenderManager.m_pSceneMgr->destroyEntity(m_pEntity);
		m_pEntity = nullptr;
	}

	m_bRenderableReady = false;
}

void RenderableObject::CreateRenderInstance()
{
	assert(!m_meshname.empty());

	DestroyRenderInstance();

	Ogre::SceneManager* sm = RenderManager.m_pSceneMgr;
	m_pSceneNode = sm->getRootSceneNode()->createChildSceneNode();
	m_pEntity = RenderManager.CreateEntityWithTangent(m_meshname, sm);
	m_pSceneNode->attachObject(m_pEntity);

	m_bRenderableReady = true;
}

void RenderableObject::SetMeshName( const STRING& meshname )
{
	m_meshname = meshname;
	CreateRenderInstance();
}




