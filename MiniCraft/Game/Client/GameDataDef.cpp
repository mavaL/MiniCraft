#include "stdafx.h"
#include "GameDataDef.h"
#include "Effect/EffectController.h"
#include "AI/BehaviorTree/BehaviorTreeTemplateManager.h"

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
		SBuildingData data;
		LoadStringInterface(data.params, pNode);

		//技能
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

		m_buildingData.insert(std::make_pair(data.params["name"], data));
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
		mapName["attack"]	= eCommandType_Attack; 

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
		SUnitData data;
		LoadStringInterface(data.params, pNode);

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
 
		m_unitData.insert(std::make_pair(data.params["name"], data));
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
				SEffectData effect;
				LoadStringInterface(effect.params, pEffectNode);		

				unitData.m_effects[szAnim].push_back(effect);
				pEffectNode = pEffectNode->next_sibling();
			}

			pAnimNode = pAnimNode->next_sibling();
		}
		pUnitNode = pUnitNode->next_sibling();
	}

	free(szData);
	XMLDoc.clear();

	//加载所有行为树模板
	Kratos::aiBehaviorTreeTemplateManager::GetSingleton().LoadAll();
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

		for (auto itData=m_buildingData.begin(); itData!=m_buildingData.end(); ++itData)
		{
			const SBuildingData& data = itData->second;
			xml_node<>* node =   doc.allocate_node(node_element, "Building");
			const Ogre::NameValuePairList& params = data.params;

			for(auto iter=params.begin(); iter!=params.end(); ++iter)
				node->append_attribute(doc.allocate_attribute(iter->first.c_str(), iter->second.c_str()));
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

void LoadStringInterface( Ogre::NameValuePairList& params, rapidxml::xml_node<>* node )
{
	while(node)
	{
		auto attr = node->first_attribute();
		while(attr)
		{
			char* name = attr->name();
			char* value = attr->value();
			params.insert(std::make_pair(name, value));
			attr = attr->next_attribute();
		}
 		node = node->next_sibling();
	}	
}

void SaveStringInterface( Ogre::StringInterface* si, Ogre::NameValuePairList& params )
{
	const Ogre::ParameterList& dict = si->getParameters();					
	std::for_each(dict.begin(), dict.end(), [&](const Ogre::ParameterDef& def)		
	{																					
		params.insert(std::make_pair(def.name, si->getParameter(def.name)));	
	});			
}
