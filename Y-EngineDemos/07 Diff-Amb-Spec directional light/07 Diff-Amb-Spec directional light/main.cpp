// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "YRenderer.h"  // our render interface
#include "Y.h"          // return values and stuff
#include "main.h"       // prototypes and stuff
#include "YEMath.h"		// The math library



//include our library
#pragma comment(lib, "YRenderer.lib")
#pragma comment(lib, "YEMath.lib")
 

// windows stuff
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");
HWND      g_hWnd  = NULL;

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bDone     = false;
FILE *pLog=NULL;
UINT g_nFontID=0;

// renderer object
LPYRENDERER     g_pRenderer = NULL;
LPYRENDERDEVICE g_pDevice   = NULL;


//-----------------------------------------------------------------------------
// Name: WinMain
// Desc: WinMain function to get the thing started.
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	WNDCLASSEX	wndclass;
	HRESULT     hr;
	HWND		hWnd;
	MSG			msg;
	ZeroMemory(&msg, sizeof(msg));

	// Set up window attributes
	wndclass.cbSize         = sizeof(wndclass);
	wndclass.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wndclass.lpfnWndProc    = MsgProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 0;
	wndclass.hInstance      = hInst;
	wndclass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
	wndclass.lpszMenuName   = NULL;
	wndclass.lpszClassName  = g_szAppClass;
	wndclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    
	if(RegisterClassEx(&wndclass) == 0)
		return 0;

	// create window
	if (!(hWnd = CreateWindowEx(NULL, g_szAppClass, "YEngine v0.1 - By Yosbi Alves",
								WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
								WS_MINIMIZEBOX | WS_VISIBLE,
			 					GetSystemMetrics(SM_CXSCREEN)/2 -400,
								GetSystemMetrics(SM_CYSCREEN)/2 -300,
								800, 600, NULL, NULL, hInst, NULL)))
		return 0;
   
	// dont show everything off yet
	ShowWindow(hWnd, SW_HIDE);
     
	g_hWnd  = hWnd;
	g_hInst = hInst;

	pLog = fopen("log_main.txt", "w");
  
	// try to start the engine
	if (FAILED( hr = EngineStartup("Direct3D") )) 
	{
		if (hr == Y_FAIL)
			fprintf(pLog, "error: ProgramStartup() failed\n");
		else if (hr == Y_CANCELED)
			fprintf(pLog, "error: ProgramStartup() canceled by user \n");
      
		fflush(pLog);
		g_bDone = true;
	}

	// everything went smooth
	else 
	{
		ShowWindow(hWnd, SW_SHOW);
	   HRESULT hr;
		if (FAILED( hr = BuildAndSetFX())) 
		{
			MessageBox(g_hWnd, "Error loading effect", "Y-Engine Report", MB_OK | MB_ICONERROR);
			g_bDone = true;
			Log(HrToStr(hr));
		}
	  
		if (FAILED( BuildGeometry() ))
			g_bDone = true;

		SetEngineFirstStates();
	}

	// main loop
	while (!g_bDone) 
	{
		// handle messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} 
		// do one frame
		if (g_bIsActive) ProgramTick();
	}
   
	// cleanup stuff
	ProgramCleanup();

	UnregisterClass(g_szAppClass, hInst);
   
	// return back to windows
	return (int)msg.wParam;
} 

//-----------------------------------------------------------------------------
// Name: MsgProc
// Desc: Deal incoming msgs
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		// our app has the focus
		case WM_ACTIVATE: {
			g_bIsActive = (BOOL)wParam;
			} break;
         
		// key was pressed
		case WM_KEYDOWN: {
			switch (wParam) {
			case VK_ESCAPE: {
				g_bDone = true;
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				return 0;
				} break;
			}
			} break;

		// we are ordered to suicide
		case WM_DESTROY: {
			g_bDone = true;
			PostQuitMessage(0);
			return 1;
			} break;

		default: break;
		}
   
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//-----------------------------------------------------------------------------
// Name: SetEngineFirstStates
// Desc: Set the initial states of the engine
//-----------------------------------------------------------------------------
void SetEngineFirstStates()
{
	// prepare viewport stage
	g_pDevice->InitStage(0.8f, NULL, 0);

	// Set the view
	YVector vR(1,0,0), vU(0,1,0), vD(0,0,1), vP(0,0,-15);
	g_pDevice->SetViewLookAt(YVector(0,3, -3), YVector(0,0,0), vU);
	//g_pDevice->SetView3D(vR,vU,vD,vP);
	
	// Set the clear color
	g_pDevice->SetClearColor(1.0f,1.0f, 1.0f);

	// Set ambient light
	g_pDevice->SetAmbientLight(0.35f,0.35f,0.35f);
}

