/********************************************************************
	created:	21:1:2013   0:51
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Object\Unit.h
	author:		maval
	
	purpose:	游戏单位类
*********************************************************************/

#ifndef Unit_h__
#define Unit_h__


#include "SelectableObject.h"
#include "ScriptSystem.h"


struct lua_State;
class AIComponent;


class Unit : public SelectableObject
{
public:
	Unit();
	Unit(lua_State* L) {}
	~Unit();

	static const char className[];
	static Luna<Unit>::RegType methods[];

	const static STRING UNIT_TABLE_NAME;
	const static STRING ENTITY_NAME_PREFIX;

public:
	virtual eObjectType GetType() const { return eObjectType_Unit; }
	virtual	void	CreateRenderInstance(const STRING& meshname);
	virtual void	SetPosition(const POS& pos);
	virtual void	Update(float dt);

public:
	void			SetAIComponent(AIComponent* pAI) { m_pAI = pAI; }
	AIComponent*	GetAIComponent() const { return m_pAI; }

	void			SetDestPos(const Ogre::Vector3& destPos);
	const Ogre::Vector3& GetDestPos() const	{ return m_destPos; }
	
	void			PlayAnimation(const STRING& animName, bool bLoop);
	void			StopAnimation();

public:
	///lua导出函数
	int				PlayAnimation(lua_State* L);
	int				StopAnimation(lua_State* L);
	int				FindPath(lua_State* L);
	int				SetState(lua_State* L);
	int				SetDestPosition(lua_State* L);
	int				GetDestPosition(lua_State* L);
	int				UpdatePathFinding(lua_State* L);
	int				GetCurCommandType(lua_State* L);
	int				AttachRes(lua_State* L);
	int				DetachRes(lua_State* L);

private:
	AIComponent*		m_pAI;			//AI组件
	Ogre::Vector3		m_destPos;
	Ogre::AnimationState* m_pAnimState;	//当前播放动画
	
};


#endif	//Unit_h__