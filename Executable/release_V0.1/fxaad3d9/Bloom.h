/*------------------------------------------------------------------------------
						BLOOM
------------------------------------------------------------------------------*/

float4 Bloom( float4 ColorInput2,float2 Tex  ) 
{
	float4 BlurColor2 =  0.0f;
	float4 BKThreshold = 0.0f;
	float NRGSamples = 1.0f;
	NRGSamples = NUM_SAMPLES2 /2;
	float	MaxSamples = (NUM_SAMPLES2+1)*(NUM_SAMPLES2+1);
	float 	MaxDistance = sqrt(NRGSamples*NRGSamples*2*Samplescaler);
	float   CurDistance = MaxDistance;
	float4	Blurtemp= 0;
	for(int Samplex = (- NRGSamples); Samplex < NRGSamples+1; Samplex = Samplex + 1)
	{
		for(int Sampley = (- NRGSamples); Sampley < NRGSamples+1; Sampley = Sampley + 1)
		{
			CurDistance = sqrt ( ((Samplex * Samplex) + (Sampley*Sampley))*Samplescaler);
			Blurtemp.rgb = tex2D(s0, float2(Tex.x +(Tex.x*Samplex*px*Samplescaler),Tex.y +(Tex.y*Sampley*py*Samplescaler)));
			BlurColor2.rgb += lerp(Blurtemp.rgb,ColorInput2.rgb, 1 - ((MaxDistance - CurDistance)/MaxDistance));
		}
	}
	BlurColor2.rgb = (BlurColor2.rgb / MaxSamples);
	float Bloomamount = saturate (    (dot(ColorInput2.xyz,float3(0.299, 0.587, 0.114))- MinBloom ) / MinBloom                 );
	float4	BlurColor = BlurColor2 * BloomScale;
	BlurColor2.rgb =lerp(ColorInput2,BlurColor, Bloomamount).rgb ;	
	BlurColor2.a = ColorInput2.a;
	return  BlurColor2;
}