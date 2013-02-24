/********************************************************************
	created:	20:1:2013   22:16
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\ObjectBase.h
	author:		maval
	
	purpose:	游戏对象基类,所有游戏实体从其继承
*********************************************************************/

#ifndef ObjectBase_h__
#define ObjectBase_h__

#include "GameDefine.h"
#include <OgreStringInterface.h>
#include "Component.h"


////////////////////////////////////////////////////
///顶层基类
class Object : public Ogre::StringInterface
{
public:
	Object();
	virtual ~Object();

public:
	int					GetID() const { return m_ID; }
	void				AddComponent(eComponentType type, Component* pCo);
	Component*			GetComponent(eComponentType type);
	void				UpdateAllComponent(float dt);
	virtual eObjectType GetType() const = 0;
	virtual	void		Update(float dt) = 0;

private:
	int					m_ID;			//对象唯一标示ID

	typedef std::unordered_map<eComponentType, Component*>	ComponentContainer;
	ComponentContainer	m_components;	//组件容器
};


////////////////////////////////////////////////////
///可渲染对象基类
class RenderableObject : public Object
{
	DECL_PARAM_COMMAND(Position)
	DECL_PARAM_COMMAND(Orientation)
	DECL_PARAM_COMMAND(MeshName)
	DECL_PARAM_COMMAND(Scale)
	DECL_PARAM_COMMAND(TeamColor)

public:
	RenderableObject();
	~RenderableObject() {}

public:
	virtual void	CreateRenderInstance();	

public:
	void			SetMeshName(const STRING& meshname);
	const STRING&	GetMeshName() const { return m_meshname; }
	void			DestroyRenderInstance();
	bool			IsRenderableReady() const { return m_bRenderableReady; }
	void			SetPosition(const POS& pos);
	void			SetOrientation(const ORIENT& orient);
	void			SetScale(const SCALE& scale);
	const POS&		GetPosition() const;
	const ORIENT&	GetOrientation() const;
	const SCALE&	GetScale() const;
	Ogre::SceneNode* GetSceneNode() { return m_pSceneNode; }
	void			SetTeamColor(const COLOR& c) { m_teamColor = c; }
	const COLOR&	GetTeamColor() const { return m_teamColor; }

protected:
	Ogre::Entity*		m_pEntity;
	Ogre::SceneNode*	m_pSceneNode;
	bool				m_bRenderableReady;	//渲染实例是否已创建
	STRING				m_meshname;
	COLOR				m_teamColor;
};



#endif // ObjectBase_h__