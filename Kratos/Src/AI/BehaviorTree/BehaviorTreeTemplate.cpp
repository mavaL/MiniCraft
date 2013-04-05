#include "BehaviorTreeTemplate.h"
#include "BehaviorTree.h"
#include "BlackBoard.h"
#include <rapidxml_utils.hpp>
#include <rapidxml_print.hpp>
#include <OgreResourceGroupManager.h>
#include "ScriptSystem.h"

namespace Kratos
{
	aiBehaviorTreeTemplate::aiBehaviorTreeTemplate()
		:m_pBT(nullptr)
		,m_pBB(nullptr)
		,m_BBScriptEntry(Ogre::StringUtil::BLANK)
		,m_BBScriptName(Ogre::StringUtil::BLANK)
	{

	}

	aiBehaviorTreeTemplate::~aiBehaviorTreeTemplate()
	{
		SAFE_DELETE(m_pBT);
		SAFE_DELETE(m_pBB);
	}

	const STRING aiBehaviorTreeTemplate::Load( const STRING& filename )
	{
		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename, "BehaviorTemplate");
		char* szData = strdup(stream->getAsString().c_str());

		rapidxml::xml_document<> XMLDoc;
		XMLDoc.parse<0>(szData);
		rapidxml::xml_node<>* pNode = XMLDoc.first_node("Root")->first_node("BehaviorTemplate");

		SAFE_DELETE(m_pBT);
		SAFE_DELETE(m_pBB);

		//加载行为树
		const STRING tmplName = pNode->first_attribute("name")->value();
		const STRING raceName = pNode->first_attribute("race")->value();

		eGameRace race;
		if(raceName == "Terran") race = eGameRace_Terran;
		else if(raceName == "Zerg") race = eGameRace_Zerg;
		else assert(0);

		m_pBT = new aiBehaviorTree(race);
		m_pBB = new aiBlackBoard(m_pBT);

		//加载黑板
		rapidxml::xml_node<>* pBBNode = pNode->first_node("BlackBoard");
		assert(pBBNode && "The Behavior tree blackboard xml is broken!");
		rapidxml::xml_node<>* pVarNode = pBBNode->first_node("Variable");

		m_pBB->LoadParams(pVarNode);

		//加载行为树节点
		rapidxml::xml_node<>* pTreeNode = pNode->first_node("BehaviorTree")->first_node();
		assert(pTreeNode && "The Behavior tree xml is broken!");

		_LoadTreeNode(pTreeNode, m_pBT->GetRootNode());

		//脚本
		Ogre::StringVectorPtr loc = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("BehaviorTemplate", "*Behaviors");
		const STRING path = loc->at(0) + "\\Script\\";

		rapidxml::xml_node<>* pScriptNode = pNode->first_node("Script");
		m_BBScriptName = pScriptNode->first_attribute("filename")->value();
		SCRIPTNAMAGER.DoFile(path+m_BBScriptName);
		m_BBScriptEntry = pScriptNode->first_attribute("entry")->value();

		free(szData);
		XMLDoc.clear();

		return tmplName;
	}

	void aiBehaviorTreeTemplate::CloneBlackBoard( aiBlackBoard& toClone )
	{
		m_pBB->Clone(toClone);
	}

	void aiBehaviorTreeTemplate::_LoadTreeNode( rapidxml::xml_node<>* node, aiBehaviorTreeNode* parent )
	{
		//self
		const STRING type = node->name();
		aiBehaviorTreeNode* newNode = nullptr;
		eNodeType etype = aiBehaviorTreeNode::GetNodeTypeFromStr(type);

		switch (etype)
		{
		case eNodeType_Sequence:
			{
				aiBTSequenceNode* seqNode = new aiBTSequenceNode;
				newNode = seqNode;
			}
			break;
		case eNodeType_Condition:
			{
				aiBTConditionNode* conditionNode = new aiBTConditionNode;
				auto attr = node->first_attribute("expression");
				if (attr)
				{
					conditionNode->SetConditions(attr->value(), m_pBB);
				}
				newNode = conditionNode;
			}
			break;
		case eNodeType_Action:
			{
				aiBTActionNode* actionNode = new aiBTActionNode;
				const STRING behavior = node->first_attribute("behavior")->value();
				actionNode->SetBehaviorName(behavior);
				newNode = actionNode;
			}
			break;
		default: assert(0);
		}

		parent->AddChild(newNode);

		//child
		rapidxml::xml_node<>* child = node->first_node();
		while(child)
		{
			_LoadTreeNode(child, newNode);
			child = child->next_sibling();
		}
	}
}

