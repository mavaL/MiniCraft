/********************************************************************
	created:	30:4:2013   21:47
	filename	EventCallback.h
	author:		maval

	purpose:	事件回调类
*********************************************************************/

#ifndef EventCallback_h__
#define EventCallback_h__

#include "EventCallbackBase.h"

////////////////////////////////////////////////////////////////////////
/// 单位相关的回调事件
class Unit;

class UnitEventCallback : public Kratos::EventCallbackBase
{
public:
	virtual	void	OnUnitDeath(Unit* pUnit) {}
};

#endif // EventCallback_h__