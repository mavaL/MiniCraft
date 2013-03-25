#include "stdafx.h"
#include "DialogGameDataBuilding.h"
#include "Utility.h"
#include "Manipulator/ManipulatorGameData.h"


BEGIN_MESSAGE_MAP(DialogGameDataBuilding, CDialog)
	ON_COMMAND(IDC_DlgBuilding_SwitchT, OnSwitchRace<eGameRace_Terran>)
	ON_COMMAND(IDC_DlgBuilding_SwitchZ, OnSwitchRace<eGameRace_Zerg>)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_DlgBuilding_NameList, OnBuildingSelChange)
	ON_CBN_SELCHANGE(IDC_DlgBuilding_SlotList, OnSlotSelChange)
	ON_CBN_SELCHANGE(IDC_DlgBuilding_AbilList, OnAbilitySelChange)
END_MESSAGE_MAP()

DialogGameDataBuilding::DialogGameDataBuilding()
:CDialog(DialogGameDataBuilding::IDD)
,m_curEditRace((eGameRace)-1)
{

}

BOOL DialogGameDataBuilding::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	//初始化slot列表控件
	CComboBox* pSlotList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_SlotList);
	for (int i=0; i<MAX_ABILITY_SLOT; ++i)
	{
		std::wstring strIndex = Utility::EngineToUnicode(Ogre::StringConverter::toString(i+1));
		pSlotList->AddString(strIndex.c_str());
	}

	//初始化ability列表控件
	CComboBox* pAbilityList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_AbilList);
	const auto vecNames = ManipulatorSystem.GetGameData().GetAbilityNames();

	pAbilityList->AddString(L"None");
	for (size_t i=0; i<vecNames.size(); ++i)
		pAbilityList->AddString(vecNames[i].c_str());

	//防止Slot控件被剪裁
	CWnd* pSlotFrame = GetDlgItem(IDC_DlgBuilding_SlotFrame);
	pSlotFrame->ModifyStyle(0, WS_CLIPSIBLINGS, 0);

	GetDlgItem(IDC_DlgBuilding_NameList)->EnableWindow(FALSE);
	GetDlgItem(IDC_DlgBuilding_SlotList)->EnableWindow(FALSE);
	GetDlgItem(IDC_DlgBuilding_AbilList)->EnableWindow(FALSE);

	return ret;
}

template<int race>
void DialogGameDataBuilding::OnSwitchRace()
{
	CComboBox* pCtrl = (CComboBox*)GetDlgItem(IDC_DlgBuilding_NameList);
	pCtrl->ResetContent();
	pCtrl->SetCurSel(-1);

	const auto vecNames = ManipulatorSystem.GetGameData().GetRaceBuildingNames((eGameRace)race);
	for(size_t i=0; i<vecNames.size(); ++i)
		pCtrl->AddString(vecNames[i].c_str());

	CComboBox* pSlotList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_SlotList);
	CComboBox* pAbilList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_AbilList);
	pSlotList->SetCurSel(-1);
	pAbilList->SetCurSel(-1);

	m_curEditRace = (eGameRace)race;
	GetDlgItem(IDC_DlgBuilding_NameList)->EnableWindow(TRUE);
	Invalidate();
}

