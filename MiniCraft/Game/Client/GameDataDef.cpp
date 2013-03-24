#include "stdafx.h"
#include "GameDataDef.h"
#include "Effect/EffectController.h"

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
		const char* szRallyPoint = pNode->first_attribute("rallypoint")->value();
		const char* szFlags = pNode->first_attribute("flags")->value();

		SBuildingData data = { (eGameRace)-1, szIcon, szMesh, Ogre::StringConverter::parseVector3(szRallyPoint) };

		//种族
		std::unordered_map<STRING, eGameRace> mapRace;
		mapRace["Terran"]	= eGameRace_Terran;
		mapRace["Zerg"]		= eGameRace_Zerg;

		data.m_race = mapRace[szRace];

		//技能
		data.m_vecAbilities.resize(MAX_ABILITY_SLOT);
		for (int i=0; i<MAX_ABILITY_SLOT; ++i)
			data.m_vecAbilities[i] = "";

		//属性集
		data.m_flags = Ogre::StringConverter::parseInt(szFlags);

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
		const char* szType = pNode->first_attribute("type")->value();
		auto attrbParam = pNode->first_attribute("extraparam");

		std::unordered_map<STRING, eCommandType> mapName;
		mapName["produce"]	= eCommandType_Produce;
		mapName["move"]		= eCommandType_Move;
		mapName["stop"]		= eCommandType_Stop;
		mapName["gather"]	= eCommandType_Gather;

		eCommandType type = mapName[szType];
		STRING param("");
		if(attrbParam)
			param = attrbParam->value();

		SAbilityData data = { szIcon, type, param };
		m_abilityData.insert(std::make_pair(szName, data));

		pNode = pNode->next_sibling();
	}
	free(szData);
	XMLDoc.clear();

	///加载Unit配表
 	stream = Ogre::ResourceGroupManager::getSingleton().openResource("UnitTable.xml", "Config");
 	szData = strdup(stream->getAsString().c_str());

	XMLDoc.parse<0>(szData);
 	pNode = XMLDoc.first_node("Root")->first_node("Unit");
 	while(pNode)
 	{
 		const char* szName		= pNode->first_attribute("name")->value();
 		const char* szTimeCost	= pNode->first_attribute("timecost")->value();
		const char* szMeshName	= pNode->first_attribute("meshname")->value();
		const char* szRace		= pNode->first_attribute("race")->value();
		const char* szPortrait	= pNode->first_attribute("portrait")->value();

 		SUnitData data = { Ogre::StringConverter::parseReal(szTimeCost), szMeshName };

		//种族
		std::unordered_map<STRING, eGameRace> mapRace;
		mapRace["Terran"]	= eGameRace_Terran;
		mapRace["Zerg"]		= eGameRace_Zerg;

		data.m_race = mapRace[szRace];

		//3D肖像
		data.m_portrait = szPortrait;

		//动画数据
		std::unordered_map<STRING, eAnimation> mapName;
		mapName["Idle"] = eAnimation_Idle;
		mapName["Move"] = eAnimation_Move;
		mapName["Gather"] = eAnimation_Gather;
		mapName["Attack"] = eAnimation_Attack;

		rapidxml::xml_node<>* pAnimNode = pNode->first_node("AnimationSet")->first_node("Animation");
		while(pAnimNode)
		{
			const char* szAnimType = pAnimNode->first_attribute("type")->value();
			const char* szAnimName = pAnimNode->first_attribute("name")->value();
			data.m_anims.insert(std::make_pair(mapName[szAnimType], szAnimName));
			pAnimNode = pAnimNode->next_sibling();
		}

		//技能
		data.m_vecAbilities.resize(MAX_ABILITY_SLOT);
		for (int i=0; i<MAX_ABILITY_SLOT; ++i)
			data.m_vecAbilities[i] = "";
		rapidxml::xml_node<>* pAbilNode = pNode->first_node("Ability");
		while(pAbilNode)
		{
			const char* szAbilName = pAbilNode->first_attribute("name")->value();
			const char* szSlotIdx = pAbilNode->first_attribute("slotindex")->value();

			data.m_vecAbilities[Ogre::StringConverter::parseInt(szSlotIdx)] = szAbilName;

			pAbilNode = pAbilNode->next_sibling();
		}
 
		m_unitData.insert(std::make_pair(szName, data));
 		pNode = pNode->next_sibling();
 	}
 	free(szData);
 	XMLDoc.clear();

	///加载挂接特效配表
	stream = Ogre::ResourceGroupManager::getSingleton().openResource("Effect.xml", "Config");
	szData = strdup(stream->getAsString().c_str());

	XMLDoc.parse<0>(szData);
	rapidxml::xml_node<>* pUnitNode = XMLDoc.first_node("Root")->first_node("Unit");
	//per unit
	while(pUnitNode)
	{
		const char* szName = pUnitNode->first_attribute("name")->value();
		SUnitData& unitData = m_unitData[szName];

		//per animation
		rapidxml::xml_node<>* pAnimNode = pUnitNode->first_node("Animation");
		while(pAnimNode)
		{
			const char* szAnim = pAnimNode->first_attribute("name")->value();

			//per effect
			rapidxml::xml_node<>* pEffectNode = pAnimNode->first_node("AttachEffect");
			while(pEffectNode)
			{
				//hack a little..
				SEffectData effect;
				int type = Ogre::StringConverter::parseInt(pEffectNode->first_attribute("type")->value());
				Kratos::EffectController ctrl(nullptr);
				Kratos::AttachEffectBase* pBase = ctrl.AddEffect(szAnim, (Kratos::eAttachEffect)type);

				PARAMS_LOAD(pBase, effect.params, pEffectNode);		

				ctrl.RemoveEffect(pBase->GetName());
				unitData.m_effects[szAnim].push_back(effect);
				pEffectNode = pEffectNode->next_sibling();
			}

			pAnimNode = pAnimNode->next_sibling();
		}
		pUnitNode = pUnitNode->next_sibling();
	}

	free(szData);
	XMLDoc.clear();
}

