#ifndef WORLD_H
#define WORLD_H

#include "Singleton.h"



namespace OgreBites
{
	class SdkCameraMan;
}


class Unit;
class OgreRecast;
class OgreDetourCrowd;
/************************************************************************/
/*								世界管理器                                */
/************************************************************************/

class World : public CSingleton<World>
{
	World();
	~World() {}
	DECLEAR_SINGLETON(World);
public:
	void	Init();
	void	Update(float dt);
	void	Shutdown();

	Ogre::Camera*	GetCamera()	{ return m_pCamera; }
	Unit*			CreateUnit(const Ogre::Vector3& pos);
	Unit*			GetUnitFromID(int ID);
	const Ogre::AxisAlignedBox&	GetResAABB() const { return m_pGold->getWorldBoundingBox(); }
	Ogre::Vector3	GetRandomPositionOnNavmesh();

	void			EnableFreeCamera(bool bEnable);
	bool			IsFreeCameraEnabled() { return m_bFreeCamMode; }
	OgreBites::SdkCameraMan*	GetCameraMan()	{ return m_cameraMan; }

	//尝试调整世界坐标在有效的NavMesh上
	bool			ClampPosToNavMesh(Ogre::Vector3& wPos);

private:
	std::vector<Unit*>			m_vecUnits;
	Ogre::SceneManager*			m_pSceneMgr;
	Ogre::Camera*				m_pCamera;
	Ogre::Entity*				m_pGold;	
	//Recast寻路库
	OgreRecast*					m_pRecast;
	OgreDetourTileCache*		m_pDetourTileCache;
	OgreDetourCrowd*			m_pDetourCrowd;

	//调试观察用第三人称摄像机
	OgreBites::SdkCameraMan*	m_cameraMan;
	bool						m_bFreeCamMode;
};


#endif