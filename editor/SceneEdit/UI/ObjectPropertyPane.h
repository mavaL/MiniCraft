/********************************************************************
	created:	8:1:2013   0:54
	filename: 	F:\MiniCraft\Editor\SceneEdit\UI\ObjectPropertyPane.h
	author:		maval
	
	purpose:	物体属性面板
*********************************************************************/

#ifndef ObjectPropertyPane_h__
#define ObjectPropertyPane_h__


#include "PropertiesPane.h"

class CXTPPropertyGridItemVec3;

class PropertyPaneObject : public CPropertiesPane
{
public:
	enum ePropertyID
	{
		////property range [propStart, propEnd)
		propStart = 0,
		////These are ReadOnly items
		propMeshName = propStart,
		////////These are Mutable items
		propMutableItemStart,
		propPosition = propMutableItemStart,
		propOrientation,
		propScale,
		propAddToNavmesh,
		propIsBuilding,
		propMutableItemEnd,
		propEnd = propMutableItemEnd,

		//Auto managed property
		propPosX, propPosY, propPosZ,
		propOrientX, propOrientY, propOrientZ, propOrientW,
		propScaleX, propScaleY, propScaleZ
	};

public:
	PropertyPaneObject():CPropertiesPane() {}
	~PropertyPaneObject() {}

	//更新所有控件数据
	void	UpdateAllFromEngine();
	//更新控件数据
	void	UpdateProperty(int id);
	//允许/禁止编辑Mutable数据(ReadOnly数据不需要管,本来就不能编辑)
	void	EnableMutableProperty(BOOL bEnable);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);

private:
	Ogre::Vector3	_UpdateVec3ItemProperty(CXTPPropertyGridItemVec3* pItem);

	std::unordered_map<int, CXTPPropertyGridItem*>	m_mapItem;
};


#endif // ObjectPropertyPane_h__














