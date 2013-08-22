#ifndef APPLICATION_H
#define APPLICATION_H


#include "KratosPrerequisites.h"


/************************************************************************/
/*							应用程序类									*/
/************************************************************************/
class Applicaton
{
public:
	Applicaton();
	~Applicaton();

public:
	bool	Init();
	void	Run();
	void	Shutdown();

private:
	Kratos::CAppStateManager*	m_stateMgr;
	Kratos::CInputManager*		m_inputMgr;
	Kratos::COgreManager*		m_ogreMgr;
	Kratos::CGUIManager*		m_guiMgr;
	Kratos::CPhysicManager*		m_phyMgr;
};


#endif