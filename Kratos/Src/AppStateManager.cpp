#include "AppStateManager.h"

namespace Kratos
{
	CAppStateManager::CAppStateManager()
	{
		m_bShutdown = false;
	}

	CAppStateManager::~CAppStateManager()
	{
		shutdown();
	}

	void CAppStateManager::manageAppState(const std::string& stateName, CAppState* state)
	{
		try
		{
			state_info new_state_info;
			new_state_info.name = stateName;
			new_state_info.state = state;
			m_States.push_back(new_state_info);
		}
		catch(std::exception&)
		{
			// 		delete state;
			// 		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new CAppState\n" + Ogre::String(e.what()), "CAppStateManager.cpp (39)");
		}
	}

	CAppState* CAppStateManager::findByName(const std::string& stateName)
	{
		std::vector<state_info>::iterator itr;

		for(itr=m_States.begin();itr!=m_States.end();itr++)
		{
			if(itr->name==stateName)
				return itr->state;
		}

		return NULL;
	}

	void CAppStateManager::changeAppState(CAppState* state)
	{
		if(!m_ActiveStateStack.empty())
		{
			m_ActiveStateStack.back()->exit();
			m_ActiveStateStack.pop_back();
		}

		m_ActiveStateStack.push_back(state);
		_init(state);
		m_ActiveStateStack.back()->enter();
	}

	bool CAppStateManager::pushAppState(CAppState* state)
	{
		if(!m_ActiveStateStack.empty())
		{
			if(!m_ActiveStateStack.back()->pause())
				return false;
		}

		m_ActiveStateStack.push_back(state);
		_init(state);
		m_ActiveStateStack.back()->enter();

		return true;
	}

	void CAppStateManager::popAppState()
	{
		if(!m_ActiveStateStack.empty())
		{
			m_ActiveStateStack.back()->exit();
			m_ActiveStateStack.pop_back();
		}

		if(!m_ActiveStateStack.empty())
		{
			_init(m_ActiveStateStack.back());
			m_ActiveStateStack.back()->resume();
		}
		else
			shutdown();
	}

	void CAppStateManager::popAllAndPushAppState(CAppState* state)
	{
		while(!m_ActiveStateStack.empty())
		{
			m_ActiveStateStack.back()->exit();
			m_ActiveStateStack.pop_back();
		}

		pushAppState(state);
	}

	void CAppStateManager::pauseAppState()
	{
		if(!m_ActiveStateStack.empty())
		{
			m_ActiveStateStack.back()->pause();
		}

		if(m_ActiveStateStack.size() > 2)
		{
			_init(m_ActiveStateStack.at(m_ActiveStateStack.size() - 2));
			m_ActiveStateStack.at(m_ActiveStateStack.size() - 2)->resume();
		}
	}

	void CAppStateManager::shutdown()
	{
		state_info si;

		while(!m_ActiveStateStack.empty())
		{
			m_ActiveStateStack.back()->exit();
			m_ActiveStateStack.pop_back();
		}

		while(!m_States.empty())
		{
			si = m_States.back();
			si.state->destroy();
			m_States.pop_back();
		}

		m_bShutdown = true;
	}

	void CAppStateManager::_init(CAppState* state)
	{
		//CInputManager::GetSingleton().SetMouseEventListener(state);
		//OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(state);
		//OgreFramework::getSingletonPtr()->m_pRenderWnd->resetStatistics();
	}

	bool CAppStateManager::UpdateCurrentState( float dt )
	{
		if(!m_ActiveStateStack.empty())
			m_ActiveStateStack.back()->update(dt); 

		return !m_bShutdown;
	}

	CAppState* CAppStateManager::getCurState()
	{
		if(m_ActiveStateStack.empty())
			return NULL;

		return m_ActiveStateStack[m_ActiveStateStack.size()-1];
	}
}


