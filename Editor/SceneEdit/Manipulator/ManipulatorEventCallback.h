/********************************************************************
	created:	27:12:2012   20:44
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorEventCallback.h
	author:		maval
	
	purpose:	��Manipulator�¼��ص���
*********************************************************************/

#ifndef ManipulatorEventCallback_h__
#define ManipulatorEventCallback_h__


////////////////////////////////////////////////////////////////////////
///�߼��¼��ص���
template<class T>
class ManipulatorCallbackManager
{
public:
	ManipulatorCallbackManager() {}
	virtual ~ManipulatorCallbackManager() {}

public:
	void	AddCallback(T* pCB);
	void	RemoveCallback(T* pCB);

protected:
	typedef std::list<T*>	lstListeners;
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
///ManipulatorEvent�ص�����
class ManipulatorEventCallbackBase
{
public:
	virtual ~ManipulatorEventCallbackBase() {}
};

////////////////////////////////////////////////////////////////////////
///ManipulatorScene�ص��¼�
class ManipulatorSceneEventCallback : public ManipulatorEventCallbackBase
{
public:
	virtual	void	OnSceneNew() {}
	virtual	void	OnSceneOpen() {}
};


#endif // ManipulatorEventCallback_h__





