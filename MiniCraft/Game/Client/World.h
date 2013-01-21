#ifndef WORLD_H
#define WORLD_H

#include "Singleton.h"

class Object;
class OgreRecast;
class OgreDetourCrowd;

//常用成员全局环境
struct SGlobalEnvironment 
{
	SGlobalEnvironment() { Reset(); }
	void	Reset() { m_pSceneMgr=nullptr; m_pRecast=nullptr; m_pCrowd=nullptr; } 

	Ogre::SceneManager* m_pSceneMgr;
	OgreRecast*			m_pRecast;
	OgreDetourCrowd*	m_pCrowd;
};

extern SGlobalEnvironment	g_Environment;

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

typedef	std::vector<Object*>	SelectedContainer;
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
	void	LoadTerrain(rapidxml::xml_node<>* XMLNode);

	Ogre::Camera*	GetCamera()	{ return m_pCamera; }
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
	//设置对象为选中状态
	void			SetObjectSelected(int ID);
	//清除所有选中状态
	void			ClearSelectedState();

	const SelectedContainer&	GetSelectedObjects() { return m_vecSelectUnis; }
	//将给定世界坐标通过射线投射夹持在地形上
	void ClampToTerrain(Ogre::Vector3& pos);

private:
	SelectedContainer			m_vecSelectUnis;	//所有选中单位
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

	Ogre::TerrainGroup*			m_terrainGroup;
	Ogre::TerrainGlobalOptions*	m_terrainOption;
	Ogre::Terrain*				m_pTerrain;
};


#endif