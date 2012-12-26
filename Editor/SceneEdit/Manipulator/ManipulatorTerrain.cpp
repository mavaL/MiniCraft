#include "stdafx.h"
#include "ManipulatorTerrain.h"
#include <OgreStreamSerialiser.h>
#include <OgreDeflate.h>
#include "../DotSceneLoader.h"
#include "../DotSceneSerializer.h"
#include "ManipulatorScene.h"
#include "../EditorDefine.h"




ManipulatorTerrain::ManipulatorTerrain()
:m_vertexPerSide(65)
,m_worldSize(128)
,m_origPos(Ogre::Vector3::ZERO)
,m_pTerrain(nullptr)
,m_curBrushIndex(1)
,m_curEditMode(eTerrainEditMode_None)
{
	m_pSceneMgr = Ogre::Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME);
	assert(m_pSceneMgr);

	m_brush[0].reset(new BrushCircle);
	m_brush[1].reset(new BrushSquare);
	m_brush[1]->SetDimension(10, 10);
}

void ManipulatorTerrain::NewFlatTerrain()
{
	m_terrainOption.reset(new TerrainGlobalOptions);
	m_terrainGroup.reset(new TerrainGroup(m_pSceneMgr, Terrain::ALIGN_X_Z, (Ogre::uint16)m_vertexPerSide, (float)m_worldSize));
	m_terrainGroup->setOrigin(m_origPos);

	_ConfigureTerrainDefaults();
	
	//初始化平坦地形
	m_terrainGroup->defineTerrain(0, 0, 0.0f);

	// sync load since we want everything in place when we start
	m_terrainGroup->loadTerrain(0, 0);

// 	if (mTerrainsImported)
// 	{
// 		TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
// 		while(ti.hasMoreElements())
// 		{
// 			Terrain* t = ti.getNext()->instance;
// 			initBlendMaps(t);
// 		}
// 	}
	m_terrainGroup->freeTemporaryResources();
	m_pTerrain = m_terrainGroup->getTerrain(0, 0);
}

void ManipulatorTerrain::_ConfigureTerrainDefaults()
{
// 	MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
// 	MaterialManager::getSingleton().setDefaultAnisotropy(7);

	// Configure global
	m_terrainOption->setMaxPixelError(8);
	// testing composite map
	//m_terrainOption->setCompositeMapDistance(3000);
	//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
	//mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
	//mTerrainGlobals->setLightMapSize(256);

	//matProfile->setLightmapEnabled(false);
	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	//mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
	m_terrainOption->setCompositeMapAmbient(m_pSceneMgr->getAmbientLight());
	//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
	//mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Terrain::ImportData& defaultimp = m_terrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = m_vertexPerSide;
	defaultimp.worldSize = m_worldSize;
	defaultimp.inputScale = 600;
	defaultimp.minBatchSize = 17;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 128;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 128;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 128;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

void ManipulatorTerrain::Load( rapidxml::xml_node<>* XMLNode )
{
	m_terrainOption.reset(new TerrainGlobalOptions);

	Ogre::Real worldSize = DotSceneLoader::getAttribReal(XMLNode, "worldSize");
	int mapSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("mapSize")->value());
	//bool colourmapEnabled = DotSceneLoader::getAttribBool(XMLNode, "colourmapEnabled");
	//int colourMapTextureSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("colourMapTextureSize")->value());
	//int compositeMapDistance = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningCompositeMapDistance")->value());
	int maxPixelError = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningMaxPixelError")->value());

	//     Ogre::Vector3 lightdir(0, -0.3, 0.75);
	//     lightdir.normalise();
	//     Ogre::Light* l = mSceneMgr->createLight("tstLight");
	//     l->setType(Ogre::Light::LT_DIRECTIONAL);
	//     l->setDirection(lightdir);
	//     l->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
	//     l->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	m_terrainOption->setMaxPixelError((Ogre::Real)maxPixelError);
	//m_terrainOption->setCompositeMapDistance((Ogre::Real)compositeMapDistance);
	// mTerrainGlobalOptions->setLightMapDirection(lightdir);
	m_terrainOption->setCompositeMapAmbient(m_pSceneMgr->getAmbientLight());
	//mTerrainGlobalOptions->setCompositeMapDiffuse(l->getDiffuseColour());

	//mSceneMgr->destroyLight("tstLight");

	m_terrainGroup.reset(new Ogre::TerrainGroup(m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, mapSize, worldSize));
	m_terrainGroup->setOrigin(m_origPos);
	m_terrainGroup->setResourceGroup("General");

	//     rapidxml::xml_node<>* pElement;
	//     rapidxml::xml_node<>* pPageElement;

	//     // Process terrain pages (*)
	//     pElement = XMLNode->first_node("terrainPages");
	//     if(pElement)
	//     {
	//         pPageElement = pElement->first_node("terrainPage");
	//         while(pPageElement)
	//         {
	//             processTerrainPage(pPageElement);
	//             pPageElement = pPageElement->next_sibling("terrainPage");
	//         }
	//     }

	//加载地形数据
	std::wstring fullPath(ManipulatorSystem.GenerateSceneFullPath());
	fullPath += L"terrain.dat";
	m_terrainGroup->defineTerrain(0, 0, Utility::UnicodeToEngine(fullPath));
	m_terrainGroup->loadTerrain(0, 0);
	m_terrainGroup->freeTemporaryResources();
	m_pTerrain = m_terrainGroup->getTerrain(0, 0);
}

