#ifndef WORLD_H
#define WORLD_H

#include "Singleton.h"


enum eQueryType
{
	eQueryType_Default	=	1<<0,
	eQueryType_WorldGeometry	=	1<<1,
	eQueryType_Unit		=	1<<2
};

namespace OgreBites
{
	class SdkCameraMan;
}

class Unit;
class OgreRecast;
class OgreDetourCrowd;

typedef	std::vector<Unit*>	UnitContainer;
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

	//返回AABB场景查询到的物体
	void			GetAABBSceneQueryResult(const Ogre::AxisAlignedBox& box, 
		std::vector<Ogre::MovableObject*>& result, int queryMask = 0xffffffff);
	//返回射线场景查询到的第1个物体
	Ogre::MovableObject*	GetRaySceneQueryResult(const Ogre::Ray& ray, int queryMask = 0xffffffff);

	//尝试调整世界坐标在有效的NavMesh上
	bool			ClampPosToNavMesh(Ogre::Vector3& wPos);

	//设置单位为选中状态
	void			SetUnitSelected(int ID);
	//清除所有选中状态
	void			ClearAllUnitSelected();

	const UnitContainer&	GetAllUnitSelected() { return m_vecSelectUnis; }

private:
	UnitContainer				m_vecUnits;			//场景中的所有单位
	UnitContainer				m_vecSelectUnis;	//所有选中单位
	Ogre::SceneManager*			m_pSceneMgr;
	Ogre::Camera*				m_pCamera;
	Ogre::Entity*				m_pGold;
	Ogre::AxisAlignedBoxSceneQuery*		m_pSceneQuery;
	Ogre::RaySceneQuery*		m_pRaySceneQuery;

	//Recast寻路库
	OgreRecast*					m_pRecast;
	OgreDetourTileCache*		m_pDetourTileCache;
	OgreDetourCrowd*			m_pDetourCrowd;

	//调试观察用第三人称摄像机
	OgreBites::SdkCameraMan*	m_cameraMan;
	bool						m_bFreeCamMode;
};


#endif