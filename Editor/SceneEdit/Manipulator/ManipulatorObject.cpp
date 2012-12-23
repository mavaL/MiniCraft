#include "stdafx.h"
#include "ManipulatorObject.h"
#include "../EditorDefine.h"
#include "ManipulatorScene.h"


bool ManipulatorObject::AddEntity( const Ogre::String& meshname, const Ogre::Vector3& worldPos )
{
	Ogre::SceneManager* pSceneMgr = Ogre::Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME);
	Ogre::Entity* newEntity = pSceneMgr->createEntity(meshname);
	if(!newEntity)
		return false;

	Ogre::SceneNode* pNode = pSceneMgr->getRootSceneNode()->createChildSceneNode(worldPos);
	pNode->attachObject(newEntity);
	pNode->setScale(0.1f, 0.1f, 0.1f);

	return true;
}

bool ManipulatorObject::AddEntity( const Ogre::String& meshname, const Ogre::Vector2& screenPos )
{
	//TODO: 修改获取OGRE常用对象的方式
	Ogre::Camera* pCam = Ogre::Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->getCamera(MAIN_CAMERA_NAME);
	assert(pCam);

	Ogre::Ray ray = pCam->getCameraToViewportRay(screenPos.x, screenPos.y);

	Vector3 pt;
	if (ManipulatorSystem.GetTerrain().GetRayIntersectPoint(ray, pt))
		return AddEntity(meshname, pt);

	return true;
}
