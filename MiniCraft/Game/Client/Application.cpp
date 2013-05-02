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
	m_phyMgr = Kratos::CPhysicManager::GetSingletonPtr();

	if(	!m_ogreMgr->Init(false)	|| 
		!m_inputMgr->Init()		||
		!m_guiMgr->Init()		||
		!m_phyMgr->Init()		)
		return false;

	CBattleState::create(m_stateMgr, CBattleState::StateName);
	LoginState::create(m_stateMgr, LoginState::StateName);

	SCRIPTNAMAGER.Init();

	return true;
}

void Applicaton::Run()
{
	m_stateMgr->changeAppState(m_stateMgr->findByName(LoginState::StateName));

	float timeSinceLastFrame = 1;
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
			m_phyMgr->Update();
			if(!m_stateMgr->UpdateCurrentState(timeSinceLastFrame))
				break;
			if(!m_ogreMgr->Update(timeSinceLastFrame))
				break;

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
	SCRIPTNAMAGER.Shutdown();
	m_phyMgr->Shutdown();
	m_guiMgr->Shutdown();
	m_stateMgr->shutdown();	
	m_inputMgr->Shutdown();
	m_ogreMgr->Shutdown();
}


