/********************************************************************
	created:	14:2:2013   13:13
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\PathComponent.h
	author:		maval
	
	purpose:	寻路组件,逻辑对象通过组合使用该类
*********************************************************************/

#ifndef PathComponent_h__
#define PathComponent_h__

#include "GameDefine.h"
#include "Component.h"

class OgreRecast;
class OgreDetourCrowd;
struct dtCrowdAgent;
class SelectableObject;

///寻路AI组件
class PathComponent : public Component
{
public:
	PathComponent(SelectableObject* pOwner);
	~PathComponent();

public:
	virtual void	Update(float dt) {}
	bool			_UpdatePathFinding(float dt);
	//进行寻路.bJustTry为true则只是尝试目标点是否可寻路
	bool			FindPath(const POS& destPos, bool bJustTry);
	//停止寻路
	bool			StopMove();
	const POS		GetAgentPos() const;
	void			SetDestPos(const POS& destPos);
	const POS&		GetDestPos() const	{ return m_destPos; }
	bool			IsMoving() const	{ return m_bIsMoving; }
	//开启/关闭阻碍规避
	void			EnableObstcleAvoidance(bool bEnable);

private:
	OgreRecast*			m_pRecast;
	OgreDetourCrowd*	m_pDetour;
	dtCrowdAgent*		m_pAgent;
	int					m_agentID;
	POS					m_destPos;
	bool				m_bIsMoving;
};

#endif // PathComponent_h__