/********************************************************************
	created:	17:2:2013   0:48
	filename: 	F:\MiniCraft\Kratos\Src\Scene.h
	author:		maval
	
	purpose:	游戏场景类
*********************************************************************/

#ifndef Scene_h__
#define Scene_h__

#include "KratosPrerequisites.h"

namespace Kratos
{
	class Scene
	{
		friend class SceneSerializer;
	public:
		Scene();
		~Scene() {}

	public:
		void	New();
		void	Load(const STRING& sceneName, const STRING& sceneGroup, SceneSerializer* pHandler);
		void	Load(const STRING& fullPath, SceneSerializer* pHandler);
		void	Save(const STRING& fullPath, SceneSerializer* pHandler);
		void	Reset();

		Ogre::TerrainGroup*			GetTerrainGroup() { return m_terrainGroup; }
		Ogre::TerrainGlobalOptions* GetTerrainOption() { return m_terrainOption; }
		Ogre::Terrain*				GetTerrain() { return m_pTerrain; }
		size_t						GetTerrainMaxLayer() const { return 5; }
		const Ogre::Vector3&		GetSunLightDirection() const { return m_sunLightDir; }
		const Ogre::ColourValue&	GetSunLightDiffuse() const { return m_sunLightDiffuse; }

	private:
		Ogre::TerrainGroup*			m_terrainGroup;
		Ogre::TerrainGlobalOptions*	m_terrainOption;
		Ogre::Terrain*				m_pTerrain;
		Ogre::Vector3				m_sunLightDir;			//全局光方向
		Ogre::ColourValue			m_sunLightDiffuse;		//全局光颜色
	};

}

#endif // Scene_h__