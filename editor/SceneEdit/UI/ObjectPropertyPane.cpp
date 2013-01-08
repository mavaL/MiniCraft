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


#define UPDATE_PROP_SHORTCUT(d1, d2, fun, item)		\
{	\
	Ogre::String str = Utility::UnicodeToEngine(Utility::StringCutPrecision(item->GetValue()));	\
	d1.d2 = Ogre::StringConverter::parseReal(str);	\
	ManipulatorSystem.GetObject().SelectionSet##fun(d1);		\
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
	pCategory->Expand();

	(dynamic_cast<CXTPPropertyGridItemVec3*>(m_mapItem[propPosition]))->SetChildItemID(propPosX, propPosY, propPosZ);
	(dynamic_cast<CXTPPropertyGridItemVec4*>(m_mapItem[propOrientation]))->SetChildItemID(propOrientX, propOrientY, propOrientZ, propOrientW);
	(dynamic_cast<CXTPPropertyGridItemVec3*>(m_mapItem[propScale]))->SetChildItemID(propScaleX, propScaleY, propScaleZ);

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
		Ogre::Vector3 objPos = manObject.GetSelection()->getParentSceneNode()->_getDerivedPosition();
		Ogre::Quaternion objOrient = manObject.GetSelection()->getParentSceneNode()->_getDerivedOrientation();
		Ogre::Vector3 objScale = manObject.GetSelection()->getParentSceneNode()->_getDerivedScale();

		switch (id)
		{
		case propPosition: 
		{
			CXTPPropertyGridItemVec3* pItemPos = dynamic_cast<CXTPPropertyGridItemVec3*>(pItem);
			manObject.SelectionSetPosition(_UpdateVec3ItemProperty(pItemPos));
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

			manObject.SelectionSetOrientation(Ogre::Quaternion(fW, fX, fY, fZ));
		}
		break;

		case propScale: 
		{
			CXTPPropertyGridItemVec3* pItemScale = dynamic_cast<CXTPPropertyGridItemVec3*>(pItem);
			manObject.SelectionSetScale(_UpdateVec3ItemProperty(pItemScale));
		}
		break;

		case propPosX: UPDATE_PROP_SHORTCUT(objPos, x, Position, pItem); break;
		case propPosY: UPDATE_PROP_SHORTCUT(objPos, y, Position, pItem); break;
		case propPosZ: UPDATE_PROP_SHORTCUT(objPos, z, Position, pItem); break;
		case propOrientX: UPDATE_PROP_SHORTCUT(objOrient, x, Orientation, pItem); break;
		case propOrientY: UPDATE_PROP_SHORTCUT(objOrient, y, Orientation, pItem); break;
		case propOrientZ: UPDATE_PROP_SHORTCUT(objOrient, z, Orientation, pItem); break;
		case propOrientW: UPDATE_PROP_SHORTCUT(objOrient, w, Orientation, pItem); break;
		case propScaleX: UPDATE_PROP_SHORTCUT(objScale, x, Scale, pItem); break;
		case propScaleY: UPDATE_PROP_SHORTCUT(objScale, y, Scale, pItem); break;
		case propScaleZ: UPDATE_PROP_SHORTCUT(objScale, z, Scale, pItem); break;

		case propAddToNavmesh:
		{
			CXTPPropertyGridItemBool* pItemNav = dynamic_cast<CXTPPropertyGridItemBool*>(pItem);
			manObject.SetObjectNavMeshFlag(manObject.GetSelection(), pItemNav->GetBool());
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
	Ogre::SceneNode* pNode = curSel->getParentSceneNode();
	assert(curSel);
	std::string strNewValue;

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
	case propAddToNavmesh:	strNewValue = Ogre::StringConverter::toString(ManipulatorSystem.GetObject().GetObjectNavMeshFlag(curSel)); break;
	default: assert(0);
	}
	
	std::wstring wcsNewValue = Utility::EngineToUnicode(strNewValue);
	m_mapItem[id]->SetValue(wcsNewValue.c_str());
}

void PropertyPaneObject::EnableMutableProperty( BOOL bEnable )
{
	for(int i=propMutableItemStart; i<propMutableItemEnd; ++i)
		m_mapItem[i]->SetReadOnly(!bEnable);
}

Ogre::Vector3 PropertyPaneObject::_UpdateVec3ItemProperty(CXTPPropertyGridItemVec3* pItem)
{
	std::wstring strX = Utility::StringCutPrecision(pItem->GetStrX());
	std::wstring strY = Utility::StringCutPrecision(pItem->GetStrY());
	std::wstring strZ = Utility::StringCutPrecision(pItem->GetStrZ());

	float fX = Ogre::StringConverter::parseReal(Utility::UnicodeToEngine(strX));
	float fY = Ogre::StringConverter::parseReal(Utility::UnicodeToEngine(strY));
	float fZ = Ogre::StringConverter::parseReal(Utility::UnicodeToEngine(strZ));

	return Ogre::Vector3(fX, fY, fZ);
}