void DialogGameDataBuilding::OnPaint()
{
	CDialog::OnPaint();

	std::wstring buildingName;
	if(!_GetCurBuildingName(buildingName))
		return;

	ManipulatorGameData& manGameData = ManipulatorSystem.GetGameData();
	CPaintDC dc(this);
	{
		//绘制建筑物图标
		CWnd* wnd = GetDlgItem(IDC_DlgBuilding_Icon);
		wnd->Invalidate();
		wnd->UpdateWindow();

		Gdiplus::Graphics g(wnd->GetDC()->GetSafeHdc());
		SBuildingData* pData = manGameData.GetBuildingData(buildingName.c_str());
		Gdiplus::Bitmap* bm = ManipulatorSystem.GetResource().GetIcon(Utility::EngineToUnicode(pData->params["icon"]));
		g.DrawImage(bm, 0, 0, 0, 0, bm->GetWidth(), bm->GetHeight(), Gdiplus::UnitPixel);
	}

	//高亮选中slot
	{
		int iCurSlot = -1;
		if (_GetCurSlot(iCurSlot))
		{
			CWnd* pSlotFrame = GetDlgItem(IDC_DlgBuilding_SlotFrame);
			pSlotFrame->Invalidate();
			pSlotFrame->UpdateWindow();

			CWnd* pSlot = GetDlgItem(IDC_DlgBuilding_Slot1 + iCurSlot);
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
		std::wstring curSelBuilding;
		if(!_GetCurBuildingName(curSelBuilding))
			return;
		
		const SBuildingData* pBuildingData = manGameData.GetBuildingData(curSelBuilding);
		for (int i=0; i<MAX_ABILITY_SLOT; ++i)
		{
			const SAbilityData* pAbilData = manGameData.GetAbilityData(Utility::EngineToUnicode(pBuildingData->m_vecAbilities[i]));
			if(!pAbilData)
				continue;

			CWnd* pSlot = GetDlgItem(IDC_DlgBuilding_Slot1+i);
			pSlot->Invalidate();
			pSlot->UpdateWindow();

			RECT rc;
			pSlot->GetClientRect(&rc);
			Gdiplus::Rect rcDest(0, 0, rc.right-rc.left, rc.bottom-rc.top);
			Gdiplus::Bitmap* bm = ManipulatorSystem.GetResource().GetIcon(Utility::EngineToUnicode(pAbilData->m_iconName));
			Gdiplus::Graphics graphSlot(pSlot->GetDC()->GetSafeHdc());
			graphSlot.DrawImage(bm, rcDest, 0, 0, bm->GetWidth(), bm->GetHeight(), Gdiplus::UnitPixel);
		}	
	}
}

void DialogGameDataBuilding::OnBuildingSelChange()
{
	CComboBox* pSlotList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_SlotList);
	CComboBox* pAbilList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_AbilList);
	pSlotList->SetCurSel(-1);
	pAbilList->SetCurSel(-1);
	GetDlgItem(IDC_DlgBuilding_SlotList)->EnableWindow(TRUE);
	Invalidate();
}

void DialogGameDataBuilding::OnSlotSelChange()
{
	std::wstring curSelBuilding;
	if(!_GetCurBuildingName(curSelBuilding))
		return;

	int iCurSlot;
	_GetCurSlot(iCurSlot);

	ManipulatorGameData& manGameData = ManipulatorSystem.GetGameData();
	const SBuildingData* pBuildingData = manGameData.GetBuildingData(curSelBuilding);
	const auto abilNames = ManipulatorSystem.GetGameData().GetAbilityNames();

	CComboBox* pAbilList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_AbilList);
	auto iter = std::find(abilNames.begin(), abilNames.end(), Utility::EngineToUnicode(pBuildingData->m_vecAbilities[iCurSlot]));
	if(iter == abilNames.end())
	{
		pAbilList->SetCurSel(0);
	}
	else
	{
		size_t distance = std::distance(abilNames.begin(), iter);
		pAbilList->SetCurSel(distance + 1);
	}

	GetDlgItem(IDC_DlgBuilding_AbilList)->EnableWindow(TRUE);
	Invalidate();
}

void DialogGameDataBuilding::OnAbilitySelChange()
{	
	std::wstring curSelBuilding;
	if(!_GetCurBuildingName(curSelBuilding))
		return;

	int iCurSlot;
	if(!_GetCurSlot(iCurSlot))
		return;

	std::wstring curSelAbil;
	if(!_GetCurAbilityName(curSelAbil))
		return;

	ManipulatorSystem.GetGameData().SetBuildingAbility(curSelBuilding, iCurSlot, curSelAbil == L"None" ? L"" : curSelAbil);

	Invalidate();
}

bool DialogGameDataBuilding::_GetCurBuildingName(std::wstring& retName) const
{
	CComboBox* pBuildingList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_NameList);
	int curAbil = pBuildingList->GetCurSel();
	if (curAbil != -1)
	{
		CString strCurSel;
		pBuildingList->GetLBText(curAbil, strCurSel);
		retName = strCurSel;

		return true;
	}
	return false;
}

bool DialogGameDataBuilding::_GetCurAbilityName(std::wstring& retName) const
{
	CComboBox* pAbilList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_AbilList);
	int curAbil = pAbilList->GetCurSel();
	if (curAbil != -1)
	{
		CString strCurSel;
		pAbilList->GetLBText(curAbil, strCurSel);
		retName = strCurSel;

		return true;
	}
	return false;
}

bool DialogGameDataBuilding::_GetCurSlot( int& retIndex ) const
{
	CComboBox* pSlotList = (CComboBox*)GetDlgItem(IDC_DlgBuilding_SlotList);	
	retIndex = pSlotList->GetCurSel();
	return retIndex != -1;
}
