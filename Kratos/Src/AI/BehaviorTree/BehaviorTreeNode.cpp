#include "BehaviorTreeNode.h"
#include <OgreException.h>
#include "BehaviorTreeCondition.h"

eEvalState aiBTSequenceNode::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
{
	for (auto iter=m_childs.begin(); iter!=m_childs.end(); ++iter)
	{
		eEvalState state = (*iter)->Evaluate(pInfo, retBehavior);
		if(state != eEvalState_Running)
			return state;
	}

	return eEvalState_Running;
}

/////////////////////////////////////////////////////////////////////
aiBTActionNode::aiBTActionNode()
:m_behaviorName(Ogre::StringUtil::BLANK)
{

}

eEvalState aiBTActionNode::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
{
	//返回行为..
	retBehavior = m_behaviorName;

	return eEvalState_Success;
}

bool aiBTActionNode::Validate()
{
	if(!m_childs.empty())
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Action node should be leaf node!", "Evaluate");
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////
aiBTConditionNode::aiBTConditionNode()
:m_conditions(Ogre::StringUtil::BLANK)
,m_pHandler(nullptr)
{

}

eEvalState aiBTConditionNode::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
{
	//执行条件判定
	if (m_conditions.empty() || m_pHandler->Evaluate(*pInfo))
	{
		return m_childs.front()->Evaluate(pInfo, retBehavior);
	}

	return eEvalState_Running;
}

bool aiBTConditionNode::Validate()
{
	if(m_childs.size() != 1)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Condition node should has exactly ONE child!", "Evaluate");
		return false;
	}
	return true;
}

void aiBTConditionNode::SetConditions( const STRING& con, aiBlackBoard* pTmplBB )
{
	SAFE_DELETE(m_pHandler);
	m_pHandler = new aiBehaviorConditon(con, *pTmplBB);
	assert(m_pHandler->Valid());
	m_conditions = con;
}
