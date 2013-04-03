/********************************************************************
	created:	27:3:2013   23:05
	filename	BehaviorTreeTemplate.h
	author:		maval

	purpose:	行为树模板,可对应一个行为树xml.
*********************************************************************/

#ifndef BehaviorTreeTemplate_h__
#define BehaviorTreeTemplate_h__

#include "KratosPrerequisites.h"
#include <rapidxml.hpp>

namespace Kratos
{
	class aiBehaviorTree;
	class aiBlackBoard;
	class aiBehaviorTreeNode;

	class aiBehaviorTreeTemplate
	{
	public:
		aiBehaviorTreeTemplate();
		~aiBehaviorTreeTemplate();

	public:
		const STRING	Load(const STRING& filename);
		void			Save() {}
		aiBehaviorTree*	GetBT() { return m_pBT; }
		void			CloneBlackBoard(aiBlackBoard& toClone);
		const STRING&	GetBBScriptEntry() const { return m_BBScript; }

	private:
		aiBehaviorTreeTemplate(const aiBehaviorTreeTemplate&);
		aiBehaviorTreeTemplate& operator= (const aiBehaviorTreeTemplate&);

		void			_LoadTreeNode(rapidxml::xml_node<>* node, aiBehaviorTreeNode* parent);

		aiBehaviorTree*		m_pBT;
		aiBlackBoard*		m_pBB;
		STRING				m_BBScript;
	};
}


#endif // BehaviorTreeTemplate_h__