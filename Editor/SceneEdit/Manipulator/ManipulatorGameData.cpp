#include "stdafx.h"
#include "ManipulatorGameData.h"
#include "Utility.h"


void ManipulatorGameData::LoadAllXml()
{
	///加载Building数据
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("RaceBuildingData.xml", "Config");
	char* szData = strdup(stream->getAsString().c_str());

	rapidxml::xml_document<> XMLDoc;
	XMLDoc.parse<0>(szData);
	rapidxml::xml_node<>* pNode = XMLDoc.first_node("Root")->first_node("Building");

	BuildingNameIndexTable buildingTable[eGameRace_Count];
	while(pNode)
	{
		const char* szRace = pNode->first_attribute("race")->value();
		const char* szName = pNode->first_attribute("name")->value();
		const char* szIcon = pNode->first_attribute("iconname")->value();
		const char* szMesh = pNode->first_attribute("meshname")->value();

		SBuildingData data = { Utility::EngineToUnicode(szIcon), szMesh };

		//建筑ability
		data.m_vecAbilities.resize(MAX_ABILITY_SLOT);
		rapidxml::xml_node<>* abilNode = pNode->first_node("Ability");
		while(abilNode)
		{
			const char* szAbilName = abilNode->first_attribute("name")->value();
			const char* szSlotIdx = abilNode->first_attribute("slotindex")->value();

			data.m_vecAbilities[Ogre::StringConverter::parseInt(szSlotIdx)] = Utility::EngineToUnicode(szAbilName);
			
			abilNode = abilNode->next_sibling();
		}

		if(strcmp(szRace, "Terran") == 0)
			buildingTable[eGameRace_Terran].insert(std::make_pair(Utility::EngineToUnicode(szName), data));
		else if(strcmp(szRace, "Zerg") == 0)
			buildingTable[eGameRace_Zerg].insert(std::make_pair(Utility::EngineToUnicode(szName), data));
		else
			assert(0);

		pNode = pNode->next_sibling();
	}

	for(int i=0; i<eGameRace_Count; ++i)
		m_buildingData.insert(std::make_pair((eGameRace)i, buildingTable[i]));

	free(szData);
	XMLDoc.clear();

	///加载Ability数据
	stream = Ogre::ResourceGroupManager::getSingleton().openResource("AbilityData.xml", "Config");
	szData = strdup(stream->getAsString().c_str());

	XMLDoc.parse<0>(szData);
	pNode = XMLDoc.first_node("Root")->first_node();
	while(pNode)
	{
		const char* szName = pNode->first_attribute("name")->value();
		const char* szIcon = pNode->first_attribute("iconname")->value();
		SAbilityData data = { Utility::EngineToUnicode(szIcon) };
		m_abilityData.insert(std::make_pair(Utility::EngineToUnicode(szName), data));

		pNode = pNode->next_sibling();
	}
	free(szData);
	XMLDoc.clear();
}

void ManipulatorGameData::SaveAllXml()
{
	using namespace rapidxml;

	xml_document<> doc;  
	//文件头
	xml_node<>* header = doc.allocate_node(rapidxml::node_pi, doc.allocate_string("xml version='1.0' encoding='utf-8'"));
	doc.append_node(header);

	//Root
	xml_node<>* root =   doc.allocate_node(node_element, "Root");
	doc.append_node(root);

	std::unordered_map<eGameRace, std::string> raceNameMap;
	raceNameMap.insert(std::make_pair(eGameRace_Terran, "Terran"));
	raceNameMap.insert(std::make_pair(eGameRace_Zerg, "Zerg"));

	//building数据
	for (auto itRace=m_buildingData.begin(); itRace!=m_buildingData.end(); ++itRace)
	{
		const BuildingNameIndexTable& table = itRace->second;
		for (auto itData=table.begin(); itData!=table.end(); ++itData)
		{
			const SBuildingData& data = itData->second;
			const std::string strIcon = Utility::UnicodeToEngine(data.m_iconName);
			const std::string strBuilding = Utility::UnicodeToEngine(itData->first);

			xml_node<>* node =   doc.allocate_node(node_element, "Building");
			node->append_attribute(doc.allocate_attribute("race", doc.allocate_string(raceNameMap[itRace->first].c_str())));
			node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(strBuilding.c_str())));
			node->append_attribute(doc.allocate_attribute("iconname", doc.allocate_string(strIcon.c_str())));
			node->append_attribute(doc.allocate_attribute("meshname", doc.allocate_string(data.m_meshname.c_str())));
			root->append_node(node);

			//ability数据
			for (int i=0; i<MAX_ABILITY_SLOT; ++i)
			{
				if(data.m_vecAbilities[i] == L"")
					continue;

				const std::string strAbil = Utility::UnicodeToEngine(data.m_vecAbilities[i]);
				const std::string strSlotIdx = Ogre::StringConverter::toString(i);

				xml_node<>* abilNode =   doc.allocate_node(node_element, "Ability");
				abilNode->append_attribute(doc.allocate_attribute("name", doc.allocate_string(strAbil.c_str())));
				abilNode->append_attribute(doc.allocate_attribute("slotindex", doc.allocate_string(strSlotIdx.c_str())));
				node->append_node(abilNode);
			}
		}
	}

	Ogre::StringVectorPtr loc = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("Config", "*Config");
	const Ogre::String filename = loc->at(0) + "\\RaceBuildingData.xml";
	std::ofstream out(filename);
	out << doc;
}

std::vector<std::wstring> ManipulatorGameData::GetRaceBuildingNames( eGameRace race ) const
{
	std::vector<std::wstring> retNames;
	const BuildingNameIndexTable& table = m_buildingData.find(race)->second;
	retNames.resize(table.size());

	int i = 0;
	for (auto iter=table.begin(); iter!=table.end(); ++iter)
		retNames[i++] = iter->first;

	return std::move(retNames);
}

const SBuildingData* ManipulatorGameData::GetBuildingData( eGameRace race, const std::wstring& name )
{
	const BuildingNameIndexTable& table = m_buildingData.find(race)->second;
	return &(table.find(name)->second);
}

std::vector<std::wstring> ManipulatorGameData::GetAbilityNames() const
{
	std::vector<std::wstring> retNames;
	retNames.resize(m_abilityData.size());

	int i = 0;
	for (auto iter=m_abilityData.begin(); iter!=m_abilityData.end(); ++iter)
		retNames[i++] = iter->first;

	return std::move(retNames);
}

const SAbilityData* ManipulatorGameData::GetAbilityData( const std::wstring& name )
{
	auto iter = m_abilityData.find(name);
	if(iter != m_abilityData.end())
		return &iter->second;
	else
		return nullptr;
}

void ManipulatorGameData::SetBuildingAbility( eGameRace race, const std::wstring& buildingName, int slotIndex, const std::wstring& abilName )
{
	assert(slotIndex >= 0 && slotIndex < 15);
	(const_cast<SBuildingData*>(GetBuildingData(race, buildingName)))->m_vecAbilities[slotIndex] = abilName;
}
