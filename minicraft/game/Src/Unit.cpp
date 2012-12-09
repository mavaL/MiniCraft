#include "stdafx.h"
#include "Unit.h"
#include "OgreDetourCrowd.h"
#include "Command.h"
#include "GameDefine.h"
#include "World.h"


//**** Define stuff for the Lua Class ****//
// Define the Lua ClassName
const char Unit::className[] = "Unit";

// Define the methods we will expose to Lua
#define method(class, name) {#name, &class::name}
Luna<Unit>::RegType Unit::methods[] = 
{
	method(Unit, PlayAnimation),
	method(Unit, StopAnimation),
	method(Unit, FindPath),
	method(Unit, SetState),
	method(Unit, SetDestPosition),
	method(Unit, GetDestPosition),
	method(Unit, GetCurCommandType),
	method(Unit, UpdatePathFinding),
	method(Unit, Lookat),
	method(Unit, AttachRes),
	method(Unit, DetachRes),
	{0,0}
};

Unit::Unit( int ID, Ogre::Entity* pEnt, Ogre::SceneNode* pNode, OgreRecast* pRecast, OgreDetourCrowd* pDetour )
:m_pCurCommand(nullptr)
,m_pEntity(pEnt)
,m_pRecast(pRecast)
,m_pDetour(pDetour)
,m_pNode(pNode)
,m_pCurState(nullptr)
,m_luaStackIdx(-1)
,m_ID(ID)
,m_pResEnt(nullptr)
,m_pResNode(nullptr)
{
	m_pNode->attachObject(m_pEntity);

	//创建寻路对象
	m_agentID = m_pDetour->addAgent(pNode->_getDerivedPosition());
	m_pAgent = const_cast<dtCrowdAgent*>(m_pDetour->getAgent(m_agentID));
	assert(m_pAgent);

	m_pNode->translate(0, UNIT_OFFSET_TO_GROURD, 0, Ogre::Node::TS_WORLD);

	//将单位缩放至适于Recast库
	float scale = m_pDetour->getAgentRadius() / pEnt->getBoundingRadius();
	m_pNode->setScale(scale, scale, scale);

	//将对象绑定到lua
	Ogre::String name("Unit");
	name += Ogre::StringConverter::toString(m_ID);
	ScriptSystem::GetSingleton().BindObjectToLua<Unit>(name, this);
}

void Unit::GiveCommand( const CommandBase& cmd )
{
	//停止当前命令
	if (m_pCurCommand)
		CancelCurCommand();

	//创建新命令
	switch (cmd.GetType())
	{
	case eCommandType_Move:		m_pCurCommand = new MoveCommand; break;
	case eCommandType_Attack:	m_pCurCommand = new AttackCommand; break;
	case eCommandType_Build:	m_pCurCommand = new BuildCommand; break;
	case eCommandType_Harvest:	m_pCurCommand = new HarvestCommand; break;
	}
	m_pCurCommand->Clone(cmd);

	//执行新命令
	m_pCurCommand->Excute();
}

void Unit::CancelCurCommand()
{
	if(!m_pCurCommand)
		return;

	m_pCurCommand->Cancel();
	delete m_pCurCommand;
	m_pCurCommand = nullptr;
}

void Unit::SetState( eUnitState state )
{
	//退出当前状态
	if(m_pCurState)
	{
		m_pCurState->Exit();
		delete m_pCurState;
	}
	//创建新状态
	switch (state)
	{
	case eUnitState_Idle:		m_pCurState = new IdleState(this); break;
	case eUnitState_Move:		m_pCurState = new MoveState(this); break;
	case eUnitState_Attack:		m_pCurState = new AttackState(this); break;
	case eUnitState_Build:		m_pCurState = new BuildState(this); break;
	case eUnitState_Collect:	m_pCurState = new CollectResState(this); break;
	case eUnitState_Return:		m_pCurState = new ReturnResState(this); break;
	default:					assert(0);
	}
	
	m_pCurState->Enter();
}

void Unit::Update( float dt )
{
	for (size_t i=0; i<m_playingAnims.size(); ++i)
		m_playingAnims[i]->addTime(dt);

	assert(m_pCurState);
	m_pCurState->Update(dt);
}

bool Unit::FindPath( const Ogre::Vector3& destPos )
{
	Ogre::Vector3 beginPos(m_pNode->_getDerivedPosition());
	World::GetSingleton().ClampPosToNavMesh(beginPos);

	Ogre::Vector3 adjustDestPos(destPos);
	World::GetSingleton().ClampPosToNavMesh(adjustDestPos);

	int ret = m_pRecast->FindPath(beginPos, adjustDestPos, 1, 1);
	if( ret >= 0)
	{
		m_pDetour->setMoveTarget(m_agentID, adjustDestPos, false);
		//绘制路径线
		m_pRecast->CreateRecastPathLine(1);
	}

	return ret >= 0;
}

