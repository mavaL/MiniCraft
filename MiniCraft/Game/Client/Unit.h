#ifndef UNIT_H
#define UNIT_H

#include "ScriptSystem.h"
#include "UnitState.h"



class CommandBase;
class OgreRecast;
class OgreDetourCrowd;
class UnitState;
struct dtCrowdAgent;
struct lua_State;

/************************************************************************/
/*							��Ϸ��λ��	                                */
/************************************************************************/

class Unit
{
public:
	Unit(int ID, Ogre::Entity* pEnt, Ogre::SceneNode* pNode, OgreRecast* pRecast, OgreDetourCrowd* pDetour);
	Unit(lua_State* L) {}
	~Unit() {}

	static const char className[];
	static Luna<Unit>::RegType methods[];

	const static Ogre::String UNIT_TABLE_NAME;
	const static Ogre::String ENTITY_NAME_PREFIX;

public:
	Ogre::SceneNode*	GetSceneNode() { return m_pNode; }
	Ogre::Entity*	GetEntity() { return m_pEntity; }
	//ָ��һ��ִ������
	void			GiveCommand(const CommandBase& cmd);
	//��ȡ��ǰ����
	CommandBase*	GetCurCommand() const { return m_pCurCommand; }
	//ȡ����ǰ����
	void			CancelCurCommand();
	//���õ�ǰ״̬
	void			SetState(eUnitState state);
	//���µ�λ
	void			Update(float dt);
	//��ȷѰ·
	bool			FindPath(const Ogre::Vector3& destPos);
	//����Ѱ·,����Ŀ�ĵ��򷵻�true
	bool			UpdatePathFinding(float dt);

	void			SetDestPos(const Ogre::Vector3& destPos);
	const Ogre::Vector3& GetDestPos() const	{ return m_destPos; }
	UnitState*		GetCurState() const { return m_pCurState; }
	void			PlayAnimation(const Ogre::String& topAnimName, const Ogre::String& baseAnimName = Ogre::StringUtil::BLANK);
	void			StopAnimation();
	dtCrowdAgent*	GetAgent() { return m_pAgent; }
	int				GetID() const { return m_ID; }
	void			SetSelected(bool bSelected);

public:
	///lua��������
	int				PlayAnimation(lua_State* L);
	int				StopAnimation(lua_State* L);
	int				FindPath(lua_State* L);
	int				SetState(lua_State* L);
	int				SetDestPosition(lua_State* L);
	int				GetDestPosition(lua_State* L);
	int				UpdatePathFinding(lua_State* L);
	int				GetCurCommandType(lua_State* L);
	int				Lookat(lua_State* L);
	int				AttachRes(lua_State* L);
	int				DetachRes(lua_State* L);

private:
	Ogre::Entity*		m_pEntity;
	Ogre::SceneNode*	m_pNode;
	OgreRecast*			m_pRecast;
	OgreDetourCrowd*	m_pDetour;
	dtCrowdAgent*		m_pAgent;		//Ѱ·����
	int					m_agentID;
	CommandBase*		m_pCurCommand;	//��ǰ����
	UnitState*			m_pCurState;	//��λ��ǰ״̬(�߼�״̬��)
	int					m_ID;
	Ogre::Vector3		m_destPos;
	bool				m_bSelected;

	typedef std::vector<Ogre::AnimationState*> VecAnims;
	VecAnims			m_playingAnims;
	Ogre::Entity*		m_pResEnt;
	Ogre::SceneNode*	m_pResNode;
};


#endif