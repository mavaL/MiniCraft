/********************************************************************
	created:	4:5:2013   16:08
	filename	LoadingState.h
	author:		maval

	purpose:	×ÊÔ´¼ÓÔØ×´Ì¬
*********************************************************************/
#ifndef LoadingState_h__
#define LoadingState_h__

#include "AppState.h"
#include "KratosPrerequisites.h"

class LoadingState
	: public Kratos::CAppState
	, public Ogre::ResourceGroupListener
{
public:
	LoadingState();
	~LoadingState() {}

	DECLARE_APPSTATE_CLASS(LoadingState)

	virtual void enter();
	virtual void exit();
	virtual void update(float timeSinceLastFrame);

	static STRING StateName;

public:
	virtual void resourceGroupScriptingStarted(const STRING& groupName, size_t scriptCount) {}
	virtual void scriptParseStarted(const STRING& scriptName, bool& skipThisScript) {}
	virtual void scriptParseEnded(const STRING& scriptName, bool skipped);
	virtual void resourceGroupScriptingEnded(const STRING& groupName) {}
	virtual void resourceGroupLoadStarted(const STRING& groupName, size_t resourceCount) {}
	virtual void resourceLoadStarted(const Ogre::ResourcePtr& resource) {}
	virtual void resourceLoadEnded(void) {}
	virtual void worldGeometryStageStarted(const STRING& description) {}
	virtual void worldGeometryStageEnded(void) {}
	virtual void resourceGroupLoadEnded(const STRING& groupName) {}

private:
	int	_GetScriptsCount();

private:
	CEGUI::Window*		m_pLayout;
	CEGUI::ProgressBar*	m_pLoadingBar;
	int					m_scriptParsed;
	int					m_totalScriptCount;
};


#endif // LoadingState_h__