#include "stdafx.h"
#include "EffectPropertyPane.h"
#include "Manipulator/ManipulatorScene.h"
#include "Utility.h"
#include "XTPCustomPropertyGridItem.h"


BEGIN_MESSAGE_MAP(PropertyPaneEffect, CPropertiesPane)
	//{{AFX_MSG_MAP(PropertyPaneEffect)
	ON_WM_CREATE()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


int PropertyPaneEffect::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CPropertiesPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CXTPPropertyGridItem* pCategory = m_wndPropertyGrid.AddCategory(L"Shadow");
	PROPERTY_REG(pCategory,	Double	, L"Far Distance"				, 300						, propShadowFarDist);
	PROPERTY_REG(pCategory,	Double	, L"Split Padding"				, 1							, propShadowSplitPadding);
	PROPERTY_REG(pCategory,	Vec3	, L"Optimal Adjust Factor"		, Ogre::Vector3(0.5f,0.8f,2)	, propShadowOptimalAdjustFactor);
	PROPERTY_REG(pCategory,	Bool	, L"Use Simple Optimal Adjust"	, TRUE						, propShadowUseSimpleOptimalAdjust);
	PROPERTY_REG(pCategory, Number	, L"Camera Light Direction Threshold", 45					, propShadowCameraLightDirectionThreshold);
	PROPERTY_REG(pCategory, Vec3	, L"Shadow Map Size"			, Ogre::Vector3(2048,1024,1024)	, propShadowMapSize);
	PROPERTY_REG(pCategory,	Bool	, L"Self Shadow"				, FALSE						, propShadowSelfShadow);
	PROPERTY_REG(pCategory, Bool	, L"Render Back Faces"			, TRUE						, propShadowCasterRenderBackFaces);
	PROPERTY_REG(pCategory,	Double	, L"Lambda"						, ManipulatorSystem.GetEffect().GetShadowPssmLambda(), propShadowLambda);
	PROPERTY_REG(pCategory,	Double	, L"Extrusion Distance"			, 10000						, propShadowDirectionalLightExtrusionDistance);
	pCategory->Expand();

	pCategory = m_wndPropertyGrid.AddCategory(L"SSAO");
	PROPERTY_REG(pCategory,	Double	, L"Sample Length"				, 20						, propSSAOSampleLength);
	PROPERTY_REG(pCategory,	Double	, L"Offset Scale"				, 1							, propSSAOOffsetScale);
	PROPERTY_REG(pCategory,	Double	, L"Default Accessibility"		, 0.5f						, propSSAODefaultAccessibility);
	PROPERTY_REG(pCategory,	Double	, L"Edge Highlight"				, 1.99f						, propSSAOEdgeHighlight);
	pCategory->Expand();

	(dynamic_cast<CXTPPropertyGridItemVec3*>(m_mapItem[propShadowOptimalAdjustFactor]))->SetChildItemID(propOptimalAdjustFactor0, propOptimalAdjustFactor1, propOptimalAdjustFactor2);
	(dynamic_cast<CXTPPropertyGridItemVec3*>(m_mapItem[propShadowMapSize]))->SetChildItemID(propShadowMapSize0, propShadowMapSize1, propShadowMapSize2);

	return 0;
}

void PropertyPaneEffect::UpdateAllFromEngine()
{
	for (int i=propStart; i<propEnd; ++i)
		UpdateProperty(i);
}

