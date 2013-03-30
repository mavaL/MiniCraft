#include "BehaviorTreeTemplateManager.h"
#include "BehaviorTreeTemplate.h"
#include <OgreResourceGroupManager.h>
#include "Behavior.h"

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
}

void aiBehaviorTreeTemplateManager::LoadAll()
{
	Ogre::StringVectorPtr files = Ogre::ResourceGroupManager::getSingleton().findResourceNames("BehaviorTemplate", "*.xml");
	for (auto iter=files->begin(); iter!=files->end(); ++iter)
	{
		aiBehaviorTreeTemplate* pTmpl = new aiBehaviorTreeTemplate;
		const STRING name = pTmpl->Load(*iter);
		m_bts.insert(std::make_pair(name, pTmpl));
	}
}

void aiBehaviorTreeTemplateManager::SaveAll()
{
	//TODO
}

aiBehaviorTreeTemplate* aiBehaviorTreeTemplateManager::GetTemplate( const STRING& name )
{
	auto iter = m_bts.find(name);
	assert(iter != m_bts.end() && "No behavior tree template with this name!");
	return iter->second;
}

aiBehavior* aiBehaviorTreeTemplateManager::GetBehavior( const STRING& name )
{
	auto iter = m_behaviorLib.find(name);
	assert(iter != m_behaviorLib.end() && "No behavior with this name!");
	return iter->second;
}

void aiBehaviorTreeTemplateManager::AddBehavior( const STRING& name, aiBehavior* behav )
{
	assert(behav);
	m_behaviorLib.insert(std::make_pair(name, behav));
}
