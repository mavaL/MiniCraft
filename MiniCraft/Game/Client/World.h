#ifndef WORLD_H
#define WORLD_H

#include "Singleton.h"
#include "GameDefine.h"
#include "SceneSerializer.h"
#include "KratosPrerequisites.h"
#include "ScriptSystem.h"

class SelectableObject;
class OgreRecast;
class OgreDetourCrowd;
class Faction;
class UiCommandPanel;
class UiInfoPanel;
class UiPortraitPanel;

//常用成员全局环境
struct SGlobalEnvironment 
{
	SGlobalEnvironment() { Reset(); }
	void	Reset() 
	{ 
		m_pRecast=nullptr; 
		m_pCrowd=nullptr; 
		m_pDetourTileCache=nullptr; 
	} 

	OgreRecast*				m_pRecast;
	OgreDetourCrowd*		m_pCrowd;
	OgreDetourTileCache*	m_pDetourTileCache;
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

class World : public CSingleton<World>, private Kratos::SceneSerializer
{
	World();
	~World();
	DECLEAR_SINGLETON(World);

protected:
	///////////////////重写SceneSerializer方法
	virtual void	_LoadObjects(rapidxml::xml_node<>* node);

public:
	static const char className[];
	static Luna<World>::RegType methods[];
	World(lua_State* L) {}
	///////////////////////////////////////////
	/////////////lua导出函数
	int		GetPlayerUnitNum(lua_State* L);
	int		SetGlobalBBParam_Int(lua_State* L);
	int		SetGlobalBBParam_Float(lua_State* L);
	int		SetGlobalBBParam_Bool(lua_State* L);

public:
	void	Init();
	void	Update(float dt);
	void	Shutdown();

	Kratos::Scene*	GetScene()	{ return m_pTestScene; }
	void			EnableFreeCamera(bool bEnable);
	bool			IsFreeCameraEnabled() { return m_bFreeCamMode; }
	OgreBites::SdkCameraMan*	GetCameraMan()	{ return m_cameraMan; }

	//AABB场景查询
	void			GetAABBSceneQueryResult(const Ogre::AxisAlignedBox& box, 
		std::vector<Ogre::MovableObject*>& result, int mask = eQueryType_All);
	//射线场景查询
	Ogre::MovableObject*	GetRaySceneQueryResult(const OIS::MouseEvent& arg, 
		int mask = eQueryType_All, POS* retIntersect = nullptr);
	//球形场景查询
	void			GetSphereSceneQueryResult(const Ogre::Sphere& s, 
		std::vector<Ogre::MovableObject*>& result, int mask = eQueryType_All);

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
	//test
	void			StartRagdoll();

	///获取UI组件
	UiCommandPanel*	GetCommandPanel() { return m_cmdPanel; }
	UiInfoPanel*	GetInfoPanel()	{ return m_infoPanel; }
	UiPortraitPanel* GetPortraitPanel() { return m_portraitPanel; }

private:
	Kratos::ScriptSystem*		m_pScriptSystem;
	Kratos::COgreManager*		m_pRenderSystem;

	SelectedContainer			m_vecSelectUnis;	//所有选中单位
	Ogre::AxisAlignedBoxSceneQuery*	m_pSceneQuery;
	Ogre::RaySceneQuery*		m_pRaySceneQuery;
	Ogre::SphereSceneQuery*		m_pSphereSceneQuery;

	//调试观察用第三人称摄像机
	OgreBites::SdkCameraMan*	m_cameraMan;
	bool						m_bFreeCamMode;

	//UI console for test
	Ogre::SceneNode*			m_pUISceneNode1;
	Ogre::SceneNode*			m_pUISceneNode2;
	Ogre::SceneNode*			m_pUISceneNode3;
	Ogre::SceneNode*			m_pUISceneNode4;
	Ogre::AnimationState*		m_pConsoleAnim1;
	Ogre::AnimationState*		m_pConsoleAnim2;

	UiCommandPanel*				m_cmdPanel;
	UiInfoPanel*				m_infoPanel;
	UiPortraitPanel*			m_portraitPanel;

	Faction*					m_player[eGameRace_Count];
	Kratos::Scene*				m_pTestScene;
};


#endif