/********************************************************************
	created:	28:3:2013   21:41
	filename	ConcreteBehavior.h
	author:		maval

	purpose:	行为树具体逻辑行为,相当逻辑相关的东西
*********************************************************************/

#ifndef ConcreteBehavior_h__
#define ConcreteBehavior_h__

#include "Behavior.h"

///空闲
class aiBehaviorIdle : public Kratos::aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
	virtual void	Exit(Ogre::Any& owner);
};

///向敌人基地移动
class aiBehaviorMoveToEnemyBase : public Kratos::aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
	virtual	void	Update(Ogre::Any& owner, float dt);
	virtual void	Exit(Ogre::Any& owner);
};

///向主基地移动返还资源
class aiBehaviorMoveToBase : public Kratos::aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
	virtual	void	Update(Ogre::Any& owner, float dt);
	virtual void	Exit(Ogre::Any& owner);
};

///向可采集资源移动
class aiBehaviorMoveToRes : public Kratos::aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
	virtual	void	Update(Ogre::Any& owner, float dt);
	virtual void	Exit(Ogre::Any& owner);
};

///采集资源
class aiBehaviorGathering : public Kratos::aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
	virtual	void	Update(Ogre::Any& owner, float dt);
	virtual void	Exit(Ogre::Any& owner);
};

///获得资源
class aiBehaviorRetriveRes : public Kratos::aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
};

///返还资源
class aiBehaviorReturnRes : public Kratos::aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
};

#endif // ConcreteBehavior_h__