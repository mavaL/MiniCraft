#ifndef WORLD_H
#define WORLD_H

#include "Singleton.h"
#include "GameDefine.h"

class SelectableObject;
class OgreRecast;
class OgreDetourCrowd;
class Faction;
class UiCommandPanel;
class UiInfoPanel;

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


namespace OgreBites
{
	class SdkCameraMan;
}

typedef	std::vector<SelectableObject*>	SelectedContainer;
/************************************************************************/
/*								世界管理器                                */
/************************************************************************/

class World : public CSingleton<World>
{
	World();
	~World();
	DECLEAR_SINGLETON(World);
public:
	void	Init();
	void	Update(float dt);
	void	Shutdown();
	void	LoadTerrain(rapidxml::xml_node<>* XMLNode);

	Ogre::Camera*	GetCamera()	{ return m_pCamera; }
	const Ogre::AxisAlignedBox&	GetResAABB() const { return m_pGold->getWorldBoundingBox(); }
	POS				GetRandomPositionOnNavmesh();

	void			EnableFreeCamera(bool bEnable);
	bool			IsFreeCameraEnabled() { return m_bFreeCamMode; }
	OgreBites::SdkCameraMan*	GetCameraMan()	{ return m_cameraMan; }

	//返回AABB场景查询到的物体
	void			GetAABBSceneQueryResult(const Ogre::AxisAlignedBox& box, 
		std::vector<Ogre::MovableObject*>& result, int queryMask = 0xffffffff);
	//返回射线场景查询到的第1个物体
	Ogre::MovableObject*	GetRaySceneQueryResult(const Ogre::Ray& ray, int queryMask = 0xffffffff);

	//尝试调整世界坐标在有效的NavMesh上
	bool			ClampPosToNavMesh(POS& wPos);
	//设置对象为选中状态
	void			SetObjectSelected(int ID);
	//清除所有选中状态
	void			ClearSelectedState();

	const SelectedContainer&	GetSelectedObjects() { return m_vecSelectUnis; }
	//将给定世界坐标通过射线投射夹持在地形上
	void			ClampToTerrain(POS& pos);
	//像一个公告板一样更新控制台UI
	void			UpdateConsoleUITransform(float dt);
	//返回相机射线与地形的交点,没相交返回false
	bool			GetTerrainIntersectPos(const FLOAT2& screenPos, POS& retPt);
	//获取玩家派系
	Faction*		GetFaction(eGameRace race) { return m_player[race]; }

	///获取UI组件
	UiCommandPanel*	GetCommandPanel() { return m_cmdPanel; }
	UiInfoPanel*	GetInfoPanel()	{ return m_infoPanel; }

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

	//UI console for test
	Ogre::SceneNode*			m_pUISceneNode1;
	Ogre::SceneNode*			m_pUISceneNode2;
	Ogre::SceneNode*			m_pUISceneNode3;
	Ogre::SceneNode*			m_pUISceneNode4;
	Ogre::AnimationState*		m_pConsoleAnim1;
	Ogre::AnimationState*		m_pConsoleAnim2;

	UiCommandPanel*				m_cmdPanel;
	UiInfoPanel*				m_infoPanel;

	Faction*					m_player[eGameRace_Count];
};


#endif