// Y-Engine constants
uniform extern float4x4 gWVP;				// World-view-projection combo matrix
uniform extern float4   gDiffuseMtrl;		// Diffuse material color
uniform extern float4   gAmbientMtrl;		// Ambient material color
uniform extern float4   gSpecularMtrl;		// Specular material color
uniform extern float	gSpecularPowerMtrl; // Specular material power
uniform extern float4   gAmbientLight;		// General ambient light
uniform extern float3   gEyePosW;			// Position of the camera

// App-constants
uniform extern float4x4 gWorldInverseTranspose;
uniform extern float4x4 gWorld;
uniform extern float4   gDiffuseLight;
uniform extern float4   gSpecularLight;
uniform extern float3   gLightVecW;
 
struct OutputVS
{
    float4 posH		: POSITION0;
    float3 normalH  : TEXCOORD0;
	float3 posW		:TEXCOORD1;
};

OutputVS AmbDiffSpecVS(float3 posL : POSITION0, float3 normalL : NORMAL0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform normal to world space.
	float3 normalW = mul(float4(normalL, 0.0f), gWorldInverseTranspose).xyz;
	outVS.normalH = normalize(normalW);

	// Transform vertex position to world space
	outVS.posW = mul(float4(posL, 1.0f), gWorld).xyz;
	
	// Compute the color
	//=====================================================
	//// Specular color
	//// compute the vector from the vertex to the eye position
	//float3 toEye = normalize(gEyePosW - posW);
	//
	//// Compute the reflection vector
	//float3 r = reflect(-gLightVecW, normalW);
//
	//// Determine how much(if any) specular light makes into the eye
	//float t = pow(max(dot(r, toEye), 0.0f), gSpecularPowerMtrl);
//
	//float3 spec = t * (gSpecularMtrl * gSpecularLight).rgb;
//
	//// Diffuse color
	//float s = max(dot(gLightVecW, normalW), 0.0f);
	//float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;
//
	//// Them ambient color
	//float3 ambient = gAmbientMtrl*gAmbientLight;
//
	//// Finally the final color
	//outVS.color.rgb = ambient + diffuse + spec;
	//outVS.color.a   = gDiffuseMtrl.a;
	//====================================================
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Done--return the output.
    return outVS;
}

float4 AmbDiffSpecPS(float3 normalW : TEXCOORD0, float3 posW : TEXCOORD1) : COLOR
{
	// Interpolated normals can become unnormal so normalize
	normalW = normalize(normalW);

	//Compute the color
	//=====================================================
	// Specular color
	// compute the vector from the vertex to the eye position
	float3 toEye = normalize(gEyePosW - posW);
	
	// Compute the reflection vector
	float3 r = reflect(-gLightVecW, normalW);

	// Determine how much(if any) specular light makes into the eye
	float t = pow(max(dot(r, toEye), 0.0f), gSpecularPowerMtrl);

	float3 spec = t * (gSpecularMtrl * gSpecularLight).rgb;

	// Diffuse color
	float s = max(dot(gLightVecW, normalW), 0.0f);
	float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;

	// Them ambient color
	float3 ambient = gAmbientMtrl*gAmbientLight;

	// Finally the final color
	return float4((ambient + diffuse + spec), gDiffuseMtrl.a);
	
	//====================================================
    
}

technique AmbDiffSpecTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 AmbDiffSpecVS();
        pixelShader  = compile ps_2_0 AmbDiffSpecPS();
		//FillMode = Wireframe;
    }
}