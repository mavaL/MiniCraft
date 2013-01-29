#include "stdafx.h"
#include "DialogGameDataBuilding.h"
#include "Manipulator/ManipulatorScene.h"


BEGIN_MESSAGE_MAP(DialogGameDataBuilding, CDialog)
	ON_COMMAND(IDC_DlgBuilding_SwitchT, OnSwitchTerran)
	ON_COMMAND(IDC_DlgBuilding_SwitchZ, OnSwitchZerg)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_DlgBuilding_NameList, OnBuildingSelChange)
END_MESSAGE_MAP()

DialogGameDataBuilding::DialogGameDataBuilding()
:CDialog(DialogGameDataBuilding::IDD)
{

}

BOOL DialogGameDataBuilding::OnInitDialog()
{
	return CDialog::OnInitDialog();
}

void DialogGameDataBuilding::OnSwitchTerran()
{
	CComboBox* pCtrl = (CComboBox*)GetDlgItem(IDC_DlgBuilding_NameList);
	pCtrl->ResetContent();
	pCtrl->SetCurSel(-1);

	const auto vecNames = ManipulatorSystem.GetGameData().GetRaceBuildingNames(eGameRace_Terran);
	for(size_t i=0; i<vecNames.size(); ++i)
		pCtrl->AddString(vecNames[i].c_str());
}

void DialogGameDataBuilding::OnSwitchZerg()
{
	CComboBox* pCtrl = (CComboBox*)GetDlgItem(IDC_DlgBuilding_NameList);
	pCtrl->ResetContent();
	pCtrl->SetCurSel(-1);

	const auto vecNames = ManipulatorSystem.GetGameData().GetRaceBuildingNames(eGameRace_Zerg);
	for(size_t i=0; i<vecNames.size(); ++i)
		pCtrl->AddString(vecNames[i].c_str());
}

void DialogGameDataBuilding::OnPaint()
{
	CDialog::OnPaint();

	CComboBox* pCtrl = (CComboBox*)GetDlgItem(IDC_DlgBuilding_NameList);
	int iCurSel = pCtrl->GetCurSel();
	if(iCurSel == -1)
		return;

	CPaintDC dc(this);
	Gdiplus::Graphics g(dc.GetSafeHdc());
	//绘制建筑物图标
	eGameRace race = IsDlgButtonChecked(IDC_DlgBuilding_SwitchT) ? eGameRace_Terran : eGameRace_Zerg;
	CString strCurSel;
	pCtrl->GetLBText(iCurSel, strCurSel);
	const SBuildingData* pData = ManipulatorSystem.GetGameData().GetBuildingData(race, (LPCTSTR)strCurSel);
	Gdiplus::Bitmap* bm = ManipulatorSystem.GetResource().GetIcon(pData->m_iconName);
	Gdiplus::Status xxx = g.DrawImage(bm, 100, 50, 0, 0, bm->GetWidth(), bm->GetHeight(), Gdiplus::UnitPixel);
	int i = 0;
}

void DialogGameDataBuilding::OnBuildingSelChange()
{
	this->Invalidate();
}
