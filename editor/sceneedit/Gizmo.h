/********************************************************************
	created:	22:12:2012   12:01
	filename: 	F:\MiniCraft\Editor\SceneEdit\Gizmo.h
	author:		maval
	
	purpose:	编辑器用的各种指示器
*********************************************************************/

#ifndef Gizmo_h__
#define Gizmo_h__


//
//矩形
//
class GizmoRectangle : public Ogre::SimpleRenderable
{
public:
	GizmoRectangle() {}
	virtual ~GizmoRectangle();

public:
	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { (void)cam; return 0; }
	virtual Ogre::Real getBoundingRadius(void) const { return 0; }

public:
	void	InitRenderable(float w, float h);
	void	DestroyRenderable();
	void	UpdatePosition(const Ogre::Vector3& pos, float w, float h);

protected:
	//返回单位矩阵.因为顶点坐标直接给的世界坐标
	virtual void getWorldTransforms( Ogre::Matrix4* xform ) const;

private:
	//点的分布密度,即1个世界单位对应点的个数
	static const int POINT_DENSITY = 5;
};

//
//圆形
//
class GizmoCircle : public Ogre::SimpleRenderable
{
public:
	GizmoCircle() {}
	virtual ~GizmoCircle();

public:
	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const { (void)cam; return 0; }
	virtual Ogre::Real getBoundingRadius(void) const { return 0; }

public:
	void	InitRenderable();
	void	DestroyRenderable() {}
};


#endif // Gizmo_h__







