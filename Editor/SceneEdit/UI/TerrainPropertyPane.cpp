#include "stdafx.h"
#include "TerrainPropertyPane.h"
#include "Manipulator/ManipulatorScene.h"


BEGIN_MESSAGE_MAP(PropertyPaneTerrain, CPropertiesPane)
	//{{AFX_MSG_MAP(PropertyPaneTerrain)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int PropertyPaneTerrain::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CPropertiesPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CXTPPropertyGridItem* pGlobals = m_wndPropertyGrid.AddCategory(_T("Terrain Info"));
	//item1
	pGlobals->AddChildItem(new CXTPPropertyGridItemDouble(_T("World Size"), 0))->SetReadOnly(TRUE);
	//item2
	pGlobals->AddChildItem(new CXTPPropertyGridItemDouble(_T("Map Size"), 0))->SetReadOnly(TRUE);

	return 0;
}

void PropertyPaneTerrain::UpdateFromEngine()
{
	CXTPPropertyGridItemDouble* pWorldSizeItem = dynamic_cast<CXTPPropertyGridItemDouble*>(m_wndPropertyGrid.GetItem(1));
	CXTPPropertyGridItemDouble* pMapSizeItem = dynamic_cast<CXTPPropertyGridItemDouble*>(m_wndPropertyGrid.GetItem(2));

	ManipulatorTerrain& manTerrain = ManipulatorSystem.GetTerrain();
	pWorldSizeItem->SetDouble(manTerrain.GetWorldSize());
	pMapSizeItem->SetDouble(manTerrain.GetMapSize());
}
