/********************************************************************
	created:	14:12:2012   1:24
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorScene.h
	author:		maval
	
	purpose:	场景加载,保存
*********************************************************************/

#ifndef ManipulatorScene_h__
#define ManipulatorScene_h__

#include "../Singleton.h"
#include "ManipulatorObject.h"
#include "ManipulatorTerrain.h"

namespace Ogre
{
	class SceneManager;
}

class DotSceneSerialezer;
class DotSceneLoader;


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

	bool	GetIsSceneReady()	{ return m_bIsSceneReay; }
	//重新绑定SceneNode(比如切换了场景)
	void	OnGizmoNodeReset();

	void	Shutdown();
	const std::wstring&	GetScenePath()	{ return m_scenePath; }
	const std::wstring&	GetSceneName()	{ return m_sceneName; }
	const std::wstring	GenerateSceneFullPath();

	ManipulatorTerrain&	GetTerrain()	{ return *m_manipulatorTerrain; }
	ManipulatorObject&	GetObject()		{ return *m_manipulatorObject; }

private:
	std::wstring		m_scenePath;
	std::wstring		m_sceneName;
	bool				m_bIsSceneReay;
	ManipulatorTerrain*	m_manipulatorTerrain;
	ManipulatorObject*	m_manipulatorObject;
	DotSceneSerialezer*	m_sceneSerializer;
	DotSceneLoader*		m_sceneLoader;
};

#define	ManipulatorSystem	ManipulatorScene::GetSingleton()

#endif // ManipulatorScene_h__







