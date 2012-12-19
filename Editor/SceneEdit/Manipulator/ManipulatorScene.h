/********************************************************************
	created:	14:12:2012   1:24
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorScene.h
	author:		maval
	
	purpose:	≥°æ∞º”‘ÿ,±£¥Ê
*********************************************************************/

#ifndef ManipulatorScene_h__
#define ManipulatorScene_h__

#include "../Singleton.h"

namespace Ogre
{
	class SceneManager;
}

class DotSceneSerialezer;
class DotSceneLoader;
class ManipulatorTerrain;
class ManipulatorObject;



class ManipulatorScene : public CSingleton<ManipulatorScene>
{
	ManipulatorScene();
	~ManipulatorScene();
	DECLEAR_SINGLETON(ManipulatorScene);

public:
	void	SceneNew(const std::wstring& sceneName);
	void	SceneOpen(const std::wstring& filepath);
	void	SceneSave();
	void	SceneClose();

	void	Shutdown();
	const std::wstring&	GetScenePath()	{ return m_scenePath; }
	const std::wstring&	GetSceneName()	{ return m_sceneName; }
	const std::wstring	GenerateSceneFullPath();
	ManipulatorTerrain&	GetTerrain()	{ return *m_manipulatorTerrain; }
	ManipulatorObject&	GetObject()		{ return *m_manipulatorObject; }

private:
	std::wstring		m_scenePath;
	std::wstring		m_sceneName;
	ManipulatorTerrain*	m_manipulatorTerrain;
	ManipulatorObject*	m_manipulatorObject;
	DotSceneSerialezer*	m_sceneSerializer;
	DotSceneLoader*		m_sceneLoader;
};


#endif // ManipulatorScene_h__







