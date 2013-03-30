/********************************************************************
	created:	27:3:2013   22:34
	filename	BehaviorTreeManager.h
	author:		maval

	purpose:	行为树模板管理器
*********************************************************************/

#ifndef BehaviorTreeTemplateManager_h__
#define BehaviorTreeTemplateManager_h__

#include "Singleton.h"
#include "KratosPrerequisites.h"

class aiBehaviorTreeTemplate;
class aiBehavior;

class aiBehaviorTreeTemplateManager : public CSingleton<aiBehaviorTreeTemplateManager>
{
	DECLEAR_SINGLETON(aiBehaviorTreeTemplateManager);
	aiBehaviorTreeTemplateManager();
	~aiBehaviorTreeTemplateManager();

public:
	void					LoadAll();
	void					SaveAll();
	void					AddBehavior(const STRING& name, aiBehavior* behav);
	aiBehaviorTreeTemplate*	GetTemplate(const STRING& name);
	aiBehavior*				GetBehavior(const STRING& name);

private:
	typedef HashMap<STRING, aiBehaviorTreeTemplate*>	BTLib;
	BTLib			m_bts;

	typedef HashMap<STRING, aiBehavior*>	BehaviorLib;
	BehaviorLib		m_behaviorLib;
};

#endif // BehaviorTreeTemplateManager_h__