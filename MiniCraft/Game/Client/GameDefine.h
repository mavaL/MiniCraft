#pragma once


typedef Ogre::Vector3		POS;
typedef Ogre::Quaternion	ORIENT;
typedef	Ogre::Vector3		SCALE;
typedef Ogre::ColourValue	COLOR;
typedef Ogre::String		STRING;
typedef Ogre::Vector3		FLOAT3;
typedef Ogre::Vector2		FLOAT2;


const std::string	SCENE_MANAGER_NAME	=	"DefaultSceneMgr";
const float			SCENE_VERSION		=	0.2f;	//当前.scene版本号,注意与编辑器同步

//对象类型
enum eObjectType
{
	eObjectType_Faction,
	eObjectType_Unit,
	eObjectType_Building,
	eObjectType_Resource
};

//场景查询类型
enum eQueryType
{
	eQueryType_Default	=	1<<0,
	eQueryType_WorldGeometry	=	1<<1,
	eQueryType_SelectableObject	=	1<<2
};

