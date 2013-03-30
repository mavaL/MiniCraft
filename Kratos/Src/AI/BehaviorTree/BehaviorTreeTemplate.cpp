#include "BehaviorTreeTemplate.h"
#include "BehaviorTree.h"
#include "BlackBoard.h"
#include <rapidxml_utils.hpp>
#include <rapidxml_print.hpp>
#include <OgreResourceGroupManager.h>

aiBehaviorTreeTemplate::aiBehaviorTreeTemplate()
:m_pBT(new aiBehaviorTree)
,m_pBB(new aiBlackBoard)
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

	//加载黑板
	rapidxml::xml_node<>* pBBNode = pNode->first_node("BlackBoard");
	assert(pBBNode && "The Behavior tree blackboard xml is broken!");
	rapidxml::xml_node<>* pVarNode = pBBNode->first_node("Variable");

	while(pVarNode)
	{
		const char* name = pVarNode->first_attribute("name")->value();
		const char* value = pVarNode->first_attribute("value")->value();
		const char* type = pVarNode->first_attribute("type")->value();

		aiBlackBoard::eVarType varType;
		if(strcmp(type, "int") == 0) varType = aiBlackBoard::eVarType_Int;
		else if(strcmp(type, "float") == 0) varType = aiBlackBoard::eVarType_Float;
		else if(strcmp(type, "bool") == 0) varType = aiBlackBoard::eVarType_Bool;
		else assert(0);

		m_pBB->DefineParam(name, value, varType);

		pVarNode = pVarNode->next_sibling();
	}

	//记载行为树
	const STRING tmplName = pNode->first_attribute("name")->value();
	rapidxml::xml_node<>* pTreeNode = pNode->first_node("BehaviorTree")->first_node();
	assert(pTreeNode && "The Behavior tree xml is broken!");

	_LoadTreeNode(pTreeNode, m_pBT->GetRootNode());
	m_pBT->ValidateTree();

	free(szData);
	XMLDoc.clear();

	return tmplName;
}

void aiBehaviorTreeTemplate::CloneBlackBoard( aiBlackBoard& toClone )
{
	//TODO: 应该有共享数据,每个单位一份占用太大
	m_pBB->Clone(toClone);
}

void aiBehaviorTreeTemplate::_LoadTreeNode( rapidxml::xml_node<>* node, aiBehaviorTreeNode* parent )
{
	//self
	const STRING type = node->name();
	aiBehaviorTreeNode* newNode = nullptr;
	if (type == "ConditionNode")
	{
		aiBTConditionNode* conditionNode = new aiBTConditionNode;
		auto attr = node->first_attribute("expression");
		if (attr)
		{
			conditionNode->SetConditions(attr->value(), m_pBB);
		}
		newNode = conditionNode;
	}
	else if (type == "SequenceNode")
	{
		aiBTSequenceNode* seqNode = new aiBTSequenceNode;
		newNode = seqNode;
	}
	else if (type == "ActionNode")
	{
		aiBTActionNode* actionNode = new aiBTActionNode;
		const STRING behavior = node->first_attribute("behavior")->value();
		actionNode->SetBehaviorName(behavior);
		newNode = actionNode;
	}
	else
	{
		assert(0);
	}

	parent->AddChild(newNode);

	//child
	rapidxml::xml_node<>* child = node->first_node();
	while(child)
	{
		_LoadTreeNode(child, newNode);
		child = child->next_sibling();
	}

	//sibling
	rapidxml::xml_node<>* bro = node->next_sibling();
	while(bro)
	{
		_LoadTreeNode(bro, parent);
		bro = bro->next_sibling();
	}
}