void GameDataDefManager::SaveAllData()
{
	using namespace rapidxml;

	//building数据
	{
		xml_document<> doc;  
		//文件头
		xml_node<>* header = doc.allocate_node(rapidxml::node_pi, "xml version='1.0' encoding='utf-8'");
		doc.append_node(header);
		//Root
		xml_node<>* root =   doc.allocate_node(node_element, "Root");
		doc.append_node(root);

		std::unordered_map<eGameRace, std::string> raceNameMap;
		raceNameMap.insert(std::make_pair(eGameRace_Terran, "Terran"));
		raceNameMap.insert(std::make_pair(eGameRace_Zerg, "Zerg"));

		for (auto itData=m_buildingData.begin(); itData!=m_buildingData.end(); ++itData)
		{
			const SBuildingData& data = itData->second;
			const std::string strIcon = data.m_iconName;
			const std::string strBuilding = itData->first;
			const std::string strRallyPt = Ogre::StringConverter::toString(data.m_rallyPoint);
			const std::string strFlags = Ogre::StringConverter::toString(data.m_flags);

			xml_node<>* node =   doc.allocate_node(node_element, "Building");
			node->append_attribute(doc.allocate_attribute("race", doc.allocate_string(raceNameMap[data.m_race].c_str())));
			node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(strBuilding.c_str())));
			node->append_attribute(doc.allocate_attribute("iconname", doc.allocate_string(strIcon.c_str())));
			node->append_attribute(doc.allocate_attribute("meshname", doc.allocate_string(data.m_meshname.c_str())));
			node->append_attribute(doc.allocate_attribute("rallypoint", doc.allocate_string(strRallyPt.c_str())));
			node->append_attribute(doc.allocate_attribute("flags", doc.allocate_string(strFlags.c_str())));
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
	

	//挂接特效
	{
		xml_document<> doc;  
		//文件头
		xml_node<>* header = doc.allocate_node(rapidxml::node_pi, "xml version='1.0' encoding='utf-8'");
		doc.append_node(header);
		//Root
		xml_node<>* root =   doc.allocate_node(node_element, "Root");
		doc.append_node(root);

		for (auto itUnit=m_unitData.begin(); itUnit!=m_unitData.end(); ++itUnit)
		{
			//per unit
			const SUnitData& unitData = itUnit->second;

			if(unitData.m_effects.empty())
				continue;

			xml_node<>* unitNode =   doc.allocate_node(node_element, "Unit");
			unitNode->append_attribute(doc.allocate_attribute("name", itUnit->first.c_str()));
			root->append_node(unitNode);

			//per animation
			for (auto itAnim=unitData.m_effects.begin(); itAnim!=unitData.m_effects.end(); ++itAnim)
			{
				xml_node<>* animNode =   doc.allocate_node(node_element, "Animation");
				animNode->append_attribute(doc.allocate_attribute("name", itAnim->first.c_str()));
				unitNode->append_node(animNode);

				//per effect
				const auto& effectSlots = itAnim->second;
				for (auto itEffect=effectSlots.begin(); itEffect!=effectSlots.end(); ++itEffect)
				{
					const SEffectData& effect = *itEffect;
					const Ogre::NameValuePairList& params = effect.params;
					xml_node<>* effectNode =   doc.allocate_node(node_element, "AttachEffect");

					for(auto iter=params.begin(); iter!=params.end(); ++iter)
						effectNode->append_attribute(doc.allocate_attribute(iter->first.c_str(), iter->second.c_str()));

					animNode->append_node(effectNode);
				}
			}
		}

		Ogre::StringVectorPtr loc = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("Config", "*Config");
		const Ogre::String filename = loc->at(0) + "\\Effect.xml";
		std::ofstream out(filename);
		out << doc;
	}
	
}
