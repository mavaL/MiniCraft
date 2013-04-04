#include "stdafx.h"
#include "ManipulatorGameData.h"
#include "Utility.h"
#include "AI/BehaviorTree/BehaviorTreeTemplateManager.h"
#include "AI/BehaviorTree/BehaviorTreeNode.h"
#include "AI/BehaviorTree/BehaviorTreeTemplate.h"
#include "AI/BehaviorTree/BehaviorTree.h"
#include "AI/BehaviorTree/BlackBoard.h"
#include "ScriptSystem.h"

ManipulatorGameData::ManipulatorGameData()
{
	m_dataMgr = GameDataDefManager::GetSingletonPtr();
	SCRIPTNAMAGER.Init();
}

ManipulatorGameData::~ManipulatorGameData()
{
	SCRIPTNAMAGER.Shutdown();

	for (size_t i=0; i<m_btTemplates.size(); ++i)
		std::for_each(m_btTemplates[i].m_nodeList.begin(), m_btTemplates[i].m_nodeList.end(), std::default_delete<BTTemplate::SBTNode>());
	m_btTemplates.clear();
}

void ManipulatorGameData::LoadAllXml()
{
	m_dataMgr->LoadAllData();
	_ParseAllBTTemplates();
}

void ManipulatorGameData::SaveAllXml()
{
	m_dataMgr->SaveAllData();
}

std::vector<std::wstring> ManipulatorGameData::GetRaceBuildingNames( eGameRace race ) const
{
	std::vector<std::wstring> retNames;
	for (auto iter=m_dataMgr->m_buildingData.begin(); iter!=m_dataMgr->m_buildingData.end(); ++iter)
	{
		SBuildingData& data = iter->second;
		int iRace = (eGameRace)Ogre::StringConverter::parseInt(data.params["race"]);
		if((eGameRace)iRace == race)
			retNames.push_back(Utility::EngineToUnicode(iter->first));
	}

	return std::move(retNames);
}

SBuildingData* ManipulatorGameData::GetBuildingData( const std::wstring& name )
{
	auto iter = m_dataMgr->m_buildingData.find(Utility::UnicodeToEngine(name));
	assert(iter != m_dataMgr->m_buildingData.end());
	return &(iter->second);
}

std::vector<std::wstring> ManipulatorGameData::GetAbilityNames() const
{
	std::vector<std::wstring> retNames;
	retNames.resize(m_dataMgr->m_abilityData.size());

	int i = 0;
	for (auto iter=m_dataMgr->m_abilityData.begin(); iter!=m_dataMgr->m_abilityData.end(); ++iter)
		retNames[i++] = Utility::EngineToUnicode(iter->first);

	return std::move(retNames);
}

const SAbilityData* ManipulatorGameData::GetAbilityData( const std::wstring& name )
{
	auto iter = m_dataMgr->m_abilityData.find(Utility::UnicodeToEngine(name));
	if(iter != m_dataMgr->m_abilityData.end())
		return &iter->second;
	else
		return nullptr;
}

void ManipulatorGameData::SetBuildingAbility( const std::wstring& buildingName, int slotIndex, const std::wstring& abilName )
{
	assert(slotIndex >= 0 && slotIndex < 15);
	(const_cast<SBuildingData*>(GetBuildingData(buildingName)))->m_vecAbilities[slotIndex] = Utility::UnicodeToEngine(abilName);
}

SUnitData* ManipulatorGameData::GetUnitData( const std::string& meshname )
{
	UnitTable& units = GameDataDefManager::GetSingleton().m_unitData;

	for(auto iter=units.begin(); iter!=units.end(); ++iter)
	{
		SUnitData& unitData = iter->second;
		if(unitData.params["meshname"] == meshname)
			return &unitData;
	}
	
	return nullptr;
}

std::vector<std::wstring> ManipulatorGameData::GetAllBehaviorTreeTemplateNames() const
{
	auto names = Kratos::aiBehaviorTreeTemplateManager::GetSingleton().GetAllTemplateNames();
	std::vector<std::wstring> ret(names.size());
	for(size_t i=0; i<names.size(); ++i)
		ret[i] = Utility::EngineToUnicode(names[i]);

	return std::move(ret);
}

