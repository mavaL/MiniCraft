/********************************************************************
	created:	30:12:2012   9:57
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorNavMesh.h
	author:		maval
	
	purpose:	Recast库导航网格管理
*********************************************************************/

#ifndef ManipulatorNavMesh_h__
#define ManipulatorNavMesh_h__

#include "ManipulatorEventCallback.h"

class OgreRecast;
class OgreDetourTileCache;
class InputGeom;

class ManipulatorNavMesh : public ManipulatorSceneEventCallback
{
public:
	ManipulatorNavMesh();
	~ManipulatorNavMesh();

public:
	////////事件回调
	virtual	void	OnSceneNew();
	virtual	void	OnSceneOpen();
	virtual void	OnSceneClose();

public:
	void		Generate();				//生成导航网格
	void		Reset();
	void		SaveObj();				//导出obj文件(包含导航网格几何数据)
	void		SaveNavMesh();			//导出导航网格数据
	void		ShowNavMesh(bool bShow);//显示导航网格
	bool		GetIsShowNavMesh() const { return m_bShowNavMesh; }
	bool		HasGenerate() const { return m_bHasGenerate; }
	void		OnGizmoNodeReset() {}

private:
	OgreRecast*				m_pRecast;
	InputGeom*				m_pInputGeom;
	OgreDetourTileCache*	m_pDetourTileCache;
	bool					m_bShowNavMesh;
	bool					m_bHasGenerate;
	Ogre::SceneNode*		m_debugRenderNode;
};


#endif // ManipulatorNavMesh_h__







