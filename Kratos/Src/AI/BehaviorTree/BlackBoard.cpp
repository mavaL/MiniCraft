#include "BlackBoard.h"

aiBlackBoard::aiBlackBoard()
{
	m_params["true"] = SValue("true", eVarType_Bool);
	m_params["false"] = SValue("false", eVarType_Bool);
}

void aiBlackBoard::DefineParam( const STRING& name, const STRING& value, eVarType type )
{
	assert(m_params.find(name) == m_params.end() && "Param already defined!");
	m_params.insert(std::make_pair(name, SValue(value, type)));
}

void aiBlackBoard::Clone( aiBlackBoard& toClone )
{
	toClone.m_params = m_params;
}

const aiBlackBoard::SValue& aiBlackBoard::GetParam( const STRING& name ) const
{
	auto iter = m_params.find(name);
	assert(iter != m_params.end() && "Param doesn't exist!");
	return iter->second;
}

void aiBlackBoard::SetParam( const STRING& name, const STRING& value )
{
	auto iter = m_params.find(name);
	assert(iter != m_params.end() && "Param doesn't exist!");
	iter->second.m_value = value;
}
