#include "stdafx.h"
#include "Application.h"
#include "InputManager.h"
#include "OgreManager.h"
#include "GUIManager.h"
#include "AppStateManager.h"
#include "ScriptSystem.h"
#include "GameProcedure/BattleState.h"
#include "PhysicManager.h"
#include "GameProcedure/LoginState.h"
#include "GameProcedure/LoadingState.h"

Applicaton::Applicaton()
:m_stateMgr(nullptr)
,m_inputMgr(nullptr)
,m_ogreMgr(nullptr)
,m_guiMgr(nullptr)
,m_phyMgr(nullptr)
{

}

Applicaton::~Applicaton()
{
}

bool Applicaton::Init()
{
	m_stateMgr = Kratos::CAppStateManager::GetSingletonPtr();
	m_inputMgr = Kratos::CInputManager::GetSingletonPtr();
	m_ogreMgr =	Kratos::COgreManager::GetSingletonPtr();
	m_guiMgr = Kratos::CGUIManager::GetSingletonPtr();
#if USE_PHYSICS == 1
	m_phyMgr = Kratos::CPhysicManager::GetSingletonPtr();
#endif

	if(	!m_ogreMgr->Init(false)	|| 
		!m_inputMgr->Init()		||
		!m_guiMgr->Init()
#if USE_PHYSICS == 1
		|| !m_phyMgr->Init()
#endif
		)
		return false;

	CBattleState::create(m_stateMgr, CBattleState::StateName);
	LoginState::create(m_stateMgr, LoginState::StateName);
	LoadingState::create(m_stateMgr, LoadingState::StateName);

	SCRIPTNAMAGER.Init();

	return true;
}

void Applicaton::Run()
{
	m_stateMgr->changeAppState(m_stateMgr->findByName(LoadingState::StateName));

	static float timeSinceLastFrame = 0;
	int startTime = 0;
	//游戏主循环
	while(true)
	{
		if(m_ogreMgr->mWindow->isClosed())
			break;

		Ogre::WindowEventUtilities::messagePump();

		if(m_ogreMgr->mWindow->isActive())
		{
			startTime = m_ogreMgr->GetTimer()->getMillisecondsCPU();

			//各子系统进行更新
			m_inputMgr->Capture();
#if USE_PHYSICS == 1
			m_phyMgr->Update();
#endif

			if(!m_stateMgr->UpdateCurrentState(timeSinceLastFrame))
				break;

			if(!m_ogreMgr->Update(timeSinceLastFrame))
				break;

			m_guiMgr->Update(timeSinceLastFrame);

			timeSinceLastFrame = static_cast<float>(m_ogreMgr->GetTimer()->getMillisecondsCPU() - startTime);
			timeSinceLastFrame /= 1000.0f;
		}
		else
		{
			Sleep(1000);
		}
	}
}

void Applicaton::Shutdown()
{
	m_stateMgr->shutdown();	
	SCRIPTNAMAGER.Shutdown();
#if USE_PHYSICS == 1
	m_phyMgr->Shutdown();
#endif
	m_guiMgr->Shutdown();
	m_inputMgr->Shutdown();
	m_ogreMgr->Shutdown();
}


