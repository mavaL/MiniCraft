LoadUnloadResourceList
#include "stdafx.h"
#include "ManipulatorTerrain.h"
#include <OgreStreamSerialiser.h>
#include <OgreDeflate.h>
#include "../DotSceneLoader.h"
#include "../DotSceneSerializer.h"
#include "ManipulatorScene.h"
#include "../EditorDefine.h"




ManipulatorTerrain::ManipulatorTerrain()
:m_terrainGroup(nullptr)
,m_vertexPerSide(65)
,m_worldSize(128)
,m_origPos(Ogre::Vector3::ZERO)
,m_pTerrain(nullptr)
,m_curBrushIndex(1)
,m_curEditMode(eTerrainEditMode_None)
,m_curEditLayer(-1)
{
	m_pSceneMgr = Ogre::Root::getSingleton().getSceneManager(SCENE_MANAGER_NAME);
	assert(m_pSceneMgr);

	m_brush[0].reset(new BrushCircle);
	m_brush[1].reset(new BrushSquare);
	m_brush[1]->SetDimension(10, 10);
}

void ManipulatorTerrain::NewFlatTerrain()
{
	SAFE_DELETE(m_terrainGroup);
	m_terrainOption.reset(new TerrainGlobalOptions);
	m_terrainGroup = new TerrainGroup(m_pSceneMgr, Terrain::ALIGN_X_Z, (Ogre::uint16)m_vertexPerSide, (float)m_worldSize);
	m_terrainGroup->setOrigin(m_origPos);

	_ConfigureTerrainDefaults();
	
	//初始化平坦地形
	m_terrainGroup->defineTerrain(0, 0, 0.0f);

	// sync load since we want everything in place when we start
	m_terrainGroup->loadTerrain(0, 0);

	//_InitBlendMap();

	m_terrainGroup->freeTemporaryResources();
	m_pTerrain = m_terrainGroup->getTerrain(0, 0);
}

void ManipulatorTerrain::_InitBlendMap()
{
	Ogre::TerrainLayerBlendMap* blendMap0 = m_pTerrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = m_pTerrain->getLayerBlendMap(2);
	float minHeight0 = 70;
	float fadeDist0 = 40;
	float minHeight1 = 70;
	float fadeDist1 = 15;
	float* pBlend1 = blendMap1->getBlendPointer();
	for (Ogre::uint16 y = 0; y < m_pTerrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < m_pTerrain->getLayerBlendMapSize(); ++x)
		{
			float tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			float height = m_pTerrain->getHeightAtTerrainPosition(tx, ty);
			float val = (height - minHeight0) / fadeDist0;
			val = Ogre::Math::Clamp(val, (float)0, (float)1);
			//*pBlend0++ = val;

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (float)0, (float)1);
			*pBlend1++ = val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}

void ManipulatorTerrain::_ConfigureTerrainDefaults()
{
// 	MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
// 	MaterialManager::getSingleton().setDefaultAnisotropy(7);

	m_terrainOption->setMaxPixelError(8);
	m_terrainOption->setCompositeMapDistance(3000);
	//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
	//m_terrainOption->getDefaultMaterialGenerator()->setDebugLevel(1);
	m_terrainOption->setLightMapSize(128);

	//matProfile->setLightmapEnabled(false);
	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	//mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
	m_terrainOption->setCompositeMapAmbient(m_pSceneMgr->getAmbientLight());
	//mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Terrain::ImportData& defaultimp = m_terrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = m_vertexPerSide;
	defaultimp.worldSize = m_worldSize;
	defaultimp.inputScale = 1.0f;
	defaultimp.minBatchSize = 17;
	defaultimp.maxBatchSize = 65;

	defaultimp.layerList.resize(TERRAIN_MAX_LAYER);
	for (int iLayer=0; iLayer<TERRAIN_MAX_LAYER; ++iLayer)
	{
		defaultimp.layerList[iLayer].worldSize = 128;
		defaultimp.layerList[iLayer].textureNames.clear();
		defaultimp.layerList[iLayer].textureNames.clear();
	}

	//仅初始化地形纹理第1层
	defaultimp.layerList[0].worldSize = 128;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");

	//设置当前编辑层为0
	m_curEditLayer = 0;
}

