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
#include "GameDefine.h"

namespace Kratos
{
	class aiBehaviorTreeTemplateManager : public CSingleton<aiBehaviorTreeTemplateManager>
	{
		DECLEAR_SINGLETON(aiBehaviorTreeTemplateManager);
		aiBehaviorTreeTemplateManager();
		~aiBehaviorTreeTemplateManager();

	public:
		void					LoadAll();
		void					UnloadAll();					

		void					AddBehavior(const STRING& name, aiBehavior* behav);
		aiBehavior*				GetBehavior(const STRING& name);
		bool					IsBehaviorExist(const STRING& name);

		void					AddTemplate(const STRING& name);
		aiBehaviorTreeTemplate*	GetTemplate(const STRING& name);
		Ogre::StringVector		GetAllTemplateNames() const;
		Ogre::StringVector		GetAllBehaviorNames() const;
		//获取种族全局黑板
		aiBlackBoard*			GetGlobalBB(eGameRace race) { return m_globalBBs[race]; }

	private:
		typedef HashMap<STRING, aiBehaviorTreeTemplate*>	BTLib;
		BTLib				m_bts;

		typedef HashMap<STRING, aiBehavior*>	BehaviorLib;
		BehaviorLib			m_behaviorLib;

		typedef HashMap<eGameRace, aiBlackBoard*>	GlobalBlackboards;
		GlobalBlackboards	m_globalBBs;
	};
}


#endif // BehaviorTreeTemplateManager_h__