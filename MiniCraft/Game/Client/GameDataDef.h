/********************************************************************
	created:	7:2:2013   12:27
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\GameDataDef.h
	author:		maval
	
	purpose:	游戏逻辑数据结构定义和管理器
*********************************************************************/
#ifndef GameDataDef_h__
#define GameDataDef_h__

#include "Singleton.h"
#include "GameDefine.h"

//建筑物属性标志
enum eBuildingFlag
{
	eBuildingFlag_ResDropOff = 1		//可以接受资源(主基地)
};

//Building相关数据,见RaceBuildingData.xml
struct SBuildingData
{
	eGameRace		m_race;				//所属种族
	STRING			m_iconName;			//编辑器用到的图标文件名
	STRING			m_meshname;			//模型文件名
	POS				m_rallyPoint;		//产兵默认集结点
	std::vector<STRING>	m_vecAbilities;	//技能
	int				m_flags;			//属性集
};

//Ability相关数据,见AbilityData.xml
struct SAbilityData
{
	STRING			m_iconName;			//图标文件名
	eCommandType	m_type;				//技能类型
	STRING			m_param;			//附加参数,见下
};

//命令附加参数
const	STRING		FORCE_EXECUTE					=	"ForceExecute";
const	STRING		TARGETING_AND_FORCE_EXECUTE		=	"TargetingAndForceExecute";

//单位的可用动画类型
enum eAnimation
{
	eAnimation_Idle,					//休闲动画
	eAnimation_Move	,					//移动动画
	eAnimation_Gather					//采集动画
};

//Unit配表,见UnitTable.xml
struct SUnitData
{
	float		m_fTimeCost;			//生产所需时间
	STRING		m_meshname;				//模型文件名
	typedef std::unordered_map<eAnimation, STRING>	AnimTable;
	AnimTable	m_anims;				//该单位所有动画的真实名字.因为各模型的导出动画名字不一定一致
	std::vector<STRING>	m_vecAbilities;	//技能
	eGameRace	m_race;					//种族
};

//设计基于值对象而不是指针,是因为一旦初始化完毕,在程序退出前,就不会去改变了,
//根据map的实现,其内部亦不会进行改动.所以效率问题不大.
typedef std::unordered_map<STRING, SBuildingData> BuildingNameIndexTable;
typedef std::unordered_map<STRING, SAbilityData> AbilityNameIdxTable;
typedef std::unordered_map<STRING, SUnitData>	UnitTable;


class GameDataDefManager : public CSingleton<GameDataDefManager>
{
	GameDataDefManager() {}
	~GameDataDefManager() {}
	DECLEAR_SINGLETON(GameDataDefManager)

public:
	void	LoadAllData();
	void	SaveAllData();

public:
	BuildingNameIndexTable	m_buildingData;
	AbilityNameIdxTable		m_abilityData;
	UnitTable				m_unitData;
};

#endif // GameDataDef_h__