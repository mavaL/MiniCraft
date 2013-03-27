#include "BehaviorTree.h"
#include <OgreException.h>

aiBehaviorTree::aiBehaviorTree( const STRING& name )
:m_name(name)
{
	m_root = new aiBTConditionNode;
}

aiBehaviorTree::~aiBehaviorTree()
{
	SAFE_DELETE(m_root);
}

bool aiBehaviorTree::Evaluate(aiBlackBoard* pInfo, STRING& retBehavior)
{
	try
	{
		return m_root->Evaluate(pInfo, retBehavior);	
	}
	catch(const Ogre::Exception&)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, 
			"Error occurs when executing the BTree: " + m_name, "Execute");
	}
}
