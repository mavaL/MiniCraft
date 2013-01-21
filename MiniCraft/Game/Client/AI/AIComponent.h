/********************************************************************
	created:	21:1:2013   23:36
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\AIComponent.h
	author:		maval
	
	purpose:	AI组件,其它对象通过组合使用该类
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
class RenderableObject;

class AIComponent
{
public:
	AIComponent(RenderableObject* pOwner);
	~AIComponent();

public:
	void			Update(float dt);
	//指派一个执行命令
	void			GiveCommand(const CommandBase& cmd);
	//获取当前命令
	CommandBase*	GetCurCommand() const { return m_pCurCommand; }
	//取消当前命令
	void			CancelCurCommand();
	//设置当前状态
	void			SetState(eUnitState state);
	//精确寻路
	bool			FindPath(const Ogre::Vector3& destPos);
	UnitState*		GetCurState() const { return m_pCurState; }
	const POS		GetAgentPos() const;

private:
	RenderableObject*	m_pOwner;		//该组件所属对象
	OgreRecast*			m_pRecast;
	OgreDetourCrowd*	m_pDetour;
	dtCrowdAgent*		m_pAgent;
	int					m_agentID;
	CommandBase*		m_pCurCommand;	//当前命令
	UnitState*			m_pCurState;	//单位当前状态(逻辑状态机)
};

#endif // AIComponent_h__