/********************************************************************
	created:	7:3:2013   21:04
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\UI\PortraitPanel.h
	author:		maval
	
	purpose:	3D肖像面板UI相关
*********************************************************************/

#ifndef PortraitPanel_h__
#define PortraitPanel_h__

class SelectableObject;

class UiPortraitPanel
{
public:
	UiPortraitPanel();
	~UiPortraitPanel() {}

public:
	void	Init();
	void	Update();
	void	Destroy();
	void	SetActiveObject(SelectableObject* pObj);

private:
	SelectableObject*		m_pActiveObject;
	CEGUI::Window*			m_pWnd;
	Ogre::TexturePtr		m_RT;
	CEGUI::Imageset*		m_image;
	Ogre::SceneManager*		m_pSceneMgr;		//渲染肖像专用
	Ogre::Light*			m_pLight;			//主光
};


#endif // PortraitPanel_h__