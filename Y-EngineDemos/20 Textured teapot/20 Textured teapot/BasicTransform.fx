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
struct OutputVS
{
	float4 posH : POSITION0;
};
OutputVS TransformVS(float3 posL : POSITION0)
{ 
	OutputVS outVS = (OutputVS)0;
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	return outVS;
}
float4 TransformPS() : COLOR
{
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
technique TransformTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 TransformVS();
		pixelShader  = compile ps_2_0 TransformPS();
		FillMode = Wireframe;
	}
}
