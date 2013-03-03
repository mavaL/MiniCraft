#ifndef __TerrainMaterialGeneratorD_H__
#define __TerrainMaterialGeneratorD_H__
 
#include "Terrain/OgreTerrainPrerequisites.h"
#include "Terrain/OgreTerrainMaterialGenerator.h"
#include "OgreGpuProgramParams.h"
 
namespace Ogre
{
	class PSSMShadowCameraSetup;
 
	/** \addtogroup Optional Components
	*  @{
	*/
	/** \addtogroup Terrain
	*  Some details on the terrain component
	*  @{
	*/
 
	/** A TerrainMaterialGenerator which can cope with normal mapped, specular mapped
		terrain. 
		@note Requires the Cg plugin to render correctly
	*/
	class TerrainMaterialGeneratorD : public TerrainMaterialGenerator
	{
	public:
		TerrainMaterialGeneratorD();
		~TerrainMaterialGeneratorD();
 
		/** Shader model 2 profile target. 
		*/
		class SM2Profile : public TerrainMaterialGenerator::Profile
		{
		public:
			SM2Profile(TerrainMaterialGenerator* parent, const String& name, const String& desc);
			~SM2Profile();
			MaterialPtr generate(const Terrain* terrain);
			MaterialPtr generateForCompositeMap(const Terrain* terrain) { return MaterialPtr(); }
			uint8 getMaxLayers(const Terrain* terrain) const;
			void updateParams(const MaterialPtr& mat, const Terrain* terrain);
			void updateParamsForCompositeMap(const MaterialPtr& mat, const Terrain* terrain);
			void requestOptions(Terrain* terrain);
			bool isVertexCompressionSupported() const;
 
			/** Whether to support a global colour map over the terrain in the shader,
				if it's present (default true). 
			*/
			bool isGlobalColourMapEnabled() const  { return mGlobalColourMapEnabled; }
			/** Whether to support a global colour map over the terrain in the shader,
			if it's present (default true). 
			*/
			void setGlobalColourMapEnabled(bool enabled);
			/** Whether to support a light map over the terrain in the shader,
			if it's present (default true). 
			*/
			bool isLightmapEnabled() const  { return mLightmapEnabled; }
			/** Whether to support a light map over the terrain in the shader,
			if it's present (default true). 
			*/
			void setLightmapEnabled(bool enabled);
			/** Whether to use the composite map to provide a lower LOD technique
				in the distance (default true). 
			*/
			bool isCompositeMapEnabled() const  { return mCompositeMapEnabled; }
			/** Whether to use the composite map to provide a lower LOD technique
			in the distance (default true). 
			*/
			void setCompositeMapEnabled(bool enabled);
 
			/// Internal
			bool _isSM3Available() const { return mSM3Available; }
			bool _isSM4Available() const { return mSM4Available; }
 
		protected:
 
			enum TechniqueType
			{
				HIGH_LOD, 
				LOW_LOD, 
				RENDER_COMPOSITE_MAP
			};
			void addTechnique(const MaterialPtr& mat, const Terrain* terrain, TechniqueType tt);
 
			/// Interface definition for helper class to generate shaders
			class ShaderHelper : public TerrainAlloc
			{
			public:
				ShaderHelper() {}
				virtual ~ShaderHelper() {}
				virtual HighLevelGpuProgramPtr generateVertexProgram(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt);
				virtual HighLevelGpuProgramPtr generateFragmentProgram(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt);
				virtual void updateParams(const SM2Profile* prof, const MaterialPtr& mat, const Terrain* terrain, bool compositeMap);
			protected:
				virtual String getVertexProgramName(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt);
				virtual String getFragmentProgramName(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt);
				virtual HighLevelGpuProgramPtr createVertexProgram(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt) = 0;
				virtual HighLevelGpuProgramPtr createFragmentProgram(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt) = 0;
				virtual void generateVertexProgramSource(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream);
				virtual void generateFragmentProgramSource(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream);
				virtual void generateVpHeader(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream) = 0;
				virtual void generateFpHeader(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream) = 0;
				virtual void generateVpLayer(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, uint layer, StringUtil::StrStreamType& outStream) = 0;
				virtual void generateFpLayer(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, uint layer, StringUtil::StrStreamType& outStream) = 0;
				virtual void generateVpFooter(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream) = 0;
				virtual void generateFpFooter(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream) = 0;
				virtual void defaultVpParams(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, const HighLevelGpuProgramPtr& prog);
				virtual void defaultFpParams(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, const HighLevelGpuProgramPtr& prog);
				virtual void updateVpParams(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, const GpuProgramParametersSharedPtr& params);
				virtual void updateFpParams(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, const GpuProgramParametersSharedPtr& params);
				static String getChannel(uint idx);
 
			};
 
			/// Utility class to help with generating shaders for Cg / HLSL.
			class ShaderHelperCg : public ShaderHelper
			{
			protected:
				HighLevelGpuProgramPtr createVertexProgram(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt);
				HighLevelGpuProgramPtr createFragmentProgram(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt);
				void generateVpHeader(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream);
				void generateFpHeader(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream);
				void generateVpLayer(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, uint layer, StringUtil::StrStreamType& outStream);
				void generateFpLayer(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, uint layer, StringUtil::StrStreamType& outStream);
				void generateVpFooter(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream);
				void generateFpFooter(const SM2Profile* prof, const Terrain* terrain, TechniqueType tt, StringUtil::StrStreamType& outStream);
			};
 
			ShaderHelper* mShaderGen;
			bool mGlobalColourMapEnabled;
			bool mLightmapEnabled;
			bool mCompositeMapEnabled;
			bool mSM3Available;
			bool mSM4Available;
		};
 
 
 
 
	};
 
 
 
	/** @} */
	/** @} */
 
 
}
 
#endif