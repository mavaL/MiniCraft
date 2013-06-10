/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2012 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/

#include "stdafx.h"
#include "GBufferMaterialGenerator.h"

//Use this directive to control whether you are writing projective (regular) or linear depth.
#define WRITE_LINEAR_DEPTH

namespace Kratos
{
	//This is the concrete implementation of the material generator.
	class GBufferMaterialGeneratorImpl : public MaterialGenerator::Impl
	{
	public:
		GBufferMaterialGeneratorImpl(const Ogre::String& baseName) : mBaseName(baseName) {}

	protected:
		Ogre::String mBaseName;
		virtual Ogre::GpuProgramPtr generateVertexShader(MaterialGenerator::Perm permutation);
		virtual Ogre::GpuProgramPtr generateFragmentShader(MaterialGenerator::Perm permutation);
		virtual Ogre::MaterialPtr generateTemplateMaterial(MaterialGenerator::Perm permutation);

	};

	GBufferMaterialGenerator::GBufferMaterialGenerator() {
		vsMask = VS_MASK;
		fsMask = FS_MASK;
		matMask = MAT_MASK;
		materialBaseName = "DeferredShading/GBuffer/";
		mImpl = new GBufferMaterialGeneratorImpl(materialBaseName);
	}

	Ogre::GpuProgramPtr GBufferMaterialGeneratorImpl::generateVertexShader(MaterialGenerator::Perm permutation)
	{
		Ogre::StringStream ss;

		ss << "void ToGBufferVP(" << std::endl;
		ss << "	float4 iPosition : POSITION," << std::endl;
		ss << "	float3 iNormal   : NORMAL," << std::endl;

		Ogre::uint32 numTexCoords = (permutation & GBufferMaterialGenerator::GBP_TEXCOORD_MASK) >> 8;
		Ogre::uint32 iTexCoord = 0;
		for (; iTexCoord<numTexCoords; iTexCoord++) 
		{
			ss << "	float2 iUV" << iTexCoord << " : TEXCOORD" << iTexCoord << ',' << std::endl;
		}

		//Input for instancing
		int nWeights = (permutation & GBufferMaterialGenerator::GBP_INSTANCING_WEIGHT_MASK) >> 20;
		if(nWeights != 0)
		{
			ss << " uniform sampler2D matrixTexture : register(s3)," << std::endl;
			for (int i=0; i<nWeights; ++i) 
			{
				ss << "	float4 m03_offset" << i << " : TEXCOORD" << iTexCoord++ << ',' << std::endl;
			}
			ss << "	float2 mOffset : TEXCOORD" << iTexCoord++ << ',' << std::endl;
			ss << "	float4 worldMatrix0 : TEXCOORD" << iTexCoord++ << ',' << std::endl;
			ss << "	float4 worldMatrix1 : TEXCOORD" << iTexCoord++ << ',' << std::endl;
			ss << "	float4 worldMatrix2 : TEXCOORD" << iTexCoord++ << ',' << std::endl;
		}

		if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
		{
			ss << "	float3 iTangent : TANGENT0," << std::endl;
		}

		if(permutation & GBufferMaterialGenerator::GBP_SKINNED)
		{
			ss << "	float4 blendWgt : BLENDWEIGHT," << std::endl;

			//No instancing
			if(nWeights == 0)
			{
				ss << "	float4 blendIdx : BLENDINDICES," << std::endl;
				//50骨骼应该够了
				ss << "	uniform float3x4   worldMatrix3x4Array[50]," << std::endl;
			}
		}
		ss << std::endl;

		ss << "	out float4 oPosition : POSITION," << std::endl;
#ifdef WRITE_LINEAR_DEPTH
		ss << "	out float3 oViewPos : TEXCOORD0," << std::endl;
#else
		ss << "	out float oDepth : TEXCOORD0," << std::endl;
#endif
		ss << "	out float3 oNormal : TEXCOORD1," << std::endl;
		int texCoordNum = 2;
		if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP) 
		{
			ss << "	out float3 oTangent : TEXCOORD" << texCoordNum++ << ',' << std::endl;
			ss << "	out float3 oBiNormal : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}
		for (Ogre::uint32 i=0; i<numTexCoords; i++) 
		{
			ss << "	out float2 oUV" << i << " : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}
		ss << std::endl;

		if(permutation & GBufferMaterialGenerator::GBP_SKINNED)
		{
			ss << "	uniform float4x4 cViewProj," << std::endl;
			ss << "	uniform float4x4 cView," << std::endl;
		}
		else
		{
			ss << "	uniform float4x4 cWorldViewProj," << std::endl;
		}
		
		ss << "	uniform float4x4 cWorldView" << std::endl;
		
		ss << "	)" << std::endl;
		ss << "{" << std::endl;

		if(permutation & GBufferMaterialGenerator::GBP_SKINNED)
		{
			///硬件蒙皮
			ss << "	float4 blendPos = float4(0,0,0,0);" << std::endl;
			ss << "	float3 norm = float3(0,0,0);" << std::endl;

			if(nWeights == 0)
			{
				///No instancing
				ss << "	for (int i = 0; i < 4; ++i)" << std::endl;
				ss << "	{" << std::endl;
				//加这个判断是因为,看这里:http://www.ogre3d.org/forums/viewtopic.php?t=12839&f=4#p491169
				ss << "		if(i==3 && blendWgt[i]==1)" << std::endl;
				ss << "			break;" << std::endl;
				ss << "		blendPos += float4(mul(worldMatrix3x4Array[blendIdx[i]], iPosition).xyz, 1.0) * blendWgt[i];" << std::endl;
				ss << "		norm += mul((float3x3)worldMatrix3x4Array[blendIdx[i]], iNormal) * blendWgt[i];" << std::endl;
				ss << "	}" << std::endl;
			}
			else
			{
				///Instancing
				ss << "	float4x4 m03_offset;" << std::endl;
				switch (nWeights)
				{
				case 4: ss << "	m03_offset[3] = m03_offset3;" << std::endl;
				case 3: ss << "	m03_offset[2] = m03_offset2;" << std::endl;
				case 2: ss << "	m03_offset[1] = m03_offset1;" << std::endl;
				case 1: ss << "	m03_offset[0] = m03_offset0;" << std::endl;
				}

				ss << "	for (int iWeight=0; iWeight<" << nWeights << "; ++iWeight)" << std::endl;
				ss << "	{" << std::endl;
				ss << "		float3x4 boneMatrix;" << std::endl;
				ss << "		boneMatrix[0] = tex2D( matrixTexture, m03_offset[iWeight].xw + mOffset );" << std::endl;
				ss << "		boneMatrix[1] = tex2D( matrixTexture, m03_offset[iWeight].yw + mOffset );" << std::endl;
				ss << "		boneMatrix[2] = tex2D( matrixTexture, m03_offset[iWeight].zw + mOffset );" << std::endl;

				ss << "		blendPos += float4(mul( boneMatrix, iPosition ).xyz, 1.0f) * blendWgt[iWeight];" << std::endl;
				ss << "		norm += mul( (float3x3)(boneMatrix), iNormal ) * blendWgt[iWeight];" << std::endl;
				ss << "	}" << std::endl;

				ss << "	float3x4 worldCompMatrix;" << std::endl;
				ss << "	worldCompMatrix[0] = worldMatrix0;" << std::endl;
				ss << "	worldCompMatrix[1] = worldMatrix1;" << std::endl;
				ss << "	worldCompMatrix[2] = worldMatrix2;" << std::endl;

				ss << "	blendPos =  float4( mul( worldCompMatrix, blendPos ).xyz, 1.0f );" << std::endl;
				ss << "	norm = mul( (float3x3)(worldCompMatrix), norm );" << std::endl;
			}

			ss << "	oPosition = mul(cViewProj, blendPos);" << std::endl;
			ss << "	oNormal = normalize(norm);" << std::endl;
			ss << "	oNormal = mul(cView, float4(oNormal,0)).xyz;" << std::endl;
		}
		else
		{
			ss << "	oPosition = mul(cWorldViewProj, iPosition);" << std::endl;
			ss << "	oNormal = mul(cWorldView, float4(iNormal,0)).xyz;" << std::endl;
		}

		if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
		{
			ss << "	oTangent = mul(cWorldView, float4(iTangent,0)).xyz;" << std::endl;
			ss << "	oBiNormal = cross(oNormal, oTangent);" << std::endl;
		}

#ifdef WRITE_LINEAR_DEPTH
		ss << "	oViewPos = mul(cWorldView, iPosition).xyz;" << std::endl;
#else
		ss << "	oDepth = oPosition.w;" << std::endl;
#endif

		for (Ogre::uint32 i=0; i<numTexCoords; i++) {
			ss << "	oUV" << i << " = iUV" << i << ';' << std::endl;
		}

		ss << "}" << std::endl;

		Ogre::String programSource = ss.str();
		Ogre::String programName = mBaseName + "VP_" + Ogre::StringConverter::toString(permutation);

#if OGRE_DEBUG_MODE
		Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(programSource);
#endif

		// Create shader object
		Ogre::HighLevelGpuProgramPtr ptrProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
			programName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			"cg", Ogre::GPT_VERTEX_PROGRAM);
		ptrProgram->setSource(programSource);
		ptrProgram->setParameter("entry_point","ToGBufferVP");
		ptrProgram->setParameter("profiles","vs_3_0 arbvp1");

		const Ogre::GpuProgramParametersSharedPtr& params = ptrProgram->getDefaultParameters();
		params->setNamedAutoConstant("cWorldView", Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);

		if(permutation & GBufferMaterialGenerator::GBP_SKINNED)
		{
			ptrProgram->setSkeletalAnimationIncluded(true);
			params->setNamedAutoConstant("cViewProj",			Ogre::GpuProgramParameters::ACT_VIEWPROJ_MATRIX);
			params->setNamedAutoConstant("cView",				Ogre::GpuProgramParameters::ACT_VIEW_MATRIX);

			//VTF
			if(nWeights != 0)
			{
				ptrProgram->setVertexTextureFetchRequired(true);
			}
			else
			{
				params->setNamedAutoConstant("worldMatrix3x4Array", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX_ARRAY_3x4);
			}
		}
		else
		{
			params->setNamedAutoConstant("cWorldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
		}
		
		ptrProgram->load();

		return Ogre::GpuProgramPtr(ptrProgram);
	}

	Ogre::GpuProgramPtr GBufferMaterialGeneratorImpl::generateFragmentShader(MaterialGenerator::Perm permutation)
	{
		Ogre::StringStream ss;

		ss << "void ToGBufferFP(" << std::endl;
#ifdef WRITE_LINEAR_DEPTH
		ss << "	float3 iViewPos : TEXCOORD0," << std::endl;
#else
		ss << "	float1 iDepth : TEXCOORD0," << std::endl;
#endif
		ss << "	float3 iNormal   : TEXCOORD1," << std::endl;

		int texCoordNum = 2;
		if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP) 
		{
			ss << "	float3 iTangent : TEXCOORD" << texCoordNum++ << ',' << std::endl;
			ss << "	float3 iBiNormal : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}

		Ogre::uint32 numTexCoords = (permutation & GBufferMaterialGenerator::GBP_TEXCOORD_MASK) >> 8;
		for (Ogre::uint32 i=0; i<numTexCoords; i++) 
		{
			ss << "	float2 iUV" << i << " : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		}

		ss << std::endl;

		ss << "	out float4 oColor0 : COLOR0," << std::endl;
		ss << "	out float4 oColor1 : COLOR1," << std::endl;
		ss << " out float4 oColor2 : COLOR2," << std::endl;
		ss << " out float4 oColor3 : COLOR3," << std::endl;

		ss << std::endl;

		int samplerNum = 0;
		if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
		{
			ss << "	uniform sampler sNormalMap : register(s" << samplerNum++ << ")," << std::endl;
		}
		// specular map [2/24/2013 mavaL]
		if (permutation & GBufferMaterialGenerator::GBP_SPECULAR_MAP)
		{
			ss << "	uniform sampler sSpecMap : register(s" << samplerNum++ << ")," << std::endl;
		}
		// emissive map [3/3/2013 mavaL]
		if (permutation & GBufferMaterialGenerator::GBP_EMISSIVE_MAP)
		{
			ss << "	uniform sampler sEmisMap : register(s" << samplerNum++ << ")," << std::endl;
		}
		// decal map [5/14/2013 mavaL]
		if (permutation & GBufferMaterialGenerator::GBP_DECAL_MAP)
		{
			ss << "	uniform sampler sDecalMap : register(s" << samplerNum++ << ")," << std::endl;
		}

		Ogre::uint32 numTextures = permutation & GBufferMaterialGenerator::GBP_TEXTURE_MASK;
		for (Ogre::uint32 i=0; i<numTextures; i++) {
			ss << "	uniform sampler sTex" << i << " : register(s" << samplerNum++ << ")," << std::endl;
		}
		if (numTextures == 0 || permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR)
		{
			ss << "	uniform float4 cDiffuseColour," << std::endl;
		}

#ifdef WRITE_LINEAR_DEPTH
		ss << "	uniform float cFarDistance," << std::endl;
#endif

		if (permutation & GBufferMaterialGenerator::GBP_TEAM_COLOR_Mask)
		{
			ss << "	uniform float4 TeamColor," << std::endl;
		}

		ss << "	uniform float materialID" << std::endl;

		ss << "	)" << std::endl;


		ss << "{" << std::endl;

		if (numTexCoords > 0 && numTextures > 0) 
		{
			ss << "	oColor0 = tex2D(sTex0, iUV0);" << std::endl;
			if (permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR)
			{
				ss << "	oColor0.rgb *= cDiffuseColour.rgb;" << std::endl;
			}
		}
		else
		{
			ss << "	oColor0.rgb = cDiffuseColour.rgb;" << std::endl;
		}

		//队伍颜色
		if (permutation & GBufferMaterialGenerator::GBP_TEAM_COLOR_Mask)
			ss << "	oColor0 = oColor0 * oColor0.a + TeamColor * (1 - oColor0.a);" << std::endl;

		// decal map [5/14/2013 mavaL]
		if (permutation & GBufferMaterialGenerator::GBP_DECAL_MAP)
		{
			ss << "	float4 texDecal = tex2D(sDecalMap, iUV1);" << std::endl;
			ss << "	oColor0 += texDecal.a;" << std::endl;
		}

		ss << "	oColor0.a = materialID;" << std::endl;
		if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP) 
		{
			//SC2的法线贴图格式!
			ss << "	float4 texNormal = (tex2D(sNormalMap, iUV0)-0.5)*2;" << std::endl;
			ss << "	float3x3 normalRotation = float3x3(iTangent, iBiNormal, iNormal);" << std::endl;
			ss << "	oColor3.xy = texNormal.wy;" << std::endl;
			ss << "	oColor3.z = sqrt(max(0, 1 - dot(oColor3.xy, oColor3.xy)));" << std::endl;
			ss << "	oColor3.xyz = normalize(mul(oColor3.xyz, normalRotation));" << std::endl;
		} 
		else 
		{
			ss << "	oColor3.rgb = normalize(iNormal);" << std::endl;
		}
#ifdef WRITE_LINEAR_DEPTH
		ss << "	oColor3.a = length(iViewPos) / cFarDistance;" << std::endl;
#else
		ss << "	oColor3.a = iDepth;" << std::endl;
#endif

		// specular map [2/24/2013 mavaL]
		if (permutation & GBufferMaterialGenerator::GBP_SPECULAR_MAP)
		{
			ss << "	oColor2 = tex2D(sSpecMap, iUV0);" << std::endl;
		}
		else
		{
			ss << "	oColor2 = float4(0,0,0,0);" << std::endl;
		}

		// emissive map [3/3/2013 mavaL]
		if (permutation & GBufferMaterialGenerator::GBP_EMISSIVE_MAP)
		{
			ss << "	oColor1 = tex2D(sEmisMap, iUV0);" << std::endl;
		}
		else
		{
			ss << "	oColor1 = float4(0,0,0,0);" << std::endl;
		}

		ss << "}" << std::endl;

		Ogre::String programSource = ss.str();
		Ogre::String programName = mBaseName + "FP_" + Ogre::StringConverter::toString(permutation);

#if OGRE_DEBUG_MODE
		Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(programSource);
#endif

		// Create shader object
		Ogre::HighLevelGpuProgramPtr ptrProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
			programName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			"cg", Ogre::GPT_FRAGMENT_PROGRAM);
		ptrProgram->setSource(programSource);
		ptrProgram->setParameter("entry_point","ToGBufferFP");
		ptrProgram->setParameter("profiles","ps_2_0 arbfp1");

		const Ogre::GpuProgramParametersSharedPtr& params = ptrProgram->getDefaultParameters();

		// material id [2/24/2013 mavaL]
		int materiaiID = 0;

		if(permutation & GBufferMaterialGenerator::GBP_SPECULAR_MAP)
			materiaiID |= 1<<0;

		if(permutation & GBufferMaterialGenerator::GBP_EMISSIVE_MAP)
			materiaiID |= 1<<2;

		//必须转换到[0,1]
		params->setNamedConstant("materialID", materiaiID / 256.0f);

		if (numTextures == 0 || permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR)
		{
			params->setNamedAutoConstant("cDiffuseColour", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
		}

#ifdef WRITE_LINEAR_DEPTH
		//TODO : Should this be the distance to the far corner, not the far clip distance?
		params->setNamedAutoConstant("cFarDistance", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
#endif

		if (permutation & GBufferMaterialGenerator::GBP_TEAM_COLOR_RED)
			params->setNamedConstant("TeamColor", COLOR(180.0f/255, 20.0f/255, 30.0f/255));
		else if (permutation & GBufferMaterialGenerator::GBP_TEAM_COLOR_Blue)
			params->setNamedConstant("TeamColor", COLOR::Blue);

		ptrProgram->load();
		return Ogre::GpuProgramPtr(ptrProgram);
	}

	Ogre::MaterialPtr GBufferMaterialGeneratorImpl::generateTemplateMaterial(MaterialGenerator::Perm permutation)
	{
		Ogre::String matName = mBaseName + "Mat_" + Ogre::StringConverter::toString(permutation);

		Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().create
			(matName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::Pass* pass = matPtr->getTechnique(0)->getPass(0);
		pass->setName(mBaseName + "Pass_" + Ogre::StringConverter::toString(permutation));
		pass->setLightingEnabled(false);

		if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
			pass->createTextureUnitState();
		if (permutation & GBufferMaterialGenerator::GBP_SPECULAR_MAP)
			pass->createTextureUnitState();
		if (permutation & GBufferMaterialGenerator::GBP_EMISSIVE_MAP)
			pass->createTextureUnitState();
		if (permutation & GBufferMaterialGenerator::GBP_DECAL_MAP)
			pass->createTextureUnitState();

		Ogre::uint32 numTextures = permutation & GBufferMaterialGenerator::GBP_TEXTURE_MASK;
		for (Ogre::uint32 i=0; i<numTextures; i++)
		{
			pass->createTextureUnitState();
		}

		return matPtr;
	}
}

