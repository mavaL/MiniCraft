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

class UiInfoPanel
{
public:
	UiInfoPanel();
	~UiInfoPanel() {}

public:
	void	Init();
	void	Destroy();
	void	SetInfoIcon(const std::string& imgName);

private:
	CEGUI::Window*		m_pLayout;
};


#endif // InfoPanel_h__