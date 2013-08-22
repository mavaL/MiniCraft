/********************************************************************
	created:	3:2:2013   11:13
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\Command.h
	author:		maval
	
	purpose:	√¸¡Ó¿‡.
*********************************************************************/

#ifndef Command_h__
#define Command_h__

#include "GameDefine.h"

class SelectableObject;

class Command
{
public:
	Command(eCommandType type, SelectableObject* pOwner);
	~Command() {}

public:
	void			Excute();
	eCommandType	GetType() const { return m_type; }

private:
	SelectableObject*	m_pOwner;
	eCommandType		m_type;
};

#endif // Command_h__