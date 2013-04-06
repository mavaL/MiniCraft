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

	//node:与引擎的次序要一致
	enum eBTNodeType
	{
		eBTNodeType_Sequence,
		eBTNodeType_Condition,
		eBTNodeType_Action
	};

	typedef std::vector<std::wstring> Blackboard;

	struct SBBParam 
	{
		std::wstring value;
		std::wstring type;
	};

	//一个行为树模板的信息抽取,供UI层使用
	//UI层不直接接触引擎层数据
	class BTTemplate 
	{
	public:
		std::wstring	m_name;
		int				race;

		struct SBTNode
		{
			SBTNode()
			:parent(nullptr),txtProperty(L""),flowGraphNodeID(-1),priority(0) {}

			std::wstring	type;
			std::wstring	txtProperty;
			int				flowGraphNodeID;
			int				priority;
			DWORD			color;
			SBTNode*		parent;
			std::vector<SBTNode*> childs;
		};
		std::list<SBTNode*>	m_nodeList;

		Blackboard			m_ownBB;
		Blackboard*			m_raceBB;
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
	BTTemplate::SBTNode*		AddBTNode(BTTemplate& tmpl, eBTNodeType type);
	//加入黑板参数
	const std::string			DefineBlackboardParam(bool bOwnBB, BTTemplate& tmpl);
	const SBBParam				GetBlackboardParam(const std::wstring& name, const BTTemplate& tmpl, bool bOwnBB) const;
	void						SetBlackboardParam(const std::wstring& name, const SBBParam& param, const BTTemplate& tmpl, bool bOwnBB);
	void						RenameBlackboardParam(const std::wstring& oldName, const std::wstring& newName, BTTemplate& tmpl, bool bOwnBB);

private:
	void						_ParseAllBTTemplates();
	DWORD						_GetBTNodeColor(eBTNodeType type);
	void						_ParseBTNode(const Kratos::aiBehaviorTreeNode* pEngineNode, BTTemplate::SBTNode* pNode,
		BTTemplate::SBTNode* parent, BTTemplate& tmpl);
	void						_ParseBlackboard(Blackboard& bb, Kratos::aiBlackBoard* pEngineBB);
	void						_SaveBTTemplate(const BTTemplate& tmpl, const STRING& filepath);
	void						_SaveBTNode(rapidxml::xml_document<>* doc, BTTemplate::SBTNode* node, rapidxml::xml_node<>* xmlNode);
	void						_SaveBlackboard(rapidxml::xml_document<>* doc, Kratos::aiBlackBoard* pBB, rapidxml::xml_node<>* xmlNode);

	GameDataDefManager*			m_dataMgr;
	BTTemplates					m_btTemplates;
	Blackboard					m_raceBlackboards[eGameRace_Count];
};


#endif // ManipulatorGameData_h__