void Unit::PlayAnimation( const Ogre::String& topAnimName, const Ogre::String& baseAnimName /*= Ogre::StringUtil::BLANK*/ )
{
	//停止当前动画
	StopAnimation();

	if(!topAnimName.empty())
	{
		Ogre::AnimationState* pTopAnim = m_pEntity->getAnimationState(topAnimName);
		assert(pTopAnim);
		pTopAnim->setEnabled(true);
		pTopAnim->setTimePosition(0);
		pTopAnim->setLoop(true);

		m_playingAnims.push_back(pTopAnim);
	}

	if (!baseAnimName.empty())
	{
		Ogre::AnimationState* pBaseAnim = m_pEntity->getAnimationState(baseAnimName);
		assert(pBaseAnim);
		pBaseAnim->setEnabled(true);
		pBaseAnim->setTimePosition(0);
		pBaseAnim->setLoop(true);

		m_playingAnims.push_back(pBaseAnim);
	}
}

bool Unit::UpdatePathFinding( float dt )
{
	if(m_pAgent->active) 
	{
		Ogre::Vector3 agentPos;
		OgreRecast::FloatAToOgreVect3(m_pAgent->npos, agentPos);

		Ogre::Vector3 graphicPos(agentPos);
		graphicPos.y = m_pNode->_getDerivedPosition().y;

		//sinbad.mesh的建模Z轴是反的
		m_pNode->lookAt(graphicPos, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);
		m_pNode->_setDerivedPosition(graphicPos);

		if (agentPos.positionEquals(m_destPos, 0.1f))
			return true;
	}
	return false;
}

void Unit::StopAnimation()
{
	for (size_t i=0; i<m_playingAnims.size(); ++i)
		m_playingAnims[i]->setEnabled(false);
	m_playingAnims.clear();
}

int Unit::PlayAnimation( lua_State* L )
{
	const Ogre::String baseAnimName = ScriptSystem::GetSingleton().Get_String(-1);
	const Ogre::String topAnimName = ScriptSystem::GetSingleton().Get_String(-2);
	PlayAnimation(topAnimName, baseAnimName);

	return 0;
}

int Unit::StopAnimation( lua_State* L )
{
	StopAnimation();

	return 0;
}

int Unit::SetState( lua_State* L )
{
	const eUnitState state = static_cast<eUnitState>(ScriptSystem::GetSingleton().Get_Int(-1));
	SetState(state);

	return 0;
}

int Unit::FindPath( lua_State* L )
{
	ScriptSystem& system = ScriptSystem::GetSingleton();
	float x = system.Get_Float(-3);
	float y = system.Get_Float(-2);
	float z = system.Get_Float(-1);

	if(FindPath(Ogre::Vector3(x, y, z)))
	{
		system.Push_Bool(true);
	}
	else
	{
		system.Push_Bool(false);
	}

	return 1;
}

int Unit::GetDestPosition( lua_State* L )
{
	ScriptSystem& system = ScriptSystem::GetSingleton();
	system.Push_Float(m_destPos.x);
	system.Push_Float(m_destPos.y);
	system.Push_Float(m_destPos.z);

	return 3;
}

int Unit::GetCurCommandType( lua_State* L )
{
	ScriptSystem::GetSingleton().Push_Int((int)m_pCurCommand->GetType());

	return 1;
}

int Unit::UpdatePathFinding( lua_State* L )
{
	const float dt = ScriptSystem::GetSingleton().Get_Float(-1);
	bool bArrived = UpdatePathFinding(dt);

	ScriptSystem::GetSingleton().Push_Bool(bArrived);

	return 1;
}

int Unit::Lookat( lua_State* L )
{
	ScriptSystem& system = ScriptSystem::GetSingleton();
	float x = system.Get_Float(-3);
	float y = system.Get_Float(-2);
	float z = system.Get_Float(-1);

	m_pNode->lookAt(Ogre::Vector3(x, y + UNIT_OFFSET_TO_GROURD, z), Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);

	return 0;
}

int Unit::AttachRes( lua_State* L )
{
	ScriptSystem& system = ScriptSystem::GetSingleton();
	float x = system.Get_Float(-4);
	float y = system.Get_Float(-3);
	float z = system.Get_Float(-2);
	float fScale = system.Get_Float(-1);
	const Ogre::String meshname = system.Get_String(-5);

	//创建背负资源
	if(m_pResEnt == nullptr)
	{
		m_pResEnt = Ogre::Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME)->createEntity(meshname.c_str());
		m_pResNode = m_pNode->createChildSceneNode(Ogre::Vector3(x, y, z));
		m_pResNode->attachObject(m_pResEnt);
		m_pResNode->scale(fScale, fScale, fScale);
	}
	m_pResNode->setVisible(true);

	return 0;
}

int Unit::DetachRes( lua_State* L )
{
	m_pResNode->setVisible(false);

	return 0;
}

int Unit::SetDestPosition( lua_State* L )
{
	ScriptSystem& system = ScriptSystem::GetSingleton();
	float x = system.Get_Float(-3);
	float y = system.Get_Float(-2);
	float z = system.Get_Float(-1);

	m_destPos = Ogre::Vector3(x, y, z);

	return 0;
}

void Unit::SetDestPos( const Ogre::Vector3& destPos )
{
	Ogre::Vector3 adjustPos(destPos);
	World::GetSingleton().ClampPosToNavMesh(adjustPos);
	
	m_destPos = adjustPos;
}
