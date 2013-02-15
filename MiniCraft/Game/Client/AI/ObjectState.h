/********************************************************************
	created:	10:2:2013   13:56
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\ObjectState.h
	author:		maval
	
	purpose:	游戏对象状态类体系.对象行为采用逻辑状态机机制.
*********************************************************************/
#ifndef ObjectState_h__
#define ObjectState_h__


enum eObjectState
{
	eObjectState_Idle,			//空闲
	eObjectState_Produce,		//生产
	eObjectState_Move,			//移动
	eObjectState_Targeting,		//选择目标
	eObjectState_Stop,			//停止
	eObjectState_Gather,		//采集资源
};

class SelectableObject;

///状态基类
class ObjectState
{
public:
	ObjectState(eObjectState type):m_type(type) {}
	virtual ~ObjectState() {}

public:
	virtual	void Enter(SelectableObject* pOwner) {}
	virtual void Update(float dt, SelectableObject* pOwner) {}
	virtual void Exit(SelectableObject* pOwner) {}

public:
	eObjectState	GetType() const { return m_type; }

private:
	eObjectState	m_type;
};

///空闲状态
class StateIdle : public ObjectState
{
public:
	StateIdle():ObjectState(eObjectState_Idle) {}

public:
	virtual	void Enter(SelectableObject* pOwner);
	virtual void Exit(SelectableObject* pOwner);
};

///选择目标状态
class StateTargeting : public ObjectState
{
public:
	StateTargeting():ObjectState(eObjectState_Targeting) {}

public:
	virtual	void Enter(SelectableObject* pOwner);
	virtual void Exit(SelectableObject* pOwner);

private:
	bool		OnInputSys_MouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
};

///移动状态
class StateMove : public ObjectState
{
public:
	StateMove():ObjectState(eObjectState_Move) {}

public:
	virtual	void Enter(SelectableObject* pOwner);
	virtual void Update(float dt, SelectableObject* pOwner);
	virtual void Exit(SelectableObject* pOwner);
};

///生产状态
class StateProduce : public ObjectState
{
public:
	StateProduce():ObjectState(eObjectState_Produce) {}

public:
	virtual	void Enter(SelectableObject* pOwner);
	virtual void Update(float dt, SelectableObject* pOwner);
	virtual void Exit(SelectableObject* pOwner);
};

///停止状态
class StateStop : public ObjectState
{
public:
	StateStop():ObjectState(eObjectState_Stop) {}

public:
	virtual	void Enter(SelectableObject* pOwner);
	virtual void Update(float dt, SelectableObject* pOwner);
};

///采集状态
class StateGather : public ObjectState
{
public:
	StateGather():ObjectState(eObjectState_Gather) {}

public:
	virtual	void Enter(SelectableObject* pOwner);
	virtual void Update(float dt, SelectableObject* pOwner);
	virtual void Exit(SelectableObject* pOwner);
};


#endif // ObjectState_h__