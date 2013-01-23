/********************************************************************
	created:	20:1:2013   22:20
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\ObjectManager.h
	author:		maval
	
	purpose:	对象管理器,管理所有游戏对象
*********************************************************************/

#ifndef ObjectManager_h__
#define ObjectManager_h__

#include "Singleton.h"
#include "GameDefine.h"

class Object;
class Faction;
class ObjectFactory;

typedef std::unordered_map<eObjectType, ObjectFactory*> FactoryContainer;
typedef std::unordered_map<int, Object*> ObjectContainer;	//全局ID索引

class ObjectManager : public CSingleton<ObjectManager>
{
	ObjectManager();
	~ObjectManager();
	DECLEAR_SINGLETON(ObjectManager);

public:
	Object*		CreateObject(eObjectType type);
	void		DestroyAll();
	void		UpdateAll(float dt);
	Object*		GetObject(int ID) { assert(m_objects[ID]); return m_objects[ID]; }

private:
	FactoryContainer	m_factories;
	ObjectContainer		m_objects;
};

#endif // ObjectManager_h__