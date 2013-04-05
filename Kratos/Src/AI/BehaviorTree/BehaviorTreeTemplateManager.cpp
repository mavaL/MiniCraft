#include "BehaviorTreeTemplateManager.h"
#include "BehaviorTreeTemplate.h"
#include <OgreResourceGroupManager.h>
#include "Behavior.h"
#include "BlackBoard.h"
#include "ScriptSystem.h"

namespace Kratos
{
	aiBehaviorTreeTemplateManager::aiBehaviorTreeTemplateManager()
	{
	}

	aiBehaviorTreeTemplateManager::~aiBehaviorTreeTemplateManager()
	{
		for(auto iter=m_bts.begin(); iter!=m_bts.end(); ++iter)
			delete iter->second;
		m_bts.clear();

		for(auto iter=m_behaviorLib.begin(); iter!=m_behaviorLib.end(); ++iter)
			delete iter->second;
		m_behaviorLib.clear();

		for(auto iter=m_globalBBs.begin(); iter!=m_globalBBs.end(); ++iter)
			delete iter->second;
		m_globalBBs.clear();
	}

	void aiBehaviorTreeTemplateManager::LoadAll()
	{
		Ogre::StringVectorPtr loc = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("BehaviorTemplate", "*Behaviors");
		const STRING filename = loc->at(0) + "\\Script\\";

		//加载全局黑板xml
		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("RaceGlobal.xml", "BehaviorTemplate");
		char* szData = strdup(stream->getAsString().c_str());

		rapidxml::xml_document<> XMLDoc;
		XMLDoc.parse<0>(szData);
		rapidxml::xml_node<>* pNode = XMLDoc.first_node("Root")->first_node("BlackBoard");

		while(pNode)
		{
			const STRING raceName = pNode->first_attribute("race")->value();
			rapidxml::xml_node<>* pVarNode = pNode->first_node("Variable");

			eGameRace race;
			if(raceName == "Terran") race = eGameRace_Terran;
			else if(raceName == "Zerg") race = eGameRace_Zerg;
			else assert(0);

			aiBlackBoard* bb = new aiBlackBoard;
			bb->LoadParams(pVarNode);
			m_globalBBs.insert(std::make_pair(race, bb));

			//脚本
			const char* szFilename = XMLDoc.first_node("Root")->first_node("Script")->first_attribute("filename")->value();
			const STRING filepath = filename + szFilename;
			SCRIPTNAMAGER.DoFile(filepath);

			pNode = pNode->next_sibling("BlackBoard");
		}

		free(szData);
		XMLDoc.clear();

		//加载各单位行为树xml
		Ogre::StringVectorPtr files = Ogre::ResourceGroupManager::getSingleton().findResourceNames("BehaviorTemplate", "*.xml");
		auto iter = std::find(files->begin(), files->end(), "RaceGlobal.xml");
		files->erase(iter);
		for (auto iter=files->begin(); iter!=files->end(); ++iter)
		{
			aiBehaviorTreeTemplate* pTmpl = new aiBehaviorTreeTemplate;
			const STRING name = pTmpl->Load(*iter);
			m_bts.insert(std::make_pair(name, pTmpl));
		}
	}

	aiBehaviorTreeTemplate* aiBehaviorTreeTemplateManager::GetTemplate( const STRING& name )
	{
		auto iter = m_bts.find(name);
		assert(iter != m_bts.end() && "No behavior tree template with this name!");
		return iter->second;
	}

	bool aiBehaviorTreeTemplateManager::IsBehaviorExist( const STRING& name )
	{
		auto iter = m_behaviorLib.find(name);
		return iter != m_behaviorLib.end();
	}

	aiBehavior* aiBehaviorTreeTemplateManager::GetBehavior( const STRING& name )
	{
		assert(IsBehaviorExist(name) && "No behavior with this name!");
		return m_behaviorLib[name];
	}

	void aiBehaviorTreeTemplateManager::AddBehavior( const STRING& name, aiBehavior* behav )
	{
		assert(behav);
		m_behaviorLib.insert(std::make_pair(name, behav));
	}

	Ogre::StringVector aiBehaviorTreeTemplateManager::GetAllTemplateNames() const
	{
		Ogre::StringVector ret(m_bts.size());
		int i = 0;
		for (auto iter=m_bts.begin(); iter!=m_bts.end(); ++iter)
			ret[i++] = iter->first;

		return std::move(ret);
	}

	Ogre::StringVector aiBehaviorTreeTemplateManager::GetAllBehaviorNames() const
	{
		Ogre::StringVector ret(m_behaviorLib.size());
		int i = 0;
		for (auto iter=m_behaviorLib.begin(); iter!=m_behaviorLib.end(); ++iter)
			ret[i++] = iter->first;

		return std::move(ret);
	}

}

