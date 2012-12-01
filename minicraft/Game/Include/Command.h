#ifndef COMMAND_H
#define COMMAND_H


enum eCommandType
{
	eCommandType_Move,
	eCommandType_Attack,
	eCommandType_Harvest,
	eCommandType_Build,
	eCommandType_None
};

class Unit;
/************************************************************************/
/*							操作命令基类                
/************************************************************************/

class CommandBase
{
public:
	CommandBase() {}
	virtual ~CommandBase() {}
public:
	virtual	void	Clone(const CommandBase& cmd) { m_pOwner = cmd.GetOwner(); }
	virtual void	Excute() = 0;
public:
	eCommandType	GetType() const { return m_type; }
	Unit*			GetOwner() const { return m_pOwner; }
	//取消命令,则转入空闲状态
	void			Cancel();

protected:
	eCommandType	m_type;
	Unit*			m_pOwner;
};

/************************************************************************/
/*							移动命令类	                                */
/************************************************************************/

class MoveCommand : public CommandBase
{
public:
	MoveCommand() {}
	MoveCommand(Unit* pOwner, const Ogre::Vector3& DestPos);
	virtual ~MoveCommand() {}
public:
	virtual	void	Clone(const CommandBase& cmd);
	virtual void	Excute();
public:
	const Ogre::Vector3&	GetDestPos() const { return m_destPos; }

private:
	Ogre::Vector3	m_destPos;
};

/************************************************************************/
/*							采集资源命令类	                            */
/************************************************************************/

class HarvestCommand : public CommandBase
{
public:
	HarvestCommand() {}
	HarvestCommand(Unit* pOwner, const Ogre::Vector3& resPos);
	virtual ~HarvestCommand() {}
public:
	virtual	void			Clone(const CommandBase& cmd);
	virtual void			Excute() {}
public:
	const Ogre::Vector3&	GetResPos() const { return m_resPos; }

private:
	Ogre::Vector3	m_resPos;
};

/************************************************************************/
/*							建造命令类		                            */
/************************************************************************/

class BuildCommand : public CommandBase
{
public:
	BuildCommand() {}
	BuildCommand(Unit* pOwner, const Ogre::Vector3& pos);
	virtual ~BuildCommand() {}
public:
	virtual	void	Clone(const CommandBase& cmd);
	virtual void	Excute() {}
public:
	const Ogre::Vector3&	GetBuildPos() const { return m_buildPos; }

private:
	Ogre::Vector3	m_buildPos;
};


/************************************************************************/
/*							攻击命令类		                            */
/************************************************************************/
class Unit;

class AttackCommand : public CommandBase
{
public:
	AttackCommand() {}
	AttackCommand(Unit* pOwner, Unit* pTarget);
	virtual ~AttackCommand() {}
public:
	virtual	void	Clone(const CommandBase& cmd);
	virtual void	Excute() {}
public:
	Unit*	GetTarget() const { return m_pTarget; }

private:
	Unit*	m_pTarget;
};


#endif