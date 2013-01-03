/********************************************************************
	created:	20:12:2012   0:39
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorObject.h
	author:		maval
	
	purpose:	可摆放物体创建,管理,操控.
*********************************************************************/

#ifndef ManipulatorObject_h__
#define ManipulatorObject_h__

#include "ManipulatorEventCallback.h"
#include "EditorDefine.h"

class GizmoAxis;

class ManipulatorObject : public ManipulatorSceneEventCallback
{
public:
	enum eEditMode
	{
		eEditMode_None,
		eEditMode_Move,
		eEditMode_Rotate,
		eEditMode_Scale,
		eEditMode_Select
	};

	//////////////事件回调
	virtual void	OnSceneClose();
	virtual void	OnSceneNew();
	virtual void	OnSceneOpen();

public:
	ManipulatorObject();
	~ManipulatorObject();

public:
	void		OnFrameMove(float dt);
	eEditMode	GetCurEditMode() const { return m_curEditMode; }
	void		SetCurEditMode(eEditMode mode) { m_curEditMode = mode; }
	//创建Entity
	bool		AddEntity(const Ogre::String& meshname, const Ogre::Vector3& worldPos);
	bool		AddEntity(const Ogre::String& meshname, const Ogre::Vector2& screenPos);
	///物体选择
	void		SetSelection(Ogre::Entity* pEnt);
	Ogre::Entity*	GetSelection() const { return m_pSelectEntity; }
	void		ClearSelection();
	//取得Entity的包围盒Gizmo.包围盒显示由我们自己管理,不使用OGRE默认的showBoundingBox
	Ogre::WireBoundingBox*	GetEntityAABBGizmo(Ogre::Entity* pEntity);
	//移动物体
	void		SelectionMove(const Ogre::Vector3& vecMove);
	//根据操作模式,显示物体相应的Gizmo
	void		ShowEntityGizmo(Ogre::Entity* pEntity, bool bShow, eEditMode mode, bool bDrift = false);
	void		HighlightGizmoAxis(bool bHighlight, eAxis axis);
	void		OnGizmoNodeReset();
	//射线查询
	Ogre::MovableObject* DoRaySceneQuery(const Ogre::Ray& ray, int queryMask = 0xffffffff);
	void		DoAABBSceneQuery(const Ogre::AxisAlignedBox& aabb, int queryMask = 0xffffffff);

private:
	eEditMode		m_curEditMode;
	Ogre::Entity*	m_pSelectEntity;	//当前选中物体
	GizmoAxis*		m_pGizmoAixs;		//坐标轴指示器
	Ogre::RaySceneQuery* m_pRaySceneQuery;
	Ogre::AxisAlignedBoxSceneQuery*	m_pAABBSceneQuery;
};


#endif // ManipulatorObject_h__







