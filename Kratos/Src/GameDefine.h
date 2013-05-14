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

//渲染类型控制
enum eRenderType
{
	eRenderType_Default		=	0xffffffff,
	eRenderType_Terrain		=	1<<0,
	eRenderType_Resource	=	1<<1,
	eRenderType_Unit		=	1<<2,
	eRenderType_Building	=	1<<3,
	eRenderType_UI			=	1<<4
};

//场景查询类型
enum eQueryType
{
	eQueryType_All				=	0xffffffff,
	eQueryType_Default			=	1<<0,
	eQueryType_Terrain			=	1<<1,
	eQueryType_Unit				=	1<<2,
	eQueryType_Resource			=	1<<3,
	eQueryType_Building			=	1<<4,
};
#define QueryTypeSelectableObject (eQueryType_Unit | eQueryType_Resource | eQueryType_Building)

//命令
enum eCommandType
{
	eCommandType_Produce,		//生产
	eCommandType_Move,			//移动
	eCommandType_Stop,			//停止
	eCommandType_Gather,		//采集
	eCommandType_Attack			//攻击
};

//单位的可用动画类型
enum eAnimation
{
	eAnimation_Idle,					//休闲
	eAnimation_Move	,					//移动
	eAnimation_Gather,					//采集
	eAnimation_Attack,					//攻击
	eAnimation_Death					//死亡
};


