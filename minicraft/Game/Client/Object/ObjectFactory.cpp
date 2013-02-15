#include "stdafx.h"
#include "ObjectFactory.h"
#include "Unit.h"
#include "Building.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////
Object* UnitFactory::CreateInstance()
{
	return new Unit;
}

void UnitFactory::DestroyInstance( Object* pObject )
{
	assert(pObject);
	delete pObject;
}

/////////////////////////////////////////////////////////////
Object* BuildingFactory::CreateInstance()
{
	return new Building;
}

void BuildingFactory::DestroyInstance( Object* pObject )
{
	assert(pObject);
	delete pObject;
}

/////////////////////////////////////////////////////////////
Object* ResourceFactory::CreateInstance()
{
	return new Resource;
}

void ResourceFactory::DestroyInstance( Object* pObject )
{
	assert(pObject);
	delete pObject;
}
