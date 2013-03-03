

#include "TerrainMaterialGeneratorD.h"
#include "Terrain/OgreTerrain.h"
#include "OgreMaterialManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include "OgreTextureUnitState.h"
#include "OgreGpuProgramManager.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreHardwarePixelBuffer.h"
 
namespace Ogre
{
	//---------------------------------------------------------------------
	TerrainMaterialGeneratorD::TerrainMaterialGeneratorD()
	{
		// define the layers
		// We expect terrain textures to have no alpha, so we use the alpha channel
		// in the albedo texture to store specular reflection
		// similarly we double-up the normal and height (for parallax)
		mLayerDecl.samplers.push_back(TerrainLayerSampler("albedo_specular", PF_BYTE_RGBA));
		mLayerDecl.samplers.push_back(TerrainLayerSampler("normal_height", PF_BYTE_RGBA));
 
		mLayerDecl.elements.push_back(
			TerrainLayerSamplerElement(0, TLSS_ALBEDO, 0, 3));
		mLayerDecl.elements.push_back(
			TerrainLayerSamplerElement(0, TLSS_SPECULAR, 3, 1));
		mLayerDecl.elements.push_back(
			TerrainLayerSamplerElement(1, TLSS_NORMAL, 0, 3));
		mLayerDecl.elements.push_back(
			TerrainLayerSamplerElement(1, TLSS_HEIGHT, 3, 1));
 
 
		mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));
		// TODO - check hardware capabilities & use fallbacks if required (more profiles needed)
		setActiveProfile("SM2");
 
	}
	//---------------------------------------------------------------------
	TerrainMaterialGeneratorD::~TerrainMaterialGeneratorD()
	{
 
	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	TerrainMaterialGeneratorD::SM2Profile::SM2Profile(TerrainMaterialGenerator* parent, const String& name, const String& desc)
		: Profile(parent, name, desc)
		, mShaderGen(0)
		, mGlobalColourMapEnabled(false)
		, mLightmapEnabled(false)
		, mCompositeMapEnabled(false)
	{
 
	}
	//---------------------------------------------------------------------
	TerrainMaterialGeneratorD::SM2Profile::~SM2Profile()
	{
		OGRE_DELETE mShaderGen;
	}	
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::requestOptions(Terrain* terrain)
	{
		terrain->_setMorphRequired(true);
		terrain->_setNormalMapRequired(true);
		terrain->_setLightMapRequired(mLightmapEnabled, true);
		terrain->_setCompositeMapRequired(mCompositeMapEnabled);
	}
	//---------------------------------------------------------------------
	bool TerrainMaterialGeneratorD::SM2Profile::isVertexCompressionSupported() const
	{
		return true;
	}
	//---------------------------------------------------------------------
	void  TerrainMaterialGeneratorD::SM2Profile::setGlobalColourMapEnabled(bool enabled)
	{
		if (enabled != mGlobalColourMapEnabled)
		{
			mGlobalColourMapEnabled = enabled;
			mParent->_markChanged();
		}
	}
	//---------------------------------------------------------------------
	void  TerrainMaterialGeneratorD::SM2Profile::setLightmapEnabled(bool enabled)
	{
		if (enabled != mLightmapEnabled)
		{
			mLightmapEnabled = enabled;
			mParent->_markChanged();
		}
	}
	//---------------------------------------------------------------------
	void  TerrainMaterialGeneratorD::SM2Profile::setCompositeMapEnabled(bool enabled)
	{
		if (enabled != mCompositeMapEnabled)
		{
			mCompositeMapEnabled = enabled;
			mParent->_markChanged();
		}
	}
	//---------------------------------------------------------------------
	uint8 TerrainMaterialGeneratorD::SM2Profile::getMaxLayers(const Terrain* terrain) const
	{
		// count the texture units free
		uint8 freeTextureUnits = 16;
		// lightmap
		--freeTextureUnits;
		// normalmap
		--freeTextureUnits;
		// colourmap
		if (terrain->getGlobalColourMapEnabled())
			--freeTextureUnits;
 
		// each layer needs 2.25 units (1xdiffusespec, 1xnormalheight, 0.25xblend)
		return static_cast<uint8>(freeTextureUnits / 2.25f);
 
 
	}
	//---------------------------------------------------------------------
	MaterialPtr TerrainMaterialGeneratorD::SM2Profile::generate(const Terrain* terrain)
	{
		// re-use old material if exists
		MaterialPtr mat = terrain->_getMaterial();
		if (mat.isNull())
		{
			MaterialManager& matMgr = MaterialManager::getSingleton();
 
			// it's important that the names are deterministic for a given terrain, so
			// use the terrain pointer as an ID
			const String& matName = terrain->getMaterialName();
			mat = matMgr.getByName(matName);
			if (mat.isNull())
			{
				mat = matMgr.create(matName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			}
		}
		// clear everything
		mat->removeAllTechniques();
 
		addTechnique(mat, terrain, HIGH_LOD);
 
// 		// LOD
// 		if(mCompositeMapEnabled)
// 		{
// 			addTechnique(mat, terrain, LOW_LOD);
// 			Material::LodValueList lodValues;
// 			lodValues.push_back(TerrainGlobalOptions::getSingleton().getCompositeMapDistance());
// 			mat->setLodLevels(lodValues);
// 			Technique* lowLodTechnique = mat->getTechnique(1);
// 			lowLodTechnique->setLodIndex(1);
// 		}
 
		updateParams(mat, terrain);

		return mat;
	}
	//---------------------------------------------------------------------
// 	MaterialPtr TerrainMaterialGeneratorD::SM2Profile::generateForCompositeMap(const Terrain* terrain)
// 	{
// 		// re-use old material if exists
// 		MaterialPtr mat = terrain->_getCompositeMapMaterial();
// 		if (mat.isNull())
// 		{
// 			MaterialManager& matMgr = MaterialManager::getSingleton();
//  
// 			// it's important that the names are deterministic for a given terrain, so
// 			// use the terrain pointer as an ID
// 			const String& matName = terrain->getMaterialName() + "/comp";
// 			mat = matMgr.getByName(matName);
// 			if (mat.isNull())
// 			{
// 				mat = matMgr.create(matName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
// 			}
// 		}
// 		// clear everything
// 		mat->removeAllTechniques();
//  
// 		addTechnique(mat, terrain, RENDER_COMPOSITE_MAP);
//  
// 		updateParamsForCompositeMap(mat, terrain);
//  
// 		return mat;
//  
// 	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::addTechnique(
		const MaterialPtr& mat, const Terrain* terrain, TechniqueType tt)
	{
		Technique* tech = mat->createTechnique();
		tech->setSchemeName("GBuffer");
 
		// Only supporting one pass
		Pass* pass = tech->createPass();
		//pass->setName("NO_DEFERRED");
 
		GpuProgramManager& gmgr = GpuProgramManager::getSingleton();
		HighLevelGpuProgramManager& hmgr = HighLevelGpuProgramManager::getSingleton();
		if (!mShaderGen)
		{
			if (hmgr.isLanguageSupported("cg"))
				mShaderGen = OGRE_NEW ShaderHelperCg();
			else
			{
				// todo
			}
 
			// check SM3 features
			mSM3Available = GpuProgramManager::getSingleton().isSyntaxSupported("ps_3_0");
			mSM4Available = GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_0");
 
		}
		HighLevelGpuProgramPtr vprog = mShaderGen->generateVertexProgram(this, terrain, tt);
		HighLevelGpuProgramPtr fprog = mShaderGen->generateFragmentProgram(this, terrain, tt);
 
		pass->setVertexProgram(vprog->getName());
		pass->setFragmentProgram(fprog->getName());
 
		if (tt == HIGH_LOD || tt == RENDER_COMPOSITE_MAP)
		{
			// global normal map
			TextureUnitState* tu = pass->createTextureUnitState();
			tu->setTextureName(terrain->getTerrainNormalMap()->getName());
			tu->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
 
			// global colour map
			if (terrain->getGlobalColourMapEnabled() && isGlobalColourMapEnabled())
			{
				tu = pass->createTextureUnitState(terrain->getGlobalColourMap()->getName());
				tu->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
			}
 
			// light map
			if (isLightmapEnabled())
			{
				tu = pass->createTextureUnitState(terrain->getLightmap()->getName());
				tu->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
			}
 
			// blend maps
			uint maxLayers = getMaxLayers(terrain);
			uint numBlendTextures = std::min(terrain->getBlendTextureCount(maxLayers), terrain->getBlendTextureCount());
			uint numLayers = std::min(maxLayers, static_cast<uint>(terrain->getLayerCount()));
			for (uint i = 0; i < numBlendTextures; ++i)
			{
				tu = pass->createTextureUnitState(terrain->getBlendTextureName(i));
				tu->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
			}
 
			// layer textures
			for (uint i = 0; i < numLayers; ++i)
			{
				// diffuse / specular
				pass->createTextureUnitState(terrain->getLayerTextureName(i, 0));
 
				// normal / height
				pass->createTextureUnitState(terrain->getLayerTextureName(i, 1));
			}
 
		}
		else
		{
			assert(0);
			// LOW_LOD textures
			// composite map
			TextureUnitState* tu = pass->createTextureUnitState();
			tu->setTextureName(terrain->getCompositeMap()->getName());
			tu->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
 
			// That's it!
 
		}
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::updateParams(const MaterialPtr& mat, const Terrain* terrain)
	{
		mShaderGen->updateParams(this, mat, terrain, false);
 
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::updateParamsForCompositeMap(const MaterialPtr& mat, const Terrain* terrain)
	{
		mShaderGen->updateParams(this, mat, terrain, true);
	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	HighLevelGpuProgramPtr 
		TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::generateVertexProgram(
			const SM2Profile* prof, const Terrain* terrain, TechniqueType tt)
	{
		HighLevelGpuProgramPtr ret = createVertexProgram(prof, terrain, tt);
 
		StringUtil::StrStreamType sourceStr;
		generateVertexProgramSource(prof, terrain, tt, sourceStr);
		ret->setSource(sourceStr.str());
		ret->load();
		defaultVpParams(prof, terrain, tt, ret);
#if OGRE_DEBUG_MODE
		LogManager::getSingleton().stream(LML_TRIVIAL) << "*** Terrain Vertex Program: " 
			<< ret->getName() << " ***\n" << ret->getSource() << "\n***   ***";
#endif
 
		return ret;
 
	}
	//---------------------------------------------------------------------
	HighLevelGpuProgramPtr 
	TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::generateFragmentProgram(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt)
	{
		HighLevelGpuProgramPtr ret = createFragmentProgram(prof, terrain, tt);
 
		StringUtil::StrStreamType sourceStr;
		generateFragmentProgramSource(prof, terrain, tt, sourceStr);
		ret->setSource(sourceStr.str());
		ret->load();
		defaultFpParams(prof, terrain, tt, ret);
 
#if 1
		LogManager::getSingleton().stream(LML_CRITICAL) << "*** Terrain Fragment Program: " 
			<< ret->getName() << " ***\n" << ret->getSource() << "\n***   ***";
#endif
 
		return ret;
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::generateVertexProgramSource(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream)
	{
		generateVpHeader(prof, terrain, tt, outStream);
 
		uint maxLayers = prof->getMaxLayers(terrain);
		uint numLayers = std::min(maxLayers, static_cast<uint>(terrain->getLayerCount()));

		for (uint i = 0; i < numLayers; ++i)
			generateVpLayer(prof, terrain, tt, i, outStream);
 
		generateVpFooter(prof, terrain, tt, outStream);
 
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::generateFragmentProgramSource(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream)
	{
		generateFpHeader(prof, terrain, tt, outStream);
 
		uint maxLayers = prof->getMaxLayers(terrain);
		uint numLayers = std::min(maxLayers, static_cast<uint>(terrain->getLayerCount()));

		for (uint i = 0; i < numLayers; ++i)
			generateFpLayer(prof, terrain, tt, i, outStream);
 
		generateFpFooter(prof, terrain, tt, outStream);
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::defaultVpParams(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, const HighLevelGpuProgramPtr& prog)
	{
		GpuProgramParametersSharedPtr params = prog->getDefaultParameters();
		params->setIgnoreMissingParams(true);
		params->setNamedAutoConstant("worldMatrix", GpuProgramParameters::ACT_WORLD_MATRIX);
		params->setNamedAutoConstant("viewMatrix", GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
		params->setNamedAutoConstant("viewProjMatrix", GpuProgramParameters::ACT_VIEWPROJ_MATRIX);
		params->setNamedAutoConstant("lodMorph", GpuProgramParameters::ACT_CUSTOM, 
			Terrain::LOD_MORPH_CUSTOM_PARAM);
		params->setNamedAutoConstant("fogParams", GpuProgramParameters::ACT_FOG_PARAMS);
 
		if (terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP)
		{
			Matrix4 posIndexToObjectSpace;
			terrain->getPointTransform(&posIndexToObjectSpace);
			params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
		}
 
 
 
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::defaultFpParams(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, const HighLevelGpuProgramPtr& prog)
	{
		GpuProgramParametersSharedPtr params = prog->getDefaultParameters();
		params->setIgnoreMissingParams(true);
		params->setNamedAutoConstant("fogColour", GpuProgramParameters::ACT_FOG_COLOUR);
		params->setNamedAutoConstant("cFarDistance", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
		params->setNamedAutoConstant("viewMatrix", GpuProgramParameters::ACT_WORLDVIEW_MATRIX); // tout sauf Z : VIEW_MATRIX
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::updateParams(
		const SM2Profile* prof, const MaterialPtr& mat, const Terrain* terrain, bool compositeMap)
	{
		Pass* p = mat->getTechnique(0)->getPass(0);
// 		if (compositeMap)
// 		{
// 			updateVpParams(prof, terrain, RENDER_COMPOSITE_MAP, p->getVertexProgramParameters());
// 			updateFpParams(prof, terrain, RENDER_COMPOSITE_MAP, p->getFragmentProgramParameters());
// 		}
//		else
		{
			// high lod
			updateVpParams(prof, terrain, HIGH_LOD, p->getVertexProgramParameters());
			updateFpParams(prof, terrain, HIGH_LOD, p->getFragmentProgramParameters());
 
// 			if(prof->isCompositeMapEnabled())
// 			{
// 				// low lod
// 				p = mat->getTechnique(1)->getPass(0);
// 				updateVpParams(prof, terrain, LOW_LOD, p->getVertexProgramParameters());
// 				updateFpParams(prof, terrain, LOW_LOD, p->getFragmentProgramParameters());
// 			}
		}
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::updateVpParams(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, const GpuProgramParametersSharedPtr& params)
	{
		params->setIgnoreMissingParams(true);
		uint maxLayers = prof->getMaxLayers(terrain);
		uint numLayers = std::min(maxLayers, static_cast<uint>(terrain->getLayerCount()));
		uint numUVMul = numLayers / 4;
		if (numLayers % 4)
			++numUVMul;
		for (uint i = 0; i < numUVMul; ++i)
		{
			Vector4 uvMul(
				terrain->getLayerUVMultiplier(i * 4), 
				terrain->getLayerUVMultiplier(i * 4 + 1), 
				terrain->getLayerUVMultiplier(i * 4 + 2), 
				terrain->getLayerUVMultiplier(i * 4 + 3) 
				);
			params->setNamedConstant("uvMul_" + StringConverter::toString(i), uvMul);
		}
 
		if (terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP)
		{
			Real baseUVScale = 1.0f / (terrain->getSize() - 1);
			params->setNamedConstant("baseUVScale", baseUVScale);
		}
 
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::updateFpParams(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, const GpuProgramParametersSharedPtr& params)
	{
		params->setIgnoreMissingParams(true);
		// TODO - parameterise this?
		/*Vector4 scaleBiasSpecular(0.03, -0.04, 32, 1);
		params->setNamedConstant("scaleBiasSpecular", scaleBiasSpecular);*/
 
	}
	//---------------------------------------------------------------------
	String TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::getChannel(uint idx)
	{
		uint rem = idx % 4;
		switch(rem)
		{
		case 0:
		default:
			return "r";
		case 1:
			return "g";
		case 2:
			return "b";
		case 3:
			return "a";
		};
	}
	//---------------------------------------------------------------------
	String TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::getVertexProgramName(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt)
	{
		String progName = terrain->getMaterialName() + "/sm2/vp";
 
// 		switch(tt)
// 		{
// 		case HIGH_LOD:
// 			progName += "/hlod";
// 			break;
// 		case LOW_LOD:
// 			progName += "/llod";
// 			break;
// 		case RENDER_COMPOSITE_MAP:
// 			progName += "/comp";
// 			break;
// 		}
		progName += "/hlod";
 
		return progName;
 
	}
	//---------------------------------------------------------------------
	String TerrainMaterialGeneratorD::SM2Profile::ShaderHelper::getFragmentProgramName(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt)
	{
 
		String progName = terrain->getMaterialName() + "/sm2/fp";
 
// 		switch(tt)
// 		{
// 		case HIGH_LOD:
// 			progName += "/hlod";
// 			break;
// 		case LOW_LOD:
// 			progName += "/llod";
// 			break;
// 		case RENDER_COMPOSITE_MAP:
// 			progName += "/comp";
// 			break;
// 		}

		progName += "/hlod";
 
		return progName;
	}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	HighLevelGpuProgramPtr
	TerrainMaterialGeneratorD::SM2Profile::ShaderHelperCg::createVertexProgram(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt)
	{
		HighLevelGpuProgramManager& mgr = HighLevelGpuProgramManager::getSingleton();
		String progName = getVertexProgramName(prof, terrain, tt);
		HighLevelGpuProgramPtr ret = mgr.getByName(progName);
		if (ret.isNull())
		{
			ret = mgr.createProgram(progName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
				"cg", GPT_VERTEX_PROGRAM);
		}
		else
		{
			ret->unload();
		}
 
		ret->setParameter("profiles", "vs_4_0 vs_3_0 vs_2_0 arbvp1");
		ret->setParameter("entry_point", "main_vp");
 
		return ret;
 
	}
	//---------------------------------------------------------------------
	HighLevelGpuProgramPtr
		TerrainMaterialGeneratorD::SM2Profile::ShaderHelperCg::createFragmentProgram(
			const SM2Profile* prof, const Terrain* terrain, TechniqueType tt)
	{
		HighLevelGpuProgramManager& mgr = HighLevelGpuProgramManager::getSingleton();
		String progName = getFragmentProgramName(prof, terrain, tt);
 
		HighLevelGpuProgramPtr ret = mgr.getByName(progName);
		if (ret.isNull())
		{
			ret = mgr.createProgram(progName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
				"cg", GPT_FRAGMENT_PROGRAM);
		}
		else
		{
			ret->unload();
		}
 
		ret->setParameter("profiles", "ps_4_0 ps_3_0 ps_2_x fp40 arbfp1");
		ret->setParameter("entry_point", "main_fp");
 
		return ret;
 
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelperCg::generateVpHeader(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream)
	{
		outStream << 
			"void main_vp(\n";
		bool compression = terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP;
		if (compression)
		{
			outStream << 
				"float2 posIndex : POSITION,\n"
				"float height  : TEXCOORD0,\n";
		}
		else
		{
			outStream <<
				"float4 pos : POSITION,\n"
				"float2 uv  : TEXCOORD0,\n";
 
		}
		if (tt != RENDER_COMPOSITE_MAP)
			outStream << "float2 delta  : TEXCOORD1,\n"; // lodDelta, lodThreshold
 
		outStream << 
			"uniform float4x4 worldMatrix,\n"
			"uniform float4x4 viewMatrix,\n"
			"uniform float4x4 viewProjMatrix,\n"
			"uniform float2   lodMorph,\n"; // morph amount, morph LOD target
 
		if (compression)
		{
			outStream << 
				"uniform float4x4   posIndexToObjectSpace,\n"
				"uniform float    baseUVScale,\n";
		}
		// uv multipliers
		uint maxLayers = prof->getMaxLayers(terrain);
		uint numLayers = std::min(maxLayers, static_cast<uint>(terrain->getLayerCount()));
		uint numUVMultipliers = (numLayers / 4);
		if (numLayers % 4)
			++numUVMultipliers;
		for (uint i = 0; i < numUVMultipliers; ++i)
			outStream << "uniform float4 uvMul_" << i << ", \n";
 
		outStream <<
			"out float4 oPos : POSITION,\n"
			"out float4 oPosObj : TEXCOORD0\n";
 
		uint texCoordSet = 1;
		outStream <<
			", out float4 oUVMisc : TEXCOORD" << texCoordSet++ <<" // xy = uv, z = camDepth\n";
 
		// layer UV's premultiplied, packed as xy/zw
		uint numUVSets = numLayers / 2;
		if (numLayers % 2)
			++numUVSets;

		for (uint i = 0; i < numUVSets; ++i)
		{
			outStream <<
				", out float4 oUV" << i << " : TEXCOORD" << texCoordSet++ << "\n";
		}
 
		if (prof->getParent()->getDebugLevel() && tt != RENDER_COMPOSITE_MAP)
		{
			outStream << ", out float2 lodInfo : TEXCOORD" << texCoordSet++ << "\n";
		}
 
		bool fog = terrain->getSceneManager()->getFogMode() != FOG_NONE && tt != RENDER_COMPOSITE_MAP;
		if (fog)
		{
			outStream <<
				", uniform float4 fogParams\n"
				", out float fogVal : COLOR\n";
		}
 
		// check we haven't exceeded texture coordinates
		if (texCoordSet > 8)
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Requested options require too many texture coordinate sets! Try reducing the number of layers.",
				__FUNCTION__);
		}
 
		outStream <<
			")\n"
			"{\n";
		if (compression)
		{
			outStream <<
				"	float4 pos;\n"
				"	pos = mul(posIndexToObjectSpace, float4(posIndex, height, 1));\n"
				"   float2 uv = float2(posIndex.x * baseUVScale, 1.0 - (posIndex.y * baseUVScale));\n";
		}
		outStream <<
			"	float4 worldPos = mul(worldMatrix, pos);\n"
			"	oPosObj = mul(viewMatrix, pos);\n";
 
		if (tt != RENDER_COMPOSITE_MAP)
		{
			// determine whether to apply the LOD morph to this vertex
			// we store the deltas against all vertices so we only want to apply 
			// the morph to the ones which would disappear. The target LOD which is
			// being morphed to is stored in lodMorph.y, and the LOD at which 
			// the vertex should be morphed is stored in uv.w. If we subtract
			// the former from the latter, and arrange to only morph if the
			// result is negative (it will only be -1 in fact, since after that
			// the vertex will never be indexed), we will achieve our aim.
			// sign(vertexLOD - targetLOD) == -1 is to morph
			outStream << 
				"	float toMorph = -min(0, sign(delta.y - lodMorph.y));\n";
			// this will either be 1 (morph) or 0 (don't morph)
			if (prof->getParent()->getDebugLevel())
			{
				// x == LOD level (-1 since value is target level, we want to display actual)
				outStream << "lodInfo.x = (lodMorph.y - 1) / " << terrain->getNumLodLevels() << ";\n";
				// y == LOD morph
				outStream << "lodInfo.y = toMorph * lodMorph.x;\n";
			}
 
			// morph
			switch (terrain->getAlignment())
			{
			case Terrain::ALIGN_X_Y:
				outStream << "	worldPos.z += delta.x * toMorph * lodMorph.x;\n";
				break;
			case Terrain::ALIGN_X_Z:
				outStream << "	worldPos.y += delta.x * toMorph * lodMorph.x;\n";
				break;
			case Terrain::ALIGN_Y_Z:
				outStream << "	worldPos.x += delta.x * toMorph * lodMorph.x;\n";
				break;
			};
		}
 
 
		// generate UVs
		for (uint i = 0; i < numUVSets; ++i)
		{
			uint layer  =  i * 2;
			uint uvMulIdx = layer / 4;

			outStream <<
				"	oUV" << i << ".xy = " << " uv.xy * uvMul_" << uvMulIdx << "." << getChannel(layer) << ";\n";
			outStream <<
				"	oUV" << i << ".zw = " << " uv.xy * uvMul_" << uvMulIdx << "." << getChannel(layer+1) << ";\n";

		}
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelperCg::generateFpHeader(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream)
	{
 
		// Main header
		outStream << 
			// helpers
			"float4 expand(float4 v)\n"
			"{ \n"
			"	return v * 2 - 1;\n"
			"}\n\n\n";
 
		outStream << 
			"void main_fp(\n"
			"float4 position : TEXCOORD0,\n"
			" out float4 oColor0 : COLOR0,\n"
			" out float4 oColor1 : COLOR1,\n"
			" out float4 oColor2 : COLOR2,\n"
			" out float4 oColor3 : COLOR3,\n"
			"uniform float cFarDistance,\n"
			"uniform float4x4 viewMatrix,\n";
 
 
		uint texCoordSet = 1;
		outStream <<
			"float4 uvMisc : TEXCOORD" << texCoordSet++ << ",\n";
 
		// UV's premultiplied, packed as xy/zw
		uint maxLayers = prof->getMaxLayers(terrain);
		uint numBlendTextures = std::min(terrain->getBlendTextureCount(maxLayers), terrain->getBlendTextureCount());
		uint numLayers = std::min(maxLayers, static_cast<uint>(terrain->getLayerCount()));
		uint numUVSets = numLayers / 2;
		if (numLayers % 2)
			++numUVSets;

		for (uint i = 0; i < numUVSets; ++i)
		{
			outStream <<
				"float4 layerUV" << i << " : TEXCOORD" << texCoordSet++ << ", \n";
		}
 
		if (prof->getParent()->getDebugLevel() && tt != RENDER_COMPOSITE_MAP)
		{
			outStream << "float2 lodInfo : TEXCOORD" << texCoordSet++ << ", \n";
		}
 
		bool fog = terrain->getSceneManager()->getFogMode() != FOG_NONE && tt != RENDER_COMPOSITE_MAP;
		if (fog)
		{
			outStream <<
				"uniform float3 fogColour, \n"
				"float fogVal : COLOR,\n";
		}
 
		uint currentSamplerIdx = 0;
 
		if (/*tt == LOW_LOD*/false)
		{
			// single composite map covers all the others below
			outStream << 
				"uniform sampler2D compositeMap : register(s" << currentSamplerIdx++ << ")\n";
		}
		else
		{
			outStream << 
				"uniform sampler2D globalNormal : register(s" << currentSamplerIdx++ << ")\n";
 
			// Blend textures - sampler definitions
			for (uint i = 0; i < numBlendTextures; ++i)
			{
				outStream << ", uniform sampler2D blendTex" << i 
					<< " : register(s" << currentSamplerIdx++ << ")\n";
			}
 
			// Layer textures - sampler definitions & UV multipliers
			for (uint i = 0; i < numLayers; ++i)
			{
				outStream << ", uniform sampler2D difftex" << i 
					<< " : register(s" << currentSamplerIdx++ << ")\n";
				outStream << ", uniform sampler2D normtex" << i 
					<< " : register(s" << currentSamplerIdx++ << ")\n";
			}
		}
 
		// check we haven't exceeded samplers
		if (currentSamplerIdx > 16)
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Requested options require too many texture samplers! Try reducing the number of layers.",
				__FUNCTION__);
		}
 
		outStream << 
			") \n"
			"{\n"
			"	float2 uv = uvMisc.xy;\n"
			// base colour
			"	oColor0 = float4(0,0,0,0);\n"
			"   oColor1 = float4(0,0,0,0);\n";
			"   oColor2 = float4(0,0,0,0);\n";
			"   oColor3 = float4(0,0,0,0);\n";
 
		outStream << "	float3 normal = expand(tex2D(globalNormal, uv));\n";

 
			// set up accumulation areas
		outStream << "	float3 diffuse = float3(0,0,0);\n"
			"	float specular = 0;\n";
 
 
		if (/*tt == LOW_LOD*/false)
		{
			// we just do a single calculation from composite map
			outStream <<
				"	float4 composite = tex2D(compositeMap, uv);\n"
				"	diffuse = composite.rgb;\n";
			// TODO - specular; we'll need normals for this!
		}
		else
		{
			// set up the blend values
			for (uint i = 0; i < numBlendTextures; ++i)
			{
				outStream << "	float4 blendTexVal" << i << " = tex2D(blendTex" << i << ", uv);\n";
			}
 
			// derive the tangent space basis
			// we do this in the pixel shader because we don't have per-vertex normals
			// because of the LOD, we use a normal map
			// tangent is always +x or -z in object space depending on alignment
			switch(terrain->getAlignment())
			{
			case Terrain::ALIGN_X_Y:
			case Terrain::ALIGN_X_Z:
				outStream << "	float3 tangent = float3(1, 0, 0);\n";
				break;
			case Terrain::ALIGN_Y_Z:
				outStream << "	float3 tangent = float3(0, 0, -1);\n";
				break;
			};
 
			// We must do normal calculations here instead of the vertex program because terrain normals
			// are provided by the "normal" texture, and not in vertex info. We multiply it by the viewMatrix
			// so the normals are in eye-space instead of object-space, as required by deferred shading.
			outStream << "	normal = mul(viewMatrix, float4(normal,0)).xyz;" << std::endl;
			outStream << "	tangent = mul(viewMatrix, float4(tangent,0)).xyz;" << std::endl;
			outStream << "	float3 binormal = cross(normal, tangent);" << std::endl;
 
			// derive final matrix
			outStream << "	float3x3 TBN = float3x3(tangent, binormal, normal);\n";
 
			// set up lighting result placeholders for interpolation
			outStream << "	float3 TSnormal;\n";
		}
 
 
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelperCg::generateVpLayer(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, uint layer, StringUtil::StrStreamType& outStream)
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelperCg::generateFpLayer(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, uint layer, StringUtil::StrStreamType& outStream)
	{
		uint uvIdx = layer / 2;
		String uvChannels = (layer % 2) ? ".zw" : ".xy";
		uint blendIdx = (layer-1) / 4;
		String blendChannel = getChannel(layer-1);
		String blendWeightStr = String("blendTexVal") + StringConverter::toString(blendIdx) + 
			"." + blendChannel;
 
		// generate UV
		outStream << "	float2 uv" << layer << " = layerUV" << uvIdx << uvChannels << ";\n";
 
		// access TS normal map
		outStream << "	TSnormal = expand(tex2D(normtex" << layer << ", uv" << layer << ")).rgb;\n";
		if (layer)
		{	
			outStream << "  oColor3.rgb = lerp(oColor3.rgb, TSnormal, " << blendWeightStr << ");\n";
		}
		else
		{
			outStream << "  oColor3.rgb = TSnormal;\n";
		}
 
		// sample diffuse texture
		outStream << "	float4 diffuseSpecTex" << layer 
			<< " = tex2D(difftex" << layer << ", uv" << layer << ");\n";
 
		// apply to common
		if (!layer)
		{
			outStream << "	diffuse = diffuseSpecTex0.rgb;\n";
			outStream << "	specular = diffuseSpecTex0.a;\n";
		}
		else
		{
			outStream << "	diffuse = lerp(diffuse, diffuseSpecTex" << layer 
				<< ".rgb, " << blendWeightStr << ");\n";
			outStream << "	specular = lerp(specular, diffuseSpecTex" << layer 
				<< ".a, " << blendWeightStr << ");\n";
 
		}
 
		// End early-out
		/* Disable - causing some issues even when trying to force the use of texldd
		if (layer && prof->_isSM3Available())
			outStream << "  } // early-out blend value\n";
		*/
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelperCg::generateVpFooter(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream)
	{
 
		outStream << 
			"	oPos = mul(viewProjMatrix, worldPos);\n" 
			"	oUVMisc.xy = uv.xy;\n";
 
		bool fog = terrain->getSceneManager()->getFogMode() != FOG_NONE && tt != RENDER_COMPOSITE_MAP;
		if (fog)
		{
			if (terrain->getSceneManager()->getFogMode() == FOG_LINEAR)
			{
				outStream <<
					"	fogVal = saturate((oPos.z - fogParams.y) * fogParams.w);\n";
			}
			else
			{
				outStream <<
					"	fogVal = 1 - saturate(1 / (exp(oPos.z * fogParams.x)));\n";
			}
		}
 
 
		outStream << 
			"}\n";
 
 
	}
	//---------------------------------------------------------------------
	void TerrainMaterialGeneratorD::SM2Profile::ShaderHelperCg::generateFpFooter(
		const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream)
	{
		// diffuse
		outStream << "	oColor0.rgb = diffuse;\n";
				"   oColor0.a = 1;\n";
		//specualr
		outStream << "	oColor2.a = specular;\n";
 
		//地形太亮,手动给个类似材质系数乘数
		outStream << "	oColor0.rgb *= 0.4f;\n";
 
		// Final return
		outStream << "  oColor3 = float4(normalize(mul(oColor3.rgb, TBN)), 0);\n"
			//depth
			<< "  oColor3.a = length(position) / cFarDistance;\n"
			<< "}\n";
	}
	//---------------------------------------------------------------------
}

