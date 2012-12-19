#include "stdafx.h"
#include "ManipulatorObject.h"
#include "../EditorDefine.h"


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

	//TODO: 暂时是硬编码测试与平板地形相交
	Ogre::Plane plane(Vector3::UNIT_Y, 0);
	auto result = ray.intersects(plane);

	if (result.first)
	{
		return AddEntity(meshname, ray.getPoint(result.second));
	}

	return true;
}
