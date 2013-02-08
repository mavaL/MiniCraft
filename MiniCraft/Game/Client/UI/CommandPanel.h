/********************************************************************
	created:	3:2:2013   1:59
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\UI\CommandPanel.h
	author:		maval
	
	purpose:	命令面板UI相关
*********************************************************************/

#ifndef CommandPanel_h__
#define CommandPanel_h__

namespace CEGUI
{
	class Window;
}

//技能槽状态
enum eSlotState
{
	eSlotState_Normal,
	eSlotState_Highlight,
	eSlotState_Disable,
	eSlotState_Empty
};

class UiCommandPanel
{
public:
	UiCommandPanel();
	~UiCommandPanel() {}

public:
	void	Init();
	void	Destroy();
	//设置能力槽图标
	void	CEGUI_SetAbilitySlot(int slotIndex, eSlotState state, const std::string& imgName = "");

private:
	CEGUI::Window*	m_pLayout;
};


#endif // CommandPanel_h__