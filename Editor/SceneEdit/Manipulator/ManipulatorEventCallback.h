/********************************************************************
	created:	27:12:2012   20:44
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorEventCallback.h
	author:		maval
	
	purpose:	各Manipulator事件回调器
*********************************************************************/

#ifndef ManipulatorEventCallback_h__
#define ManipulatorEventCallback_h__

////////////////////////////////////////////////////////////////////////
///ManipulatorScene回调事件
class ManipulatorSceneEventCallback : public Kratos::EventCallbackBase
{
public:
	virtual	void	OnSceneNew() {}
	virtual	void	OnSceneOpen() {}
	virtual void	OnSceneClose() {}
};

////////////////////////////////////////////////////////////////////////
///ManipulatorObject回调事件
class ManipulatorObjectEventCallback : public Kratos::EventCallbackBase
{
public:
	virtual void	OnObjectSetSelection(Ogre::Entity* pObject) {}
	virtual void	OnObjectClearSelection(Ogre::Entity* pObject) {}
	virtual	void	OnObjectPropertyChanged(Ogre::Entity* pObject) {}
};


#endif // ManipulatorEventCallback_h__






