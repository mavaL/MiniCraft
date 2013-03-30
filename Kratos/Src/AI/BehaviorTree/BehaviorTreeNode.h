/********************************************************************
	created:	26:3:2013   22:25
	filename	BehaviorTreeNode.h
	author:		maval

	purpose:	行为树节点簇
*********************************************************************/

#ifndef BehaviorTreeNode_h__
#define BehaviorTreeNode_h__

#include "KratosPrerequisites.h"

class aiBlackBoard;
class aiBehaviorConditon;

enum eEvalState
{
	eEvalState_Success,
	eEvalState_Failure,
	eEvalState_Running
};

///基类节点
class aiBehaviorTreeNode
{
public:
	aiBehaviorTreeNode() {}
	virtual ~aiBehaviorTreeNode() {}

public:
	virtual	eEvalState	Evaluate(aiBlackBoard* pInfo, STRING& retBehavior) = 0;
	virtual bool		Validate() { return true; }
	void				AddChild(aiBehaviorTreeNode* node) { m_childs.push_back(node); }

protected:
	std::list<aiBehaviorTreeNode*>	m_childs;
};

///序列节点,从左往右进行
class aiBTSequenceNode : public aiBehaviorTreeNode
{
public:
	aiBTSequenceNode() {}
	~aiBTSequenceNode() {}

public:
	virtual	eEvalState	Evaluate(aiBlackBoard* pInfo, STRING& retBehavior);
};

///行为节点,定为叶节点,代表一个具体执行的行为
class aiBTActionNode : public aiBehaviorTreeNode
{
public:
	aiBTActionNode();
	~aiBTActionNode() {}

public:
	virtual	eEvalState	Evaluate(aiBlackBoard* pInfo, STRING& retBehavior);
	virtual bool		Validate();
	void				SetBehaviorName(const STRING& name) { m_behaviorName = name; }

private:
	STRING		m_behaviorName;
};

///条件节点
class aiBTConditionNode : public aiBehaviorTreeNode
{
public:
	aiBTConditionNode();
	~aiBTConditionNode() {}

public:
	virtual	eEvalState	Evaluate(aiBlackBoard* pInfo, STRING& retBehavior);
	virtual bool		Validate();
	void				SetConditions(const STRING& con, aiBlackBoard* pTmplBB);
	const STRING&		GetConditions() const { return m_conditions; } 

private:
	STRING				m_conditions;	//判定条件字符串
	aiBehaviorConditon* m_pHandler;
};

#endif // BehaviorTreeNode_h__