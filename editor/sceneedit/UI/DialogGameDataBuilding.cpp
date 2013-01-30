#include "stdafx.h"
#include "DialogGameDataBuilding.h"
#include "Manipulator/ManipulatorScene.h"
#include "Utility.h"


BEGIN_MESSAGE_MAP(DialogGameDataBuilding, CDialog)
	ON_COMMAND(IDC_DlgBuilding_SwitchT, OnSwitchTerran)
	ON_COMMAND(IDC_DlgBuilding_SwitchZ, OnSwitchZerg)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_DlgBuilding_NameList, OnBuildingSelChange)
	ON_CBN_SELCHANGE(IDC_DlgBuilding_SlotList, OnSlotSelChange)
	ON_CBN_SELCHANGE(IDC_DlgBuilding_AbilList, OnAbilitySelChange)
END_MESSAGE_MAP()

DialogGameDataBuilding::DialogGameDataBuilding()
:CDialog(DialogGameDataBuilding::IDD)
{

}

BOOL DialogGameDataBuilding::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	//初始化slot列表控件
	const int totalSlotCnt = 15;
	CComboBox* pSlotList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_SlotList);
	for (int i=0; i<totalSlotCnt; ++i)
	{
		std::wstring strIndex = Utility::EngineToUnicode(Ogre::StringConverter::toString(i+1));
		pSlotList->AddString(strIndex.c_str());

// 		CWnd* pSlot = GetDlgItem(IDC_DlgBuilding_Slot1 + i);
// 		pSlot->SetWindowPos(nullptr, 0, 0, 32, 32, SWP_NOMOVE);
	}

	//初始化ability列表控件
	CComboBox* pAbilityList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_AbilList);
	const auto vecNames = ManipulatorSystem.GetGameData().GetAbilityNames();
	for (size_t i=0; i<vecNames.size(); ++i)
		pAbilityList->AddString(vecNames[i].c_str());

	return ret;
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
	{
		//绘制建筑物图标
		CWnd* wnd = GetDlgItem(IDC_DlgBuilding_Icon);
		wnd->Invalidate();
		wnd->UpdateWindow();

		Gdiplus::Graphics g(wnd->GetDC()->GetSafeHdc());
		eGameRace race = IsDlgButtonChecked(IDC_DlgBuilding_SwitchT) ? eGameRace_Terran : eGameRace_Zerg;
		CString strCurSel;
		pCtrl->GetLBText(iCurSel, strCurSel);
		const SBuildingData* pData = ManipulatorSystem.GetGameData().GetBuildingData(race, (LPCTSTR)strCurSel);
		Gdiplus::Bitmap* bm = ManipulatorSystem.GetResource().GetIcon(pData->m_iconName);
		g.DrawImage(bm, 0, 0, 0, 0, bm->GetWidth(), bm->GetHeight(), Gdiplus::UnitPixel);
	}

	//高亮选中slot
	{
		CComboBox* pSlotList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_SlotList);
		int curSelSlot = pSlotList->GetCurSel();
		if (curSelSlot != -1)
		{
			CWnd* pSlotFrame = GetDlgItem(IDC_DlgBuilding_SlotFrame);
			pSlotFrame->Invalidate();
			pSlotFrame->UpdateWindow();

			CWnd* pSlot = GetDlgItem(IDC_DlgBuilding_Slot1 + curSelSlot);
			RECT rc;
			pSlot->GetClientRect(&rc);
			pSlot->ClientToScreen(&rc);
			pSlotFrame->ScreenToClient(&rc);

			Gdiplus::Graphics graphSlotFrame(pSlotFrame->GetDC()->GetSafeHdc());
			Gdiplus::Pen pen(Gdiplus::Color::Blue, 2);
			graphSlotFrame.DrawRectangle(&pen, rc.left-4, rc.top-4, rc.right-rc.left+8, rc.bottom-rc.top+8);
		}
	}

	//绘制各slot ability
	{
		CComboBox* pCtrl = (CComboBox*)GetDlgItem(IDC_DlgBuilding_AbilList);
		int iCurSel = pCtrl->GetCurSel();
		if(iCurSel != -1)
		{
			CWnd* pSlotFrame = GetDlgItem(IDC_DlgBuilding_SlotFrame);
			pSlotFrame->Invalidate();
			pSlotFrame->UpdateWindow();

			CString strCurSel;
			pCtrl->GetLBText(iCurSel, strCurSel);
			const SAbilityData* pData = ManipulatorSystem.GetGameData().GetAbilityData((LPCTSTR)strCurSel);
			Gdiplus::Bitmap* bm = ManipulatorSystem.GetResource().GetIcon(pData->m_iconName);
			Gdiplus::Graphics graphSlot(GetDlgItem(IDC_DlgBuilding_Slot1)->GetDC()->GetSafeHdc());
			Gdiplus::Status xx = graphSlot.DrawImage(bm, 0, 0, 0, 0, bm->GetWidth(), bm->GetHeight(), Gdiplus::UnitPixel);
			int i = 0;
		}
	}
}

void DialogGameDataBuilding::OnBuildingSelChange()
{
	Invalidate();
}

void DialogGameDataBuilding::OnSlotSelChange()
{
	Invalidate();
}

void DialogGameDataBuilding::OnAbilitySelChange()
{

	Invalidate();
}
