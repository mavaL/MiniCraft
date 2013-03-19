/*======================================================================================
                             "USER" ADJUSTABLE SETTINGS
======================================================================================*/

// TODO: Insert FXAA SHADER

/*------------------------------------------------------------------------------
						FILTER SELECTION
------------------------------------------------------------------------------*/
// Comment to deactivate an effect. 
// Example: To disable the tonemap effect, use // in front of #define USE_TONEMAP
#define USE_PRE_SHARPEN
//#define USE_TONEMAP
//#define USE_VIGNETTE
//#define USE_BLOOM
#define USE_POST_SHARPEN
//#define USE_FINAL_LIMITER


/*------------------------------------------------------------------------------
						FXAA SHADER
------------------------------------------------------------------------------*/
// Set values to calculate the amount of Anti Aliasing applied
int fxaaQualitySubpix = 0.60;  // Default: 0.75 Raise to increase amount of blur
int fxaaQualityEdgeThreshold = 0.166; // Lower the value for more smoothing
int fxaaQualityEdgeThresholdMin = 0.0833; // Lower the value for more smoothing

/*------------------------------------------------------------------------------
						PRE_SHARPEN
------------------------------------------------------------------------------*/
// Set values to calculate the amount of  AA produced blur to consider for the sharpening pass
#define Average 0.4
#define CoefBlur 2

// Set values of the sharpening amount
#define SharpenEdge 0.2
#define Sharpen_val0 1.2

/*------------------------------------------------------------------------------
						TONEMAP
------------------------------------------------------------------------------*/
#define Gamma 1.00
#define Opacity 0.02 // Bleach bypass, higher = stronger effect
/*
#define Defog 0.000 // Strength of Lens Colors.
#define FogColor float4(0.0, 0.0, 0.0, 0.0) //Lens-style color filters for Blue, Red, Yellow, White.
#define Exposure 0.000
#define BlueShift 0.10 // Higher = more blue in image.
#define saturation 0.02 // use negative values for less saturation. 

/*------------------------------------------------------------------------------
						VIGNETTE
------------------------------------------------------------------------------*/
// Vignette effect, process by which there is loss in clarity towards the corners and sides of the image, like a picture frame
#define VignetteCenter float2(0.500, 0.500) // Center of screen for effect.
#define VignetteRadius 0.60 // lower values = stronger radial effect from center
#define VignetteAmount -0.20 // Strength of black. -2.00 = Max Black, 1.00 = Max White.

/*------------------------------------------------------------------------------
						BLOOM
------------------------------------------------------------------------------*/
// Number of samples per pixel taken for the Bloom effect. Don't set it to high!  4 = 25spp, 8 = 81spp, 16 = 289spp
#define NUM_SAMPLES2 2 // Must be set with a value dividable by 2
float MinBloom = 0.60f; // The min. level at which the effect starts
float Samplescaler = 0.7f;	// Sets the width of the effect
float BloomScale = 0.80f; // The power of the effect

/*------------------------------------------------------------------------------
						POST_SHARPEN
------------------------------------------------------------------------------*/
// Controls additional sharpening applied after previous processing. Strength should be max 0.25!
float sharpen = 0.12;

/*------------------------------------------------------------------------------
						FINAL_LIMITER
------------------------------------------------------------------------------*/
// Controls the strenght of the limiter. 1.000 for default setting
int LimiterStrenght = 1.000; 