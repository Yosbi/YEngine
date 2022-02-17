// Y-Engine constants
uniform extern float4x4 gWVP;			// World-view-projection combo matrix
uniform extern float4   gDiffuseMtrl;	// Diffuse material color
uniform extern float4   gAmbientMtrl;	// Ambient material color
uniform extern float4   gAmbientLight;  // General ambient light

// App-constants
uniform extern float4x4 gWorldInverseTranspose;
uniform extern float4   gDiffuseLight;
uniform extern float3   gLightVecW;
 
struct OutputVS
{
    float4 posH  : POSITION0;
    float4 color : COLOR0;
};

OutputVS AmbientDiffuseVS(float3 posL : POSITION0, float3 normalL : NORMAL0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform normal to world space.
	float3 normalW = mul(float4(normalL, 0.0f), gWorldInverseTranspose).xyz;
	normalW = normalize(normalW);
	
	// Compute the color
	// First diffuse color
	float s = max(dot(gLightVecW, normalW), 0.0f);
	float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;

	// Them ambient color
	float3 ambient = gAmbientMtrl*gAmbientLight;

	// Finally the final color
	outVS.color.rgb = ambient + diffuse;
	outVS.color.a   = gDiffuseMtrl.a;
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Done--return the output.
    return outVS;
}

float4 AmbientDiffusePS(float4 c : COLOR0) : COLOR
{
    return c;
}

technique AmbientDiffuseTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 AmbientDiffuseVS();
        pixelShader  = compile ps_2_0 AmbientDiffusePS();
		//FillMode = Wireframe;
    }
}
