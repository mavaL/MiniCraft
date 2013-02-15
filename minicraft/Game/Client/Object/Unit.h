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

class Unit : public SelectableObject
{
	DECL_PARAM_COMMAND(ClampPos)
	DECL_PARAM_COMMAND(UnitName)
public:
	Unit();
	Unit(lua_State* L) {}
	~Unit();

	static const char className[];
	static Luna<Unit>::RegType methods[];

	const static STRING UNIT_TABLE_NAME;

public:
	virtual eObjectType GetType() const { return eObjectType_Unit; }
	virtual void	Update(float dt);
	virtual void	_OnCommandFinished(eCommandType cmd);

public:
	void			SetUnitName(const STRING& name);
	const STRING&	GetUnitName() const {return m_unitName; } 
	void			PlayAnimation(eAnimation type, bool bLoop);
	void			StopAnimation();
	//设置单位坐标,内部会自动夹持到地形上
	void			SetClampPos(const POS& pos);
	const POS&		GetClampPos() const { return GetPosition(); }
	//停止行动
	void			StopAction();
	void			SetStopTime(float fTime) { m_fStopTime = fTime; }
	float			GetStopTime() const	{ return m_fStopTime; }

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
	STRING					m_unitName;		//单位名称,如:scv,marine...
	Ogre::AnimationState*	m_pAnimState;	//当前播放动画
	float					m_fStopTime;	//已经进入停止状态多久
	
};


#endif	//Unit_h__