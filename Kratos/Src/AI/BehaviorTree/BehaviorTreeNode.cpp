#include "BehaviorTreeNode.h"
#include <OgreException.h>


bool aiBehaviorTreeNode::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
{
	if(m_childs.size() != 1)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Root node should has exactly ONE child!", "Evaluate");
		return false;
	}

	return m_childs.front()->Evaluate(pInfo, retBehavior);
}

/////////////////////////////////////////////////////////////////////
bool aiBTSequenceNode::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
{
	for (auto iter=m_childs.begin(); iter!=m_childs.end(); ++iter)
	{
		if((*iter)->Evaluate(pInfo, retBehavior))
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
aiBTActionNode::aiBTActionNode()
:m_behaviorName(Ogre::StringUtil::BLANK)
{

}

bool aiBTActionNode::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
{
	if(!m_childs.empty())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Action node should be leaf node!", "Evaluate");
		return false;
	}

	//·µ»ØÐÐÎª..
	retBehavior = m_behaviorName;

	return true;
}

/////////////////////////////////////////////////////////////////////
aiBTConditionNode::aiBTConditionNode()
:m_conditions(Ogre::StringUtil::BLANK)
{

}

bool aiBTConditionNode::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
{
	if(m_childs.size() != 1)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Condition node should has exactly ONE child!", "Evaluate");
		return false;
	}

	//TODO:
	if (false)
	{
		return m_childs.front()->Evaluate(pInfo, retBehavior);
	}

	return false;
}
