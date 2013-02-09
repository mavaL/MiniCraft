/********************************************************************
	created:	21:1:2013   23:36
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\AiComponent.h
	author:		maval
	
	purpose:	AI组件,逻辑对象通过组合使用该类
*********************************************************************/

#ifndef AIComponent_h__
#define AIComponent_h__

#include "UnitState.h"
#include "GameDefine.h"

class CommandBase;
class OgreRecast;
class OgreDetourCrowd;
class UnitState;
struct dtCrowdAgent;
class SelectableObject;

///AI组件基类
class AiComponent
{
public:
	AiComponent(SelectableObject* pOwner);
	virtual ~AiComponent() {}

public:
	void			Update(float dt);
	void			GiveCommand(const CommandBase& cmd);

// 	void			SetState(eUnitState state);
// 	UnitState*		GetCurState() const { return m_pCurState; }

protected:
	SelectableObject*	m_pOwner;		//该组件所属对象

	typedef std::list<const CommandBase*> CommandQueue;
	CommandQueue		m_cmdQueue;		//命令队列
	bool				m_bExecuting;	//当前是否在执行命令
	//UnitState*			m_pCurState;	//单位当前状态(逻辑状态机)
};

///寻路AI组件
class AiPath : public AiComponent
{
public:
	AiPath(SelectableObject* pOwner);
	~AiPath();

public:
	bool			FindPath(const Ogre::Vector3& destPos);
	const POS		GetAgentPos() const;

private:
	OgreRecast*			m_pRecast;
	OgreDetourCrowd*	m_pDetour;
	dtCrowdAgent*		m_pAgent;
	int					m_agentID;
};

#endif // AIComponent_h__