#pragma once

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
	eCommandType_Move,			//移动命令
	eCommandType_Stop,			//停止命令
	eCommandType_Gather			//采集命令
};

