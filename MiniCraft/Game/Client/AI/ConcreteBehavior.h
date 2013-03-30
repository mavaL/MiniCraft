/********************************************************************
	created:	28:3:2013   21:41
	filename	ConcreteBehavior.h
	author:		maval

	purpose:	行为树具体行为备用库
*********************************************************************/

#ifndef ConcreteBehavior_h__
#define ConcreteBehavior_h__

#include "Behavior.h"

///空闲行为
class aiBehaviorIdle : public aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
};

///向敌人基地移动行为
class aiBehaviorMoveToEnemyBase : public aiBehavior
{
public:
	virtual	void	Execute(Ogre::Any& owner);
};

#endif // ConcreteBehavior_h__