void ManipulatorTerrain::Shutdown()
{
	SAFE_DELETE(m_terrainGroup);
	m_terrainOption.reset();
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

	SAFE_DELETE(m_terrainGroup);
	m_terrainGroup = new Ogre::TerrainGroup(m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, mapSize, worldSize);
	m_terrainGroup->setOrigin(m_origPos);
	m_terrainGroup->setResourceGroup("General");

	//加载地形数据
	std::wstring fullPath(ManipulatorSystem.GenerateSceneFullPath());
	fullPath += L"terrain.dat";
	m_terrainGroup->defineTerrain(0, 0, Utility::UnicodeToEngine(fullPath));
	m_terrainGroup->loadTerrain(0, 0);
	m_terrainGroup->freeTemporaryResources();
	m_pTerrain = m_terrainGroup->getTerrain(0, 0);

	//设置当前编辑层为0
	m_curEditLayer = 0;
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

	//第0层不能编辑BlendMap,应该从第1层开始
	if(m_curEditMode == eTerrainEditMode_Splat && m_curEditLayer == 0)
		return;

	const Vector3 brushPos = m_brush[m_curBrushIndex]->GetPosition();
	Vector3 tsPos;
	m_pTerrain->getTerrainPosition(brushPos, &tsPos);

	float brushSizeW, brushSizeH;
	m_brush[m_curBrushIndex]->GetDimension(brushSizeW, brushSizeH);
	brushSizeW /= m_worldSize;
	brushSizeH /= m_worldSize;

	int multiplier;
	Ogre::TerrainLayerBlendMap* layer = nullptr;
	if(m_curEditMode == eTerrainEditMode_Deform)
	{
		multiplier = m_pTerrain->getSize() - 1;
	}
	else
	{
		multiplier = m_pTerrain->getLayerBlendMapSize();
		layer = m_pTerrain->getLayerBlendMap(m_curEditLayer);
	}

	long startx = (long)((tsPos.x - brushSizeW / 2) * multiplier);
	long starty = (long)((tsPos.y - brushSizeH / 2) * multiplier);
	long endx = (long)((tsPos.x + brushSizeW / 2) * multiplier);
	long endy= (long)((tsPos.y + brushSizeH / 2) * multiplier);
	startx = max(startx, 0L);
	starty = max(starty, 0L);
	endx = min(endx, (long)multiplier);
	endy = min(endy, (long)multiplier);
	
	for (long y = starty; y <= endy; ++y)
	{
		for (long x = startx; x <= endx; ++x)
		{
			float tsXdist = (x / multiplier) - tsPos.x;
			float tsYdist = (y / multiplier)  - tsPos.y;

			if(m_curEditMode == eTerrainEditMode_Deform)
			{
				float* pData = m_pTerrain->getHeightData();
				pData[y*m_vertexPerSide+x] += 100.0f * dt;

			}
			else
			{
				float* pData = layer->getBlendPointer();
				size_t imgY = multiplier - y;
				float newValue = pData[imgY*multiplier+x] + dt;
				newValue = Ogre::Math::Clamp(newValue, 0.0f, 1.0f);
				pData[imgY*multiplier+x] = newValue;
			}
		}
	}
	
	
	if(m_curEditMode == eTerrainEditMode_Deform)
	{
		Ogre::Rect rect(startx, starty, endx, endy);
		m_pTerrain->dirtyRect(rect);
		m_pTerrain->update();
	}
	else
	{
		size_t imgStartY = multiplier - starty;
		size_t imgEndY = multiplier - endy;
		Ogre::Rect rect(startx, min(imgStartY,imgEndY), endx, max(imgStartY,imgEndY));

		layer->dirtyRect(rect);
		layer->update();
	}
}

const Ogre::StringVector& ManipulatorTerrain::GetAllLayerTexThumbnailNames()
{
	m_vecLayerTex.clear();

	Ogre::FileInfoListPtr fileinfo = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo(
		"TerrainTextures", "*.png");

	int i = 0;
	m_vecLayerTex.resize(fileinfo->size());
	for (auto iter=fileinfo->begin(); iter!=fileinfo->end(); ++iter)
	{
		const Ogre::FileInfo& info = (*iter);
		m_vecLayerTex[i++] = info.archive->getName() + "/" + info.filename;
	}

	return m_vecLayerTex;
}

void ManipulatorTerrain::SetLayerTexWorldSize(int nLayer, float fSize)
{
	assert(nLayer >=0 && nLayer<(int)m_pTerrain->getLayerCount());
	m_pTerrain->setLayerWorldSize(nLayer, fSize);
}

float ManipulatorTerrain::GetLayerTexWorldSize( int nLayer )
{
	assert(nLayer >=0 && nLayer<(int)m_pTerrain->getLayerCount());
	return m_pTerrain->getLayerWorldSize(nLayer);
}

void ManipulatorTerrain::SetLayerTexture( int nLayer, const std::string& diffuseMapName )
{
	assert(nLayer >=0 && nLayer<(int)m_pTerrain->getLayerCount());

	Ogre::String name = diffuseMapName.substr(0, diffuseMapName.find("diffuse"));
	
	//diffuse map
	m_pTerrain->setLayerTextureName(nLayer, 0, name + "diffusespecular.dds");
	//normal map
	m_pTerrain->setLayerTextureName(nLayer, 1, name + "normalheight.dds");
}

void ManipulatorTerrain::SetLayerTexture( int nLayer, int diffuseMapID )
{
	assert(nLayer >=0 && nLayer<(int)m_pTerrain->getLayerCount());

	Ogre::String filename, path;
	Ogre::StringUtil::splitFilename(m_vecLayerTex[diffuseMapID], filename, path);

	SetLayerTexture(nLayer, filename);
}

const std::string ManipulatorTerrain::GetLayerDiffuseMap( int nLayer ) const
{
	assert(nLayer >=0 && nLayer<(int)m_pTerrain->getLayerCount());
	return m_pTerrain->getLayerTextureName(nLayer, 0);
}

const std::string ManipulatorTerrain::GetLayerNormalMap( int nLayer ) const
{
	assert(nLayer >=0 && nLayer<(int)m_pTerrain->getLayerCount());
	return m_pTerrain->getLayerTextureName(nLayer, 1);
}

