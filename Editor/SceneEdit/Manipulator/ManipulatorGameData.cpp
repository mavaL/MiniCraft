#include "stdafx.h"
#include "ManipulatorGameData.h"
#include "Utility.h"
#include "AI/BehaviorTree/BehaviorTreeTemplateManager.h"
#include "ScriptSystem.h"

ManipulatorGameData::ManipulatorGameData()
{
	m_dataMgr = GameDataDefManager::GetSingletonPtr();
	SCRIPTNAMAGER.Init();
}

ManipulatorGameData::~ManipulatorGameData()
{
	SCRIPTNAMAGER.Shutdown();
}

void ManipulatorGameData::LoadAllXml()
{
	m_dataMgr->LoadAllData();
}

void ManipulatorGameData::SaveAllXml()
{
	m_dataMgr->SaveAllData();
}

std::vector<std::wstring> ManipulatorGameData::GetRaceBuildingNames( eGameRace race ) const
{
	std::vector<std::wstring> retNames;
	for (auto iter=m_dataMgr->m_buildingData.begin(); iter!=m_dataMgr->m_buildingData.end(); ++iter)
	{
		SBuildingData& data = iter->second;
		int iRace = (eGameRace)Ogre::StringConverter::parseInt(data.params["race"]);
		if((eGameRace)iRace == race)
			retNames.push_back(Utility::EngineToUnicode(iter->first));
	}

	return std::move(retNames);
}

SBuildingData* ManipulatorGameData::GetBuildingData( const std::wstring& name )
{
	auto iter = m_dataMgr->m_buildingData.find(Utility::UnicodeToEngine(name));
	assert(iter != m_dataMgr->m_buildingData.end());
	return &(iter->second);
}

std::vector<std::wstring> ManipulatorGameData::GetAbilityNames() const
{
	std::vector<std::wstring> retNames;
	retNames.resize(m_dataMgr->m_abilityData.size());

	int i = 0;
	for (auto iter=m_dataMgr->m_abilityData.begin(); iter!=m_dataMgr->m_abilityData.end(); ++iter)
		retNames[i++] = Utility::EngineToUnicode(iter->first);

	return std::move(retNames);
}

const SAbilityData* ManipulatorGameData::GetAbilityData( const std::wstring& name )
{
	auto iter = m_dataMgr->m_abilityData.find(Utility::UnicodeToEngine(name));
	if(iter != m_dataMgr->m_abilityData.end())
		return &iter->second;
	else
		return nullptr;
}

void ManipulatorGameData::SetBuildingAbility( const std::wstring& buildingName, int slotIndex, const std::wstring& abilName )
{
	assert(slotIndex >= 0 && slotIndex < 15);
	(const_cast<SBuildingData*>(GetBuildingData(buildingName)))->m_vecAbilities[slotIndex] = Utility::UnicodeToEngine(abilName);
}

SUnitData* ManipulatorGameData::GetUnitData( const std::string& meshname )
{
	UnitTable& units = GameDataDefManager::GetSingleton().m_unitData;

	for(auto iter=units.begin(); iter!=units.end(); ++iter)
	{
		SUnitData& unitData = iter->second;
		if(unitData.params["meshname"] == meshname)
			return &unitData;
	}
	
	return nullptr;
}

std::vector<std::wstring> ManipulatorGameData::GetAllBehaviorTreeTemplateNames() const
{
	auto names = Kratos::aiBehaviorTreeTemplateManager::GetSingleton().GetAllTemplateNames();
	std::vector<std::wstring> ret(names.size());
	for(size_t i=0; i<names.size(); ++i)
		ret[i] = Utility::EngineToUnicode(names[i]);

	return std::move(ret);
}
