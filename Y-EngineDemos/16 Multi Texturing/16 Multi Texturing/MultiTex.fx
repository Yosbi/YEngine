// Y-Engine variables
uniform extern float4x4  gWVP;				 // World view proj combo matrix
uniform extern float3    gEyePosW;			 // Camera position
uniform extern float4	 gAmbientMtrl;		 // Material ambient color
uniform extern float4	 gDiffuseMtrl;		 // Material diffuse color
uniform extern float4    gEmissiveMtrl;		 // Material emissive color
uniform extern float4	 gSpecularMtrl;		 // Material specular color
uniform extern float	 gSpecularPowerMtrl; // Material specular power
uniform extern float4	 gAmbientLight;		 // Ambient light color
uniform extern texture	 gTex1;				 // Texture 1	ground
uniform extern texture	 gTex2;				 // Texture 2	grass
uniform extern texture	 gTex3;				 // Texture 3	stone
uniform extern texture	 gTex4;				 // Texture 4	blendmap
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

sampler Tex1 = sampler_state
{
	Texture = <gTex1>;
	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler Tex2 = sampler_state
{
	Texture = <gTex2>;
	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler Tex3 = sampler_state
{
	Texture = <gTex3>;
	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler Tex4 = sampler_state
{
	Texture = <gTex4>;
	MinFilter = Anisotropic;
	MagFilter = Anisotropic;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU  = WRAP;
    AddressV  = WRAP;
};
 
struct OutputVS
{
    float4 posH			  : POSITION0;
    float4 diffuse		  : COLOR0;
    float4 spec			  : COLOR1;
    float2 tiledTex       : TEXCOORD0;
	float2 nontiledTex    : TEXCOORD1;
};

OutputVS MultiTexVS(float3 posL : POSITION0, float3 normalL : NORMAL0, float2 tex0: TEXCOORD0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Transform normal to world space.
	float3 normalW = mul(float4(normalL, 0.0f), gWorldInverseTranspose).xyz;
	normalW = normalize(normalW);
	
	// Transform vertex position to world space.
	float3 posW  = mul(float4(posL, 1.0f), gWorld).xyz;
	
	//=======================================================
	// Compute the color
	
	// Compute the vector from the vertex to the eye position.
	float3 toEye = normalize(gEyePosW - posW);
	
	// Compute the reflection vector.
	float3 r = reflect(-gLightVecW, normalW);
	
	// Determine how much (if any) specular light makes it into the eye.
	float t  = pow(max(dot(r, toEye), 0.0f),  gSpecularPowerMtrl);
	
	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(gLightVecW, normalW), 0.0f);
	
	// Compute the ambient, diffuse and specular terms separatly. 
	float3 spec = t*(gSpecularMtrl*gSpecularLight).rgb;
	float3 diffuse = s*(gDiffuseMtrl*gDiffuseLight).rgb;
	float3 ambient = gAmbientMtrl*gAmbientLight;
	
	// Sum all the terms together and copy over the diffuse alpha.
	outVS.diffuse.rgb = ambient + diffuse;
	outVS.diffuse.a   = gDiffuseMtrl.a;
	outVS.spec = float4(spec, 0.0f);
	//=======================================================
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	
	// Pass on texture coordinates to be interpolated in rasterization.
	outVS.tiledTex = tex0 * 16.0f; // scale tex coord
	outVS.nontiledTex = tex0;
	// Done--return the output.
    return outVS;
}

float4 MultiTexPS(float4 diffuse : COLOR0, float4 spec : COLOR1, float2 tiledTex : TEXCOORD0, float2 nontiledTex : TEXCOORD1) : COLOR
{
	// Sample the textures
	float3 c0 = tex2D(Tex1, tiledTex).rgb;
	float3 c1 = tex2D(Tex2, tiledTex).rgb;
	float3 c2 = tex2D(Tex3, tiledTex).rgb;

	// Sample the blend map
	float3 B = tex2D(Tex4, nontiledTex).rgb;

	// Find the inverse of all the blend weights to scaale
	// the total color to range [0,1]
	float totalInverse = 1.0f/ (B.r + B.g + B.b);

	// Scale thecolors by their corresponding layer
	c0 *= B.b * totalInverse;
	c1 *= B.r * totalInverse;
	c2 *= B.g * totalInverse;

	// Sum the colors and mod w diffuse/ambient
	float3 final = (c0 + c1 + c2) * diffuse.rgb;

	return float4(final + spec, diffuse.a);
}

technique MultiTexTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_2_0 MultiTexVS();
        pixelShader  = compile ps_2_0 MultiTexPS();
    }
}
