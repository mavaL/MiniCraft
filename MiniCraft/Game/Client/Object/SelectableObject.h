/********************************************************************
	created:	23:1:2013   1:06
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Selectable.h
	author:		maval
	
	purpose:	可选中物体基类.
*********************************************************************/

#ifndef SelectableObject_h__
#define SelectableObject_h__

#include "ObjectBase.h"
#include "GameDefine.h"
#include "Ability.h"


///该类实现来自OgreProcedural库
class SelectableObject : public RenderableObject
{
public:
	SelectableObject();
	~SelectableObject();

public:
	void	SetSelected(bool bSelected);
	bool	GetSelected() const { return m_bSelected; }

	//!!在Ogre Root销毁前必须调用
	static void	ReleaseMeshCache();

protected:
	//子类进行被选中行为的特化
	virtual	void	OnSelected(bool bSelected) {}

private:
	Ogre::MeshPtr	_CreateSelectionCircleMesh(const Ogre::MeshPtr& objMesh);

	typedef std::unordered_map<STRING, Ogre::MeshPtr>	SelectionCircleCache;
	static SelectionCircleCache	m_selCircleCache;	//cache每种单位的选中框mesh
	bool						m_bSelected;		//该对象是否被选中
	Ogre::SceneNode*			m_pSelCircleNode;

	typedef std::vector<Ability>	AbilitySlots;
	AbilitySlots				m_abilities;		//该物体的能力
};

#endif // SelectableObject_h__