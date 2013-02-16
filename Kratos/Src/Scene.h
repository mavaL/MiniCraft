/********************************************************************
	created:	17:2:2013   0:48
	filename: 	F:\MiniCraft\Kratos\Src\Scene.h
	author:		maval
	
	purpose:	”Œœ∑≥°æ∞¿‡
*********************************************************************/

#ifndef Scene_h__
#define Scene_h__

#include <OgrePrerequisites.h>

namespace Ogre
{
	class TerrainGroup;
	class TerrainGlobalOptions;
	class Terrain;
}

class SceneSerializer;

class Scene
{
public:
	Scene();
	~Scene() {}

public:
	void	Load(const std::string& sceneName, const std::string& sceneGroup, SceneSerializer* pHandler);
	void	Load(const std::string& fullPath, SceneSerializer* pHandler);
	void	Save();
	void	Reset();

	void	_SetTerrainParam(Ogre::TerrainGroup* group, Ogre::TerrainGlobalOptions* option, Ogre::Terrain* terrain);
	Ogre::TerrainGroup* GetTerrainGroup() { return m_terrainGroup; }
	Ogre::TerrainGlobalOptions* GetTerrainOption() { return m_terrainOption; }
	Ogre::Terrain*		GetTerrain() { return m_pTerrain; }

private:
	Ogre::TerrainGroup*			m_terrainGroup;
	Ogre::TerrainGlobalOptions*	m_terrainOption;
	Ogre::Terrain*				m_pTerrain;
};

#endif // Scene_h__