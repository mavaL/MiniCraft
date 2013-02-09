/********************************************************************
	created:	3:2:2013   11:13
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\Command.h
	author:		maval
	
	purpose:	命令类体系.
*********************************************************************/

#ifndef Command_h__
#define Command_h__

#include "GameDefine.h"

class Object;
struct SAbilityData;

class CommandBase
{
public:
	CommandBase(Object* pOwner, const SAbilityData* param);
	virtual ~CommandBase() {}

public:
	virtual void	Excute() const = 0;

protected:
	Object*				m_pOwner;
	const SAbilityData*	m_pParam;
};

///操作命令:移动,攻击等
class CmdOperation : public CommandBase
{
public:
	CmdOperation(Object* pOwner, const SAbilityData* param);

public:
	virtual void	Excute() const {}
};

///生产命令
class CmdProduce : public CommandBase
{
public:
	CmdProduce(Object* pOwner, const SAbilityData* param);

public:
	virtual void	Excute() const;
};

#endif // Command_h__