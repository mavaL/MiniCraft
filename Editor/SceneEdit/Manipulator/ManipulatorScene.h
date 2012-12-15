/********************************************************************
	created:	14:12:2012   1:24
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorScene.h
	author:		maval
	
	purpose:	场景加载,保存
*********************************************************************/

#ifndef ManipulatorScene_h__
#define ManipulatorScene_h__

#include "../Singleton.h"

namespace Ogre
{
	class SceneManager;
}

class ManipulatorTerrain;
class DotSceneSerialezer;
class DotSceneLoader;

//当前.scene版本号
const	float	SCENE_VERSION	=	0.1f;
//默认场景管理器名字
const	std::string	SCENE_MANAGER_NAME	=	"DefaultSceneMgr";

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
	const std::wstring GenerateSceneFullPath();
	ManipulatorTerrain&	GetTerrain()	{ return *m_manipulatorTerrain; }

private:
	std::wstring		m_scenePath;
	std::wstring		m_sceneName;
	ManipulatorTerrain*	m_manipulatorTerrain;
	DotSceneSerialezer*	m_sceneSerializer;
	DotSceneLoader*		m_sceneLoader;
};


#endif // ManipulatorScene_h__







