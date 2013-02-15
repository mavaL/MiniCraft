/********************************************************************
	created:	20:1:2013   22:58
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Object\ObjectFactory.h
	author:		maval
	
	purpose:	所有对象工厂都在这里
*********************************************************************/

#ifndef ObjectFactory_h__
#define ObjectFactory_h__

#include "GameDefine.h"

class Object;

////////////////////////////////////////////////////////////
///工厂基类
class ObjectFactory
{
public:
	ObjectFactory() {}
	virtual ~ObjectFactory() {}

public:
	virtual eObjectType GetType() const = 0;
	virtual Object*		CreateInstance() = 0;
	virtual void		DestroyInstance(Object* pObject) = 0;
};

////////////////////////////////////////////////////////////
class UnitFactory : public ObjectFactory
{
public:
	virtual eObjectType GetType() const { return eObjectType_Unit; }
	virtual Object*		CreateInstance();
	virtual void		DestroyInstance(Object* pObject);
};

////////////////////////////////////////////////////////////
class BuildingFactory : public ObjectFactory
{
public:
	virtual eObjectType GetType() const { return eObjectType_Building; }
	virtual Object*		CreateInstance();
	virtual void		DestroyInstance(Object* pObject);
};

////////////////////////////////////////////////////////////
class ResourceFactory : public ObjectFactory
{
public:
	virtual eObjectType GetType() const { return eObjectType_Resource; }
	virtual Object*		CreateInstance();
	virtual void		DestroyInstance(Object* pObject);
};


#endif // ObjectFactory_h__