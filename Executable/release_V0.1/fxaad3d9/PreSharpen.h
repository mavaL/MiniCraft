/*------------------------------------------------------------------------------
						PRE_SHARPEN
------------------------------------------------------------------------------*/

float4 SharpenPass( float2 tex ) 
{
	// Recover the original pixels
	float4 ori = tex2D(s0, tex);
	// Gaussian filter
	//   [ 1, 2 , 1 ]
	//   [ 2, 4 , 2 ]
	//   [ 1, 2 , 1 ]
	float4 c1 = tex2D(s0, tex + float2(-dx,-dy));
	float4 c2 = tex2D(s0, tex + float2(0,-dy));
	float4 c3 = tex2D(s0, tex + float2(dx,-dy));
	float4 c4 = tex2D(s0, tex + float2(-dx,0));
	float4 c5 = tex2D(s0, tex + float2(dx,0));
	float4 c6 = tex2D(s0, tex + float2(-dx,dy));
	float4 c7 = tex2D(s0, tex + float2(0,dy));
	float4 c8 = tex2D(s0, tex + float2(dx,dy));
	//Normalize the values. Formula: 1 / (1+2+1+2+4+2+1+2+1) = 1 / 16 = .0625
	float4 blur = (c1+c3+c6+c8 + 2*(c2+c4+c5+c7)+ 4*ori)*0.0625;
	//Subtracting the blurred image from the original image
	float4 cori = CoefOri*ori - CoefBlur*blur;
	//For higher precision in the calculation of contour, requires slightly more processing power
	//   [ c1, c2 , c3 ]
	//   [ c4, ori , c5 ]
	//   [ c6, c7 , c8 ]
	/*
	c1 = tex2D(s0, tex + float2(-px,-py));
	c2 = tex2D(s0, tex + float2(0,-py));
	c3 = tex2D(s0, tex + float2(px,-py));
	c4 = tex2D(s0, tex + float2(-px,0));
	c5 = tex2D(s0, tex + float2(px,0));
	c6 = tex2D(s0, tex + float2(-px,py));
	c7 = tex2D(s0, tex + float2(0,py));
	c8 = tex2D(s0, tex + float2(px,py));
	*/
	// Horizontal gradient
	//   [ -1, 0 ,1 ]
	//   [ -2, 0, 2 ]
	//   [ -1, 0 ,1 ]
	float delta1 = (c3 + 2*c5 + c8)-(c1 + 2*c4 + c6);
	// Vertical gradient
	//   [ -1,- 2,-1 ]
	//   [  0,  0, 0 ]
	//   [  1,  2, 1 ]
	float delta2 = (c6 + 2*c7 + c8)-(c1 + 2*c2 + c3);
	// Calculate and sharpen the blurry edges
	if (sqrt( mul(delta1,delta1) + mul(delta2,delta2)) > SharpenEdge) 
		{
			// Contour sharpening
			return ori*Sharpen_val0 - (c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 ) * Sharpen_val1;
		}
	else 
		{
			// Return corrected image
			return cori;
		}
}