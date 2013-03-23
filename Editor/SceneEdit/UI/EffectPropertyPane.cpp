#include "stdafx.h"
#include "EffectPropertyPane.h"
#include "Manipulator/ManipulatorScene.h"
#include "Utility.h"
#include "XTPCustomPropertyGridItem.h"

bool PropertyPaneEffect::_OnCreate()
{
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

	return true;
}

void PropertyPaneEffect::_SetProperty( int id)
{
	ManipulatorEffect& manEffect = ManipulatorSystem.GetEffect();
	CXTPPropertyGridItem* pItem = m_mapItem[id];

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

	case propOptimalAdjustFactor0:
	case propOptimalAdjustFactor1:
	case propOptimalAdjustFactor2: pItem = m_mapItem[propShadowOptimalAdjustFactor];
	case propShadowOptimalAdjustFactor: 
		{
			CXTPPropertyGridItemVec3* pItemVal = dynamic_cast<CXTPPropertyGridItemVec3*>(pItem);
			const Ogre::Vector3 factor = GetVec3Value(pItemVal, id != propShadowOptimalAdjustFactor);
			manEffect.SetShadowOptimalAdjustFactor(0, factor.x);
			manEffect.SetShadowOptimalAdjustFactor(1, factor.y);
			manEffect.SetShadowOptimalAdjustFactor(2, factor.z);
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

	case propShadowMapSize0:
	case propShadowMapSize1:
	case propShadowMapSize2: pItem = m_mapItem[propShadowMapSize];
	case propShadowMapSize: 
		{
			CXTPPropertyGridItemVec3* pItemVal = dynamic_cast<CXTPPropertyGridItemVec3*>(pItem);
			const Ogre::Vector3 size = GetVec3Value(pItemVal, id != propShadowMapSize);
			manEffect.SetShadowMapSize(0, (int)size.x);
			manEffect.SetShadowMapSize(1, (int)size.y);
			manEffect.SetShadowMapSize(2, (int)size.z);
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
}

void PropertyPaneEffect::_UpdateProperty( int id )
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
