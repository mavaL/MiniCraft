#include "stdafx.h"
#include "Building.h"
#include "World.h"

const STRING Building::BUILDING_NAME_PREFIX	=	"EntBuilding";

void Building::CreateRenderInstance( const STRING& meshname )
{
	DestroyRenderInstance();

	m_pSceneNode = g_Environment.m_pSceneMgr->getRootSceneNode()->createChildSceneNode();

	//Éè¶¨Ãû×Ö
	STRING entName(BUILDING_NAME_PREFIX);
	entName += Ogre::StringConverter::toString(GetID());

	m_pEntity = g_Environment.m_pSceneMgr->createEntity(entName, meshname);
	m_pEntity->setQueryFlags(eQueryType_SelectableObject);
	m_pSceneNode->attachObject(m_pEntity);

	m_bRenderableReady = true;
}