//-----------------------------------------------------------------------------
// Name: EngineStartup
// Desc: Turn on the engine
//-----------------------------------------------------------------------------
HRESULT EngineStartup(char *chAPI) 
{
	// create a render objekt
	g_pRenderer = new YRenderer(g_hInst);
   
	// create a device for the chosen api
	if (FAILED( g_pRenderer->CreateDevice(chAPI) )) return E_FAIL;
   
	// get a pointer on that device
	g_pDevice = g_pRenderer->GetDevice();
	if (g_pDevice == NULL) return E_FAIL;

	// init render device
	if (FAILED( g_pDevice->Init(g_hWnd, NULL, 0, true, true ) )) 
	{
		fprintf(pLog, "error: Init() failed in ProgramStartup()\n");
		return E_FAIL;
	}	
   
	POINT ptRes; int fs=0;
	g_pDevice->GetResolution(&ptRes);

	// font size
	fs  = ptRes.x / 40;
	if (FAILED( g_pDevice->CreateFont( 0, false, false,	false, fs, &g_nFontID) )) 
	{
		fprintf(pLog, "error: YRenderDevice::CreateFont failed\n");
		return Y_FAIL;
	}
	return Y_OK;
} 

//-----------------------------------------------------------------------------
// Name: ProgramCleanup
// Desc: Releases all the resourses
//-----------------------------------------------------------------------------
HRESULT ProgramCleanup(void) 
{
	if (g_pRenderer) 
	{
		delete g_pRenderer;
		g_pRenderer = NULL;		
	}

	if (pLog)
		fclose(pLog);

	return S_OK;
} 

//-----------------------------------------------------------------------------
// Name: ProgramTick
// Desc: Do a frame
//-----------------------------------------------------------------------------
HRESULT ProgramTick(void) 
{
	HRESULT hr = Y_FAIL;
	YMatrix mat;
	mat.Identity();

	static __int64 prevTimeStamp = 0;
	static __int64 cntsPerSec    = 0;
	static float   secsPerCnt    = 0;
	static float TimeElapsed =0;
	if(cntsPerSec == 0)
	{// get the procesor frequency
		QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
		secsPerCnt = 1.0f / (float)cntsPerSec;
	}
   
	__int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;
	prevTimeStamp = currTimeStamp;

	TimeElapsed += dt;
	// clear buffers and start scene
	g_pDevice->BeginRendering(true,true,true);
   
	Render(TimeElapsed);  

	//g_pDevice->UseShaders(false);
	g_pDevice->DrawText(g_nFontID,30, 20, 0, 200, 0, "Y-Engine Demo: Diffuse-Ambient-Specular Directional light");
	g_pDevice->DrawText(g_nFontID, 30, 55, 0, 200, 0, "By: Yosbi Alves (yosbito@gmail.com)");

	// flip backbuffer to front 
	g_pDevice->EndRendering();
	return hr;
} 

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Render the scene
//-----------------------------------------------------------------------------
HRESULT Render(float TimeElapsed)
{
	HRESULT hr = g_pDevice->ActivateFX(1);

	YMatrix vmat;
	vmat.Identity();
	vmat.RotaY(-0.4f * ((40 * TimeElapsed * 3.14)/180));   
    g_pDevice->SetViewLookAt(YVector(0,3, -5) * vmat, YVector(0,0,0), YVector(0,1,0));
	

	YMatrix wmat;
	wmat.Identity();
	g_pDevice->SetWorldTransform(&wmat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &wmat, 0);

	wmat.InverseOf(wmat);
	wmat.TransposeOf(wmat);
	
	YCOLOR DiffuseLight = { 1.0f,1.0f,1.0f,1.0f};	
	YCOLOR SpecularLight = { 1.0f,1.0f,1.0f,1.0f};
	YVector LightVecW(0.0f, 0.0f, -1.0f);
	
	//Log(HrToStr(hr));
	//g_pDevice->SetFXConstant(DAT_FLOAT, "gTime", (void*)&TimeElapsed, 0);
	
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &wmat, 0);
	g_pDevice->SetFXConstant(DAT_OTHER, "gDiffuseLight", &DiffuseLight, sizeof(YCOLOR));
	g_pDevice->SetFXConstant(DAT_OTHER, "gSpecularLight", &SpecularLight, sizeof(YCOLOR));
	g_pDevice->SetFXConstant(DAT_OTHER, "gLightVecW", &LightVecW, sizeof(float)*3);

	return g_pDevice->MeshRender(0, 0);
}

