#define FXAA_PC 1
#define FXAA_HLSL_3 1
#define FXAA_QUALITY__PRESET 39
//a通道保存luma值那个方法没效果...
#define FXAA_GREEN_AS_LUMA 1

#include "Fxaa3_11.h"

// Set values to calculate the amount of Anti Aliasing applied
float fxaaQualitySubpix = 0.60f;				// Default: 0.75 Raise to increase amount of blur
float fxaaQualityEdgeThreshold = 0.166f;		// Lower the value for more smoothing
float fxaaQualityEdgeThresholdMin = 0.0833f;	// Lower the value for more smoothing

float4 main
(	float2 uv : TEXCOORD0,
    uniform sampler2D sceneTex : TEXUNIT0,
	uniform float invTexWidth,
	uniform float invTexHeight
): COLOR0
{
	float4 c0 = FxaaPixelShader(
		uv, //pos
		0, //fxaaConsolePosPos (?)
		sceneTex, //tex
		sceneTex, //fxaaConsole360TexExpBiasNegOne
		sceneTex, //fxaaConsole360TexExpBiasNegTwo
		float2(invTexWidth, invTexHeight), //fxaaQualityRcpFrame
		float4(-0.5*invTexWidth,-0.5*invTexHeight,0.5*invTexWidth,0.5*invTexHeight), //fxaaConsoleRcpFrameOpt
		float4(-2.0*invTexWidth,-2.0*invTexHeight,2.0*invTexWidth,2.0*invTexHeight), //fxaaConsoleRcpFrameOpt2
		float4(8.0*invTexWidth,8.0*invTexHeight,-4.0*invTexWidth,-4.0*invTexHeight), //fxaaConsole360RcpFrameOpt2
		fxaaQualitySubpix,
		fxaaQualityEdgeThreshold,
		fxaaQualityEdgeThresholdMin,
		8.0, //fxaaConsoleEdgeSharpness
		0.125, //fxaaConsoleEdgeThreshold
		0.05, //fxaaConsoleEdgeThresholdMin
		0 //fxaaConsole360ConstDir
	);
	c0.w = 1;
    return saturate(c0);
}