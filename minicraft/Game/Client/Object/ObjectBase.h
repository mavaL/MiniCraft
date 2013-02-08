/********************************************************************
	created:	20:1:2013   22:16
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\ObjectBase.h
	author:		maval
	
	purpose:	游戏对象基类,所有游戏实体从其继承
*********************************************************************/

#ifndef ObjectBase_h__
#define ObjectBase_h__

#include "GameDefine.h"
#include "OgreStringInterface.h"

///注册OgreStringInterface
#define DECL_PARAM_COMMAND(name)						\
class Cmd##name : public Ogre::ParamCommand				\
{														\
public:													\
	STRING doGet(const void* target) const;				\
	void doSet(void* target, const STRING& val);		\
};														\
static Cmd##name	m_sCmd##name;						

#define IMPL_PARAM_COMMAND(classname, cmdname, paramtype)								\
classname::Cmd##cmdname	classname::m_sCmd##cmdname;										\
STRING classname::Cmd##cmdname::doGet(const void* target) const							\
{																						\
	const classname* obj = static_cast<const classname*>(target);						\
	return Ogre::StringConverter::toString(obj->Get##cmdname());						\
}																						\
																						\
void classname::Cmd##cmdname::doSet(void* target, const STRING& val)					\
{																						\
	classname* obj = static_cast<classname*>(target);									\
	obj->Set##cmdname(Ogre::StringConverter::parse##paramtype(val));					\
}																						\


////////////////////////////////////////////////////
///顶层基类
class Object : public Ogre::StringInterface
{
public:
	Object();
	virtual ~Object() {}

public:
	int					GetID() const { return m_ID; }
	virtual eObjectType GetType() const = 0;
	virtual const STRING& GetNamePrefix() const = 0;
	virtual	void		Update(float dt) = 0;

private:
	int	m_ID;	//对象唯一标示ID
};


////////////////////////////////////////////////////
///可渲染对象基类
class RenderableObject : public Object
{
	/// Command object
	DECL_PARAM_COMMAND(Position)
	DECL_PARAM_COMMAND(Orientation)

public:
	RenderableObject();
	~RenderableObject() {}

public:
	virtual	void	CreateRenderInstance(const STRING& meshname);
	virtual void	DestroyRenderInstance();
	virtual void	SetPosition(const POS& pos);
	virtual void	SetOrientation(const ORIENT& orient);
	virtual void	SetScale(const SCALE& scale);

public:
	bool			IsRenderableReady() const { return m_bRenderableReady; }

	const POS&		GetPosition() const;
	const ORIENT&	GetOrientation() const;
	const SCALE&	GetScale() const;

protected:
	Ogre::Entity*		m_pEntity;
	Ogre::SceneNode*	m_pSceneNode;
	bool				m_bRenderableReady;	//渲染实例是否已创建
};



#endif // ObjectBase_h__