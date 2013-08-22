#include "stdafx.h"
#include "PortraitPanel.h"
#include "OgreManager.h"
#include "GUIManager.h"
#include "GameDefine.h"
#include "SelectableObject.h"
#include "GameDataDef.h"
#include "Unit.h"

using namespace CEGUI;

UiPortraitPanel::UiPortraitPanel()
:m_pWnd(nullptr)
,m_pActiveObject(nullptr)
,m_pSceneMgr(nullptr)
,m_image(nullptr)
,m_pLight(nullptr)
{

}

void UiPortraitPanel::Init()
{
	m_pWnd = WindowManager::getSingleton().getWindow("InfoPanel/Portrait");
	assert(m_pWnd);

	m_pSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_INTERIOR, "PortraitSceneMgr");
	Ogre::Camera* cam = m_pSceneMgr->createCamera("PortraitCamera");

	//打光
	m_pSceneMgr->setAmbientLight(COLOR(0.2f, 0.2f, 0.2f));

	m_pLight = m_pSceneMgr->createLight("PortraitLight");
	m_pLight->setType(Ogre::Light::LT_DIRECTIONAL);
	m_pLight->setDirection(FLOAT3(-1, -1, -1));
	m_pLight->setDiffuseColour(COLOR::White);

	int w = (int)m_pWnd->getWidth().asAbsolute(0);
	int h = (int)m_pWnd->getHeight().asAbsolute(0);
	m_RT = RenderManager.CreateRT("PortraitRT", w, h, Ogre::PF_A8R8G8B8);
	assert(m_RT.get());

	Ogre::RenderTexture* rt = m_RT->getBuffer()->getRenderTarget();
	rt->setAutoUpdated(false);

	//cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	cam->setNearClipDistance(0.1f);
	cam->setFarClipDistance(100);
	cam->setPosition(-0.1f, 0.72f, 0.3f);
	cam->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Z);
	cam->yaw(Ogre::Degree(-20));
	Ogre::Viewport* vp = rt->addViewport(cam);
	cam->setFOVy(Ogre::Radian(Ogre::Math::PI / 4));
	cam->setAspectRatio((Ogre::Real)vp->getActualWidth()/(Ogre::Real)vp->getActualHeight());

	OgreTexture* tex = static_cast<OgreTexture*>(&System::getSingleton().getRenderer()->createTexture());
	tex->setOgreTexture(m_RT);
	m_image = &CEGUI::ImagesetManager::getSingleton().create("PortraitImageSet", *tex);
	m_image->defineImage("PortraitImage", CEGUI::Point(0,0), tex->getSize(), CEGUI::Point(0,0));
}

void UiPortraitPanel::Destroy()
{
	m_pWnd = nullptr;
	m_RT.setNull();
	m_pActiveObject = nullptr;
	Ogre::Root::getSingleton().destroySceneManager(m_pSceneMgr);
	m_pSceneMgr = nullptr;
	ImagesetManager::getSingleton().destroy(*m_image);
}

void UiPortraitPanel::SetActiveObject( SelectableObject* pObj )
{
	m_pSceneMgr->getRootSceneNode()->detachAllObjects();
	m_pActiveObject = nullptr;
	//绑定肖像模型到节点
	if(pObj)
	{
		if (pObj->GetType() == eObjectType_Unit)
		{
			Unit* pUnit = static_cast<Unit*>(pObj);
			m_pSceneMgr->getRootSceneNode()->attachObject(pUnit->GetPortrait(m_pSceneMgr, m_pLight));
			m_pActiveObject = pObj;
			m_pWnd->setVisible(true);

			return;
		}
	}

	m_pWnd->setVisible(false);
}

void UiPortraitPanel::Update()
{
	if(!m_pActiveObject)
		return;

	//更新3D肖像的渲染
	m_RT->getBuffer()->getRenderTarget()->update();
	m_pWnd->setProperty("Image", "set:PortraitImageSet image:PortraitImage");
}


