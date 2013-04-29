/********************************************************************
	created:	7:2:2013   12:27
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\GameDataDef.h
	author:		maval
	
	purpose:	游戏数据中心.
				对象属性是数据驱动的,查看各对象的stringInterface数据字典
*********************************************************************/
#ifndef GameDataDef_h__
#define GameDataDef_h__

#include "Singleton.h"
#include "GameDefine.h"
#include "KratosPrerequisites.h"


//建筑物属性标志
enum eBuildingFlag
{
	eBuildingFlag_ResDropOff = 1		//可以接受资源(主基地)
};

//命令附加参数
const	STRING		FORCE_EXECUTE					=	"ForceExecute";
const	STRING		TARGETING_AND_FORCE_EXECUTE		=	"TargetingAndForceExecute";

/////////////////////////////////////////////////////////////////////////
////	Building相关数据,见RaceBuildingData.xml
struct SBuildingData
{
	Ogre::NameValuePairList params;
	std::vector<STRING>	m_vecAbilities;	//技能
};

/////////////////////////////////////////////////////////////////////////
////	Ability相关数据,见AbilityData.xml
struct SAbilityData
{
	STRING			m_iconName;			//图标文件名
	eCommandType	m_type;				//技能类型
	STRING			m_param;			//附加参数,见下
};

/////////////////////////////////////////////////////////////////////////
////	粒子等特效配表,见Effect.xml
struct SEffectData
{
	Ogre::NameValuePairList params;															
};

/////////////////////////////////////////////////////////////////////////
////	单位战斗数据配表,见BattleData.xml
struct SBattleData
{
	Ogre::NameValuePairList params;															
};

/////////////////////////////////////////////////////////////////////////
////	Unit配表,见UnitTable.xml
struct SUnitData
{
	Ogre::NameValuePairList params;
	std::vector<STRING>		m_vecAbilities;			//技能
	typedef HashMap<eAnimation, STRING>	AnimTable;
	AnimTable				m_anims;				//该单位所有动画的真实名字.因为各模型的导出动画名字不一定一致
	typedef HashMap<STRING, std::vector<SEffectData>> EffectDataMap;
	EffectDataMap			m_effects;				//挂接特效
	SBattleData				m_battleInfo;			//战斗数据
};

//设计基于值对象而不是指针,是因为一旦初始化完毕,在程序退出前,就不会去改变了,
//根据map的实现,其内部亦不会进行改动.所以效率问题不大.
typedef HashMap<STRING, SBuildingData> BuildingNameIndexTable;
typedef HashMap<STRING, SAbilityData> AbilityNameIdxTable;
typedef HashMap<STRING, SUnitData>	UnitTable;


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

///属性持久化
void	LoadStringInterface(Ogre::NameValuePairList& params, rapidxml::xml_node<>* node);
void	SaveStringInterface(Ogre::StringInterface* si, Ogre::NameValuePairList& params);

#endif // GameDataDef_h__