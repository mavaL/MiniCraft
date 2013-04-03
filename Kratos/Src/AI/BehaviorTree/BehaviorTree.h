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
#include "GameDefine.h"

namespace Kratos
{
	class aiBehaviorTree : public aiBehaviorTreeNode
	{
	public:
		aiBehaviorTree(eGameRace race);
		~aiBehaviorTree();

	public:
		eGameRace				GetRace() const { return m_race; }
		aiBehaviorTreeNode*		GetRootNode() const { return m_root; }
		void					AddNode(aiBehaviorTreeNode* node, aiBehaviorTreeNode* parent);
		//遍历评估整个树,得到要进行的行为
		virtual	eEvalState		Evaluate(aiBlackBoard* pInfo, STRING& retBehavior);
		//校验有效性
		void					ValidateTree();		

	private:
		aiBehaviorTree(const aiBehaviorTree&);
		aiBehaviorTree& operator= (const aiBehaviorTree&);

		aiBehaviorTreeNode*		m_root;
		typedef HashMap<int, aiBehaviorTreeNode*> NodeMap;
		NodeMap					m_nodeMap;	//根据节点地址快速获取
		eGameRace				m_race;
	};
}


#endif // BehaviorTree_h__