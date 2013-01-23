#include "stdafx.h"
#include "ObjectFactory.h"
#include "Faction.h"
#include "Unit.h"
#include "Building.h"


/////////////////////////////////////////////////////////////
Object* FactionFactory::CreateInstance()
{
	return new Faction;
}

void FactionFactory::DestroyInstance( Object* pObject )
{
	assert(pObject);
	delete pObject;
}

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
