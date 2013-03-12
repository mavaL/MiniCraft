/********************************************************************
	created:	29:1:2013   20:55
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorGameData.h
	author:		maval
	
	purpose:	游戏客户端数据相关编辑管理.模仿SC2,所有游戏配置文件都通过XML完成.
*********************************************************************/

#ifndef ManipulatorGameData_h__
#define ManipulatorGameData_h__

#include "../../MiniCraft/Game/Client/GameDataDef.h"

class GameDataDefManager;

class ManipulatorGameData
{
public:
	ManipulatorGameData();
	~ManipulatorGameData() {}

public:
	//加载所有编辑器要用到的游戏XML配置文件
	void						LoadAllXml();
	//保存所有XML
	void						SaveAllXml();
	//获取种族的建筑名称列表
	std::vector<std::wstring>	GetRaceBuildingNames(eGameRace race) const;
	//获取building数据
	const SBuildingData*		GetBuildingData(const std::wstring& name);
	//获取所有ability名称列表
	std::vector<std::wstring>	GetAbilityNames() const;
	//获取ability数据
	const SAbilityData*			GetAbilityData(const std::wstring& name);
	//设置建筑的能力
	void						SetBuildingAbility(const std::wstring& buildingName, int slotIndex, const std::wstring& abilName);
	//获取单位数据.注意参数是mesh名字
	SUnitData*					GetUnitData(const std::string& meshname);

private:
	GameDataDefManager*			m_dataMgr;
};


#endif // ManipulatorGameData_h__







