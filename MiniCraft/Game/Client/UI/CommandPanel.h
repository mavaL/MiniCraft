/********************************************************************
	created:	3:2:2013   1:59
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\UI\CommandPanel.h
	author:		maval
	
	purpose:	√¸¡Ó√Ê∞ÂUIœ‡πÿ
*********************************************************************/

#ifndef CommandPanel_h__
#define CommandPanel_h__

namespace CEGUI
{
	class Window;
}


class UiCommandPanel
{
public:
	UiCommandPanel();
	~UiCommandPanel() {}

public:
	void	Init();
	void	Destroy();

private:
	CEGUI::Window*	m_pLayout;
};


#endif // CommandPanel_h__