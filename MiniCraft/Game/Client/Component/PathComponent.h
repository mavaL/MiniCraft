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
#include "KratosPrerequisites.h"

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
		
	static void		InitRecastLib(const STRING& sceneName);
	static void		DestroyRecastLib();
	static void		UpdateRecastLib(float dt);
	//尝试调整世界坐标在有效的NavMesh上
	static bool		ClampPosToNavMesh(POS& wPos);

public:
	virtual void	Update(float dt) {}
	bool			UpdatePathFinding(float dt);
	void			StopAgent();
	POS				GetRandomPositionOnNavmesh();
	//进行寻路.bJustTry为true则只是尝试目标点是否可寻路
	bool			FindPath(POS& destPos, bool bJustTry);
	//停止寻路
	bool			StopMove();
	const POS		GetAgentPos() const;
	void			SetDestPos(const POS& destPos);
	POS&			GetDestPos()	{ return m_destPos; }
	bool			IsMoving() const	{ return m_bIsMoving; }
	//开启/关闭阻碍规避
	void			EnableObstcleAvoidance(bool bEnable);

private:
	static OgreRecast*			m_pRecast;
	static OgreDetourCrowd*		m_pDetour;
	static OgreDetourTileCache*	m_pDetourTileCache;

	dtCrowdAgent*		m_pAgent;
	int					m_agentID;
	POS					m_destPos;
	bool				m_bIsMoving;
};

#endif // PathComponent_h__