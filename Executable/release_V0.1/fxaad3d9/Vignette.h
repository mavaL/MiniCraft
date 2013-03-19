/*------------------------------------------------------------------------------
						VIGNETTE
------------------------------------------------------------------------------*/

float4 VignettePass( float4 colorInput, float2 tex )
{
	float4 vignette = colorInput;
	float2 tc = tex - VignetteCenter;
	float v = length(tc) / VignetteRadius;
	vignette += pow(v, 4) * VignetteAmount;
	return vignette;
}