#include "stdafx.h"
#include "Building.h"
#include "AIComponent.h"
#include "GUIManager.h"
#include "Faction.h"
#include "World.h"
#include "GameDataDef.h"
#include "PathComponent.h"

IMPL_PARAM_COMMAND_STR(Building, Name)

Building::Building()
:SelectableObject()
,m_fCurProgress(0)
,m_buildingName(Ogre::StringUtil::BLANK)
,m_param(nullptr)
,m_rallyPoint(POS::ZERO)
{
	if (InitParamDict("Building"))
	{
		Ogre::ParamDictionary* dict = getParamDictionary();
		dict->addParameter(Ogre::ParameterDef("name", "Building name", Ogre::PT_STRING), &m_sCmdName);
	}
}

void Building::Update( float dt )
{
	if(GetAi()->GetCurState() == eObjectState_Produce)
		m_fCurProgress += dt;

	__super::Update(dt);
}

void Building::_OnCommandFinished(eCommandType cmd)
{
	__super::_OnCommandFinished(cmd);
	if(cmd == eCommandType_Produce)
		m_fCurProgress = 0;
}

void Building::Init( const POS& pos, const ORIENT& orient, const SCALE& scale )
{
	assert(m_param);

	//初始化组件
	AddComponent(eComponentType_AI, new AiComponent(this));

	//创建渲染实例
	SetMeshName(m_param->params["meshname"]);
	SetPosition(pos);
	SetOrientation(orient);
	SetScale(scale);

	World& world = World::GetSingleton();
	//设置默认集结点
	m_rallyPoint = pos + Ogre::StringConverter::parseVector3(m_param->params["rallypoint"]);
	PathComponent::ClampPosToNavMesh(m_rallyPoint);
	world.ClampToTerrain(m_rallyPoint);

	//初始化技能
	for (int iAbil=0; iAbil<MAX_ABILITY_SLOT; ++iAbil)
	{
		const std::string& strAbil = m_param->m_vecAbilities[iAbil];
		if (strAbil != "")
		{
			const SAbilityData& pAbil = GameDataDefManager::GetSingleton().m_abilityData.find(strAbil)->second;
			SetAbility(iAbil, &pAbil);
		}
	}

	//属性集
	eGameRace race = (eGameRace)GetRace();
	if(GetFlags() & eBuildingFlag_ResDropOff)
		world.GetFaction(race)->SetBase(this);

	//明确归属
	GetAi()->SetFaction(world.GetFaction(race));

	GetEntity()->setQueryFlags(eQueryType_Building);
}

int Building::GetRace() const
{
	return Ogre::StringConverter::parseInt(m_param->params["race"]);
}

const POS& Building::GetRallyPoint() const
{
	return m_rallyPoint;
}

const STRING& Building::GetIcon() const
{
	return m_param->params["icon"];
}

int Building::GetFlags() const
{
	return Ogre::StringConverter::parseInt(m_param->params["flags"]);
}

void Building::SetName( const STRING& name )
{
	GameDataDefManager& dataMgr = GameDataDefManager::GetSingleton();
	auto iter = dataMgr.m_buildingData.find(name);
	assert(iter != dataMgr.m_buildingData.end());
	m_param = &iter->second;

	m_buildingName = name;
}

