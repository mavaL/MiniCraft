#include "stdafx.h"
#include "BehaviorTreeNode.h"
#include "BehaviorTreeCondition.h"
#include "BehaviorTreeTemplateManager.h"

namespace Kratos
{
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

	bool aiBTSequenceNode::Validate()
	{
		return __super::Validate();
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
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Action node should be leaf node!", "aiBTActionNode::Validate()");
			return false;
		}

		Kratos::aiBehaviorTreeTemplateManager& btMgr = Kratos::aiBehaviorTreeTemplateManager::GetSingleton();
		if (!btMgr.IsBehaviorExist(m_behaviorName))
		{
			STRING errMsg = m_behaviorName;
			errMsg += ", behavior doesn't exist!";
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, errMsg, "aiBTActionNode::Validate()");
			return false;
		}

		return __super::Validate();
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
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Condition node should has exactly ONE child!", "aiBTConditionNode::Validate()");
			return false;
		}
		else if (m_pHandler && !m_pHandler->Valid())
		{
			STRING errMsg = m_conditions;
			errMsg += ", conditon expression invalid!";
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, errMsg, "aiBTConditionNode::Validate()");
			return false;
		}

		return __super::Validate();
	}

	void aiBTConditionNode::SetConditions( const STRING& con, aiBlackBoard* pTmplBB )
	{
		SAFE_DELETE(m_pHandler);
		m_pHandler = new aiBehaviorConditon(con, *pTmplBB);
		m_conditions = con;
	}

	/////////////////////////////////////////////////////////////////////
	const STRING aiBehaviorTreeNode::GetNodeTypeToStr( eNodeType type )
	{
		switch (type)
		{
		case eNodeType_Sequence:	return "Sequence"; break;
		case eNodeType_Condition:	return "Condition"; break;
		case eNodeType_Action:		return "Action"; break;
		default: assert(0);			return "";
		}
	}

	Kratos::eNodeType aiBehaviorTreeNode::GetNodeTypeFromStr( const STRING& type )
	{
		if(type == "Sequence" || type == "SequenceNode")
			return eNodeType_Sequence;
		if(type == "Condition" || type == "ConditionNode")
			return eNodeType_Condition;
		if(type == "Action" || type == "ActionNode")
			return eNodeType_Action;
		
		assert(0);
		return (eNodeType)-1;
	}

	bool aiBehaviorTreeNode::Validate()
	{
		for(auto iter=m_childs.begin(); iter!=m_childs.end(); ++iter)
		{
			if(!(*iter)->Validate())
				return false;
		}
		return true;
	}

}

