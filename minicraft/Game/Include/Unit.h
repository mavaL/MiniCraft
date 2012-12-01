#ifndef UNIT_H
#define UNIT_H


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

/************************************************************************/
/*							游戏单位类	                                */
/************************************************************************/

class Unit
{
public:
	Unit(Ogre::Entity* pEnt, Ogre::SceneNode* pNode, OgreRecast* pRecast, OgreDetourCrowd* pDetour);
	~Unit() {}
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
	void			SetState(const UnitState& newState);
	//更新单位
	void			Update(float dt);
	//进行寻路
	bool			FindPath(const Ogre::Vector3& destPos);
	
	void			MoveTo(const Ogre::Vector3& destPos);
	dtCrowdAgent*	GetAgent() { return m_pAgent; }

private:
	Ogre::Entity*		m_pEntity;
	Ogre::SceneNode*	m_pNode;
	OgreRecast*			m_pRecast;
	OgreDetourCrowd*	m_pDetour;
	dtCrowdAgent*		m_pAgent;		//寻路对象
	int					m_agentID;
	CommandBase*		m_pCurCommand;	//当前命令
	UnitState*			m_pCurState;	//单位当前状态(逻辑状态机)
};


#endif