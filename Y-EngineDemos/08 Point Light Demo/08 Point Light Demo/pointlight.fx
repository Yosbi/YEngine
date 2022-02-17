// Y-Engine vars
uniform extern float4x4 gWVP;				// World view proj combo matrix
uniform extern float3   gEyePosW;			// Camera position
uniform extern float4	gAmbientMtrl;		// Material ambient color
uniform extern float4	gDiffuseMtrl;		// Material diffuse color
uniform extern float4	gSpecularMtrl;		// Material specular color	
uniform extern float	gSpecularPowerMtrl; // Material specular power
uniform extern float4	gAmbientLight;		// Ambient light color

// APP vars
uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldInverseTranspose;

uniform extern float4	gDiffuseLight;
uniform extern float4	gSpecularLight;
uniform extern float3	gLightPosW;  
uniform extern float3	gAttenuation012; 

struct OutputVS
{
    float4 posH		: POSITION0;
    float3 normalW  : TEXCOORD0;
	float3 posW		: TEXCOORD1;
};

OutputVS PointLightVS(float3 posL : POSITION0, float3 normalL : NORMAL0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform normal to world space.
	float3 normalW = mul(float4(normalL, 0.0f), gWorldInverseTranspose).xyz;
	outVS.normalW = normalize(normalW);
	
	// Transform vertex position to world space.
	outVS.posW  = mul(float4(posL, 1.0f), gWorld).xyz;
	
	 //Unit vector from vertex to light source.
	//float3 lightVecW = normalize(gLightPosW - posW);
//
	////---------------------------------------Light calc---------------------------------------------------
	//// Ambient Light Computation.
	//float3 ambient = (gAmbientMtrl*gAmbientLight).rgb;
	//
	//// Diffuse Light Computation.
	//float s = max(dot(normalW, lightVecW), 0.0f);
	//float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;
	//
	//// Specular Light Computation.
	//float3 toEyeW   = normalize(gEyePosW - posW);
	//float3 reflectW = reflect(-lightVecW, normalW);
	//float t = pow(max(dot(reflectW, toEyeW), 0.0f), gSpecularPowerMtrl);
	//float3 spec = t*(gSpecularMtrl*gSpecularLight).rgb;
	//
	//// Attentuation.
	//float d = distance(gLightPosW, posW);
	//float A = gAttenuation012.x + gAttenuation012.y*d + gAttenuation012.z*d*d;
	//
	//// Everything together.
	//float3 color = ambient + ((diffuse + spec) / A); 
	//
	//// Pass on color and diffuse material alpha.
	//outVS.color = float4(color, gDiffuseMtrl.a);
	////----------------------------------------------------------------------------------------------------------

	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Done--return the output.
    return outVS;
}

float4 PointLightPS(float3 normalW : TEXCOORD0, float3 posW : TEXCOORD1) : COLOR
{
	// normalize the normal 
	normalW = normalize(normalW);

    //Unit vector from vertex to light source.
	float3 lightVecW = normalize(gLightPosW - posW);

	//---------------------------------------Light calc---------------------------------------------------
	// Ambient Light Computation.
	float3 ambient = (gAmbientMtrl*gAmbientLight).rgb;
	
	// Diffuse Light Computation.
	float s = max(dot(normalW, lightVecW), 0.0f);
	float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;
	
	// Specular Light Computation.
	float3 toEyeW   = normalize(gEyePosW - posW);
	float3 reflectW = reflect(-lightVecW, normalW);
	float t = pow(max(dot(reflectW, toEyeW), 0.0f), gSpecularPowerMtrl);
	float3 spec = t*(gSpecularMtrl*gSpecularLight).rgb;
	
	// Attentuation.
	float d = distance(gLightPosW, posW);
	float A = gAttenuation012.x + gAttenuation012.y*d + gAttenuation012.z*d*d;
	
	// Everything together.
	return float4(( ambient + ((diffuse + spec) / A)),gDiffuseMtrl.a); 
	//--------------------------------------------------------------------------------------------------------
}

technique PointLightTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 PointLightVS();
        pixelShader  = compile ps_2_0 PointLightPS();
    }
}
