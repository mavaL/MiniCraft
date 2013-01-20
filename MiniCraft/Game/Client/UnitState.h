#ifndef UNIT_STATE_H
#define UNIT_STATE_H



class Unit;

enum eUnitState
{
	///!!!!这里要与UnitState.lua中的常量相对应
	// TODO:DRY原则,也许这里需要重新设计
	eUnitState_Idle,
	eUnitState_Move,
	eUnitState_Attack,
	eUnitState_Collect,
	eUnitState_Return,
	eUnitState_Build
};
/************************************************************************/
/*							单位状态基类	                                */
/************************************************************************/

class UnitState
{
public:
	UnitState(Unit* pOwner):m_pOwner(pOwner) {}
	UnitState(const UnitState& rhs):m_type(rhs.GetType()),m_pOwner(rhs.GetOwner()) {}
	virtual ~UnitState() {}
public:
	virtual	void Enter() = 0;
	virtual void Update(float dt) = 0;
	virtual void Exit() = 0;
public:
	eUnitState	GetType() const { return m_type; }
	Unit*		GetOwner() const { return m_pOwner; }

protected:
	eUnitState	m_type;
	Unit*		m_pOwner;	//该状态所属单位
};

/************************************************************************/
/*							空闲状态		                                */
/************************************************************************/

class IdleState : public UnitState
{
public:
	IdleState(Unit* pOwner):UnitState(pOwner),m_pIdleAnim(nullptr) { m_type = eUnitState_Idle; }
	IdleState(const IdleState& rhs):UnitState(rhs) {}
	virtual ~IdleState() {}
public:
	virtual	void Enter();
	virtual void Update(float dt);
	virtual void Exit();

private:
	Ogre::AnimationState*	m_pIdleAnim;
};

/************************************************************************/
/*							移动状态		                                */
/************************************************************************/

class MoveState : public UnitState
{
public:
	MoveState(Unit* pOwner);
	MoveState(const MoveState& rhs):UnitState(rhs) {}
	virtual ~MoveState() {}

public:
	virtual	void Enter();
	virtual void Update(float dt);
	virtual void Exit();
};

/************************************************************************/
/*							攻击状态			                            */
/************************************************************************/

class AttackState : public UnitState
{
public:
	AttackState(Unit* pOwner):UnitState(pOwner) { m_type = eUnitState_Attack; }
	AttackState(const AttackState& rhs):UnitState(rhs) {}
	virtual ~AttackState() {}
public:
	virtual	void Enter() {}
	virtual void Update(float dt) {}
	virtual void Exit() {}
};

/************************************************************************/
/*							建造状态			                            */
/************************************************************************/

class BuildState : public UnitState
{
public:
	BuildState(Unit* pOwner):UnitState(pOwner) { m_type = eUnitState_Build; }
	BuildState(const BuildState& rhs):UnitState(rhs) {}
	virtual ~BuildState() {}
public:
	virtual	void Enter() {}
	virtual void Update(float dt) {}
	virtual void Exit() {}
};


/************************************************************************/
/*							采集状态			                            */
/************************************************************************/

class CollectResState : public UnitState
{
public:
	CollectResState(Unit* pOwner);
	CollectResState(const CollectResState& rhs):UnitState(rhs) {}
	virtual ~CollectResState() {}
public:
	virtual	void Enter();
	virtual void Update(float dt);
	virtual void Exit();
};

/************************************************************************/
/*							返还资源状态			                            */
/************************************************************************/

class ReturnResState : public UnitState
{
public:
	ReturnResState(Unit* pOwner);
	ReturnResState(const ReturnResState& rhs):UnitState(rhs) {}
	virtual ~ReturnResState() {}
public:
	virtual	void Enter();
	virtual void Update(float dt);
	virtual void Exit();
};


#endif