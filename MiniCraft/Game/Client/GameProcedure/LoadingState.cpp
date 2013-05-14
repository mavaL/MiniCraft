#include "stdafx.h"
#include "LoadingState.h"
#include "LoginState.h"
#include "BattleState.h"
#include "GUIManager.h"
#include "OgreManager.h"
#include "DeferredShading/DeferredShading.h"

STRING LoadingState::StateName = "LoadingState";



LoadingState::LoadingState()
:m_pLayout(nullptr)
,m_pLoadingBar(nullptr)
,m_scriptParsed(0)
,m_totalScriptCount(0)
{

}

void LoadingState::enter()
{
	Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener(this);
	m_pLayout = GUIMANAGER.LoadWindowLayout("Loading.layout");
	GUIMANAGER.SetGUISheet(m_pLayout);

	m_pLoadingBar = static_cast<CEGUI::ProgressBar*>(m_pLayout->getChild("LoadingBar"));
	m_pLoadingBar->setProgress(0.0f);

	m_totalScriptCount = _GetScriptsCount();

	//开始解析所有资源脚本
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	RenderManager.m_pDS->initialize();
	RenderManager.m_pDS->setActive(false);

	changeAppState(findByName(CBattleState::StateName));
}

void LoadingState::exit()
{
	Ogre::ResourceGroupManager::getSingleton().removeResourceGroupListener(this);
	GUIMANAGER.UnloadWindowLayout(m_pLayout);
	GUIMANAGER.SetGUISheet(nullptr);
}

void LoadingState::update( float timeSinceLastFrame )
{
	
}

void LoadingState::scriptParseEnded( const Ogre::String& scriptName, bool skipped )
{
	//TODO: 后台加载最好..
	++m_scriptParsed;
	assert(m_scriptParsed <= m_totalScriptCount);

	float fProgress = m_scriptParsed / (float)m_totalScriptCount;
	m_pLoadingBar->setProgress(fProgress);

	RenderManager.Update(1/60.0f);
}

int LoadingState::_GetScriptsCount()
{
	using namespace Ogre;

	// Count up the number of scripts we have to parse
	typedef list<FileInfoListPtr>::type FileListList;
	typedef SharedPtr<FileListList> FileListListPtr;
	size_t scriptCount = 0;

	ResourceGroupManager& resMgr = ResourceGroupManager::getSingleton();
	const StringVector resGroupNames = resMgr.getResourceGroups();
	auto& scriptLoaderMap = resMgr._getScriptLoaderMap();

	for (size_t iGroup=0; iGroup<resGroupNames.size(); ++iGroup)
	{
		for(auto oi=scriptLoaderMap.begin(); oi!=scriptLoaderMap.end(); ++oi)
		{
			ScriptLoader* su = oi->second;
			// MEMCATEGORY_GENERAL is the only category supported for SharedPtr
			FileListListPtr fileListList(OGRE_NEW_T(FileListList, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);

			// Get all the patterns and search them
			const StringVector& patterns = su->getScriptPatterns();
			for (StringVector::const_iterator p = patterns.begin(); p != patterns.end(); ++p)
			{
				FileInfoListPtr fileList = resMgr.findResourceFileInfo(resGroupNames[iGroup], *p);
				scriptCount += fileList->size();
				fileListList->push_back(fileList);
			}
		}
	}

	return scriptCount;
}
