#include "BehaviorTreeTemplate.h"
#include "BehaviorTree.h"
#include "BlackBoard.h"
#include <rapidxml.hpp>    
#include <rapidxml_utils.hpp>
#include <rapidxml_print.hpp>
#include <OgreResourceGroupManager.h>

aiBehaviorTreeTemplate::aiBehaviorTreeTemplate()
:m_pBT(nullptr)
,m_params(nullptr)
{

}

void aiBehaviorTreeTemplate::Load( const STRING& filename )
{
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename, "BehaviorTemplate");
	char* szData = strdup(stream->getAsString().c_str());

	rapidxml::xml_document<> XMLDoc;
	XMLDoc.parse<0>(szData);
	
	//TODO:

	free(szData);
	XMLDoc.clear();
}

void aiBehaviorTreeTemplate::CloneBlackBoard( aiBlackBoard& toClone )
{
	m_params->Clone(toClone);
}
