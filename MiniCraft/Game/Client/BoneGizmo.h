/********************************************************************
	created:	25:4:2013   22:06
	filename	BoneGizmo.h
	author:		maval

	purpose:	¹Ç÷Àdebug render
*********************************************************************/

#ifndef BoneGizmo_h__
#define BoneGizmo_h__

class GizmoAxis
{
public:
	GizmoAxis();
	~GizmoAxis();

	enum eAxis
	{
		eAxis_X,
		eAxis_Y,
		eAxis_Z,
		eAxis_None
	};

public:
	void	Attach(Ogre::Node* pNode);
	void	Detach();
	void	Show(bool bShow);
	void	Update();

private:
	void	_Init();
	void	_Destroy();

private:
	Ogre::SceneNode*	m_pObjGizmoNode;
	Ogre::Node*			m_pAttachNode;
	Ogre::SceneNode*	m_pXAxis;
	Ogre::SceneNode*	m_pYAxis;
	Ogre::SceneNode*	m_pZAxis;
	Ogre::Entity*		m_pAxisMove[3];		//×ø±êÖá
};

#endif // BoneGizmo_h__