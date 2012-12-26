/********************************************************************
	created:	24:12:2012   0:02
	filename: 	F:\MiniCraft\Editor\SceneEdit\Action\ActionBase.h
	author:		maval
	
	purpose:	编辑器行为状态基类.采用状态模式解耦编辑器所处的行为状态,如
				移动物体,刷地形高度等等所有行为.
*********************************************************************/
#ifndef ActionBase_h__
#define ActionBase_h__

//Action携带数据
struct SActionParam 
{
	Ogre::Vector2	m_ptPixel;		//鼠标在View的坐标,像素度量
	bool			m_bHitTerrain;	//鼠标所在点射线查询是否击中地形
	Ogre::Vector3	m_ptTerrain;	//鼠标所在点相交的地形坐标
};


class ActionBase
{
public:
	ActionBase() {}
	virtual ~ActionBase() {}

public:
	virtual	void	Enter() = 0;
	virtual void	Leave()	= 0;
	virtual	void	OnMouseLButtonDown(const SActionParam& param) {}
	virtual void	OnMouseLButtonUp(const SActionParam& param) {}
	virtual void	OnMouseRButtonDown(const SActionParam& param) {}
	virtual void	OnMouseRButtonUp(const SActionParam& param) {}
	virtual void	OnMouseMove(const SActionParam& param) {}
	virtual void	OnFrameMove(float dt) {}
};


#endif // ActionBase_h__







