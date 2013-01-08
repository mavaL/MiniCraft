/********************************************************************
	created:	27:12:2012   0:04
	filename: 	F:\MiniCraft\Editor\SceneEdit\UI\TerrainPropertyPane.h
	author:		maval
	
	purpose:	地形属性面板
*********************************************************************/

#ifndef TerrainPropertyPane_h__
#define TerrainPropertyPane_h__


#include "PropertiesPane.h"


class PropertyPaneTerrain : public CPropertiesPane
{
public:
	enum ePropertyID
	{
		////property range [propStart, propEnd)
		propStart = 0,
		////These are ReadOnly items
		propWorldSize = 0,
		propMapSize,
		propMinBatchSize,
		propMaxBatchSize,
		propSkirtSize,
		propLayerNormalMap0,
		propLayerNormalMap1,
		propLayerNormalMap2,
		propLayerNormalMap3,
		propLayerNormalMap4,
		////////These are Mutable items
		propMutableItemStart,
		propMaxPixelError = propMutableItemStart,
		propCompMapDist,

		propLayerWorldSize0,
		propLayerWorldSize1,
		propLayerWorldSize2,
		propLayerWorldSize3,
		propLayerWorldSize4,
		propLayerDiffuseMap0,
		propLayerDiffuseMap1,
		propLayerDiffuseMap2,
		propLayerDiffuseMap3,
		propLayerDiffuseMap4,
		propMutableItemEnd,
		propEnd = propMutableItemEnd
	};

public:
	PropertyPaneTerrain():CPropertiesPane() {}
	~PropertyPaneTerrain() {}

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
	//创建layer纹理下拉列表
	void			_CreateLayerTexList(int itemID);

private:
	std::unordered_map<int, CXTPPropertyGridItem*>	m_mapItem;
};


#endif // TerrainPropertyPane_h__














