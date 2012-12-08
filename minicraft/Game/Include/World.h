#ifndef WORLD_H
#define WORLD_H

#include "Singleton.h"


class Unit;
class OgreRecast;
class OgreDetourCrowd;
/************************************************************************/
/*								世界管理器                                */
/************************************************************************/

class World : public CSingleton<World>
{
	World();
	~World() {}
	DECLEAR_SINGLETON(World);
public:
	void	Init();
	void	Update(float dt);
	void	Shutdown();

	Unit*	CreateUnit();
	Unit*	GetUnitFromID(int ID);

	const Ogre::AxisAlignedBox&	GetResAABB() const { return m_pGold->getWorldBoundingBox(); }

private:
	std::vector<Unit*>			m_vecUnits;
	Ogre::SceneManager*			m_pSceneMgr;
	Ogre::Entity*				m_pGold;	
	//Recast寻路库
	OgreRecast*					m_pRecast;
	OgreDetourTileCache*		m_pDetourTileCache;
	OgreDetourCrowd*			m_pDetourCrowd;
};


#endif