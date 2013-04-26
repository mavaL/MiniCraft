/********************************************************************
	created:	27:3:2013   23:05
	filename	BehaviorTreeTemplate.h
	author:		maval

	purpose:	行为树模板,可对应一个行为树xml.
*********************************************************************/

#ifndef BehaviorTreeTemplate_h__
#define BehaviorTreeTemplate_h__

#include "KratosPrerequisites.h"

namespace Kratos
{
	class aiBehaviorTreeTemplate
	{
	public:
		aiBehaviorTreeTemplate();
		~aiBehaviorTreeTemplate();

	public:
		void			Reset();
		const STRING	Load(const STRING& filename);
		void			Save() {}
		aiBehaviorTree*	GetBT() { return m_pBT; }
		aiBlackBoard*	GetBB() { return m_pBB; }
		void			CloneBlackBoard(aiBlackBoard& toClone);
		const STRING&	GetBBScriptName() const { return m_BBScriptName; }
		const STRING&	GetBBScriptEntry() const { return m_BBScriptEntry; }

	private:
		aiBehaviorTreeTemplate(const aiBehaviorTreeTemplate&);
		aiBehaviorTreeTemplate& operator= (const aiBehaviorTreeTemplate&);

		void			_LoadTreeNode(rapidxml::xml_node<>* node, aiBehaviorTreeNode* parent);

		aiBehaviorTree*		m_pBT;
		aiBlackBoard*		m_pBB;
		STRING				m_BBScriptEntry;
		STRING				m_BBScriptName;
	};
}


#endif // BehaviorTreeTemplate_h__