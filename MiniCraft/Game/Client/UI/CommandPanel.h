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
class SelectableObject;

class UiCommandPanel
{
public:
	UiCommandPanel();
	~UiCommandPanel() {}

public:
	void	Init();
	void	Destroy();
	//设置能力槽图标
	void	CEGUI_SetAbilitySlot(int slotIndex, bool bEnable, const std::string& imgName = "");
	//设置当前选中物体
	void	SetActiveObject(SelectableObject* pObject) { m_pActiveObj = pObject; }

private:
	///UI响应
	template<int Slot>
	bool CEGUI_OnCommandBtnClicked(const CEGUI::EventArgs& e);

private:
	CEGUI::Window*		m_pLayout;
	SelectableObject*	m_pActiveObj;
};


#endif // CommandPanel_h__