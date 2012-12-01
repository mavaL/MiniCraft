#ifndef APPLICATION_H
#define APPLICATION_H


class CAppStateManager;
class CInputManager;
class COgreManager;
class CGUIManager;


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
	CAppStateManager*	m_stateMgr;
	CInputManager*		m_inputMgr;
	COgreManager*		m_ogreMgr;
	CGUIManager*		m_guiMgr;
};


#endif