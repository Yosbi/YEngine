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

UINT g_sMesh=0;

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
	   
		if (FAILED(BuildAndSetFX())) 
		{
			MessageBox(g_hWnd, "Sorry, pixel shaders 3.0 required to run this demo.", "Y-Engine Report", MB_OK | MB_ICONERROR);
			g_bDone = true;
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
	g_pDevice->SetViewLookAt(YVector(0,25, -50), YVector(0,0,0), vU);
	//g_pDevice->SetView3D(vR,vU,vD,vP);
	
	// Set the clear color
	g_pDevice->SetClearColor(1.0f,1.0f, 1.0f);
	g_pDevice->SetAmbientLight(0.4f, 0.4f, 0.4f);
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
	static float dt = 0.0f;
	if(cntsPerSec == 0)
	{// get the procesor frequency
		QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
		secsPerCnt = 1.0f / (float)cntsPerSec;
	}   

		
	static __int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;
	prevTimeStamp = currTimeStamp;
	
	TimeElapsed += dt;

	// clear buffers and start scene
	g_pDevice->BeginRendering(true,true,true);
    g_pDevice->ActivateFX(1);
	Render(TimeElapsed);  // ambient


	g_pDevice->DrawText(g_nFontID, 55, 20, 0, 200, 0, "Y-Engine Demo: Color Sin Waves Demo (gelatin)");
	g_pDevice->DrawText(g_nFontID, 55, 55, 0, 200, 0, "By: Yosbi Alves (yosbito@gmail.com) ");

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
	YMatrix vmat;
	vmat.Identity();
	vmat.RotaY(-0.4f * ((40 * TimeElapsed * 3.14)/180));   
    g_pDevice->SetViewLookAt(YVector(0,30, -50) * vmat, YVector(0,0,0), YVector(0,1,0));

	YMatrix mat;
	mat.Identity();
	mat.RotaY(-0.4f);     
	g_pDevice->SetWorldTransform(&mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &mat, 0);
	
	mat.InverseOf(mat);
	mat.TransposeOf(mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat, 0);

	// Set the scene light
	YCOLOR  cLight	= {1.0f, 1.0f, 1.0f, 1.0f};
	YVector vcLight	= YVector(0.0f, 0.0f, -1.0f);
	vcLight.Normalize();

	g_pDevice->SetFXConstant(DAT_OTHER, "gLightVecW", &vcLight, sizeof(float) * 3);
	g_pDevice->SetFXConstant(DAT_OTHER, "gDiffuseLight", &cLight, sizeof(YCOLOR));
	g_pDevice->SetFXConstant(DAT_OTHER, "gSpecularLight", &cLight, sizeof(YCOLOR));
	
	g_pDevice->SetFXConstant(DAT_FLOAT, "gTime", (void*)&TimeElapsed, 0);

	return g_pDevice->GetVertexManager()->Render(g_sMesh);
}

//-----------------------------------------------------------------------------
// Name: BuildAndSetFX
// Desc: Load the FX to the engine
//-----------------------------------------------------------------------------
HRESULT BuildAndSetFX() 
{	
	 return g_pDevice->CreateFX("heightcolor.fx", "HeightColorTech", true, NULL);		
} 

//-----------------------------------------------------------------------------
// Name: BuildGeometry
// Desc: Build the geometry to render
//-----------------------------------------------------------------------------
HRESULT BuildGeometry(void) 
{
	HRESULT hr=Y_OK;
	VERTEX *v = NULL;
   
	int nIndis = 0;
	int nVerts = 0;
	WORD   *i  = NULL; 

	GenGrid(200, 200, 0.25f, 0.25f, YVector(0.0f, 0.0f, 0.0f),VID_UU, v, i, &nIndis, &nVerts);

	return Y_OK;
} 

//-----------------------------------------------------------------------------
// Name: GenGrid
// Desc: Generates a grid
//-----------------------------------------------------------------------------
void GenGrid(int numVertRows, int numVertCols, float dx, float dz, const YVector& center, YVERTEXID vID, 
				VERTEX *verts, WORD *indices, int *nIndis, int *nVerts)
{
	*nVerts			= numVertRows*numVertCols;
	int numCellRows = numVertRows-1;
	int numCellCols = numVertCols-1;

	int numTris = numCellRows*numCellCols*2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;
	
	// Build vertices.	

	verts = new VERTEX[*nVerts];	

	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f; 
	float zOffset =  depth * 0.5f;

	int k = 0;
	for(float i = 0; i < numVertRows; ++i)
	{
		for(float j = 0; j < numVertCols; ++j)
		{
			// Negate the depth coordinate to put in quadrant four.  
			// Then offset to center about coordinate system.
			verts[k].x =  j * dx + xOffset;
			verts[k].z = -i * dz + zOffset;
			verts[k].y =  0.0f;

			// Translate so that the center of the grid is at the
			// specified 'center' parameter.
			YMatrix T;
			T.SetTranslation(YVector(center.x, center.y, center.z),true);

			YVector vc(verts[k].x, verts[k].y, verts[k].z);
			vc = vc * T;

			verts[k].x = vc.x;
			verts[k].y = vc.y;
			verts[k].z = vc.z;			
			verts[k].vcN[0] = verts[k].vcN[2] = 0.0f;
			verts[k].vcN[1] = 1.0f;

			++k; // Next vertex
		}
	}

	// Build indices.
	*nIndis = numTris * 3;
	indices = new WORD[*nIndis];
	
	 
	// Generate indices for each quad.
	k = 0;
	for(WORD i = 0; i < (WORD)numCellRows; ++i)
	{
		for(WORD j = 0; j < (WORD)numCellCols; ++j)
		{
			indices[k]     =   i   * numVertCols + j;
			indices[k + 1] =   i   * numVertCols + j + 1;
			indices[k + 2] = (i+1) * numVertCols + j;
					
			indices[k + 3] = (i+1) * numVertCols + j;
			indices[k + 4] =   i   * numVertCols + j + 1;
			indices[k + 5] = (i+1) * numVertCols + j + 1;

			// next quad
			k += 6;
		}
	}

	YCOLOR diff = { 0.0f, 0.0f, 1.0f, 1.0f };
	YCOLOR ambient = { 0.0f, 0.0f, 1.0f, 1.0f };
	YCOLOR spec = { 1.0f, 1.0f, 1.0f, 1.0f };
	float fSpec = 16.0f;
	UINT z = -99;
	g_pDevice->GetSkinManager()->AddSkin(&ambient, &diff, &diff, &spec, fSpec, &z);
	
	g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, z, *nVerts, *nIndis, verts, indices, &g_sMesh);
	delete [] verts;
	delete [] indices;
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