void ManipulatorGameData::_ParseAllBTTemplates()
{
	m_btTemplates.clear();
	Kratos::aiBehaviorTreeTemplateManager& btMgr = Kratos::aiBehaviorTreeTemplateManager::GetSingleton();
	auto names = btMgr.GetAllTemplateNames();
	m_btTemplates.resize(names.size());

	for (size_t i=0; i<names.size(); ++i)
	{
		BTTemplate& tmpl = m_btTemplates.at(i);
		Kratos::aiBehaviorTreeTemplate* pEngineTmpl = btMgr.GetTemplate(names[i]);
		tmpl.m_name = Utility::EngineToUnicode(names[i]);
		tmpl.race = (int)pEngineTmpl->GetBT()->GetRace();
		Kratos::aiBehaviorTreeNode* pNode = pEngineTmpl->GetBT()->GetRootNode();
		BTTemplate::SBTNode* root = new BTTemplate::SBTNode;
		_ParseBTNode(pNode, root, nullptr, tmpl);
	}
}

void ManipulatorGameData::_ParseBTNode( const Kratos::aiBehaviorTreeNode* pEngineNode, BTTemplate::SBTNode* pNode, 
	BTTemplate::SBTNode* parent, BTTemplate& tmpl )
{
	pNode->parent = parent;
	const STRING type = Kratos::aiBehaviorTreeNode::GetNodeTypeToStr(pEngineNode->GetType());
	pNode->type = Utility::EngineToUnicode(type);

	switch (pEngineNode->GetType())
	{
	case Kratos::eNodeType_Sequence: 
		{
			pNode->color = 0xffc080ff;
			pNode->txtProperty = L"";
		}
		break;

	case Kratos::eNodeType_Condition: 
		{
			pNode->color = 0xff008888;
			const Kratos::aiBTConditionNode* conditionNode = dynamic_cast<const Kratos::aiBTConditionNode*>(pEngineNode);
			pNode->txtProperty = Utility::EngineToUnicode(conditionNode->GetConditions());
		}
		break;

	case Kratos::eNodeType_Action: 
		{ 
			pNode->color = 0xff008800; 
			const Kratos::aiBTActionNode* actionNode = dynamic_cast<const Kratos::aiBTActionNode*>(pEngineNode);
			pNode->txtProperty = Utility::EngineToUnicode(actionNode->GetBehaviorName());
		} 
		break;
	}

	auto& childs = pEngineNode->GetChilds();
	for(auto iter=childs.begin(); iter!=childs.end(); ++iter)
	{
		BTTemplate::SBTNode* childNode = new BTTemplate::SBTNode;
		_ParseBTNode(*iter, childNode, pNode, tmpl);
		pNode->childs.push_back(childNode);
	}

	tmpl.m_nodeList.push_back(pNode);
}

ManipulatorGameData::BTTemplate& ManipulatorGameData::GetBTTemplate( const std::wstring& name )
{
	auto itTmpl= std::find_if(m_btTemplates.begin(), m_btTemplates.end(), [&](const ManipulatorGameData::BTTemplate& tmpl) { return tmpl.m_name == name; });
	assert(itTmpl != m_btTemplates.end());
	return *itTmpl;
}

std::vector<std::wstring> ManipulatorGameData::GetAllBehaviorNames() const
{
	std::vector<std::wstring> ret;
	ret.push_back(L"Idle");
	ret.push_back(L"MoveToEnemyBase");

	return std::move(ret);
}

void ManipulatorGameData::SaveAllBehaviorTreeTemplates()
{
	Ogre::StringVectorPtr loc = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("BehaviorTemplate", "*Behaviors");

	for (size_t i=0; i<m_btTemplates.size(); ++i)
	{
		STRING filepath = loc->at(0) + "\\" + Utility::UnicodeToEngine(m_btTemplates[i].m_name) + ".xml";
		_SaveBTTemplate(m_btTemplates.at(i), filepath);
	}
}

