/********************************************************************
	created:	26:3:2013   22:23
	filename	BehaviorTree.h
	author:		maval

	purpose:	行为选择树类,可树与树之间连接,形成森林
				本来想用中间件的,结果GameBrains已不知所踪,Havok要买,无奈
*********************************************************************/

#ifndef BehaviorTree_h__
#define BehaviorTree_h__

#include "KratosPrerequisites.h"
#include "BehaviorTreeNode.h"


class aiBehaviorTree : public aiBehaviorTreeNode
{
public:
	aiBehaviorTree(const STRING& name);
	~aiBehaviorTree();

public:
	const STRING&	GetName() const { return m_name; }
	//遍历评估整个树,得到要进行的行为
	virtual	bool	Evaluate(aiBlackBoard* pInfo, STRING& retBehavior);

private:
	STRING					m_name;		//该行为树的名字
	aiBehaviorTreeNode*		m_root;
};

#endif // BehaviorTree_h__