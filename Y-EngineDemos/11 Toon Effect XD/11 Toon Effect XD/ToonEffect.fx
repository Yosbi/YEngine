// Y-Engine vars
uniform extern float4x4 gWVP;				// World view proj combo matrix
uniform extern float3   gEyePosW;			// Camera position
uniform extern float4	gAmbientMtrl;		// Material ambient color
uniform extern float4	gDiffuseMtrl;		// Material diffuse color
uniform extern float4	gSpecularMtrl;		// Material specular color	
uniform extern float	gSpecularPowerMtrl; // Material specular power
uniform extern float4	gAmbientLight;		// Ambient light color

// App-constants
uniform extern float4x4 gWorldInverseTranspose;
uniform extern float4x4 gWorld;
uniform extern float4   gDiffuseLight;
uniform extern float4   gSpecularLight;
uniform extern float3   gLightVecW;
 
struct OutputVS
{
    float4 posH		: POSITION0;
	float  s		: TEXCOORD0;
};

OutputVS ToonEffectVS(float3 posL : POSITION0, float3 normalL : NORMAL0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform normal to world space.
	float3 normalW = mul(float4(normalL, 0.0f), gWorldInverseTranspose).xyz;
	normalW = normalize(normalW);

	outVS.s = max(dot(gLightVecW, normalW), 0.0f);
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Done--return the output.
    return outVS;
}

float DiscreteFunction(float s)
{
	if (s <= 0.25f)
		return 0.4f;
	else if (s <= 0.85f)
		return 0.6f;
	else 
		return 1.0f;
}

float4 ToonEffectPS(float s : TEXCOORD0) : COLOR
{
	// Compute the quantity of diffuse color to make the toon effect
	s = DiscreteFunction(s);

	//Compute the color
	//=====================================================
	// Toon like Diffuse color	
	float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;

	// Them ambient color
	float3 ambient = gAmbientMtrl*gAmbientLight;

	// Finally the final color
	return float4((ambient + diffuse), gDiffuseMtrl.a);
	
	//====================================================
    
}

technique ToonEffectTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 ToonEffectVS();
        pixelShader  = compile ps_2_0 ToonEffectPS();
		//FillMode = Wireframe;
    }
}
