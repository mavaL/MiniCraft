#include "stdafx.h"
#include "ActionObjectEdit.h"
#include "EditorDefine.h"
#include "Manipulator/ManipulatorScene.h"
#include "Gizmo.h"


ActionObjectEdit::ActionObjectEdit()
:m_bLBDown(false)
{

}
void ActionObjectEdit::Enter()
{
	ManipulatorObject& manObject = ManipulatorSystem.GetObject();
	ManipulatorObject::eEditMode mode = manObject.GetCurEditMode();

	//ÏÔÊ¾gizmo
	manObject.ShowEntityGizmo(manObject.GetSelection(), true, mode);
	//ÏÔÊ¾°üÎ§ºÐ
	manObject.ShowEntityGizmo(manObject.GetSelection(), true, ManipulatorObject::eEditMode_Select);
}

void ActionObjectEdit::Leave()
{
	ManipulatorObject& manObject = ManipulatorSystem.GetObject();
	//Òþ²Øgizmo
	manObject.ShowEntityGizmo(manObject.GetSelection(), false, ManipulatorObject::eEditMode_Move);
	manObject.ShowEntityGizmo(manObject.GetSelection(), false, ManipulatorObject::eEditMode_Rotate);
	//Òþ²Ø°üÎ§ºÐ
	manObject.ShowEntityGizmo(manObject.GetSelection(), false, ManipulatorObject::eEditMode_Select);
}

void ActionObjectEdit::OnMouseLButtonDown( const SActionParam& param )
{
	m_bLBDown = true;
	GizmoAxis* pGizmo = ManipulatorSystem.GetObject().GetGizmoAxis();
	ManipulatorObject::eEditMode mode = ManipulatorSystem.GetObject().GetCurEditMode();
	if (pGizmo->IsActive())
	{
		assert(pGizmo->GetActiveAxis() != eAxis_None);
		const Ogre::Ray ray = ManipulatorSystem.m_pMainCamera->getCameraToViewportRay(param.m_ptRelative.x, param.m_ptRelative.y);
		if(mode == ManipulatorObject::eEditMode_Move)
			m_vecAdjust = _ComputeTranslateVector(ray, pGizmo->GetActiveAxis(), false);
		else if(mode == ManipulatorObject::eEditMode_Scale)
			m_vecAdjust = _ComputeTranslateVector(ray, pGizmo->GetActiveAxis(), true);
	}
}

void ActionObjectEdit::OnMouseLButtonUp( const SActionParam& param )
{
	m_bLBDown = false;
}

void ActionObjectEdit::OnMouseMove( const SActionParam& param )
{
	ManipulatorObject& manObject = ManipulatorSystem.GetObject();
	GizmoAxis* pGizmo = manObject.GetGizmoAxis();
	const Ogre::Ray ray = ManipulatorSystem.m_pMainCamera->getCameraToViewportRay(param.m_ptRelative.x, param.m_ptRelative.y);
	ManipulatorObject::eEditMode mode = manObject.GetCurEditMode();

	if (pGizmo->IsActive() && m_bLBDown)
	{
		//¼ÆËãÊó±êÆ«ÒÆÁ¿
		Ogre::Entity* pSelEnt = manObject.GetSelection();
		Ogre::Vector3 oldPos = pSelEnt->getParentSceneNode()->_getDerivedPosition();
		Ogre::SceneNode* pNode = manObject.GetSelection()->getParentSceneNode();

		if(mode == ManipulatorObject::eEditMode_Move)
		{
			Ogre::Vector3 delta = _ComputeTranslateVector(ray, pGizmo->GetActiveAxis(), false);
			//ÒÆ¶¯
			Ogre::Vector3 newPos = delta - m_vecAdjust + oldPos;
			pNode->_setDerivedPosition(newPos);
		}
		else if (mode == ManipulatorObject::eEditMode_Rotate)
		{
			//Ðý×ª
			switch (pGizmo->GetActiveAxis())
			{
			case eAxis_X: pNode->pitch(Ogre::Radian(param.m_ptDeltaRel.x * 15.0f)); break;
			case eAxis_Y: pNode->yaw(Ogre::Radian(param.m_ptDeltaRel.x * 15.0f)); break;
			case eAxis_Z: pNode->roll(Ogre::Radian(param.m_ptDeltaRel.x * 15.0f)); break;
			}
		}
		else if (mode == ManipulatorObject::eEditMode_Scale)
		{
			//Ëõ·Å
			float scale = param.m_ptDeltaRel.x > 0 ? 1.2f : 0.8f;
			switch (pGizmo->GetActiveAxis())
			{
			case eAxis_X: pNode->scale(scale, 1.0f, 1.0f); break;
			case eAxis_Y: pNode->scale(1.0f, scale, 1.0f); break;
			case eAxis_Z: pNode->scale(1.0f, 1.0f, scale); break;
			}
		}
	}
	else
	{
		pGizmo->Update(ray, mode);
	}
}

const Ogre::Vector3 ActionObjectEdit::_ComputeTranslateVector(const Ogre::Ray& ray, eAxis axis, bool bAxisLocal)
{
	using namespace Ogre;

	Entity* pSelEnt = ManipulatorSystem.GetObject().GetSelection();
	Quaternion qOrient = pSelEnt->getParentSceneNode()->_getDerivedOrientation();
	Vector3 oldPos = pSelEnt->getParentSceneNode()->_getDerivedPosition();

	Plane planeX(qOrient.xAxis(), oldPos);
	Plane planeY(qOrient.yAxis(), oldPos);
	Plane planeZ(qOrient.zAxis(), oldPos);

	float vX = planeX.projectVector(ray.getDirection()).length();
	float vY = planeY.projectVector(ray.getDirection()).length();
	float vZ = planeZ.projectVector(ray.getDirection()).length();

	Vector3 vAxis = Vector3::ZERO;
	switch (axis)
	{
	case eAxis_X: { vX = 10000.0f; vAxis = bAxisLocal ? Vector3::UNIT_X : qOrient.xAxis(); } break;
	case eAxis_Y: { vY = 10000.0f; vAxis = bAxisLocal ? Vector3::UNIT_Y : qOrient.yAxis(); } break;
	case eAxis_Z: { vZ = 10000.0f; vAxis = bAxisLocal ? Vector3::UNIT_Z : qOrient.zAxis(); } break;
	}

	Plane planeToUse;
	if (vX < vY && vX < vZ)
	{
		planeToUse = planeX;
	}
	else
	{
		if (vY < vX && vY < vZ )
			planeToUse = planeY;
		else
			planeToUse = planeZ;
	}

	std::pair<bool,Ogre::Real> result;
	result = ray.intersects(planeToUse);
	if (result.first)
	{
		Vector3 Proj = ray.getPoint(result.second) - oldPos;
		vAxis.normalise();
		Vector3 deltaPos = vAxis.dotProduct(Proj) * vAxis;

		return deltaPos;
	}
	
	return Vector3::ZERO;
}




