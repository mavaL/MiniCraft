#include "stdafx.h"
#include "Building.h"
#include "AIComponent.h"
#include "GUIManager.h"

IMPL_PARAM_COMMAND(Building, RallyPoint, Vector3)

Building::Building()
:SelectableObject()
,m_fCurProgress(0)
,m_rallyPoint(POS::ZERO)
,m_buildingName(Ogre::StringUtil::BLANK)
{
	Ogre::ParamDictionary* dict = getParamDictionary();
	dict->addParameter(Ogre::ParameterDef("rallypoint", "default rally point", Ogre::PT_VECTOR3), &m_sCmdRallyPoint);
}

void Building::Update( float dt )
{
	if(GetAiComponent()->GetCurState() == eObjectState_Produce)
		m_fCurProgress += dt;

	__super::Update(dt);
}

void Building::_OnCommandFinished(eCommandType cmd)
{
	__super::_OnCommandFinished(cmd);
	if(cmd == eCommandType_Produce)
		m_fCurProgress = 0;
}

void Building::Init( const STRING& name, const POS& pos, const ORIENT& orient, const SCALE& scale )
{
	m_buildingName = name;

	GameDataDefManager& dataMgr = GameDataDefManager::GetSingleton();
	auto iter = dataMgr.m_buildingData.find(m_buildingName);
	assert(iter != dataMgr.m_buildingData.end());
	const SBuildingData& data = iter->second;

	//创建渲染实例
	setParameter("meshname", data.m_meshname);
	SetPosition(pos);
	SetOrientation(orient);
	SetScale(scale);

	//设置默认集结点
	setParameter("rallypoint", Ogre::StringConverter::toString(pos + data.m_rallyPoint));

	//初始化技能
	for (int iAbil=0; iAbil<MAX_ABILITY_SLOT; ++iAbil)
	{
		const std::string& strAbil = data.m_vecAbilities[iAbil];
		if (strAbil != "")
		{
			const SAbilityData& pAbil = dataMgr.m_abilityData.find(strAbil)->second;
			SetAbility(iAbil, &pAbil);
		}
	}	
}

