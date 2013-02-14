/********************************************************************
	created:	14:2:2013   13:13
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\AI\PathComponent.h
	author:		maval
	
	purpose:	寻路组件,逻辑对象通过组合使用该类
*********************************************************************/

#ifndef PathComponent_h__
#define PathComponent_h__

#include "GameDefine.h"
#include "Command.h"
#include "ObjectState.h"

class OgreRecast;
class OgreDetourCrowd;
struct dtCrowdAgent;
class SelectableObject;

///寻路AI组件
class PathComponent
{
public:
	PathComponent(SelectableObject* pOwner);
	~PathComponent();

public:
	//进行寻路.bJustTry为true则只是尝试目标点是否可寻路
	bool			FindPath(const Ogre::Vector3& destPos, bool bJustTry);
	//停止寻路
	bool			StopMove();
	const POS		GetAgentPos() const;

private:
	SelectableObject*	m_pOwner;		//该组件所属对象
	OgreRecast*			m_pRecast;
	OgreDetourCrowd*	m_pDetour;
	dtCrowdAgent*		m_pAgent;
	int					m_agentID;
};

#endif // PathComponent_h__