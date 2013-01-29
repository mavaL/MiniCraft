/********************************************************************
	created:	29:1:2013   19:48
	filename: 	F:\MiniCraft\Editor\SceneEdit\UI\DialogGameDataBuilding.h
	author:		maval
	
	purpose:	建筑数据编辑对话框
*********************************************************************/

#ifndef DialogGameDataBuilding_h__
#define DialogGameDataBuilding_h__

#include "../resource.h"


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
	afx_msg void	OnSwitchTerran();
	afx_msg void	OnSwitchZerg();
	afx_msg void	OnPaint();
	afx_msg void	OnBuildingSelChange();

private:
};


#endif // DialogGameDataBuilding_h__














