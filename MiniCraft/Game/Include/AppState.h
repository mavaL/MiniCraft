#ifndef APP_STATE_H
#define APP_STATE_H


class CAppState;
/*------------------------------------------------
			游戏状态管理监听器,Observer模式	
-------------------------------------------------*/
class AppStateListener
{
public:
	AppStateListener(){};
	virtual ~AppStateListener(){};

	virtual void manageAppState(const std::string& stateName, CAppState* state) = 0;

	virtual CAppState* getCurState() = 0;
	virtual CAppState*	findByName(const std::string& stateName) = 0;
	virtual void		changeAppState(CAppState *state) = 0;
	virtual bool		pushAppState(CAppState* state) = 0;
	virtual void		popAppState() = 0;
	virtual void		pauseAppState() = 0;
	virtual void		shutdown() = 0;
    virtual void        popAllAndPushAppState(CAppState* state) = 0;
};

/*-----------------------------------------------------------------
			游戏状态抽象基类,具体的游戏状态从这里继承.
---------------------------------------------------------------*/
class CAppState
{
public:
	static void create(AppStateListener* parent, const std::string& name){};

	void destroy(){delete this;}

	virtual void enter() = 0;
	virtual void exit() = 0;
	virtual bool pause(){return true;}
	virtual void resume(){};
	virtual void update(float timeSinceLastFrame) = 0;
	virtual void render() = 0;

protected:
	CAppState(){};

	CAppState*	findByName(const std::string& stateName){return m_pParent->findByName(stateName);}
	void		changeAppState(CAppState* state){m_pParent->changeAppState(state);}
	bool		pushAppState(CAppState* state){return m_pParent->pushAppState(state);}
	void		popAppState(){m_pParent->popAppState();}
	void		shutdown(){m_pParent->shutdown();}
    void        popAllAndPushAppState(CAppState* state){m_pParent->popAllAndPushAppState(state);}

	AppStateListener*			m_pParent;
};

#define DECLARE_APPSTATE_CLASS(T)										\
static void create(AppStateListener* parent, const std::string& name)	\
{																		\
	T* myAppState = new T();											\
	myAppState->m_pParent = parent;										\
	parent->manageAppState(name, myAppState);							\
}

#endif