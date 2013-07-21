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
#include "EventCallback.h"

struct lua_State;
struct SUnitData;

class Unit 
	: public SelectableObject
	, public Kratos::EventCallbackManager<UnitEventCallback>
{
	DECL_PARAM_COMMAND(Name)
	DECL_PARAM_COMMAND(Race)
public:
	Unit();
	Unit(lua_State* L) {}
	~Unit();

	static const char className[];
	static Luna<Unit>::RegType methods[];

	const static STRING UNIT_TABLE_NAME;

	////////////////////////////////////////////////////////////////////////
	///回调事件
	virtual	void	OnUnitDeath(Unit* pUnit);

public:
	virtual eObjectType GetType() const { return eObjectType_Unit; }
	virtual void	Update(float dt);
	virtual void	CreateRenderInstance();
	virtual void	DestroyRenderInstance();
	virtual void	InitTeamColor(const COLOR& color);
	virtual void	_OnCommandFinished(eCommandType cmd);
	void			_OnAttacked(Unit* attcker);

public:
	void			Init();
	void			SetName(const STRING& name);
	const STRING&	GetName() const		{ return m_unitName; }
	void			SetRace(int race)	{ assert(0); /*shouldn't be called!*/ }
	int				GetRace() const;
	const STRING&	GetPortraitName() const;
	float			GetProduceTime() const;
	float			GetAttackInterval() const;
	float			GetAttackDamage() const;
	void			SetShowHpBar(bool bShow);

	SUnitData*		GetUnitData() { return m_data; }
	Ogre::Entity*	GetPortrait(Ogre::SceneManager* sm, Ogre::Light* light);
	void			StopAction();
	void			SetStopTime(float fTime) { m_fStopTime = fTime; }
	float			GetStopTime() const	{ return m_fStopTime; }
	void			SetAttackTarget(int ID);
	Unit*			GetAttackTarget();
	float			GetLastAttackPastTime() { return m_fAttkTime; }
	void			SetLastAttackPastTime(float time) { m_fAttkTime = time; }
	void			SetHP(float hp) { m_fCurHP = hp; }
	float			GetHP() const	{ return m_fCurHP; }

public:
	///lua导出函数
	int				SetBlackboardParamInt(lua_State* L);
	int				SetBlackboardParamFloat(lua_State* L);
	int				SetBlackboardParamBool(lua_State* L);
	int				GetHarvestStage(lua_State* L);
	int				GetGatheringTime(lua_State* L);
	int				GetAttackTargetID(lua_State* L);
	int				SetAttackTargetID(lua_State* L);
	int				FindNearestEnemy(lua_State* L);

protected:
	virtual void	_OnSelected(bool bSelected);

private:
	SUnitData*				m_data;
	STRING					m_unitName;		//单位名称,如:scv,marine...
	float					m_fStopTime;	//已经进入停止状态多久
	static std::unordered_map<STRING, Ogre::Entity*>			m_portraitCache;		//cache每种单位的3D肖像模型
	static std::unordered_map<STRING, Ogre::AnimationState*>	m_portraitAnimCache;	//cache每种单位的3D肖像动画
	Ogre::BillboardSet*		m_pHealthBar;
	Kratos::ScriptSystem*	m_pScriptSystem;
	int						m_attkTargetID;
	float					m_fAttkTime;	//距上次攻击过去的时间
	float					m_fCurHP;		//当前血量
	float					m_fFullHP;		//满血
};


#endif	//Unit_h__