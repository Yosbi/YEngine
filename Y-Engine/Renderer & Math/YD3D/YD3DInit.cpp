//-------------------------------------------------------------------------------
// CD3DInit.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: In this file it the implementation of the code to initialize
//		the engine and of the methods to export from the dll
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Include
//--------------------------------------------------------------------------
#include "YD3D.h"

//-----------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------
bool  g_bLF = false; // To know if i have to mantain a logs files
//-----------------------------------------------------------------------------
// Name: DLLEntryPoint
// Desc: Is similar to WinMain()/main() functions, is used to initialize
//		the DLL
//-----------------------------------------------------------------------------
BOOL WINAPI DllEntryPoint( HINSTANCE hDll, DWORD fdwReason, LPVOID lpcReserved)
{
	switch (fdwReason)
	{
	 //Called when we attach to the dll
	case DLL_PROCESS_ATTACH:
		/* dll init/setup stuff */
		break;
	case DLL_PROCESS_DETACH:
		/* dll shutdow/release stuff */
		break;
	default:
		break;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: CreateRenderDevice
// Desc: Exported created function, creates a new YRenderDevice object
//-----------------------------------------------------------------------------
HRESULT CreateRenderDevice (HINSTANCE hDLL, YRenderDevice **pDevice)
{
	if(!*pDevice)
	{
		*pDevice = new YD3D(hDLL);
		return Y_OK;
	}
	return Y_FAIL;
}

//-----------------------------------------------------------------------------
// Name: ReleaseRenderDevice
// Desc: Exported release function: Releases the given YRenderDevice object
//-----------------------------------------------------------------------------
HRESULT ReleaseRenderDevice( YRenderDevice **pDevice )
{
	if(!*pDevice) return Y_FAIL;
	
	delete *pDevice;
	*pDevice = NULL;
	return Y_OK;	
}

//-----------------------------------------------------------------------------
// YD3D class implementations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name: YD3D
// Desc: Constructor
//-----------------------------------------------------------------------------
YD3D::YD3D( HINSTANCE hDLL)
{
	m_hDLL				= hDLL;	
	m_pD3D				= NULL;
	m_pDevice			= NULL;
	m_pLog				= NULL;
	m_ClearColor		= D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	m_bRunning			= false;
	m_bIsSceneRunning	= false;
	m_bStencil			= false;
	//m_bUseShaders		= false;
	//m_bCanDoShaders		= false;
	m_bAdditive			= false;
	m_bColorBuffer		= true;
	//m_bTextures			= true;
	m_pSkinManager		= NULL;
	m_pVertexManager	= NULL;
	
	m_pDeclVertex       = NULL;
	m_pDeclPVertex      = NULL;
	m_pDeclLVertex      = NULL;
	m_pDeclCVertex      = NULL;
	m_pDecl3TVertex     = NULL;
	m_pDeclTVertex      = NULL;
	m_pFont				= NULL;
	
	ZeroMemory(&m_pMesh, sizeof(LPD3DXMESH) * MAX_MESH);

	// Dont use swapchain at first
	m_nActivehWnd		= 0;
	m_nNumVShaders		= 0;
	m_nNumPShaders		= 0;
	m_nNumFonts			= 0;	
	m_nActiveFX			= 0;
	m_nNumFX			= 0;
	m_nNumMeshes		= 0;
	m_nPasses			= 0;
	YCOLOR yc			= {1.0f,1.0f,1.0f,1.0f};
	m_ycAmbientLight	= yc;
	m_vcEyePos			= YVector(0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
// Name: ~YD3D
// Desc: Destructor
//-----------------------------------------------------------------------------
YD3D::~YD3D()
{
	if (g_bLF)
	{
		Log("Shutting down Direct3D\n");
		if(m_pLog) fflush(m_pLog);
	}
	// Clear all values
	this->Release();
}

//-----------------------------------------------------------------------------
// Name: Release
// Desc: Release all the Direct3D COM stuff
//-----------------------------------------------------------------------------
void YD3D::Release()
{
	if (m_pSkinManager)
	{
		delete m_pSkinManager;
		m_pSkinManager = NULL;
	}

	if (m_pVertexManager)
	{
		delete m_pVertexManager;
		m_pVertexManager = NULL;
	}

	// Release shader stuff
	if (m_pDeclVertex) 
	{
		m_pDeclVertex->Release();
		m_pDeclVertex = NULL;
	}
	if (m_pDeclPVertex) 
	{
		m_pDeclPVertex->Release();
		m_pDeclPVertex = NULL;
	}
	if (m_pDeclLVertex)
	{
		m_pDeclLVertex->Release();
		m_pDeclLVertex = NULL;
	}
	if (m_pDeclCVertex)
	{
		m_pDeclCVertex->Release();
		m_pDeclCVertex = NULL;
	}
	if (m_pDecl3TVertex)
	{
		m_pDecl3TVertex->Release();
		m_pDecl3TVertex = NULL;
	}
	if (m_pDeclTVertex) 
	{
		m_pDeclTVertex->Release();
		m_pDeclTVertex = NULL;
	}

	for (UINT i = 0; i < m_nNumVShaders; i++)
	{
		if (m_pVShader[i])
		{
			m_pVShader[i]->Release();
			m_pVShader[i] = NULL;
		}
	}
	m_nNumVShaders = 0;

	for (UINT j = 0; j < m_nNumPShaders; j++) 
	{
		if (m_pPShader[j]) 
		{
			m_pPShader[j]->Release();
			m_pPShader[j] = NULL;
		}
	}
	m_nNumPShaders = 0;

	for (UINT k = 0; k < m_nNumFonts; k++) 
	{
		if (m_pFont[k]) 
		{
			m_pFont[k]->Release();
			m_pFont[k] = NULL;
		}
	}

	if (m_pFont) 
	{
		free(m_pFont);
		m_pFont		= NULL;
		m_nNumFonts = 0;
	}

	// If meshes
	for (int a = 0; a < m_nNumMeshes; a++)
	{
		if (m_pMesh[a])
			m_pMesh[a]->Release();
	}
	m_nNumMeshes = 0;

	// Main things
	if(m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	if(m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
	if (g_bLF)Log("Shutdown completed\n");
	fclose(m_pLog);
}

//-----------------------------------------------------------------------------
// Name: Init
// Desc: Initialize dialogbox to select device and format
//-----------------------------------------------------------------------------
HRESULT YD3D::Init( HWND hWnd, const HWND *hWnd3D, int nNumhWnd, bool bStencil, bool bSaveLog)
{
	int nResult;

	// If the user wants to salve a log
	if(bSaveLog)
	{
		g_bLF = bSaveLog;
		fopen_s(&m_pLog,"Log_RenderDevice.txt", "w");
		if(!m_pLog) return Y_FAIL;
	}	

	// Are there any child windows to use?
	if(nNumhWnd > 0)
	{
		if(nNumhWnd > MAX_3DHWND) nNumhWnd = MAX_3DHWND;
		memcpy(&m_hWnd[0], hWnd3D, sizeof(HWND) * nNumhWnd);
		m_nNumhWnd = nNumhWnd;
	}
	else // Store handle to main window at least
	{
		m_hWnd[0] = hWnd;
		m_nNumhWnd = 0;
	}
	m_hWndMain = hWnd;
	m_nActivehWnd = 0; // Set the default active window

	// Is the user using stencil?
	m_bStencil = bStencil;
	/*if (bStencil) m_bStencil = true;
	else m_bStencil = false;*/

	if (g_bLF)Log("YEngine YD3D-RenderDevice log File:\n\n");

	// Build the main direct 3Dobject
	if(m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}

	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	if (! m_pD3D)
	{
		if (g_bLF)Log("Error: Direct3DCreate9()\n");
		return Y_CREATEAPI;
	}

	// Objects to call for the dialog
	YD3DSettingsDlg D3DSettingsDlg;
    YD3DInitialize  Initialize;
	D3DDISPLAYMODE  MatchMode;
	
	// Call to enumerate the setings of the hardware
	if(FAILED(Initialize.Enumerate( m_pD3D )))
	{
		if (g_bLF)Log("Failed in enumeration of hardware\n");
		return Y_CREATEDEVICE;
	}
	
	if (g_bLF)Log("Calling the dialog... \n");
	//  Attempt to find a good default windowed set
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &MatchMode);
	// Attempt to find a good default fullscreen set
    Initialize.FindBestFullscreenMode( m_D3DSettings, &MatchMode );	
	// Attempt to find a good default windowed mode set
    Initialize.FindBestWindowedMode( m_D3DSettings );
	// Call the dialog
	nResult = D3DSettingsDlg.ShowDialog( &Initialize, &m_D3DSettings, hWnd, m_hDLL);	

	if (nResult == -1)
	{
		if (g_bLF)Log("Selection dialog error\n");
		return Y_FAIL;
	}
	else if ((nResult == 0) || (nResult == 2))
	{
		if (g_bLF)Log("Selection dialog canceled by the user\n");
		return Y_CANCELED;
	}
	else if (nResult != IDOK)
	{
		if (g_bLF)Log("Failed in dialog\n");
		return Y_FAIL;
	}
	else
	{
		if (g_bLF)Log("%i Selection dialg ok\n[YD3DDevice]: Firing up Direct3D\n", nResult);
		m_D3DSettings = D3DSettingsDlg.GetD3DSettings();
		return Go();
	}
	
}

//-----------------------------------------------------------------------------
// Name: Go
// Desc: Start API values from the dialogbox
//-----------------------------------------------------------------------------
HRESULT YD3D::Go()
{
	HRESULT hr;
	HWND	hwnd;
	ULONG	CreateFlags = 0;

	
	// prepare present parameters structure
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));

	// Obtaining the settings of the CD3DSettings
	//m_D3DSettings.Windowed = true;
	m_bWindowed = m_D3DSettings.Windowed;
	YD3DSettings::Settings *pSettings = m_D3DSettings.GetSettings();   

	// Fill out common present parameters

	//MUST DO A FUNCTION TO GET THE MAX BACKBUFFER COUNT !!!!IS IMPORTANT YOSBI!!!!
	m_d3dpp.BackBufferCount           = 1;
	m_d3dpp.BackBufferFormat          = pSettings->BackBufferFormat;
	m_d3dpp.Windowed                  = m_bWindowed;
	m_d3dpp.MultiSampleType           = pSettings->MultisampleType;
	m_d3dpp.MultiSampleQuality        = pSettings->MultisampleQuality;
	m_d3dpp.EnableAutoDepthStencil    = TRUE;
	m_d3dpp.AutoDepthStencilFormat    = pSettings->DepthStencilFormat;
	m_d3dpp.PresentationInterval      = pSettings->PresentInterval;
	m_d3dpp.Flags                     = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_d3dpp.SwapEffect                = D3DSWAPEFFECT_DISCARD;
	
	// stencil buffer active?
	if	  ((pSettings->DepthStencilFormat == D3DFMT_D24S8)
		|| (pSettings->DepthStencilFormat == D3DFMT_D24X4S4)
		|| (pSettings->DepthStencilFormat == D3DFMT_D15S1)		)
		m_bStencil = true;
	else
		m_bStencil = false;

	// Fullscreen mode?
	if(!m_bWindowed)
	{
		m_d3dpp.hDeviceWindow			   = hwnd = m_hWndMain;
		m_d3dpp.FullScreen_RefreshRateInHz = pSettings->DisplayMode.RefreshRate;
		m_d3dpp.BackBufferWidth            = pSettings->DisplayMode.Width;
		m_d3dpp.BackBufferHeight           = pSettings->DisplayMode.Height;
		m_dwHeight						   = pSettings->DisplayMode.Height;
		m_dwWidth						   = pSettings->DisplayMode.Width;
	}
	else{
		// Windowed mode
		m_d3dpp.hDeviceWindow			  = hwnd = m_hWnd[0];
		m_d3dpp.BackBufferWidth			  = GetSystemMetrics(SM_CXSCREEN);
		m_d3dpp.BackBufferHeight		  = GetSystemMetrics(SM_CYSCREEN);
		m_dwHeight						  = GetSystemMetrics(SM_CYSCREEN);
		m_dwWidth						  = GetSystemMetrics(SM_CXSCREEN);
	}

	// Create flags
	if ( pSettings->VertexProcessingType == PURE_HARDWARE_VP )
		CreateFlags = D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else if ( pSettings->VertexProcessingType == HARDWARE_VP )
		CreateFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else if ( pSettings->VertexProcessingType == MIXED_VP )
		CreateFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
	else if ( pSettings->VertexProcessingType == SOFTWARE_VP )
		CreateFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Create Direct3D Device
	if(m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	hr = m_pD3D->CreateDevice( pSettings->AdapterOrdinal, pSettings->DeviceType,
							   hwnd, CreateFlags, &m_d3dpp, &m_pDevice );
	if(FAILED(hr))
	{
		if (g_bLF)Log("Error: IDirect3D::CreateDevice()");
		if (hr == D3DERR_NOTAVAILABLE)
			if (g_bLF)Log("D3DERR_NOTAVAILABLE");
		else if (hr == D3DERR_INVALIDCALL)
			if (g_bLF)Log("D3DERR_INVALIDCALL");
		else if (hr == D3DERR_OUTOFVIDEOMEMORY)
			if (g_bLF)Log("D3DERR_OUTOFVIDEOMEMORY");
		else
			if (g_bLF)Log("unknown error");

		return Y_CREATEDEVICE;
	}

	// Create additional swap chains if wished and possible
	if(( m_nNumhWnd > 0) && m_bWindowed)
	{
		for (UINT i = 0; i < m_nNumhWnd; i++)
		{
			m_d3dpp.hDeviceWindow = m_hWnd[i];
			m_pDevice->CreateAdditionalSwapChain(&m_d3dpp, &m_pChain[i]);
		}	
	}

	
	//m_pDevice->GetDeviceCaps((&g_xDevice.d3dCaps);

	m_bRunning			= true;
	m_bIsSceneRunning	= false;

	return FirstTimeInit();

}

//-----------------------------------------------------------------------------
// Name: Log
// Desc: Go to init defaults render states and other stuff
//-----------------------------------------------------------------------------
HRESULT YD3D::FirstTimeInit()
{
	

	// Bring material and vertex cache managers online
	m_pSkinManager   = new YD3DSkinManager(m_pDevice, m_pLog);
	m_pVertexManager = new YD3DVertexCacheManager((YD3DSkinManager*)m_pSkinManager, m_pDevice, this, 8192, 8192, m_pLog);

	// Activate render states
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); 
    m_pDevice->SetRenderState(D3DRS_ZENABLE,  D3DZB_TRUE);
	m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DFILL_SOLID);
	// Solid rendering
	//m_ShadeMode = RS_SHADE_SOLID;

	// Create and set standard material
	memset(&m_StdMtrl, 0, sizeof(D3DMATERIAL9));
	m_StdMtrl.Ambient.r  = 1.0f;
	m_StdMtrl.Ambient.g  = 1.0f;
	m_StdMtrl.Ambient.b  = 1.0f;
	m_StdMtrl.Ambient.a  = 1.0f;

	if (FAILED(m_pDevice->SetMaterial(&m_StdMtrl))) 
	{
		if (g_bLF)Log("error: set material (FirstTimeInit)");
		return Y_FAIL;
    }

	//// Set texture filtering
	//m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// Go to 3D mode in first stage
	YVIEWPORT vpView = { 0, 0, m_dwWidth, m_dwHeight };
	m_Mode			 = EMD_PERSPECTIVE;
	m_nStage		 = -1;
	SetActiveSkinID(MAX_ID);

	
	// Check for at least shader 2.0 support
	if (CanDoShaderVersion(SH_2_0))
	{
		// init shaders(vertex dcl)
		PrepareShaderStuff();

		// Build an BASIC effect file
		FILE *m_pEff;
		fopen_s(&m_pEff,"BasicTransform.fx", "w");
		const char basicEffect[] =
		"// Y-Engine variables\n"\
		"uniform extern float4x4  gWVP;				 // World view proj combo matrix\n"\
		"uniform extern float3    gEyePosW;			 // Camera position\n"\
		"uniform extern float4	 gAmbientMtrl;		 // Material ambient color\n"\
		"uniform extern float4	 gDiffuseMtrl;		 // Material diffuse color\n"\
		"uniform extern float4    gEmissiveMtrl;		 // Material emissive color\n"\
		"uniform extern float4	 gSpecularMtrl;		 // Material specular color\n"\
		"uniform extern float	 gSpecularPowerMtrl; // Material specular power\n"\
		"uniform extern float4	 gAmbientLight;		 // Ambient light color\n"\
		"uniform extern texture	 gTex1;				 // Texture 1\n"\
		"uniform extern texture	 gTex2;				 // Texture 2\n"\
		"uniform extern texture	 gTex3;				 // Texture 3\n"\
		"uniform extern texture	 gTex4;				 // Texture 4\n"\
		"uniform extern texture	 gTex5;				 // Texture 5\n"\
		"uniform extern texture	 gTex6;				 // Texture 6\n"\
		"uniform extern texture	 gTex7;				 // Texture 7\n"\
		"uniform extern texture	 gTex8;				 // Texture 8\n"\
		"struct OutputVS\n"\
		"{\n"\
		"	float4 posH : POSITION0;\n"\
		"};\n"\
		"OutputVS TransformVS(float3 posL : POSITION0)\n"\
		"{ \n"\
		"	OutputVS outVS = (OutputVS)0;\n"\
		"	outVS.posH = mul(float4(posL, 1.0f), gWVP);\n"\
		"	return outVS;\n"\
		"}\n"\
		"float4 TransformPS() : COLOR\n"\
		"{\n"\
		"	return float4(0.0f, 0.0f, 0.0f, 1.0f);\n"\
		"}\n"\
		"technique TransformTech\n"\
		"{\n"\
		"	pass P0\n"\
		"	{\n"\
		"		vertexShader = compile vs_2_0 TransformVS();\n"\
		"		pixelShader  = compile ps_2_0 TransformPS();\n"\
		"		FillMode = Wireframe;\n"\
		"	}\n"\
		"}\n";
		fprintf(m_pEff, basicEffect);
		fflush(m_pEff);
		fclose(m_pEff);

		CreateFX("BasicTransform.fx", "TransformTech", true, NULL);
		ActivateFX(0);
		
	}
	else
	{
		if (g_bLF)Log("FATAL ERROR: HW Has No Shader Support");
		Release();
		return Y_NOSHADERSUPPORT;
	}

	// Identity matrix for view
	IDENTITY(m_mView3D);

	// Set clipping plane values
	SetClippingPlanes(0.1f, 1000.0f);

	// Set Ambient light level 
    SetAmbientLight(1.0f, 1.0f, 1.0f);

	// set perspective projection stage 0
	if (FAILED(InitStage(0.8f, &vpView, 0)))
		 return Y_FAIL;

	// set perspective projection
	if (FAILED(SetMode(EMD_PERSPECTIVE, 0)))
		 return Y_FAIL;

	// World matrix to identity, this is needed to set
    // correct values to vertex shader registers also
    SetWorldTransform(NULL);
	
    if (g_bLF)Log("First time init complete");
	if (g_bLF)Log("Initialized (online and ready)\n");
    return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: Log
// Desc: Write output string to attribut outputstring if exist
//-----------------------------------------------------------------------------
void YD3D::Log(char *chString, ...)
{
	if (g_bLF)
	{
		char ch[2048];
		char *pArgs;
   
		pArgs = (char*) &chString + sizeof(chString);
		vsprintf(ch, chString, pArgs);
		fprintf(m_pLog, "[YD3DDevice]: ");
		fprintf(m_pLog, ch);
		fprintf(m_pLog, "\n");   
   
		 fflush(m_pLog);
	}
}