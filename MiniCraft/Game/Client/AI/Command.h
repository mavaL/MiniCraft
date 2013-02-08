/********************************************************************
	created:	3:2:2013   11:13
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\Command.h
	author:		maval
	
	purpose:	命令基类与基本命令:移动,攻击等.
*********************************************************************/

#ifndef Command_h__
#define Command_h__


enum eCommandType
{
	///!!!!这里要与UnitState.lua中的常量相对应
	eCommandType_Move,
	eCommandType_Attack,
	eCommandType_Harvest,
	eCommandType_Build,
	eCommandType_Ability,
	eCommandType_None
};

class Object;
/************************************************************************/
/*							操作命令基类                
/************************************************************************/

class CommandBase
{
public:
	CommandBase(eCommandType type);
	virtual ~CommandBase() {}
public:
	//virtual	void	Clone(const CommandBase& cmd) { m_type = cmd.GetType(); m_pOwner = cmd.GetOwner(); }
	virtual void	Excute() = 0;
public:
	eCommandType	GetType() const { return m_type; }
	Object*			GetOwner() const { return m_pOwner; }
// 	//取消命令,则转入空闲状态
// 	void			Cancel();

protected:
	eCommandType	m_type;
	Object*			m_pOwner;
};

// /************************************************************************/
// /*							移动命令类	                                */
// /************************************************************************/
// 
// class MoveCommand : public CommandBase
// {
// public:
// 	MoveCommand() {}
// 	MoveCommand(Unit* pOwner, const Ogre::Vector3& DestPos);
// 	virtual ~MoveCommand() {}
// public:
// 	virtual	void	Clone(const CommandBase& cmd);
// 	virtual void	Excute();
// public:
// 	const Ogre::Vector3&	GetDestPos() const { return m_destPos; }
// 
// private:
// 	Ogre::Vector3	m_destPos;
// };
// 
// /************************************************************************/
// /*							采集资源命令类	                            */
// /************************************************************************/
// 
// class HarvestCommand : public CommandBase
// {
// public:
// 	HarvestCommand() {}
// 	HarvestCommand(Unit* pOwner);
// 	virtual ~HarvestCommand() {}
// public:
// 	virtual	void			Clone(const CommandBase& cmd);
// 	virtual void			Excute();
// };
// 
// /************************************************************************/
// /*							建造命令类		                            */
// /************************************************************************/
// 
// class BuildCommand : public CommandBase
// {
// public:
// 	BuildCommand() {}
// 	BuildCommand(Unit* pOwner, const Ogre::Vector3& pos);
// 	virtual ~BuildCommand() {}
// public:
// 	virtual	void	Clone(const CommandBase& cmd);
// 	virtual void	Excute() {}
// public:
// 	const Ogre::Vector3&	GetBuildPos() const { return m_buildPos; }
// 
// private:
// 	Ogre::Vector3	m_buildPos;
// };
// 
// 
// /************************************************************************/
// /*							攻击命令类		                            */
// /************************************************************************/
// class Unit;
// 
// class AttackCommand : public CommandBase
// {
// public:
// 	AttackCommand() {}
// 	AttackCommand(Unit* pOwner, Unit* pTarget);
// 	virtual ~AttackCommand() {}
// public:
// 	virtual	void	Clone(const CommandBase& cmd);
// 	virtual void	Excute() {}
// public:
// 	Unit*	GetTarget() const { return m_pTarget; }
// 
// private:
// 	Unit*	m_pTarget;
// };


#endif // Command_h__