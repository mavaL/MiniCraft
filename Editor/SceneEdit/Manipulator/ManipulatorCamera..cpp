#include "stdafx.h"
#include "ManipulatorCamera.h"
#include "OgreManager.h"

ManipulatorCamera::ManipulatorCamera()
:m_pCamera(RenderManager.m_pMainCamera)
,m_bRBDown(false)
,m_bLBDown(false)
,m_pTarget(nullptr)
{
	Reset();
}

void ManipulatorCamera::Reset()
{
	m_pCamera->setFixedYawAxis(true);
// 	m_pCamera->setNearClipDistance(0.1f);
// 	m_pCamera->setFarClipDistance(5000);
	m_pCamera->setPosition(0,30,0);
	m_pCamera->lookAt(0,0,10);
	m_pTarget = nullptr;
	m_type = eCameraType_RTS;
}

void ManipulatorCamera::OnLButtonDown( const SActionParam& param )
{
	m_bLBDown = true;
}

void ManipulatorCamera::OnLButtonUp( const SActionParam& param )
{
	m_bLBDown = false;
}

void ManipulatorCamera::OnRButtonDown( const SActionParam& param )
{
	m_bRBDown = true;
}

void ManipulatorCamera::OnRButtonUp( const SActionParam& param )
{
	m_bRBDown = false;
}

void ManipulatorCamera::OnMouseMove( const SActionParam& param )
{
	if(m_type == eCameraType_RTS && m_bRBDown)
	{
		m_pCamera->move(Ogre::Vector3(param.m_ptDeltaRel.x*30, 0, param.m_ptDeltaRel.y*30));
	}
	else if (m_type == eCameraType_ModelViewer && m_bLBDown)
	{
		const Ogre::Vector3& targetPos = m_pTarget->getParentSceneNode()->_getDerivedPosition();
		const float radius = m_pCamera->getRealPosition().distance(targetPos);
		m_pCamera->setPosition(targetPos);
		m_pCamera->yaw(Ogre::Radian(-param.m_ptDeltaRel.x*5));
		m_pCamera->pitch(Ogre::Radian(-param.m_ptDeltaRel.y*5));
		m_pCamera->moveRelative(Ogre::Vector3(0,0,radius));
	}
}

void ManipulatorCamera::OnMouseWheel( short nNotch )
{
	m_pCamera->move(Ogre::Vector3(0,(float)nNotch*2,0));
}

void ManipulatorCamera::SetModelViewerTarget( Ogre::Entity* pTarget )
{
	assert(pTarget);
	if (m_type == eCameraType_ModelViewer)
	{
		m_pCamera->setAutoTracking(true, pTarget->getParentSceneNode());
		m_pTarget = pTarget;
	}
}

void ManipulatorCamera::SetType( eCameraType type )
{
	if(m_type == type)
		return;

	if (type == eCameraType_RTS)
	{
		//�ָ�ΪRTS�ӽ�
		const Ogre::Vector3& pos = m_pCamera->getRealPosition();
		m_pCamera->setAutoTracking(false);
		m_pCamera->setPosition(pos.x, 30, pos.z);
		m_pCamera->lookAt(pos.x, 0, pos.z + 10);
	}
	m_type = type;
}
