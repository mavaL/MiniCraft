#include "BlackBoard.h"

aiBlackBoard::aiBlackBoard()
{
}

void aiBlackBoard::DefineParam( const STRING& name, const STRING& value )
{
	m_params.insert(std::make_pair(name, value));
}

void aiBlackBoard::Clone( aiBlackBoard& toClone )
{
	toClone.m_params = m_params;
}
