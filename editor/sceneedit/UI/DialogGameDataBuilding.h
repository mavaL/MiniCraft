/********************************************************************
	created:	29:1:2013   19:48
	filename: 	F:\MiniCraft\Editor\SceneEdit\UI\DialogGameDataBuilding.h
	author:		maval
	
	purpose:	建筑数据编辑对话框
*********************************************************************/

#ifndef DialogGameDataBuilding_h__
#define DialogGameDataBuilding_h__

#include "../resource.h"
#include "Manipulator/ManipulatorScene.h"


class DialogGameDataBuilding : public CDialog
{
public:
	enum { IDD = IDD_DlgGameDataBuilding };
	
	DialogGameDataBuilding();
	~DialogGameDataBuilding() {}

protected:
	virtual BOOL	OnInitDialog();

private:
	DECLARE_MESSAGE_MAP()
	template<int race> afx_msg void	OnSwitchRace();
	afx_msg void	OnPaint();
	afx_msg void	OnBuildingSelChange();
	afx_msg void	OnSlotSelChange();
	afx_msg void	OnAbilitySelChange();

private:
	bool			_GetCurBuildingName(std::wstring& retName) const;
	bool			_GetCurAbilityName(std::wstring& retName) const;
	bool			_GetCurSlot(int& retIndex) const;

private:
	eGameRace		m_curEditRace;
};


#endif // DialogGameDataBuilding_h__














