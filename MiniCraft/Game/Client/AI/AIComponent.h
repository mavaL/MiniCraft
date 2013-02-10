/********************************************************************
	created:	21:1:2013   23:36
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\AiComponent.h
	author:		maval
	
	purpose:	AI组件,逻辑对象通过组合使用该类
*********************************************************************/

#ifndef AIComponent_h__
#define AIComponent_h__

#include "GameDefine.h"
#include "Command.h"
#include "ObjectState.h"

class OgreRecast;
class OgreDetourCrowd;
struct dtCrowdAgent;
class SelectableObject;

///AI组件基类
class AiComponent
{
public:
	AiComponent(SelectableObject* pOwner);
	virtual ~AiComponent();

public:
	void			Update(float dt);
	void			GiveCommand(Command& cmd);
	void			SetCurState(eObjectState state);
	eObjectState	GetCurState() const { return m_curState; }

	//当前命令完成,接到通知
	void			_OnCommandFinished();

protected:
	typedef std::vector<ObjectState*>	StateLib;
	StateLib			m_states;		//状态容器.每类状态只生成一个实例存储在这里供取用

	typedef std::list<Command> CommandQueue;
	CommandQueue		m_cmdQueue;		//命令队列

	SelectableObject*	m_pOwner;		//该组件所属对象
	bool				m_bExecuting;	//当前是否在执行命令
	eObjectState		m_curState;		//对象当前所处状态
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