/********************************************************************
	created:	1:1:2013   21:46
	filename: 	F:\MiniCraft\Editor\SceneEdit\Action\ActionObjectEdit.h
	author:		maval
	
	purpose:	物体编辑状态:移动,旋转,缩放
*********************************************************************/

#ifndef ActionObjectEdit_h__
#define ActionObjectEdit_h__



#include "ActionBase.h"
#include "EditorDefine.h"


class ActionObjectEdit : public ActionBase
{
public:
	ActionObjectEdit();
	~ActionObjectEdit() {}

public:
	virtual	void	Enter();
	virtual void	Leave();
	virtual	void	OnMouseLButtonDown(const SActionParam& param);
	virtual void	OnMouseLButtonUp(const SActionParam& param);
	virtual void	OnMouseMove(const SActionParam& param);

private:
	bool	m_bCanMove;
	bool	m_bLBDown;
	eAxis	m_curActiveAxis;
	Ogre::Vector3 m_vecAdjust;

	//抽取自Ogitor
	const Ogre::Vector3	_ComputeTranslateVector(const Ogre::Ray& ray, eAxis axis);
};



#endif // ActionObjectEdit_h__