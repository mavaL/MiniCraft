/*------------------------------------------------------------------------------
						FINAL_LIMITER
------------------------------------------------------------------------------*/

// This has always to be the last pass, and can be used to limit the range of the final output 
float4 MinColor= float4(0.0f,0.0f,0.0f,0.0f);
float4 MaxColor= float4(1.0f,1.0f,1.0f,1.0f);
float4 limiter (float4 orig,float2 tex)  // this has always to be the last pass
{   
    orig = (LimiterStrenght*orig*(MaxColor-MinColor)+MinColor);
    return orig;
}