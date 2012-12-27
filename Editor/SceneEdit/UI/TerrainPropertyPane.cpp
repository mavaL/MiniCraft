#include "stdafx.h"
#include "TerrainPropertyPane.h"
#include "Manipulator/ManipulatorScene.h"


BEGIN_MESSAGE_MAP(PropertyPaneTerrain, CPropertiesPane)
	//{{AFX_MSG_MAP(PropertyPaneTerrain)
	ON_WM_CREATE()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()

enum ePropertyID
{
	propWorldSize = 0,
	propMapSize,
	propMinBatchSize,
	propMaxBatchSize,
	propSkirtSize,
	propMaxPixelError,
	propCompMapDist
};

//×¢²áproerty
#define PROPERTY_REG(category, type, name, value, id, readonly)	\
{	\
	CXTPPropertyGridItem* pItem = category->AddChildItem(new CXTPPropertyGridItem##type(name, value));	\
	pItem->SetID(id);								\
	pItem->SetReadOnly(readonly);					\
	m_mapItem.insert(std::make_pair(id, pItem));	\
}	\


int PropertyPaneTerrain::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CPropertiesPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"Terrain Info");
	PROPERTY_REG(pCategory, Double, L"World Size"			, 0, propWorldSize, TRUE);
	PROPERTY_REG(pCategory, Number, L"Map Size"				, 0, propMapSize, TRUE);
	PROPERTY_REG(pCategory, Number, L"Min Batch Size"		, 0, propMinBatchSize, TRUE);
	PROPERTY_REG(pCategory, Number, L"Max Batch Size"		, 0, propMaxBatchSize, TRUE);
	PROPERTY_REG(pCategory, Number, L"Skirt Size"			, 0, propSkirtSize, TRUE);

	pCategory = m_wndPropertyGrid.AddCategory(L"Terrain Config");
	PROPERTY_REG(pCategory, Number, L"Max Pixel Error"		, 0, propMaxPixelError, FALSE);
	PROPERTY_REG(pCategory, Double, L"CompositeMap Distance", 0, propCompMapDist, FALSE);

	return 0;
}

void PropertyPaneTerrain::UpdateFromEngine()
{
	ManipulatorTerrain& manTerrain = ManipulatorSystem.GetTerrain();

	std::wstring strWorldSize = Utility::EngineToUnicode(Ogre::StringConverter::toString(manTerrain.GetWorldSize()));
	std::wstring strMapSize = Utility::EngineToUnicode(Ogre::StringConverter::toString(manTerrain.GetMapSize()));
	std::wstring strMinBatch = Utility::EngineToUnicode(Ogre::StringConverter::toString(manTerrain.GetMinBatchSize()));
	std::wstring strMaxBatch = Utility::EngineToUnicode(Ogre::StringConverter::toString(manTerrain.GetMaxBatchSize()));
	std::wstring strSkirtSize = Utility::EngineToUnicode(Ogre::StringConverter::toString(manTerrain.GetSkirtSize()));
	std::wstring strMaxPixelError = Utility::EngineToUnicode(Ogre::StringConverter::toString(manTerrain.GetMaxPixelError()));
	std::wstring strCompMapDist = Utility::EngineToUnicode(Ogre::StringConverter::toString(manTerrain.GetCompositeMapDist()));

	m_mapItem[propWorldSize]->		SetValue(strWorldSize.c_str());
	m_mapItem[propMapSize]->		SetValue(strMapSize.c_str());
	m_mapItem[propMinBatchSize]->	SetValue(strMinBatch.c_str());
	m_mapItem[propMaxBatchSize]->	SetValue(strMaxBatch.c_str());
	m_mapItem[propSkirtSize]->		SetValue(strSkirtSize.c_str());
	m_mapItem[propMaxPixelError]->	SetValue(strMaxPixelError.c_str());
	m_mapItem[propCompMapDist]->	SetValue(strCompMapDist.c_str());
}

LRESULT PropertyPaneTerrain::OnGridNotify( WPARAM wParam, LPARAM lParam )
{
	ManipulatorTerrain& manTerrain = ManipulatorSystem.GetTerrain();

	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		const UINT id = pItem->GetID();

		switch (id)
		{
		case propMaxPixelError: 
		{
			CXTPPropertyGridItemNumber* pNumItem = dynamic_cast<CXTPPropertyGridItemNumber*>(pItem);
			manTerrain.SetMaxPixelError((float)pNumItem->GetNumber());
		}
		break;

		case propCompMapDist:
		{
			CXTPPropertyGridItemDouble* pDoubleItem = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
			manTerrain.SetCompositeMapDist((float)pDoubleItem->GetDouble());
		}
		break;

		default: assert(0);
		}
	}
	return 0;
}
