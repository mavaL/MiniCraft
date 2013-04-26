#include "stdafx.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "BehaviorTreeTemplateManager.h"

namespace Kratos
{
	aiBlackBoard::aiBlackBoard(aiBehaviorTree* parent)
		:m_parent(parent)
	{
		m_params["true"] = SValue("true", eVarType_Bool, false);
		m_params["false"] = SValue("false", eVarType_Bool, false);
	}

	aiBlackBoard::aiBlackBoard()
		:m_parent(nullptr)
	{

	}

	void aiBlackBoard::DefineParam( const STRING& name, const STRING& value, eVarType type )
	{
		assert(m_params.find(name) == m_params.end() && "Param already defined!");
		m_params.insert(std::make_pair(name, SValue(value, type, true)));
	}

	void aiBlackBoard::Clone( aiBlackBoard& toClone )
	{
		toClone.m_params = m_params;
		toClone.m_parent = m_parent;
	}

	aiBlackBoard::SValue& aiBlackBoard::GetParam( const STRING& name )
	{
		assert(IsParamExists(name) && "Param doesn't exist!");
		return m_params.find(name)->second;
	}

	bool aiBlackBoard::IsParamExists( const STRING& name ) const
	{
		return m_params.find(name) != m_params.end();
	}

	void aiBlackBoard::SetParam( const STRING& name, const STRING& value )
	{
		assert(IsParamExists(name) && "Param doesn't exist!");
		m_params.find(name)->second.m_value = value;
	}

	bool aiBlackBoard::IsGlobalParam( const STRING& name ) const
	{
		if (IsParamExists(name))
		{
			return false;
		}
		else
		{
			aiBlackBoard* global = aiBehaviorTreeTemplateManager::GetSingleton().GetGlobalBB(m_parent->GetRace());
			assert(global->IsParamExists(name));
			return true;
		}	
	}

	void aiBlackBoard::LoadParams( rapidxml::xml_node<>* node )
	{
		while(node)
		{
			const char* name = node->first_attribute("name")->value();
			const char* value = node->first_attribute("value")->value();
			const char* type = node->first_attribute("type")->value();

			aiBlackBoard::eVarType varType;
			if(strcmp(type, "int") == 0) varType = aiBlackBoard::eVarType_Int;
			else if(strcmp(type, "float") == 0) varType = aiBlackBoard::eVarType_Float;
			else if(strcmp(type, "bool") == 0) varType = aiBlackBoard::eVarType_Bool;
			else assert(0);

			DefineParam(name, value, varType);

			node = node->next_sibling();
		}
	}

	void aiBlackBoard::RemoveParam( const STRING& name )
	{
		assert(IsParamExists(name));
		m_params.erase(name);
	}

}

