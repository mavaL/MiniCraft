/********************************************************************
	created:	11:3:2013   1:52
	filename: 	F:\MiniCraft\Editor\SceneEdit\UI\AttachmentPropertyPane.h
	author:		maval
	
	purpose:	模型挂接物配置面板,attachment即对应M3模型在max中的attachment节点
*********************************************************************/

#ifndef AttachmentPropertyPane_h__
#define AttachmentPropertyPane_h__

#include "PropertiesPane.h"

class PropertyPaneAttachment : public CPropertiesPane
{
public:
	enum ePropertyID
	{
		////property range [propStart, propEnd)
		propStart = 0,
		////These are ReadOnly items
		propAnimName = propStart,						//模型当前动画
		////////These are Mutable items					
		propMutableItemStart,							
		propLocator = propMutableItemStart,				//挂接点选择
		propParticle,									//挂接粒子特效
		propStartTime,									//特效相对动画开始的时间
		propLifeTime,									//特效时长
		propMutableItemEnd,
		propEnd = propMutableItemEnd,
	};

public:
	PropertyPaneAttachment():CPropertiesPane() {}
	~PropertyPaneAttachment() {}

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
	std::unordered_map<int, CXTPPropertyGridItem*>	m_mapItem;
};


#endif // AttachmentPropertyPane_h__














