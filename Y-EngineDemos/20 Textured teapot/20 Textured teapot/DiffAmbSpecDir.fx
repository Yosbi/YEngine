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

// App-constants
uniform extern float4x4 gWorldInverseTranspose;
uniform extern float4x4 gWorld;
uniform extern float4   gDiffuseLight;
uniform extern float4   gSpecularLight;
uniform extern float3   gLightVecW;

sampler Tex1 = sampler_state
{
	Texture			= <gTex1>;
	MinFilter		= Anisotropic;
	MagFilter		= Anisotropic;
	MipFilter		= LINEAR;
	MaxAnisotropy	= 8;
	AddressU		= WRAP;
	AddressV		= WRAP;
};
 
struct OutputVS
{
    float4 posH		: POSITION0;
    float3 normalW  : TEXCOORD2;
	float3 posW		: TEXCOORD1;
	float2 tex1		: TEXCOORD0;
};

OutputVS AmbDiffSpecVS(float3 posL : POSITION0, float3 normalL : NORMAL0, float2 texCoord : TEXCOORD0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform normal to world space.
	float3 normalW = mul(float4(normalL, 0.0f), gWorldInverseTranspose).xyz;
	outVS.normalW = normalize(normalW);

	// Transform vertex position to world space
	outVS.posW = mul(float4(posL, 1.0f), gWorld).xyz;
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Set texture coord
	outVS.tex1 = texCoord;

	// Done--return the output.
    return outVS;
}

float4 AmbDiffSpecPS(float3 normalW : TEXCOORD2, float3 posW : TEXCOORD1, float2 texCoord : TEXCOORD0) : COLOR
{
	// Interpolated normals can become unnormal so normalize
	normalW = normalize(normalW);

	//Compute the color
	//=====================================================
	// Jim Blinn Specular color
	
	// compute the vector from the vertex to the eye position
	float3 toEye = normalize(gEyePosW - posW);
	
	// Compute halfway vector
	float3 h = normalize(gLightVecW + toEye);

	// Determine how much(if any) specular light makes into the eye
	float t = pow(max(dot(h, normalW), 0.0f), gSpecularPowerMtrl);

	float3 spec = t * (gSpecularMtrl * gSpecularLight).rgb;

	// Diffuse color
	float s = max(dot(gLightVecW, normalW), 0.0f);
	float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;

	// Them ambient color
	float3 ambient = gAmbientMtrl*gAmbientLight;

	// Sample texture color
	float3 cTex = tex2D(Tex1, texCoord).rgb;

	// Finally the final color
	return float4((((ambient + diffuse) * cTex)  + spec), gDiffuseMtrl.a);
	
	//====================================================
    
}

technique AmbDiffSpecTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 AmbDiffSpecVS();
        pixelShader  = compile ps_2_0 AmbDiffSpecPS();
		// FillMode = Wireframe;
    }
}
