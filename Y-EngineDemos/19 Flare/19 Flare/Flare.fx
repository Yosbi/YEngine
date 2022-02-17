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
uniform extern float gTexOffset0;
uniform extern float gTexOffset1;

sampler Flare0 = sampler_state
{
	Texture = <gTex1>;
	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};
 
sampler Flare1 = sampler_state
{
	Texture = <gTex2>;
	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};
 
struct OutputVS
{
    float4 posH : POSITION0;
    float2 tex0 : TEXCOORD0;
    float2 tex1 : TEXCOORD1;
};

OutputVS CloudsVS(float3 posL : POSITION0, float2 tex0: TEXCOORD0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Translate texture coords
	float u = tex0.x;
	float v = tex0.y;

	// Translate coords to the center 
	u = u - 0.5f;
	v = (-v + 0.5);

	// Rotate coords
	float u0 = u * cos(gTexOffset0) - v * sin(gTexOffset0);
	float v0 = u * sin(gTexOffset0) + v * cos(gTexOffset0);
	float u1 = u * cos(gTexOffset1) - v * sin(gTexOffset1);
	float v1 = u * sin(gTexOffset1) + v * cos(gTexOffset1);

	// Set it back to texture coords
	outVS.tex0 = float2 (u0 + 0.5, -(v0 - 0.5)); 
	outVS.tex1 = float2 (u1 + 0.5, -(v1 - 0.5)); 

    return outVS;
}

float4 CloudsPS(float4 c : COLOR0, float2 tex0 : TEXCOORD0, float2 tex1 : TEXCOORD1) : COLOR
{
	float3 c0 = tex2D(Flare0, tex0).rgb;
	float3 c1 = tex2D(Flare1, tex1).rgb;


    return float4(c0 * c1 , 1.0f);
}


technique CloudsTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 CloudsVS();
        pixelShader  = compile ps_2_0 CloudsPS();
		CullMode = NONE;
    }
}
