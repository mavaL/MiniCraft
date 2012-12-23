/********************************************************************
	created:	20:12:2012   0:39
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorObject.h
	author:		maval
	
	purpose:	可摆放物体创建,管理,操控.
*********************************************************************/

#ifndef ManipulatorObject_h__
#define ManipulatorObject_h__



class ManipulatorObject
{
public:
	ManipulatorObject() {}
	~ManipulatorObject() {}

public:
	//创建Entity
	bool	AddEntity(const Ogre::String& meshname, const Ogre::Vector3& worldPos);
	bool	AddEntity(const Ogre::String& meshname, const Ogre::Vector2& screenPos);

	void	OnGizmoNodeReset() {}
};


#endif // ManipulatorObject_h__







