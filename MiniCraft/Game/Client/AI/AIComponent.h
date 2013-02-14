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

class SelectableObject;

///AI组件基类
class AiComponent
{
public:
	AiComponent(SelectableObject* pOwner);
	virtual ~AiComponent();

public:
	void			Update(float dt);
	//执行命令.bForceExecute为true则取消当前命令,直接执行该命令
	void			GiveCommand(Command& cmd, bool bForceExecute = false);
	//鼠标右键命令
	void			GiveCommand(const OIS::MouseEvent& arg);
	void			CancelCurCommand();
	eCommandType	GetCurCommand() const;
	void			SetCurState(eObjectState state);
	eObjectState	GetCurState() const { return m_curState; }

	//当前命令完成,接到通知
	void			_OnCommandFinished();

private:
	typedef std::vector<ObjectState*>	StateLib;
	StateLib			m_states;		//状态容器.每类状态只生成一个实例存储在这里供取用

	typedef std::list<Command> CommandQueue;
	CommandQueue		m_cmdQueue;		//命令队列

	SelectableObject*	m_pOwner;		//该组件所属对象
	bool				m_bExecuting;	//当前是否在执行命令
	eObjectState		m_curState;		//对象当前所处状态
};

#endif // AIComponent_h__