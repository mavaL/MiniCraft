/********************************************************************
	created:	3:2:2013   11:13
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\Command.h
	author:		maval
	
	purpose:	ÃüÁîÀà.
*********************************************************************/

#ifndef Command_h__
#define Command_h__

class SelectableObject;

enum eCommandType
{
	eCommandType_Produce		//Éú²úÃüÁî
};

class Command
{
public:
	Command(eCommandType type, SelectableObject* pOwner);
	~Command() {}

public:
	void	Excute();

protected:
	SelectableObject*	m_pOwner;
	eCommandType		m_type;
};

#endif // Command_h__