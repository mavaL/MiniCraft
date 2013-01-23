/********************************************************************
	created:	23:1:2013   1:06
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Selectable.h
	author:		maval
	
	purpose:	可选中物体基类.
*********************************************************************/

#ifndef Selectable_h__
#define Selectable_h__

#include "ObjectBase.h"
#include "GameDefine.h"


///该类实现来自OgreProcedural库
class SelectableObject : public RenderableObject
{
	typedef std::unordered_map<STRING, Ogre::MeshPtr>	SelectionCircleCache;

public:
	SelectableObject();
	~SelectableObject();

public:
	void	SetSelected(bool bSelected);
	bool	GetSelected() const { return m_bSelected; }

private:
	Ogre::MeshPtr	_CreateSelectionCircleMesh(const Ogre::MeshPtr& objMesh);

private:
	static SelectionCircleCache	m_selCircleCache;	//cache每种单位的选中框mesh

	bool						m_bSelected;		//该对象是否被选中
	Ogre::SceneNode*			m_pSelCircleNode;

};

#endif // Selectable_h__