/********************************************************************
	created:	24:12:2012   0:15
	filename: 	F:\MiniCraft\Editor\SceneEdit\Action\ActionTerrainDeform.h
	author:		maval
	
	purpose:	���θ߶ȱ��α༭״̬
*********************************************************************/

#ifndef ActionTerrainDeform_h__
#define ActionTerrainDeform_h__

#include "ActionBase.h"


class ActionTerrainDeform : public ActionBase
{
public:
	ActionTerrainDeform() {}
	~ActionTerrainDeform() {}

public:
	virtual	void	Enter();
	virtual void	Leave();
	virtual	void	OnMouseLButtonDown(const SActionParam& param);
	virtual void	OnMouseLButtonUp(const SActionParam& param);
	virtual void	OnMouseMove(const SActionParam& param);
};



#endif // ActionTerrainDeform_h__