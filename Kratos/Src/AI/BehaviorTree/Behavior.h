/********************************************************************
	created:	28:3:2013   1:35
	filename	Behavior.h
	author:		maval

	purpose:	Behavior表示ActionNode执行的一个行为.
				所有可能的行为都定义在这里,单位任何时刻一定处于某一个行为中
*********************************************************************/

#ifndef Behavior_h__
#define Behavior_h__

#include "KratosPrerequisites.h"

namespace Ogre
{
	class Any;
}

///行为基类
class aiBehavior
{
public:
	aiBehavior() {}
	virtual ~aiBehavior() {}

public:
	virtual	void	Execute(Ogre::Any& owner) = 0;

private:
	aiBehavior(const aiBehavior&);
	aiBehavior& operator= (const aiBehavior&);
};

#endif // Behavior_h__