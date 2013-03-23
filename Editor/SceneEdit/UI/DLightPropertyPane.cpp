#include "stdafx.h"
#include "DLightPropertyPane.h"
#include "Manipulator/ManipulatorScene.h"
#include "Utility.h"

bool PropertyPaneDLight::_OnCreate()
{
	bool ret = __super::_OnCreate();

	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"Deferred Light");
	PROPERTY_REG(pCategory,			, L"Light Type"		, L""	, propLightType		);
	pCategory->Expand();
	m_mapItem[propLightType]->SetFlags(xtpGridItemHasComboButton);

	//µã¹â
	pCategory = m_wndPropertyGrid.AddCategory(L"Point Light");
	PROPERTY_REG(pCategory, Double, L"Radius"			, 0, propPointLightRadius	);
	pCategory->Expand();

	return ret;
}

void PropertyPaneDLight::_SetProperty( int nID) 
{
	if(nID < propDerivedStart)
		__super::_SetProperty(nID);

// 	switch (nID)
// 	{
// 	case propPointLightRadius:	manEffect.SetEffectParam("locator", paramValue); break;
// 	default: assert(0);
// 	}
}

void PropertyPaneDLight::_UpdateProperty( int id )
{
	if(id < propDerivedStart)
		__super::_UpdateProperty(id);

	ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();
	CXTPPropertyGridItem* pItem = m_mapItem[id];
	std::string strNewValue;

	switch (id)
	{
	case propLightType:
		{
			CXTPPropertyGridItemConstraints* pList = m_mapItem[propLightType]->GetConstraints();
			pList->RemoveAll();
			pList->AddConstraint(L"Point");
			pList->AddConstraint(L"Spot");

			int type = Ogre::StringConverter::parseInt(manEffect.GetEffectParam("lighttype"));
			strNewValue = type == 0 ? "Point" : "Spot";
		}
		break;

	case propPointLightRadius: manEffect.SetEffectParam("radius", Utility::UnicodeToEngine(pItem->GetValue())); break;

	default: assert(0);
	}

	std::wstring wcsNewValue = Utility::EngineToUnicode(strNewValue);
	m_mapItem[id]->SetValue(wcsNewValue.c_str());
}
