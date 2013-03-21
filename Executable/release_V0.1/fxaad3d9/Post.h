/*======================================================================================
                             POST FXAA RENDERING PASSES
======================================================================================*/

#define USE_ADDITIONAL_SHADER 1
#define s0 screenSampler
#define width BUFFER_WIDTH
#define height BUFFER_HEIGHT
#define px BUFFER_RCP_WIDTH
#define py BUFFER_RCP_HEIGHT
#define dx (Average*px)
#define dy (Average*py)
#define CoefOri (1+ CoefBlur)
#define Sharpen_val1 ((Sharpen_val0-1) / 8.0)

/*------------------------------------------------------------------------------
						FILTER TO USE CHECK
------------------------------------------------------------------------------*/

#ifdef USE_PRE_SHARPEN
#include "fxaad3d9\PreSharpen.h"
#endif
#ifdef USE_TONEMAP
#include "fxaad3d9\Tonemap.h"
#endif
#ifdef USE_VIGNETTE
#include "fxaad3d9\Vignette.h"
#endif
#ifdef USE_BLOOM
#include "fxaad3d9\Bloom.h"
#endif
#ifdef USE_POST_SHARPEN
#include "fxaad3d9\PostSharpen.h"
#endif
#ifdef USE_FINAL_LIMITER
#include "fxaad3d9\FinalLimiter.h"
#endif


/*------------------------------------------------------------------------------
						RENDERING PASSES
------------------------------------------------------------------------------*/

float4 main( float2 tex)
{
	// PreSharpenPass (has to be the first pass because it samples multiple texels)
	#ifdef USE_PRE_SHARPEN
		float4 pass1 = SharpenPass(tex);
	#else
		float4 pass1 = tex2D(s0,tex);
	#endif
	// TonemapPass
	#ifdef USE_TONEMAP
		float4 pass2 = TonemapPass( pass1, tex);
	#else
		float4 pass2 = pass1;
	#endif
	//VignettePass
	#ifdef USE_VIGNETTE
		float4 pass3 = VignettePass( pass2, tex);
	#else
		float4 pass3 = pass2;
	#endif
	// BloomPass
	#ifdef USE_BLOOM
		float4 pass4 = Bloom (pass3,tex);
	#else
		float4 pass4 = pass3;
	#endif
	// PostSharpenPass
	#ifdef USE_POST_SHARPEN
		float4 pass5 = sharp (pass4,tex);
	#else
		float4 pass5 = pass4;
	#endif
	// FinalLimiterPass (always last)
	#ifdef USE_FINAL_LIMITER
		float4 pass6 = limiter (pass5,tex);
	#else
		float4 pass6 = pass5;
	#endif
	// ReturnFinalColor
	return pass6;
}