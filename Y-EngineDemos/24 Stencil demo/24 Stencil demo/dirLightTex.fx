// Y-Engine variables
uniform extern float4x4  gWVP;				 // World view proj combo matrix
uniform extern float3    gEyePosW;			 // Camera position
uniform extern float4	 gAmbientMtrl;		 // Material ambient color
uniform extern float4	 gDiffuseMtrl;		 // Material diffuse color
uniform extern float4    gEmissiveMtrl;		 // Material emissive color
uniform extern float4	 gSpecularMtrl;		 // Material specular color
uniform extern float	 gSpecularPowerMtrl; // Material specular power
uniform extern float4	 gAmbientLight;		 // Ambient light color
uniform extern texture	 gTex1;				 // Texture 1
uniform extern texture	 gTex2;				 // Texture 2
uniform extern texture	 gTex3;				 // Texture 3
uniform extern texture	 gTex4;				 // Texture 4
uniform extern texture	 gTex5;				 // Texture 5
uniform extern texture	 gTex6;				 // Texture 6
uniform extern texture	 gTex7;				 // Texture 7
uniform extern texture	 gTex8;				 // Texture 8


// App Vars
uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldInverseTranspose;
uniform extern float4   gDiffuseLight;
uniform extern float4   gSpecularLight;
uniform extern float3   gLightVecW;

sampler TexS = sampler_state
{
	Texture = <gTex1>;
	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = WRAP;
    AddressV  = WRAP;
};
 
struct OutputVS
{
    float4 posH    : POSITION0;
    float2 tex0    : TEXCOORD0;
	float3 posW	   : TEXCOORD1;
	float3 normalW : TEXCOORD2;
};

OutputVS DirLightTexVS(float3 posL : POSITION0, float3 normalL : NORMAL0, float2 tex0: TEXCOORD0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform normal to world space.
	float3 normalW = mul(float4(normalL, 0.0f), gWorldInverseTranspose).xyz;
	outVS.normalW = normalize(normalW);
	
	// Transform vertex position to world space.
	outVS.posW  = mul(float4(posL, 1.0f), gWorld).xyz;	
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Pass on texture coordinates to be interpolated in rasterization.
	outVS.tex0 = tex0;
	
	// Done--return the output.
    return outVS;
}

float4 DirLightTexPS(float2 tex0 : TEXCOORD0, float3 posW : TEXCOORD1, float3 normalW : TEXCOORD2) : COLOR
{
	normalW = normalize(normalW);
	// Spec light//////////////////////7
	// Compute the vector from the vertex to the eye position.
	float3 toEye = normalize(gEyePosW - posW);
	
	// Compute the reflection vector.
	float3 r = reflect(-gLightVecW, normalW);
	
	// Determine how much (if any) specular light makes it into the eye.
	float t  = pow(max(dot(r, toEye), 0.0f),  gSpecularPowerMtrl);
	
	// Diffuse light//////////////////
	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(gLightVecW, normalW), 0.0f);
	
	// Compute the ambient, diffuse and specular terms separatly. 
	float4 spec = float4(t*(gSpecularMtrl*gSpecularLight).rgb, 0.0f);
	float4 diffuse = s*(gDiffuseMtrl*gDiffuseLight);
	float4 ambient = gAmbientMtrl*gAmbientLight;
	
	// Sum all the terms together and copy over the diffuse alpha.
	diffuse.rgb = ambient.rgb + diffuse.rgb;
	diffuse.a   = gDiffuseMtrl.a;
	//=======================================================

	float4 texColor = tex2D(TexS, tex0); // sample texture
	diffuse.rgb = diffuse.rgb * texColor.rgb;
    return float4(diffuse.rgb + spec.rgb, texColor.a * diffuse.a); 
}

technique DirLightTexTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 DirLightTexVS();
        pixelShader  = compile ps_2_0 DirLightTexPS();

		//AlphaBlendEnable = true;
		//SrcBlend = SRCALPHA;
		//DestBlend = INVSRCALPHA;
		//BlendOp = ADD;
//
		//AlphaTestEnable = true;
		//AlphaFunc = GreaterEqual;
		//AlphaRef = 0;
		//CullMode = None;

    }
}
