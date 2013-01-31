/********************************************************************
	created:	29:1:2013   20:55
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorGameData.h
	author:		maval
	
	purpose:	游戏客户端数据相关编辑管理.模仿SC2,所有游戏配置文件都通过XML完成.
*********************************************************************/

#ifndef ManipulatorGameData_h__
#define ManipulatorGameData_h__

#define MAX_ABILITY_SLOT	15

//游戏种族:人族,神族,虫族
enum eGameRace
{
	eGameRace_Terran,
	//eGameRace_Protoss,
	eGameRace_Zerg,
	eGameRace_Count
};

//Building相关数据,见RaceBuildingData.xml
struct SBuildingData
{
	std::wstring	m_iconName;			//编辑器用到的图标文件名
	std::string		m_meshname;			//模型文件名
	std::vector<std::wstring>	m_vecAbilities;	//建筑的能力
};

//Ability相关数据,见AbilityData.xml
struct SAbilityData
{
	std::wstring m_iconName;	//图标文件名
};

class ManipulatorGameData
{
public:
	ManipulatorGameData() {}
	~ManipulatorGameData() {}

public:
	//加载所有编辑器要用到的游戏XML配置文件
	void						LoadAllXml();
	//保存所有XML
	void						SaveAllXml();
	//获取种族的建筑名称列表
	std::vector<std::wstring>	GetRaceBuildingNames(eGameRace race) const;
	//获取building数据
	const SBuildingData*		GetBuildingData(eGameRace race, const std::wstring& name);
	//获取所有ability名称列表
	std::vector<std::wstring>	GetAbilityNames() const;
	//获取ability数据
	const SAbilityData*			GetAbilityData(const std::wstring& name);
	//设置建筑的能力
	void						SetBuildingAbility(eGameRace race, const std::wstring& buildingName, 
		int slotIndex, const std::wstring& abilName);

private:
	//设计基于值对象而不是指针,是因为一旦初始化完毕,在程序退出前,就不会去改变了,
	//根据map的实现,其内部亦不会进行改动.所以效率问题不大.
	typedef std::unordered_map<std::wstring, SBuildingData> BuildingNameIndexTable;
	typedef std::unordered_map<eGameRace, BuildingNameIndexTable> RaceBuildingTable;
	RaceBuildingTable	m_buildingData;

	typedef std::unordered_map<std::wstring, SAbilityData> AbilityNameIdxTable;
	AbilityNameIdxTable	m_abilityData;
};


#endif // ManipulatorGameData_h__







