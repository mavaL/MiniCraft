#include "stdafx.h"
#include "ObjectPropertyPane.h"
#include "Manipulator/ManipulatorScene.h"
#include "XTPCustomPropertyGridItem.h"
#include "Utility.h"


BEGIN_MESSAGE_MAP(PropertyPaneObject, CPropertiesPane)
	//{{AFX_MSG_MAP(PropertyPaneObject)
	ON_WM_CREATE()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


#define UPDATE_PROP_SHORTCUT(ent, d1, d2, fun, item)		\
{	\
	Ogre::String str = Utility::UnicodeToEngine(Utility::StringCutPrecision(item->GetValue()));	\
	d1.d2 = Ogre::StringConverter::parseReal(str);	\
	ManipulatorSystem.GetObject().Set##fun(ent, d1);		\
}


int PropertyPaneObject::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CPropertiesPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"Entity");
	PROPERTY_REG(pCategory,			, L"Mesh File"			, L""					, propMeshName		);
	PROPERTY_REG(pCategory,	Vec3	, L"Position"			, Ogre::Vector3::ZERO	, propPosition		);
	PROPERTY_REG(pCategory,	Vec4	, L"Orientation"		, Ogre::Vector4(0,0,0,1), propOrientation	);
	PROPERTY_REG(pCategory,	Vec3	, L"Scale"				, Ogre::Vector3(1,1,1)	, propScale			);
	PROPERTY_REG(pCategory, Bool	, L"Add to NavMesh"		, false					, propAddToNavmesh	);
	PROPERTY_REG(pCategory, Bool	, L"Is Building"		, false					, propIsBuilding	);
	PROPERTY_REG(pCategory,			, L"Building Name"		, L""					, propBuildingName	);
	PROPERTY_REG(pCategory, Bool	, L"Is Resource"		, false					, propIsResource	);
	pCategory->Expand();

	(dynamic_cast<CXTPPropertyGridItemVec3*>(m_mapItem[propPosition]))->SetChildItemID(propPosX, propPosY, propPosZ);
	(dynamic_cast<CXTPPropertyGridItemVec4*>(m_mapItem[propOrientation]))->SetChildItemID(propOrientX, propOrientY, propOrientZ, propOrientW);
	(dynamic_cast<CXTPPropertyGridItemVec3*>(m_mapItem[propScale]))->SetChildItemID(propScaleX, propScaleY, propScaleZ);

	//初始化建筑物名称列表
	for (int iRace=0; iRace<eGameRace_Count; ++iRace)
	{
		const auto vecNames = ManipulatorSystem.GetGameData().GetRaceBuildingNames((eGameRace)iRace);
		CXTPPropertyGridItemConstraints* pList = m_mapItem[propBuildingName]->GetConstraints();
		for (size_t iBuilding=0; iBuilding<vecNames.size(); ++iBuilding)
		{
			pList->AddConstraint(vecNames[iBuilding].c_str());
		}
	}
	m_mapItem[propBuildingName]->SetFlags(xtpGridItemHasComboButton);
	m_mapItem[propBuildingName]->SetReadOnly(TRUE);

	return 0;
}

void PropertyPaneObject::UpdateAllFromEngine()
{
	for (int i=propStart; i<propEnd; ++i)
		UpdateProperty(i);
}

