#include "stdafx.h"
#include "GameDataDef.h"

void GameDataDefManager::LoadAllData()
{
	///加载Building数据
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("RaceBuildingData.xml", "Config");
	char* szData = strdup(stream->getAsString().c_str());

	rapidxml::xml_document<> XMLDoc;
	XMLDoc.parse<0>(szData);
	rapidxml::xml_node<>* pNode = XMLDoc.first_node("Root")->first_node("Building");

	while(pNode)
	{
		const char* szRace = pNode->first_attribute("race")->value();
		const char* szName = pNode->first_attribute("name")->value();
		const char* szIcon = pNode->first_attribute("iconname")->value();
		const char* szMesh = pNode->first_attribute("meshname")->value();

		SBuildingData data = { (eGameRace)-1, szIcon, szMesh };

		if(strcmp(szRace, "Terran") == 0)
			data.m_race = eGameRace_Terran;
		else if(strcmp(szRace, "Zerg") == 0)
			data.m_race = eGameRace_Zerg;
		else
			assert(0);

		//建筑ability
		data.m_vecAbilities.resize(MAX_ABILITY_SLOT);
		for (int i=0; i<MAX_ABILITY_SLOT; ++i)
			data.m_vecAbilities[i] = "";

		rapidxml::xml_node<>* abilNode = pNode->first_node("Ability");
		while(abilNode)
		{
			const char* szAbilName = abilNode->first_attribute("name")->value();
			const char* szSlotIdx = abilNode->first_attribute("slotindex")->value();

			data.m_vecAbilities[Ogre::StringConverter::parseInt(szSlotIdx)] = szAbilName;

			abilNode = abilNode->next_sibling();
		}

		m_buildingData.insert(std::make_pair(szName, data));
		pNode = pNode->next_sibling();
	}

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
		SAbilityData data = { szIcon };
		m_abilityData.insert(std::make_pair(szName, data));

		pNode = pNode->next_sibling();
	}
	free(szData);
	XMLDoc.clear();
}

void GameDataDefManager::SaveAllData()
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
	for (auto itData=m_buildingData.begin(); itData!=m_buildingData.end(); ++itData)
	{
		const SBuildingData& data = itData->second;
		const std::string strIcon = data.m_iconName;
		const std::string strBuilding = itData->first;

		xml_node<>* node =   doc.allocate_node(node_element, "Building");
		node->append_attribute(doc.allocate_attribute("race", doc.allocate_string(raceNameMap[data.m_race].c_str())));
		node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(strBuilding.c_str())));
		node->append_attribute(doc.allocate_attribute("iconname", doc.allocate_string(strIcon.c_str())));
		node->append_attribute(doc.allocate_attribute("meshname", doc.allocate_string(data.m_meshname.c_str())));
		root->append_node(node);

		//ability数据
		for (int i=0; i<MAX_ABILITY_SLOT; ++i)
		{
			if(data.m_vecAbilities[i] == "")
				continue;

			const std::string strAbil = data.m_vecAbilities[i];
			const std::string strSlotIdx = Ogre::StringConverter::toString(i);

			xml_node<>* abilNode =   doc.allocate_node(node_element, "Ability");
			abilNode->append_attribute(doc.allocate_attribute("name", doc.allocate_string(strAbil.c_str())));
			abilNode->append_attribute(doc.allocate_attribute("slotindex", doc.allocate_string(strSlotIdx.c_str())));
			node->append_node(abilNode);
		}
	}

	Ogre::StringVectorPtr loc = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("Config", "*Config");
	const Ogre::String filename = loc->at(0) + "\\RaceBuildingData.xml";
	std::ofstream out(filename);
	out << doc;
}
