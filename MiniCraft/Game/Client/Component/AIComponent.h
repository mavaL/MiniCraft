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
#include "Component.h"

class SelectableObject;
class Faction;

///AI组件基类
class AiComponent : public Component
{
public:
	AiComponent(SelectableObject* pOwner);
	~AiComponent();

public:
	virtual void	Update(float dt);
	void			SetCpuControl(bool bCpu) { m_bCpuControl = bCpu; }
	bool			GetCpuControl() const	{ return m_bCpuControl; }
	//执行命令.bForceExecute为true则取消当前命令,直接执行该命令
	void			GiveCommand(Command& cmd, bool bForceExecute = false);
	//鼠标右键命令
	void			GiveCommand(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	//停止当前命令
	void			CancelCurCommand();
	//清空所有命令
	void			CancelAllCommand();
	eCommandType	GetCurCommand() const;
	void			SetCurState(eObjectState state);
	eObjectState	GetCurState() const { return m_curState; }
	void			SetParallelState(eObjectState state);
	void			ClearParallelState();
	void			SetFaction(Faction* player);
	Faction*		GetFaction()	{ return m_player; }
	bool			IsAlly(SelectableObject* obj);

	//当前命令完成,接到通知
	void			_OnCommandFinished();

private:
	typedef std::vector<ObjectState*>	StateLib;
	StateLib			m_states;		//状态容器.每类状态只生成一个实例存储在这里供取用

	typedef std::list<Command> CommandQueue;
	CommandQueue		m_cmdQueue;		//命令队列

	bool				m_bCpuControl;	//该对象是否完全由电脑控制
	bool				m_bExecuting;	//当前是否在执行命令
	eObjectState		m_curState;		//对象当前所处状态
	ObjectState*		m_parallelState;//并行状态.比如StateTargeting就需要与其他状态同时进行	
	Faction*			m_player;		//该对象所属玩家
};

#endif // AIComponent_h__