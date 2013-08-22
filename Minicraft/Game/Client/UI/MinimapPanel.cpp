#include "stdafx.h"
#include "GameDefine.h"
#include "MinimapPanel.h"
#include "GUIManager.h"
#include "OgreManager.h"
#include "Scene.h"

using namespace CEGUI;

UiMinimapPanel::UiMinimapPanel()
:m_pWndRoot(nullptr)
,m_image(nullptr)
,m_pWndMinimap(nullptr)
{

}

void UiMinimapPanel::Init()
{
	m_pWndRoot = GUIMANAGER.LoadWindowLayout("MinimapPanel.layout");
	assert(m_pWndRoot);
	m_pWndMinimap = m_pWndRoot->getChild("MinimapTex");
	assert(m_pWndMinimap);
	m_pWndRoot->hide();
}

void UiMinimapPanel::Destroy()
{
	GUIMANAGER.UnloadWindowLayout(m_pWndRoot);
	m_pWndRoot = nullptr;
	m_pWndMinimap = nullptr;
	m_RT.setNull();
	ImagesetManager::getSingleton().destroy(*m_image);
}

void UiMinimapPanel::Update()
{

}

void UiMinimapPanel::GenerateMinimap( Kratos::Scene* pScene )
{
	using namespace Ogre;

	Kratos::COgreManager& renderMgr = RenderManager;
	SceneManager* sm = renderMgr.m_pSceneMgr;
	Camera* cam = sm->createCamera("MinimapCamera");

	int w = (int)m_pWndMinimap->getWidth().asAbsolute(0);
	int h = (int)m_pWndMinimap->getHeight().asAbsolute(0);
	m_RT = renderMgr.CreateRT("MinimapRT", w, h, Ogre::PF_A8R8G8B8);
	assert(m_RT.get());

	Ogre::RenderTexture* rt = m_RT->getBuffer()->getRenderTarget();
	rt->setAutoUpdated(false);

	cam->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	cam->setPosition(0, 500, 0);
	cam->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);
	cam->setFOVy(Ogre::Radian(90));
	cam->setAspectRatio(w / (float)h);
	cam->setOrthoWindowWidth(w);  
	cam->setNearClipDistance(10);
	cam->setFarClipDistance(20000);

	Ogre::Viewport* vp = rt->addViewport(cam);

	sm->setAmbientLight(COLOR::White);
	const Kratos::SEffectConfig origCfg = renderMgr.GetEffectConfig();
	renderMgr.EnableFXAA(false);
	renderMgr.EnableShadow(false);
	renderMgr.EnableSharpen(false);
	renderMgr.EnableSSAO(false);

	vp->setVisibilityMask(eRenderType_Default);
	vp->setBackgroundColour(COLOR::Blue);
	vp->setOverlaysEnabled(false);
 	rt->update();
 
 	rt->writeContentsToFile("minimap.png");

	OgreTexture* tex = static_cast<OgreTexture*>(&System::getSingleton().getRenderer()->createTexture());
	tex->setOgreTexture(m_RT);
	m_image = &CEGUI::ImagesetManager::getSingleton().create("MinimapImageSet", *tex);
	m_image->defineImage("MinimapImage", CEGUI::Point(0,0), tex->getSize(), CEGUI::Point(0,0));
	m_pWndMinimap->setProperty("Image", "set:MinimapImageSet image:MinimapImage");

	//»Ö¸´
	renderMgr.EnableFXAA(origCfg.bFXAA);
	renderMgr.EnableShadow(origCfg.bShadow);
	renderMgr.EnableSharpen(origCfg.bSharpen);
	renderMgr.EnableSSAO(origCfg.bSSAO);
	sm->setAmbientLight(pScene->GetAmbient());
 	sm->destroyCamera(cam);
}


