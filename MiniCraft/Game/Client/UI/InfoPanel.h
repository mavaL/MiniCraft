/********************************************************************
	created:	9:2:2013   19:58
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\UI\InfoPanel.h
	author:		maval
	
	purpose:	信息面板UI相关
*********************************************************************/

#ifndef InfoPanel_h__
#define InfoPanel_h__

namespace CEGUI
{
	class Window;
}

class SelectableObject;

class UiInfoPanel
{
public:
	UiInfoPanel();
	~UiInfoPanel() {}

public:
	void	Init();
	void	Update();
	void	Destroy();
	void	ShowInfoIcon(bool bShow, const std::string& imgSetName = "", const std::string& imgName = "");
	void	ShowProgressQueue(bool bShow);
	void	SetProgress(float fPercent);
	void	SetVisible(bool bVisible);
	void	SetActiveObject(SelectableObject* pObj) { m_pActiveObject = pObj; }

private:
	CEGUI::Window*		m_pLayout;
	SelectableObject*	m_pActiveObject;
};


#endif // InfoPanel_h__