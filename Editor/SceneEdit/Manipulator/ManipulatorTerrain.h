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

#include "../Brush.h"


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

	void	OnGizmoNodeReset();
	float	GetHeightAt(const Ogre::Vector2& worldPos);
	bool	GetRayIntersectPoint(const Ogre::Ray& worldRay, Ogre::Vector3& retPt);

	void	SetTerrainModifyEnabled(bool bEnable);
	bool	GetTerrainModifyEnabled() { return m_brush[m_curBrushIndex]->GetActive(); }
	void	SetBrushPosition(const Ogre::Vector3& pos);
	void	SetSquareBrushWidth(float w);
	void	SetSquareBrushHeight(float h);
	float	GetSquareBrushWidth();
	float	GetSquareBrushHeight();	

private:
	void	_ConfigureTerrainDefaults();

private:
	std::unique_ptr<TerrainGroup>			m_terrainGroup;
	std::unique_ptr<TerrainGlobalOptions>	m_terrainOption;
	Ogre::SceneManager*						m_pSceneMgr;
	std::unique_ptr<Brush>					m_brush[2]; //0:cirle 1:square
	int										m_curBrushIndex;
};





#endif // ManipulatorTerrain_h__







