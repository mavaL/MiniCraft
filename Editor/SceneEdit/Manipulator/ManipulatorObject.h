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
#include "KratosPrerequisites.h"

class GizmoAxis;


///物体信息封装结构
struct SObjectInfo 
{
	SObjectInfo():	m_bAddToNavmesh(false),m_bIsBuilding(false),m_bIsResource(false) {}
	bool			m_bAddToNavmesh;	//是否参与NavMesh构建
	bool			m_bIsBuilding;		//是否是游戏建筑
	std::string		m_buildingName;		//若是建筑,建筑名称
	bool			m_bIsResource;		//是否是游戏资源
	std::string		m_meshname;
	Ogre::Vector3	m_pos;
	Ogre::Quaternion m_rot;
	Ogre::Vector3	m_scale;
};

typedef std::unordered_map<Ogre::Entity*, SObjectInfo*> ObjectContainer;

class ManipulatorObject : public ManipulatorSceneEventCallback, public ManipulatorCallbackManager<ManipulatorObjectEventCallback>
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
	void					Load(rapidxml::xml_node<>* XMLNode);
	void					Serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* XMLNode);
	void					OnFrameMove(float dt);
	eEditMode				GetCurEditMode() const { return m_curEditMode; }
	void					SetCurEditMode(eEditMode mode) { m_curEditMode = mode; }
	const ObjectContainer&	GetAllObjects() const { return m_objects; }
	void					SetObjectInfo(Ogre::Entity* ent, const SObjectInfo& info);
	SObjectInfo*			GetObjectInfo(Ogre::Entity* ent);

	//创建Entity
	Ogre::Entity*			AddEntity(const STRING& meshname, const POS& worldPos, bool bOp = true,
		const ORIENT& orient = ORIENT::IDENTITY, const SCALE& scale = SCALE(1,1,1));
	Ogre::Entity*			AddEntity(const Ogre::String& meshname, const Ogre::Vector2& screenPos);
	//删除物体
	void					RemoveEntity(Ogre::Entity* ent, bool bOp = true);

	///物体选择/编辑
	void					SetSelection(Ogre::Entity* pEnt);
	Ogre::Entity*			GetSelection() const { return m_pSelectEntity; }
	void					ClearSelection();
	void					SetPosition(Ogre::Entity* ent, const Ogre::Vector3& newPos, bool bOp = true);
	void					SetOrientation(Ogre::Entity* ent, const Ogre::Quaternion& orient, bool bOp = true);
	void					SetScale(Ogre::Entity* ent, const Ogre::Vector3 scale, bool bOp = true);
	void					Rotate(Ogre::Entity* ent, float radian, bool bOp = true);
	void					Scale(Ogre::Entity* ent, const Ogre::Vector3& scaleMultiplier, bool bOp = true);

	//取得Entity的包围盒Gizmo.包围盒显示由我们自己管理,不使用OGRE默认的showBoundingBox
	Ogre::WireBoundingBox*	GetEntityAABBGizmo(Ogre::Entity* pEntity);
	//根据操作模式,显示物体相应的Gizmo
	void					ShowEntityGizmo(Ogre::Entity* pEntity, bool bShow, eEditMode mode, bool bDrift = false);
	GizmoAxis*				GetGizmoAxis() const { return m_pGizmoAixs; }
	void					OnGizmoNodeReset();
	void					CommitEditOperation(Ogre::Entity* ent, eEditMode mode, const Ogre::Any& oldValue, const Ogre::Any& newValue);

	//射线查询
	Ogre::MovableObject* DoRaySceneQuery(const Ogre::Ray& ray, int queryMask = 0xffffffff);
	void		DoAABBSceneQuery(const Ogre::AxisAlignedBox& aabb, int queryMask = 0xffffffff);

private:
	void		_UpdateAABBOfEntity(Ogre::Entity* pEntity);

private:
	ObjectContainer	m_objects;			//场景中的所有摆放物体
	eEditMode		m_curEditMode;
	Ogre::Entity*	m_pSelectEntity;	//当前选中物体
	GizmoAxis*		m_pGizmoAixs;		//坐标轴指示器
	Ogre::RaySceneQuery* m_pRaySceneQuery;
	Ogre::AxisAlignedBoxSceneQuery*	m_pAABBSceneQuery;
};


#endif // ManipulatorObject_h__







