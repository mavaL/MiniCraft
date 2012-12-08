#ifndef UNIT_H
#define UNIT_H

#include "ScriptSystem.h"
#include "UnitState.h"

enum QueryFlags {
	DEFAULT_MASK = 1u<<0,
	NAVMESH_MASK = 1u<<1,
	OBSTACLE_MASK= 1u<<2
};

class CommandBase;
class OgreRecast;
class OgreDetourCrowd;
class UnitState;
struct dtCrowdAgent;
struct lua_State;

/************************************************************************/
/*							游戏单位类	                                */
/************************************************************************/

class Unit
{
public:
	Unit(int ID, Ogre::Entity* pEnt, Ogre::SceneNode* pNode, OgreRecast* pRecast, OgreDetourCrowd* pDetour);
	Unit(lua_State* L) {}
	~Unit() {}

	static const char className[];
	static Luna<Unit>::RegType methods[];

	static const int	OFFSET_TO_GROURD = 5;

public:
	Ogre::SceneNode*	GetSceneNode() { return m_pNode; }
	Ogre::Entity*	GetEntity() { return m_pEntity; }
	//指派一个执行命令
	void			GiveCommand(const CommandBase& cmd);
	//获取当前命令
	CommandBase*	GetCurCommand() const { return m_pCurCommand; }
	//取消当前命令
	void			CancelCurCommand();
	//设置当前状态
	void			SetState(eUnitState state);
	//更新单位
	void			Update(float dt);
	//精确寻路
	bool			FindPath(const Ogre::Vector3& destPos);
	//如果目标点不能寻路,则寻找最近点
	bool			FindNearestPath(const Ogre::Vector3& destPos, Ogre::Vector3* pRetNearestPt);
	//更新寻路,到达目的地则返回true
	bool			UpdatePathFinding(float dt);

	void			SetDestPos(const Ogre::Vector3& destPos)	{ m_destPos = destPos; }
	const Ogre::Vector3& GetDestPos() const	{ return m_destPos; }
	UnitState*		GetCurState() const { return m_pCurState; }
	void			PlayAnimation(const Ogre::String& topAnimName, const Ogre::String& baseAnimName = Ogre::StringUtil::BLANK);
	void			StopAnimation();
	dtCrowdAgent*	GetAgent() { return m_pAgent; }
	int				GetID() const { return m_ID; }
	//获取该对象在lua栈上的索引
	// TODO:抽象出Scriptable基类
	int				GetLuaStackIndex() const { return m_luaStackIdx; }

public:
	///lua导出函数
	int				PlayAnimation(lua_State* L);
	int				StopAnimation(lua_State* L);
	int				FindPath(lua_State* L);
	int				SetState(lua_State* L);
	int				SetDestPosition(lua_State* L);
	int				GetDestPosition(lua_State* L);
	int				UpdatePathFinding(lua_State* L);
	int				GetCurCommandType(lua_State* L);
	int				Lookat(lua_State* L);
	int				AttachRes(lua_State* L);
	int				DetachRes(lua_State* L);

private:
	Ogre::Entity*		m_pEntity;
	Ogre::SceneNode*	m_pNode;
	OgreRecast*			m_pRecast;
	OgreDetourCrowd*	m_pDetour;
	dtCrowdAgent*		m_pAgent;		//寻路对象
	int					m_agentID;
	CommandBase*		m_pCurCommand;	//当前命令
	UnitState*			m_pCurState;	//单位当前状态(逻辑状态机)
	int					m_ID;
	int					m_luaStackIdx;
	Ogre::Vector3		m_destPos;

	typedef std::vector<Ogre::AnimationState*> VecAnims;
	VecAnims			m_playingAnims;
	Ogre::Entity*		m_pResEnt;
	Ogre::SceneNode*	m_pResNode;
};


#endif