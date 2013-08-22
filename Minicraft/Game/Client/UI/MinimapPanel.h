/********************************************************************
	created:	10:5:2013   23:08
	filename	MinimapPanel.h
	author:		maval

	purpose:	Ð¡µØÍ¼Ãæ°å
				not working yet...
*********************************************************************/

#ifndef MinimapPanel_h__
#define MinimapPanel_h__

#include "KratosPrerequisites.h"

class UiMinimapPanel
{
public:
	UiMinimapPanel();
	~UiMinimapPanel() {}

public:
	void	Init();
	void	Update();
	void	Destroy();
	void	GenerateMinimap(Kratos::Scene* pScene);

private:
	CEGUI::Window*			m_pWndRoot;
	CEGUI::Window*			m_pWndMinimap;
	Ogre::TexturePtr		m_RT;
	CEGUI::Imageset*		m_image;
};


#endif // MinimapPanel_h__