void ManipulatorGameData::_SaveBTTemplate( const BTTemplate& tmpl, const STRING& filepath )
{
	using namespace rapidxml;

	xml_document<> doc;  
	xml_node<>* header = doc.allocate_node(rapidxml::node_pi, "xml version='1.0' encoding='utf-8'");
	doc.append_node(header);
	xml_node<>* root = doc.allocate_node(node_element, "Root");
	doc.append_node(root);
	xml_node<>* tmplNode = doc.allocate_node(node_element, "BehaviorTemplate");
	root->append_node(tmplNode);

	STRING race;
	eGameRace eRace = (eGameRace)tmpl.race;
	if(eRace = eGameRace_Terran) race = "Terran";
	else if(eRace = eGameRace_Zerg) race = "Zerg";
	else assert(0);
	tmplNode->append_attribute(doc.allocate_attribute("name", doc.allocate_string(Utility::UnicodeToEngine(tmpl.m_name).c_str())));
	tmplNode->append_attribute(doc.allocate_attribute("race", race.c_str()));
	xml_node<>* btreeNode = doc.allocate_node(node_element, "BehaviorTree");
	tmplNode->append_node(btreeNode);

	//寻找根节点
	BTTemplate::SBTNode* node = tmpl.m_nodeList.front();
	while(node->parent)
		node = node->parent;

	//保存树结构
	_SaveBTNode(&doc, node->childs[0], btreeNode);

	//保存黑板
	Kratos::aiBehaviorTreeTemplate* engineTmpl = Kratos::aiBehaviorTreeTemplateManager::GetSingleton().GetTemplate(
		Utility::UnicodeToEngine(tmpl.m_name));
	xml_node<>* bbNode = doc.allocate_node(node_element, "BlackBoard");
	tmplNode->append_node(bbNode);
	const Kratos::aiBlackBoard::ParamMap& params = engineTmpl->GetBB()->GetParams();
	for (auto iter=params.begin(); iter!=params.end(); ++iter)
	{
		const Kratos::aiBlackBoard::SValue& val = iter->second;
		xml_node<>* varNode = doc.allocate_node(node_element, "Variable");
		bbNode->append_node(varNode);
		varNode->append_attribute(doc.allocate_attribute("name", iter->first.c_str()));
		varNode->append_attribute(doc.allocate_attribute("value", val.m_value.c_str()));

		STRING type;
		switch (val.m_type)
		{
		case Kratos::aiBlackBoard::eVarType_Int: type = "int"; break;
		case Kratos::aiBlackBoard::eVarType_Float: type = "float"; break;
		case Kratos::aiBlackBoard::eVarType_Bool: type = "bool"; break;
		default: assert(0);
		}
		varNode->append_attribute(doc.allocate_attribute("type", type.c_str()));
	}

	//保存脚本信息
	xml_node<>* scriptNode = doc.allocate_node(node_element, "Script");
	tmplNode->append_node(scriptNode);
	scriptNode->append_attribute(doc.allocate_attribute("filename", engineTmpl->GetBBScriptName().c_str()));
	scriptNode->append_attribute(doc.allocate_attribute("entry", engineTmpl->GetBBScriptEntry().c_str()));

	std::ofstream out(filepath);
	out << doc;
}

void ManipulatorGameData::_SaveBTNode( rapidxml::xml_document<>* doc, BTTemplate::SBTNode* node, rapidxml::xml_node<>* xmlNode )
{
	//自己
	rapidxml::xml_node<>* treeNode = nullptr;
	if(node->type == L"Sequence") 
	{
		treeNode = doc->allocate_node(rapidxml::node_element, "SequenceNode");
	}
	else if(node->type == L"Condition")
	{
		treeNode = doc->allocate_node(rapidxml::node_element, "ConditionNode");
		treeNode->append_attribute(doc->allocate_attribute("expression", doc->allocate_string(Utility::UnicodeToEngine(node->txtProperty).c_str())));
	}
	else if(node->type == L"Action")
	{
		treeNode = doc->allocate_node(rapidxml::node_element, "ActionNode");
		treeNode->append_attribute(doc->allocate_attribute("expression", doc->allocate_string(Utility::UnicodeToEngine(node->txtProperty).c_str())));
	}
	else 
	{
		assert(0);
	}
	xmlNode->append_node(treeNode);

	//子节点
	for(size_t i=0; i<node->childs.size(); ++i)
		_SaveBTNode(doc, node->childs[i], treeNode);
}

void ManipulatorGameData::ValidateBehaviorTemplate( const BTTemplate& tmpl )
{
	//先保存到临时xml中
	Ogre::StringVectorPtr loc = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("BehaviorTemplate", "*Behaviors");
	const STRING filepath = loc->at(0) + "\\TempBehaviorTreeTemplate.xml";
	_SaveBTTemplate(tmpl, filepath);

	//然后加载模板,进行校验
	Kratos::aiBehaviorTreeTemplate temp;
	temp.Load("TempBehaviorTreeTemplate.xml");

	try
	{
		temp.GetBT()->ValidateTree();
	} 
	catch(...) {}
	
	//最后删除临时xml
	Ogre::ResourceGroupManager::getSingleton().deleteResource("TempBehaviorTreeTemplate.xml", "BehaviorTemplate");
}