void ManipulatorTerrain::Shutdown()
{
	m_terrainGroup.reset();
	m_terrainOption.reset();
}

void ManipulatorTerrain::Serialize( rapidxml::xml_document<>& doc, rapidxml::xml_node<>* XMLNode )
{
	String strWorldSize = Ogre::StringConverter::toString(m_worldSize);
	String strTerrainSize = Ogre::StringConverter::toString(m_vertexPerSide);
	String strPixelError = Ogre::StringConverter::toString(m_terrainOption->getMaxPixelError());

	XMLNode->append_attribute(doc.allocate_attribute("worldSize", doc.allocate_string(strWorldSize.c_str())));
	XMLNode->append_attribute(doc.allocate_attribute("mapSize", doc.allocate_string(strTerrainSize.c_str())));
	XMLNode->append_attribute(doc.allocate_attribute("tuningMaxPixelError", doc.allocate_string(strPixelError.c_str())));

	//保存地形数据
	std::wstring fullPath(ManipulatorSystem.GenerateSceneFullPath());
	fullPath += L"terrain.dat";
	Ogre::DataStreamPtr stream = Ogre::Root::getSingleton().createFileStream(Utility::UnicodeToEngine(fullPath), 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	//文件压缩.地形数据貌似很大,以后研究为什么
	Ogre::DataStreamPtr compressStream(new Ogre::DeflateStream(Utility::UnicodeToEngine(fullPath), stream));
	Ogre::StreamSerialiser ser(compressStream);

	m_terrainGroup->getTerrain(0, 0)->save(ser);
}

float ManipulatorTerrain::GetHeightAt( const Ogre::Vector2& worldPos )
{
	assert(m_terrainGroup);

	Terrain* pTerrain = nullptr;
	float retH = m_terrainGroup->getHeightAtWorldPosition(worldPos.x, 0, worldPos.y, &pTerrain);
	//assert(pTerrain && "worldPos is invalid!");

	return retH;
}

void ManipulatorTerrain::SetTerrainDeformEnabled(bool bEnable)
{
	m_brush[m_curBrushIndex]->SetActive(bEnable);
	m_curEditMode = bEnable ? eTerrainEditMode_Deform : eTerrainEditMode_None;
}

void ManipulatorTerrain::SetTerrainSplatEnabled( bool bEnable )
{
	m_brush[m_curBrushIndex]->SetActive(bEnable);
	m_curEditMode = bEnable ? eTerrainEditMode_Splat : eTerrainEditMode_None;
}

void ManipulatorTerrain::SetBrushPosition( const Ogre::Vector3& pos )
{
	//检测防止画刷范围超出地形边界
	Ogre::Vector3 clampPos(pos);
	float brushDim1, brushDim2;
	m_brush[m_curBrushIndex]->GetDimension(brushDim1, brushDim2);

	if (m_curBrushIndex == 0)	//circle
	{
		if(clampPos.x - brushDim2 < -m_worldSize/2)	clampPos.x = brushDim2 - m_worldSize/2;
		if(clampPos.x + brushDim2 > m_worldSize/2)	clampPos.x = m_worldSize/2 - brushDim2;
		if(clampPos.z - brushDim2 < -m_worldSize/2)	clampPos.z = brushDim2 - m_worldSize/2;
		if(clampPos.z + brushDim2 > m_worldSize/2)	clampPos.z = m_worldSize/2 - brushDim2;
	}
	else	//square
	{
		if(clampPos.x - brushDim1/2 < -m_worldSize/2)	clampPos.x = brushDim1/2 - m_worldSize/2;
		if(clampPos.x + brushDim1/2 > m_worldSize/2)	clampPos.x = m_worldSize/2 - brushDim1/2;
		if(clampPos.z - brushDim2/2 < -m_worldSize/2)	clampPos.z = brushDim2/2 - m_worldSize/2;
		if(clampPos.z + brushDim2/2 > m_worldSize/2)	clampPos.z = m_worldSize/2 - brushDim2/2;
	}

	m_brush[m_curBrushIndex]->SetPosition(clampPos);
}

bool ManipulatorTerrain::GetRayIntersectPoint( const Ogre::Ray& worldRay, Ogre::Vector3& retPt )
{
	assert(m_terrainGroup);

	auto result = m_terrainGroup->rayIntersects(worldRay);

	if (result.hit)
	{
		retPt = result.position;
		return true;
	}

	return false;
}

void ManipulatorTerrain::OnGizmoNodeReset()
{
	m_brush[0]->OnGizmoNodeReset();
	m_brush[1]->OnGizmoNodeReset();
}

void ManipulatorTerrain::SetSquareBrushWidth( float w )
{
	float oldW, oldH;
	m_brush[1]->GetDimension(oldW, oldH);
	m_brush[1]->SetDimension(w, oldH);
}

void ManipulatorTerrain::SetSquareBrushHeight( float h )
{
	float oldW, oldH;
	m_brush[1]->GetDimension(oldW, oldH);
	m_brush[1]->SetDimension(oldW, h);
}

float ManipulatorTerrain::GetSquareBrushWidth()
{
	float oldW, oldH;
	m_brush[1]->GetDimension(oldW, oldH);

	return oldW;
}

float ManipulatorTerrain::GetSquareBrushHeight()
{
	float oldW, oldH;
	m_brush[1]->GetDimension(oldW, oldH);

	return oldH;
}

void ManipulatorTerrain::OnEdit( float dt )
{
	assert(m_curEditMode != eTerrainEditMode_None);
}
