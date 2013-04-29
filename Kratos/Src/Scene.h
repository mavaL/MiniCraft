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
		void	New(const STRING& sceneName);
		void	Load(const STRING& sceneName, const STRING& sceneGroup, SceneSerializer* pHandler);
		void	Load(const STRING& fullPath, SceneSerializer* pHandler);
		void	Save(const STRING& fullPath, SceneSerializer* pHandler);
		void	Reset();

		const STRING&				GetSceneName() const { return m_sceneName; }
		Ogre::TerrainGroup*			GetTerrainGroup() { return m_terrainGroup; }
		Ogre::TerrainGlobalOptions* GetTerrainOption() { return m_terrainOption; }
		Ogre::Terrain*				GetTerrain() { return m_pTerrain; }
		size_t						GetTerrainMaxLayer() const { return 5; }
		const FLOAT3&				GetSunLightDirection() const { return m_sunLightDir; }
		const COLOR&				GetSunLightDiffuse() const { return m_sunLightDiffuse; }

	private:
		STRING						m_sceneName;
		Ogre::TerrainGroup*			m_terrainGroup;
		Ogre::TerrainGlobalOptions*	m_terrainOption;
		Ogre::Terrain*				m_pTerrain;
		FLOAT3						m_sunLightDir;			//全局光方向
		COLOR						m_sunLightDiffuse;		//全局光颜色
	};

}

#endif // Scene_h__