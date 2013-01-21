#pragma once



typedef Ogre::Vector3		POS;
typedef Ogre::Quaternion	ORIENT;
typedef	Ogre::Vector3		SCALE;
typedef Ogre::ColourValue	COLOR;


const std::string	SCENE_MANAGER_NAME	=	"DefaultSceneMgr";
const float			SCENE_VERSION		=	0.1f;	//当前.scene版本号,注意与编辑器同步

//游戏种族:人族,神族,虫族
enum eGameRace
{
	eGameRace_Terrain,
	eGameRace_Protoss,
	eGameRace_Zerg
};

//对象类型
enum eObjectType
{
	eObjectType_Faction,
	eObjectType_Unit,
	eObjectType_Building,
	eObjectType_Resource
};

