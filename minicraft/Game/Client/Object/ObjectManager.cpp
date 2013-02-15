#include "stdafx.h"
#include "ObjectManager.h"
#include "ObjectFactory.h"
#include "ObjectBase.h"


ObjectManager::ObjectManager()
{
	//注册所有对象工厂
 	m_factories.insert(std::make_pair(eObjectType_Unit, new UnitFactory));
 	m_factories.insert(std::make_pair(eObjectType_Building, new BuildingFactory));
	m_factories.insert(std::make_pair(eObjectType_Resource, new ResourceFactory));
}

ObjectManager::~ObjectManager()
{
	for(auto iter=m_factories.begin(); iter!=m_factories.end(); ++iter)
		delete iter->second;
	m_factories.clear();

	DestroyAll();
}

Object* ObjectManager::CreateObject( eObjectType type )
{
	auto iter = m_factories.find(type);
	assert(iter != m_factories.end());

	Object* pNewInst = (iter->second)->CreateInstance();
	assert(pNewInst);

	m_objects.insert(std::make_pair(pNewInst->GetID(), pNewInst));

	return pNewInst;
}

void ObjectManager::DestroyAll()
{
	for(auto iter=m_objects.begin(); iter!=m_objects.end(); ++iter)
	{
		auto iterFactory = m_factories.find((iter->second)->GetType());
		ObjectFactory* pFactory = iterFactory->second;
		pFactory->DestroyInstance(iter->second);
	}
	m_objects.clear();
}

void ObjectManager::UpdateAll( float dt )
{
	for(auto iter=m_objects.begin(); iter!=m_objects.end(); ++iter)
	{
		Object* pObj = (iter->second);
		pObj->UpdateAllComponent(dt);
		pObj->Update(dt);
	}
}