LRESULT PropertyPaneEffect::OnGridNotify( WPARAM wParam, LPARAM lParam )
{
	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		const UINT id = pItem->GetID();

		switch (id)
		{
		case propShadowFarDist: 
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetShadowFarDist((float)pItemVal->GetDouble());
			}
			break;

		case propShadowSplitPadding:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetShadowSplitPadding((float)pItemVal->GetDouble());
			}
			break;

		case propShadowOptimalAdjustFactor: 
			{
				CXTPPropertyGridItemVec3* pItemVal = dynamic_cast<CXTPPropertyGridItemVec3*>(pItem);
				const Ogre::Vector3 factor = UpdateVec3ItemProperty(pItemVal);
				manEffect.SetShadowOptimalAdjustFactor(0, factor.x);
				manEffect.SetShadowOptimalAdjustFactor(1, factor.y);
				manEffect.SetShadowOptimalAdjustFactor(2, factor.z);
			}
			break;

		case propOptimalAdjustFactor0:
		case propOptimalAdjustFactor1:
		case propOptimalAdjustFactor2:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetShadowOptimalAdjustFactor(id-propOptimalAdjustFactor0, (float)pItemVal->GetDouble());
			}
			break;

		case propShadowUseSimpleOptimalAdjust:
			{
				CXTPPropertyGridItemBool* pItemVal = dynamic_cast<CXTPPropertyGridItemBool*>(pItem);
				manEffect.SetShadowUseSimpleOptimalAdjust(pItemVal->GetBool());
			}
			break;

		case propShadowCameraLightDirectionThreshold:
			{
				CXTPPropertyGridItemNumber* pItemVal = dynamic_cast<CXTPPropertyGridItemNumber*>(pItem);
				manEffect.SetShadowCameraLightDirectionThreshold(pItemVal->GetNumber());
			}
			break;

		case propShadowMapSize: 
			{
				CXTPPropertyGridItemVec3* pItemVal = dynamic_cast<CXTPPropertyGridItemVec3*>(pItem);
				const Ogre::Vector3 size = UpdateVec3ItemProperty(pItemVal);
				manEffect.SetShadowMapSize(0, (int)size.x);
				manEffect.SetShadowMapSize(1, (int)size.y);
				manEffect.SetShadowMapSize(2, (int)size.z);
			}
			break;

		case propShadowMapSize0:
		case propShadowMapSize1:
		case propShadowMapSize2:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				int size = (int)pItemVal->GetDouble();
				manEffect.SetShadowMapSize(id-propShadowMapSize0, size);
			}
			break;

		case propShadowSelfShadow:
			{
				CXTPPropertyGridItemBool* pItemVal = dynamic_cast<CXTPPropertyGridItemBool*>(pItem);
				manEffect.SetShadowSelfShadow(pItemVal->GetBool());
			}
			break;
			
		case propShadowCasterRenderBackFaces:
			{
				CXTPPropertyGridItemBool* pItemVal = dynamic_cast<CXTPPropertyGridItemBool*>(pItem);
				manEffect.SetShadowCasterRenderBackFaces(pItemVal->GetBool());
			}
			break;

		case propShadowLambda:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetShadowPssmLambda((float)pItemVal->GetDouble());
			}
			break;

		case propShadowDirectionalLightExtrusionDistance:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetShadowDirectionalLightExtrusionDistance((float)pItemVal->GetDouble());
			}
			break;

		case propSSAOSampleLength:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetSSAOSampleLength((float)pItemVal->GetDouble());
			}
			break;

		case propSSAOOffsetScale:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetSSAOOffsetScale((float)pItemVal->GetDouble());
			}
			break;

		case propSSAODefaultAccessibility:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetSSAODefaultAccessibility((float)pItemVal->GetDouble());
			}
			break;

		case propSSAOEdgeHighlight:
			{
				CXTPPropertyGridItemDouble* pItemVal = dynamic_cast<CXTPPropertyGridItemDouble*>(pItem);
				manEffect.SetSSAOEdgeHighlight((float)pItemVal->GetDouble());
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

void PropertyPaneEffect::UpdateProperty( int id )
{
	ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();
	std::string strNewValue;

	switch (id)
	{
	case propShadowFarDist:					strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowFarDist()); break;
	case propShadowSplitPadding:			strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowSplitPadding()); break;
	case propOptimalAdjustFactor0:
	case propOptimalAdjustFactor1:
	case propOptimalAdjustFactor2:			id = propShadowOptimalAdjustFactor;
	case propShadowOptimalAdjustFactor:		strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowOptimalAdjustFactor()); break;
	case propShadowUseSimpleOptimalAdjust : strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowUseSimpleOptimalAdjust()); break;
	case propShadowCameraLightDirectionThreshold:	strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowCameraLightDirectionThreshold()); break;
	case propShadowMapSize0:
	case propShadowMapSize1:
	case propShadowMapSize2:				id = propShadowMapSize;
	case propShadowMapSize:					strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowMapSize()); break;
	case propShadowSelfShadow:				strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowSelfShadow()); break;
	case propShadowCasterRenderBackFaces:	strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowCasterRenderBackFaces()); break;
	case propShadowLambda:					strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowPssmLambda()); break;
	case propShadowDirectionalLightExtrusionDistance:	strNewValue = Ogre::StringConverter::toString(manEffect.GetShadowDirectionalLightExtrusionDistance()); break;
	case propSSAOSampleLength:				strNewValue = Ogre::StringConverter::toString(manEffect.GetSSAOSampleLength()); break;
	case propSSAOOffsetScale:				strNewValue = Ogre::StringConverter::toString(manEffect.GetSSAOOffsetScale()); break;
	case propSSAODefaultAccessibility:		strNewValue = Ogre::StringConverter::toString(manEffect.GetSSAODefaultAccessibility()); break;
	case propSSAOEdgeHighlight:				strNewValue = Ogre::StringConverter::toString(manEffect.GetSSAOEdgeHighlight()); break;
	default: assert(0);
	}
	
	std::wstring wcsNewValue = Utility::EngineToUnicode(strNewValue);
	m_mapItem[id]->SetValue(wcsNewValue.c_str());
}

void PropertyPaneEffect::EnableMutableProperty( BOOL bEnable )
{
	for(int i=propMutableItemStart; i<propMutableItemEnd; ++i)
		m_mapItem[i]->SetReadOnly(!bEnable);
}
