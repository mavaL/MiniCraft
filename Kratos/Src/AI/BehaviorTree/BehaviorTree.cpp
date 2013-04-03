#include "BehaviorTree.h"
#include <OgreException.h>

namespace Kratos
{
	aiBehaviorTree::aiBehaviorTree(eGameRace race)
		:m_root(new aiBTConditionNode)
		,m_race(race)
	{

	}

	aiBehaviorTree::~aiBehaviorTree()
	{
		SAFE_DELETE(m_root);
	}

	eEvalState aiBehaviorTree::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
	{
		return m_root->Evaluate(pInfo, retBehavior);	
	}

	void aiBehaviorTree::ValidateTree()
	{
		try
		{
			m_root->Validate();	
		}
		catch(const Ogre::Exception& e)
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, 
				e.getDescription(), "aiBehaviorTree::Validate()");
		}
	}

	void aiBehaviorTree::AddNode( aiBehaviorTreeNode* node, aiBehaviorTreeNode* parent )
	{
		assert(node && parent);
		int addrHash = (int)parent;
		auto iter = m_nodeMap.find(addrHash);
		assert(iter != m_nodeMap.end());
		iter->second->AddChild(node);
	}
}

