/********************************************************************
	created:	23:1:2013   1:06
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Selectable.h
	author:		maval
	
	purpose:	可选中物体基类.可选中物体能够拥有技能Command.
*********************************************************************/

#ifndef SelectableObject_h__
#define SelectableObject_h__

#include "ObjectBase.h"
#include "GameDefine.h"
#include "GameDataDef.h"
#include "Command.h"
#include "AIComponent.h"
#include "PathComponent.h"

///该类实现来自OgreProcedural库
class SelectableObject : public RenderableObject
{
public:
	SelectableObject();
	~SelectableObject();

public:
	virtual	void	Update(float dt);
	virtual void	CreateRenderInstance();
	//当前命令完成,接到通知
	virtual void	_OnCommandFinished(eCommandType cmd);

public:
	//!!在Ogre Root销毁前必须调用
	static void		ReleaseMeshCache();

	void			SetSelected(bool bSelected);
	bool			GetSelected() const { return m_bSelected; }
	void			SetAbility(int slotIndex, const SAbilityData* pData);
	void			SetActiveAbility(int slotIndex) { m_pActiveAbility = m_pAbilitySlots[slotIndex]; } 
	const SAbilityData*	GetActiveAbility() const { return m_pActiveAbility; }
	bool			HasAbility(eCommandType type);

	inline AiComponent*		GetAi() { return QueryComponent(this, eComponentType_AI, AiComponent); }
	inline PathComponent*	GetPath() { return QueryComponent(this, eComponentType_Path, PathComponent); }
	
private:
	Ogre::MeshPtr	_CreateSelectionCircleMesh(const Ogre::MeshPtr& objMesh);
	void			_OnSelected(bool bSelected);

	typedef std::unordered_map<STRING, Ogre::MeshPtr>	SelectionCircleCache;
	static SelectionCircleCache	m_selCircleCache;	//cache每种单位的选中框mesh
	bool						m_bSelected;		//该对象是否被选中
	Ogre::SceneNode*			m_pSelCircleNode;
	SAbilityData*				m_pAbilitySlots[MAX_ABILITY_SLOT];
	const SAbilityData*			m_pActiveAbility;
};

#endif // SelectableObject_h__