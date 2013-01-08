/********************************************************************
	created:	27:12:2012   20:44
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorEventCallback.h
	author:		maval
	
	purpose:	各Manipulator事件回调器
*********************************************************************/

#ifndef ManipulatorEventCallback_h__
#define ManipulatorEventCallback_h__


////////////////////////////////////////////////////////////////////////
///逻辑事件回调器
template<class TBase>
class ManipulatorCallbackManager : public TBase
{
public:
	ManipulatorCallbackManager() {}
	virtual ~ManipulatorCallbackManager() {}

public:
	void	AddCallback(TBase* pCB);
	void	RemoveCallback(TBase* pCB);
	//对所有注册的监听者执行回调
	//这样写是为了解除派生类对m_listeners的耦合
	template<class Func> void	Excute(Func&& fun)
	{
		std::for_each(m_listeners.begin(), m_listeners.end(), fun);
	}

private:
	typedef std::list<TBase*>	lstListeners;
	lstListeners	m_listeners;
};

template<class T> void ManipulatorCallbackManager<T>::AddCallback( T* pCB )
{
	assert(std::find(m_listeners.begin(), m_listeners.end(), pCB) == m_listeners.end() && "The callback had already registered!");
	m_listeners.push_back(pCB);
}

template<class T> void ManipulatorCallbackManager<T>::RemoveCallback( T* pCB )
{
	auto iter = std::find(m_listeners.begin(), m_listeners.end(), pCB);
	assert(iter != m_listeners.end() && "The callback didn't registered!");
	m_listeners.erase(iter);
}


////////////////////////////////////////////////////////////////////////
///ManipulatorEvent回调基类
class ManipulatorEventCallbackBase
{
public:
	virtual ~ManipulatorEventCallbackBase() {}
};

////////////////////////////////////////////////////////////////////////
///ManipulatorScene回调事件
class ManipulatorSceneEventCallback : public ManipulatorEventCallbackBase
{
public:
	virtual	void	OnSceneNew() {}
	virtual	void	OnSceneOpen() {}
	virtual void	OnSceneClose() {}
};

////////////////////////////////////////////////////////////////////////
///ManipulatorObject回调事件
class ManipulatorObjectEventCallback : public ManipulatorEventCallbackBase
{
public:
	virtual	void	OnObjectPropertyChanged(Ogre::Entity* pEntiy) {}
};


#endif // ManipulatorEventCallback_h__