LRESULT PropertyPaneObject::OnGridNotify( WPARAM wParam, LPARAM lParam )
{
	ManipulatorObject& manObject = ManipulatorSystem.GetObject();

	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		const UINT id = pItem->GetID();
		Ogre::Entity* ent			= manObject.GetSelection();
		Ogre::Vector3 objPos		= ent->getParentSceneNode()->_getDerivedPosition();
		Ogre::Quaternion objOrient	= ent->getParentSceneNode()->_getDerivedOrientation();
		Ogre::Vector3 objScale		= ent->getParentSceneNode()->_getDerivedScale();
		SObjectInfo* objInfo		= manObject.GetObjectInfo(ent);

		switch (id)
		{
		case propPosition: 
			{
				CXTPPropertyGridItemVec3* pItemPos = dynamic_cast<CXTPPropertyGridItemVec3*>(pItem);
				manObject.SetPosition(ent, UpdateVec3ItemProperty(pItemPos));
			}
			break;

		case propOrientation:
			{
				CXTPPropertyGridItemVec4* pItemOrient = dynamic_cast<CXTPPropertyGridItemVec4*>(pItem);
				std::wstring strX = Utility::StringCutPrecision(pItemOrient->GetStrX());
				std::wstring strY = Utility::StringCutPrecision(pItemOrient->GetStrY());
				std::wstring strZ = Utility::StringCutPrecision(pItemOrient->GetStrZ());
				std::wstring strW = Utility::StringCutPrecision(pItemOrient->GetStrW());

				float fX = Ogre::StringConverter::parseReal(Utility::UnicodeToEngine(strX));
				float fY = Ogre::StringConverter::parseReal(Utility::UnicodeToEngine(strY));
				float fZ = Ogre::StringConverter::parseReal(Utility::UnicodeToEngine(strZ));
				float fW = Ogre::StringConverter::parseReal(Utility::UnicodeToEngine(strW));

				manObject.SetOrientation(ent, Ogre::Quaternion(fW, fX, fY, fZ));
			}
			break;

		case propScale: 
			{
				CXTPPropertyGridItemVec3* pItemScale = dynamic_cast<CXTPPropertyGridItemVec3*>(pItem);
				manObject.SetScale(ent, UpdateVec3ItemProperty(pItemScale));
			}
			break;

		case propPosX: UPDATE_PROP_SHORTCUT(ent, objPos, x, Position, pItem); break;
		case propPosY: UPDATE_PROP_SHORTCUT(ent, objPos, y, Position, pItem); break;
		case propPosZ: UPDATE_PROP_SHORTCUT(ent, objPos, z, Position, pItem); break;
		case propOrientX: UPDATE_PROP_SHORTCUT(ent, objOrient, x, Orientation, pItem); break;
		case propOrientY: UPDATE_PROP_SHORTCUT(ent, objOrient, y, Orientation, pItem); break;
		case propOrientZ: UPDATE_PROP_SHORTCUT(ent, objOrient, z, Orientation, pItem); break;
		case propOrientW: UPDATE_PROP_SHORTCUT(ent, objOrient, w, Orientation, pItem); break;
		case propScaleX: UPDATE_PROP_SHORTCUT(ent, objScale, x, Scale, pItem); break;
		case propScaleY: UPDATE_PROP_SHORTCUT(ent, objScale, y, Scale, pItem); break;
		case propScaleZ: UPDATE_PROP_SHORTCUT(ent, objScale, z, Scale, pItem); break;

		case propAddToNavmesh:
			{
				CXTPPropertyGridItemBool* pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>(pItem);
				objInfo->m_bAddToNavmesh = pItemBool->GetBool();
			}
			break;

		case propIsBuilding:
			{
				CXTPPropertyGridItemBool* pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>(pItem);
				objInfo->m_bIsBuilding = pItemBool->GetBool();
			}
			break;

		case propBuildingName:
			{
				std::string name = Utility::UnicodeToEngine(pItem->GetValue());
				objInfo->m_buildingName = name;
			}
			break;

		case propIsResource:
			{
				CXTPPropertyGridItemBool* pItemBool = dynamic_cast<CXTPPropertyGridItemBool*>(pItem);
				objInfo->m_bIsResource = pItemBool->GetBool();
			}
			break;

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

void PropertyPaneObject::UpdateProperty( int id )
{
	Ogre::Entity* curSel = ManipulatorSystem.GetObject().GetSelection();
	if (!curSel)
	{
		m_mapItem[id]->SetValue(L"");
		return;
	}

	Ogre::SceneNode* pNode = curSel->getParentSceneNode();
	std::string strNewValue;
	const SObjectInfo* objInfo = ManipulatorSystem.GetObject().GetObjectInfo(curSel);

	switch (id)
	{
	case propMeshName:		strNewValue = curSel->getMesh()->getName(); break;
	case propPosX:
	case propPosY:
	case propPosZ:			id = propPosition;
	case propPosition:		strNewValue = Ogre::StringConverter::toString(pNode->_getDerivedPosition()); break;
	case propOrientX:
	case propOrientY:
	case propOrientZ:
	case propOrientW:		id = propOrientation;
	case propOrientation:	strNewValue = Ogre::StringConverter::toString(pNode->_getDerivedOrientation()); break;
	case propScaleX:
	case propScaleY:
	case propScaleZ:		id = propScale;
	case propScale:			strNewValue = Ogre::StringConverter::toString(pNode->_getDerivedScale()); break;
	case propAddToNavmesh:	strNewValue = Ogre::StringConverter::toString(objInfo->m_bAddToNavmesh); break;
	case propIsBuilding:	strNewValue = Ogre::StringConverter::toString(objInfo->m_bIsBuilding); break;
	case propBuildingName:	strNewValue = objInfo->m_buildingName; break;
	case propIsResource:	strNewValue = Ogre::StringConverter::toString(objInfo->m_bIsResource); break;
	default: assert(0);
	}
	
	std::wstring wcsNewValue = Utility::EngineToUnicode(strNewValue);
	m_mapItem[id]->SetValue(wcsNewValue.c_str());
}

void PropertyPaneObject::EnableMutableProperty( BOOL bEnable )
{
	for(int i=propMutableItemStart; i<propMutableItemEnd; ++i)
		m_mapItem[i]->SetReadOnly(!bEnable);

	CXTPPropertyGridItemBool* pItem = dynamic_cast<CXTPPropertyGridItemBool*>(m_mapItem[propIsBuilding]);
	m_mapItem[propBuildingName]->SetReadOnly(!pItem->GetBool());
}