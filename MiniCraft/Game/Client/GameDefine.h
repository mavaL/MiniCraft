#pragma once


typedef Ogre::Vector3		POS;
typedef Ogre::Quaternion	ORIENT;
typedef	Ogre::Vector3		SCALE;
typedef Ogre::ColourValue	COLOR;
typedef Ogre::String		STRING;
typedef Ogre::Vector3		FLOAT3;
typedef Ogre::Vector2		FLOAT2;


const		std::string		SCENE_MANAGER_NAME	=	"DefaultSceneMgr";
const		float			SCENE_VERSION		=	0.2f;
const		int				MAX_ABILITY_SLOT	=	15;		//命令面板技能槽个数
const		int				MAX_COMMAND_QUEUE_LEN	=	1;	//命令队列最大长度

//游戏种族:人族,神族,虫族
enum eGameRace
{
	eGameRace_Terran,
	//eGameRace_Protoss,
	eGameRace_Zerg,
	eGameRace_Count
};

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

//命令
enum eCommandType
{
	eCommandType_Produce,		//生产命令
	eCommandType_Move			//移动命令
};

///注册OgreStringInterface
#define DECL_PARAM_COMMAND(name)						\
class Cmd##name : public Ogre::ParamCommand				\
{														\
public:													\
	STRING doGet(const void* target) const;				\
	void doSet(void* target, const STRING& val);		\
};														\
static Cmd##name	m_sCmd##name;						

#define IMPL_PARAM_COMMAND(classname, cmdname, paramtype)								\
classname::Cmd##cmdname	classname::m_sCmd##cmdname;										\
STRING classname::Cmd##cmdname::doGet(const void* target) const							\
{																						\
	const classname* obj = static_cast<const classname*>(target);						\
	return Ogre::StringConverter::toString(obj->Get##cmdname());						\
}																						\
																						\
void classname::Cmd##cmdname::doSet(void* target, const STRING& val)					\
{																						\
	classname* obj = static_cast<classname*>(target);									\
	obj->Set##cmdname(Ogre::StringConverter::parse##paramtype(val));					\
}

#define IMPL_PARAM_COMMAND_STR(classname, cmdname)										\
classname::Cmd##cmdname	classname::m_sCmd##cmdname;										\
STRING classname::Cmd##cmdname::doGet(const void* target) const							\
{																						\
	const classname* obj = static_cast<const classname*>(target);						\
	return obj->Get##cmdname();															\
}																						\
																						\
void classname::Cmd##cmdname::doSet(void* target, const STRING& val)					\
{																						\
	classname* obj = static_cast<classname*>(target);									\
	obj->Set##cmdname(val);																\
}

