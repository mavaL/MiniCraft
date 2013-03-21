/*------------------------------------------------------------------------------
						TONEMAP
------------------------------------------------------------------------------*/

float4 TonemapPass( float4 colorInput, float2 tex )
{
	float4 color = colorInput;
	/*
	color = max(0, color - Defog * FogColor);
	color *= pow(2.0f, Exposure);
	*/
	color = pow(color, Gamma);
	/*
	color = lerp(color, d, BlueShift);
	*/
	float4 d = color * float4(1.05f, 0.97f, 1.27f, 1.0f);
	float3 lumCoeff = float3(0.299, 0.587, 0.114); // Original float3 was (0.25, 0.65, 0.1), changed it to match the lumcalc of the Mainshader
	float lum = dot (lumCoeff, color.rgb);
	float3 blend = lum.rrr;
	float L = min(1, max (0, 10 * (lum - 0.45)));
	float3 result1 = 2.0f * color.rgb * blend;
	float3 result2 = 1.0f - 2.0f * (1.0f - blend) * (1.0f - color.rgb);
	float3 newColor = lerp(result1, result2, L);
	float A2 = Opacity * color.rgb;
	float3 mixRGB = A2 * newColor;
	color.rgb += ((1.0f - A2) * mixRGB);
	// Sepia Tones - float3(1.00, 1.00, 1.00) values are for R G B
	/* 
	float gray = dot(color.rgb, float4(0.3, 0.59, 0.11, 0)); 
	color.rgb = float4(color.rgb * float3(1.00, 1.00, 1.00) , 1);
	*/
	float4 middlegray = float(color.r + color.g + color.b) * 0.333;
	float4 diffcolor = color - middlegray;
	/*
	color += diffcolor *+ saturation;
	*/
	return color;
}