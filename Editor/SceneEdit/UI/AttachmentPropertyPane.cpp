#include "stdafx.h"
#include "AttachmentPropertyPane.h"
#include "Manipulator/ManipulatorScene.h"
#include "Utility.h"

BEGIN_MESSAGE_MAP(PropertyPaneAttachment, CPropertiesPane)
	//{{AFX_MSG_MAP(PropertyPaneAttachment)
	ON_WM_CREATE()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


int PropertyPaneAttachment::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CPropertiesPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"Particle Effect");
	PROPERTY_REG(pCategory,			, L"Locator"		, L""	, propLocator		);
	PROPERTY_REG(pCategory,			, L"Particle"		, L""	, propParticle		);
	PROPERTY_REG(pCategory,	  Double, L"Start Time"		, 0		, propStartTime		);
	PROPERTY_REG(pCategory,	  Double, L"Life Time"		, 0		, propLifeTime		);
	pCategory->Expand();

	m_mapItem[propLocator]->SetFlags(xtpGridItemHasComboButton);
	m_mapItem[propParticle]->SetFlags(xtpGridItemHasComboButton);

	return 0;
}

void PropertyPaneAttachment::UpdateAllFromEngine()
{
	for (int i=propStart; i<propEnd; ++i)
		UpdateProperty(i);
}

LRESULT PropertyPaneAttachment::OnGridNotify( WPARAM wParam, LPARAM lParam )
{
	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		const UINT id = pItem->GetID();
		const std::string& paramValue = Utility::UnicodeToEngine(pItem->GetValue());

		switch (id)
		{
		case propLocator:	manEffect.SetEffectParam("locator", paramValue); break;
		case propParticle:	manEffect.SetEffectParam("template", paramValue); break;
		case propStartTime: manEffect.SetEffectParam("starttime", paramValue); break;
		case propLifeTime: manEffect.SetEffectParam("lifetime", paramValue); break;
		default: assert(0);
		}

		//进行了精度截断,需要反向更新到控件
		UpdateProperty(id);

		//让控件失去焦点
		((CFrameWnd*)AfxGetMainWnd())->GetActiveView()->SetFocus();
	}
	else if (wParam == XTP_PGN_AFTEREDIT)
	{
		//让控件失去焦点
		((CFrameWnd*)AfxGetMainWnd())->GetActiveView()->SetFocus();
	}

	return 0;
}

void PropertyPaneAttachment::UpdateProperty( int id )
{
	ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();
	std::string strNewValue;

	switch (id)
	{
	case propLocator:
		{
			const auto locators = manEffect.GetLocatorNames();
			CXTPPropertyGridItemConstraints* pList = m_mapItem[propLocator]->GetConstraints();
			pList->RemoveAll();
			for (size_t i=0; i<locators.size(); ++i)
				pList->AddConstraint(locators[i].c_str());
			
			strNewValue = manEffect.GetEffectParam("locator");
		}
		break;

	case propParticle:
		{
			const auto names = manEffect.GetParticleTmpNames();
			CXTPPropertyGridItemConstraints* pList = m_mapItem[propParticle]->GetConstraints();
			pList->RemoveAll();
			for (size_t i=0; i<names.size(); ++i)
				pList->AddConstraint(names[i].c_str());

			 strNewValue = manEffect.GetEffectParam("template");
		}
		break;

	case propStartTime:	strNewValue = manEffect.GetEffectParam("starttime"); break;
	case propLifeTime:	strNewValue = manEffect.GetEffectParam("lifetime"); break;

	default: assert(0);
	}
	
	std::wstring wcsNewValue = Utility::EngineToUnicode(strNewValue);
	m_mapItem[id]->SetValue(wcsNewValue.c_str());
}

void PropertyPaneAttachment::EnableMutableProperty( BOOL bEnable )
{
	for(int i=propMutableItemStart; i<propMutableItemEnd; ++i)
		m_mapItem[i]->SetReadOnly(!bEnable);
}
