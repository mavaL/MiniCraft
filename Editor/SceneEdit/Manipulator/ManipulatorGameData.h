/********************************************************************
	created:	29:1:2013   20:55
	filename: 	F:\MiniCraft\Editor\SceneEdit\Manipulator\ManipulatorGameData.h
	author:		maval
	
	purpose:	游戏客户端数据相关编辑管理.模仿SC2,所有游戏配置文件都通过XML完成.
*********************************************************************/

#ifndef ManipulatorGameData_h__
#define ManipulatorGameData_h__

#include "../../MiniCraft/Game/Client/GameDataDef.h"
#include "rapidxml.hpp"

class GameDataDefManager;

class ManipulatorGameData
{
public:
	ManipulatorGameData();
	~ManipulatorGameData();

	//一个行为树模板的信息抽取,供UI层使用
	//UI层不直接接触引擎层数据
	class BTTemplate 
	{
	public:
		std::wstring	m_name;
		int				race;
		struct SBTNode
		{
			SBTNode():parent(nullptr) {}

			std::wstring	type;
			std::wstring	txtProperty;
			int				flowGraphNodeID;
			DWORD			color;
			SBTNode*		parent;
			std::vector<SBTNode*> childs;
		};
		std::list<SBTNode*>	m_nodeList;
	};
	typedef std::vector<BTTemplate>	BTTemplates;

public:
	//加载所有编辑器要用到的游戏XML配置文件
	void						LoadAllXml();
	//保存所有XML
	void						SaveAllXml();
	//获取种族的建筑名称列表
	std::vector<std::wstring>	GetRaceBuildingNames(eGameRace race) const;
	//获取building数据
	SBuildingData*				GetBuildingData(const std::wstring& name);
	//获取所有ability名称列表
	std::vector<std::wstring>	GetAbilityNames() const;
	//获取ability数据
	const SAbilityData*			GetAbilityData(const std::wstring& name);
	//设置建筑的能力
	void						SetBuildingAbility(const std::wstring& buildingName, int slotIndex, const std::wstring& abilName);
	//获取单位数据.注意参数是mesh名字
	SUnitData*					GetUnitData(const std::string& meshname);
	//获取所有行为树模板名字
	std::vector<std::wstring>	GetAllBehaviorTreeTemplateNames() const;
	BTTemplate&					GetBTTemplate(const std::wstring& name);
	void						SaveAllBehaviorTreeTemplates();
	//获取所有可用behavior名字
	std::vector<std::wstring>	GetAllBehaviorNames() const;
	//校验该行为树模板的有效性
	void						ValidateBehaviorTemplate(const BTTemplate& tmpl);

private:
	void						_ParseAllBTTemplates();
	void						_ParseBTNode(const Kratos::aiBehaviorTreeNode* pEngineNode, BTTemplate::SBTNode* pNode,
		BTTemplate::SBTNode* parent, BTTemplate& tmpl);
	void						_SaveBTTemplate(const BTTemplate& tmpl, const STRING& filepath);
	void						_SaveBTNode(rapidxml::xml_document<>* doc, BTTemplate::SBTNode* node, rapidxml::xml_node<>* xmlNode);

	GameDataDefManager*			m_dataMgr;
	BTTemplates					m_btTemplates;
};


#endif // ManipulatorGameData_h__







