/********************************************************************
	created:	14:12:2012   0:52
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorTerrain.h
	author:		maval
	
	purpose:	地形操作类.
*********************************************************************/
#ifndef ManipulatorTerrain_h__
#define ManipulatorTerrain_h__

namespace Ogre
{
	class Terrain;
	class TerrainGlobalOptions;
	class TerrainGroup;
	class SceneManager;
}
using Ogre::Terrain;
using Ogre::TerrainGlobalOptions;
using Ogre::TerrainGroup;


class ManipulatorTerrain
{
public:
	ManipulatorTerrain();
	~ManipulatorTerrain() {}

public:
	void	NewFlatTerrain();
	void	Shutdown();
	void	Load(rapidxml::xml_node<>* XMLNode);
	void	Serialize(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* XMLNode);

private:
	void	_ConfigureTerrainDefaults();

private:
	std::unique_ptr<TerrainGroup>			m_terrainGroup;
	std::unique_ptr<TerrainGlobalOptions>	m_terrainOption;
	Ogre::SceneManager*						m_pSceneMgr;
};





#endif // ManipulatorTerrain_h__







