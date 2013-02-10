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
#include "Command.h"

//Building相关数据,见RaceBuildingData.xml
struct SBuildingData
{
	eGameRace		m_race;				//所属种族
	STRING			m_iconName;			//编辑器用到的图标文件名
	STRING			m_meshname;			//模型文件名
	std::vector<STRING>	m_vecAbilities;	//建筑的能力
};

//Ability相关数据,见AbilityData.xml
struct SAbilityData
{
	STRING			m_iconName;			//图标文件名
	eCommandType	m_type;				//技能类型
	STRING			m_param;			//附加参数
};

//Unit配表,见UnitTable.xml
struct SUnitData
{
	float		m_fTimeCost;			//生产所需时间
	STRING		m_meshname;				//模型文件名
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