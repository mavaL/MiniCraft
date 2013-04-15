#include "stdafx.h"
#include "BehaviorTree.h"

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

	bool aiBehaviorTree::ValidateTree()
	{
		bool ret = m_root->Validate();
		if(!ret)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, 
			"Behavior Tree validate failed!", "aiBehaviorTree::Validate()");
		return ret;
	}
}

