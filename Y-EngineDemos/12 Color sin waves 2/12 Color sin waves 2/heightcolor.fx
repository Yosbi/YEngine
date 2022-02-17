// Y-Engine vars
uniform extern float4x4 gWVP;				// World view proj combo matrix
uniform extern float3   gEyePosW;			// Camera position
uniform extern float4	gAmbientMtrl;		// Material ambient color
uniform extern float4	gDiffuseMtrl;		// Material diffuse color
uniform extern float4	gSpecularMtrl;		// Material specular color	
uniform extern float	gSpecularPowerMtrl; // Material specular power
uniform extern float4	gAmbientLight;		// Ambient light color

// App vars
uniform extern float 	gTime;
uniform extern float3   gLightVecW;
uniform extern float4	gDiffuseLight;
uniform extern float4   gSpecularLight;
uniform extern float4x4 gWorldInverseTranspose;
uniform extern float4x4 gWorld;

struct OutputVS
{
	float4 posH		: POSITION0;
	//float3 normalW  : TEXCOORD0;
	float3 posW		: TEXCOORD1;
};

// Amplitudes
static float a[3] = {0.8f, 0.2f, 0.08f};
	
// Angular wave numbers.
static float k[3] = {1.0, 0.8f, 2.0f};
	
// Angular frequency.
static float w[3] = {1.0f, 8.0f, 10.0f};
	
// Phase shifts.
static float p[3] = {0.0f, 1.0f, 1.0f};

float SumOfRadialSineWaves(float x, float z)
{
	// Distance of vertex from source of waves (which we set
	// as the origin of the local space).
	float d = sqrt(x*x + z*z);
	//float d = sqrt (z);
	
	// Sum the waves.
	float sum = 0.0f;
	for(int i = 0; i < 2; ++i)
		sum += a[i]*sin(k[i]*d - gTime*w[i] + p[i]);
		
	return sum;
}

void Partials(float x, float z, out float dhOverdx, out float dhOverdz)

{
      // Distance of vertex from source of waves (which we set
      // as the origin of the local space).
      float d = sqrt(x*x + z*z);

      // Derivative of a sum of functions is the sum of the derivatives.
      dhOverdx = 0.0f;
      dhOverdz = 0.0f;
      for(int i = 0; i < 2; ++i)
      {
            dhOverdx += (a[i]*k[i]*x*cos(k[i]*d - gTime*w[i] + p[i]))/d;
            dhOverdz += (a[i]*k[i]*z*cos(k[i]*d - gTime*w[i] + p[i]))/d;
      }
}


OutputVS ColorVS(float3 posL : POSITION0, float3 normalL : NORMAL0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;

	// Pass to pixel shader the pos in world
	outVS.posW = mul(float4(posL, 1.0f), gWorld).xyz;

	//// find the Y coord
	posL.y = SumOfRadialSineWaves(outVS.posW.x, outVS.posW.z);

	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
	 
	// Done--return the output.
    return outVS;
}

float4 ColorPS( float3 posW : TEXCOORD1) : COLOR
{
	float3 normalW;
	float dhOverdx;
	float dhOverdz;
	float3 u;
	float3 v;

	// Get the partials dx and dz of the point to calculate the normal
	Partials(posW.x, posW.z, dhOverdx, dhOverdz);
	
	//calculate the normal
	if (((posW.x*posW.x) < 0.01f) && ((posW.z*posW.z) < 0.01f))
	{
		normalW = float3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		v = float3(1.0f, dhOverdx, 0.0f);
		u = float3(0.0f, dhOverdz, 1.0f);

		normalW = normalize(cross(u,v));
	}		

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

	// Finally the final color
	return float4((ambient + diffuse + spec), gDiffuseMtrl.a);
	
	//====================================================
}

technique HeightColorTech
{
    pass P0
    {
        // Specify the vertex and pixel shader associated with this pass.
        vertexShader = compile vs_3_0 ColorVS();
        pixelShader  = compile ps_3_0 ColorPS();
		//FillMode = Wireframe;
    }
}