//-----------------------------------------------------------------------------
// Name: BuildAndSetFX
// Desc: Load the FX to the engine
//-----------------------------------------------------------------------------
HRESULT BuildAndSetFX() 
{		
	 return g_pDevice->CreateFX("DiffAmbSpecDir.fx", "AmbDiffSpecTech", true, NULL);		
} 

//-----------------------------------------------------------------------------
// Name: BuildGeometry
// Desc: Build the geometry to render
//-----------------------------------------------------------------------------
HRESULT BuildGeometry(void) 
{
	YCOLOR dMtrl = { 0.0f, 0.0f, 1.0f, 1.0f };
	YCOLOR aMtrl = { 0.0f, 0.0f, 1.0f, 1.0f };
	YCOLOR sMtrl = {0.8f, 0.8f, 0.8f, 1.0f};
	

	UINT    s=-99;
	if(FAILED(g_pDevice->GetSkinManager()->AddSkin(&aMtrl, &dMtrl, &aMtrl, &sMtrl, 8.0f, &s)))
	{}

	g_pDevice->MeshCreateTeapot(NULL);
	return Y_OK;
} 

//-----------------------------------------------------------------------------
// Name: HrToStr
// Desc: Convert error code to generic text string.
//-----------------------------------------------------------------------------
TCHAR* HrToStr(HRESULT hr) {
	switch (hr) {
		case Y_OK:               return TEXT("Y_OK");
		case Y_FAIL:             return TEXT("Y_FAIL");
		case Y_CREATEAPI:        return TEXT("Y_CREATEAPI");
		case Y_CREATEDEVICE:     return TEXT("Y_CREATEDEVICE");
		case Y_INVALIDPARAM:     return TEXT("Y_INVALIDPARAM");
		case Y_INVALIDID:        return TEXT("Y_INVALIDID");
		case Y_BUFFERSIZE:       return TEXT("Y_BUFFERSIZE");
		case Y_BUFFERLOCK:       return TEXT("Y_BUFFERLOCK");
		//case Y_NOTCOMPATIBLE:    return TEXT("Y_NOTCOMPATIBLE");
		case Y_OUTOFMEMORY:      return TEXT("Y_OUTOFMEMORY");
		case Y_FILENOTFOUND:     return TEXT("Y_FILENOTFOUND");
		case Y_INVALIDFILE:      return TEXT("Y_INVALIDFILE");
		default: return TEXT("unknown");
		}
	} 

//-----------------------------------------------------------------------------
// Name: Log
// Desc: Error an warnings log
//-----------------------------------------------------------------------------
void Log(char *chString, ...)
{
	
		char ch[256];
		char *pArgs;

		pArgs = (char*) &chString + sizeof(chString);
		vsprintf(ch, chString, pArgs);
		fprintf(pLog, "[Debug]: ");
		fprintf(pLog, ch);
		fprintf(pLog, "\n");
		
		fflush(pLog);
	
}