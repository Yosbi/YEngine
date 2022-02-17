// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "../../../Y-Engine/Renderer & Math/YRenderer/YRenderer.h"
#include "../../../Y-Engine/Renderer & Math/YD3D/Y.h"
#include "main.h"       // prototypes and stuff
#include "../../../Y-Engine/Renderer & Math/YEMath/YEMath.h"
#include "../../../Y-Engine/YInput/YInput/YInput.h"
#include "../../../Y-Engine/YInput/YInput/YInputDevice.h"


//include our library
#pragma comment(lib, "../../../Y-Engine/Renderer & Math/Debug/YRenderer.lib")
#pragma comment(lib, "../../../Y-Engine/Renderer & Math/Debug/YEMath.lib")
#pragma comment(lib, "../../../Y-Engine/YInput/Debug/YInput.lib")

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

// Input object
LPYINPUT g_pYInput;
LPYINPUTDEVICE g_pYInputDevice;

UINT g_Mesh = 0, g_MeshSkin = 0;
UINT g_Cube = 0, g_CubeSkin = 0;

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
	// Try to start the input module
	else if (FAILED (InputStartup()))
	{
		Log("Error: InputStartup() failed");
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
// Name: InputStartup()
// Desc: Turn on the input module
//-----------------------------------------------------------------------------
HRESULT InputStartup()
{
	RECT mouseCage   = {0,0,0,0};
	mouseCage.right  = 800;
	mouseCage.bottom = 600;

	// Create device
	g_pYInput = new YInput(g_hInst);
	if (FAILED(g_pYInput->CreateDevice())) return E_FAIL;

	// Get the device
	g_pYInputDevice = g_pYInput->GetDevice();
	if(g_pYInputDevice == NULL) return E_FAIL;

	// Init the device
	if (FAILED(g_pYInputDevice->Init(g_hWnd, &mouseCage, true)))
	{
		Log("Error could not init YInputDevice");
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
	g_pDevice->DrawText(g_nFontID,30, 20, 0, 200, 0, "Y-Engine Demo: Textured Transparent Teapod");
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

	// get the imputs to control the camera
	g_pYInputDevice->Update();
	
	// rotation
	static POINT sump  = {0,0};
	static float fBackForw = 3.0f;
	POINT p = g_pYInputDevice->GetMouseDelta();
	sump.x += p.x;
	fBackForw += p.y;

	if (fBackForw < 3.0f)
		fBackForw =  3.0f;

	// up & down
	static float up = 3.0f;
	if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, YVK_W))
	{
		up +=0.01f;
		if (up > 50.0f)
			up = 50.0f;
	}
	if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, YVK_S))
	{
		up -=0.01f;
		if (up < -50.0f)
			up = -50.0f;
	}

	YMatrix vmat;
	vmat.Identity();
	vmat.RotaY(-0.4f * ((sump.x * 3.14)/180));   
	g_pDevice->SetViewLookAt(YVector(0,up, 0.2f * (-fBackForw)) * vmat, YVector(0,0,0), YVector(0,1,0));

		
	
	YCOLOR DiffuseLight = { 1.0f,1.0f,1.0f,1.0f};	
	YCOLOR SpecularLight = { 1.0f,1.0f,1.0f,1.0f};
	YVector LightVecW(0.0f, 0.0f, -1.0f);
	
	//Log(HrToStr(hr));
	//g_pDevice->SetFXConstant(DAT_FLOAT, "gTime", (void*)&TimeElapsed, 0);
	
	
	g_pDevice->SetFXConstant(DAT_OTHER, "gDiffuseLight", &DiffuseLight, sizeof(YCOLOR));
	g_pDevice->SetFXConstant(DAT_OTHER, "gSpecularLight", &SpecularLight, sizeof(YCOLOR));
	g_pDevice->SetFXConstant(DAT_OTHER, "gLightVecW", &LightVecW, sizeof(float)*3);

	// render cube
	YMatrix wmat;	
	wmat.Translate(0.0f, 0.0f, 10.0f);
	g_pDevice->SetWorldTransform(&wmat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &wmat, 0);
	wmat.InverseOf(wmat);
	wmat.TransposeOf(wmat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &wmat, 0);
	g_pDevice->GetVertexManager()->Render(g_Cube);

	// Render teapot
	wmat.Identity();
	g_pDevice->SetWorldTransform(&wmat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &wmat, 0);
	wmat.InverseOf(wmat);
	wmat.TransposeOf(wmat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &wmat, 0);
	g_pDevice->SetWorldTransform(&wmat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &wmat, 0);
	g_pDevice->MeshRender(g_Mesh, g_MeshSkin);

	

	return Y_OK;
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
	// Create teapot
	YCOLOR dMtrl = { 1.0f, 1.0f, 1.0f, 1.0f };
	YCOLOR aMtrl = { 1.0f,1.0f, 1.0f, 1.0f };
	YCOLOR sMtrl = {0.8f, 0.8f, 0.8f, 1.0f};
	g_pDevice->GetSkinManager()->AddSkin(&aMtrl, &dMtrl, &aMtrl, &sMtrl, 16.0f, &g_MeshSkin);
	g_pDevice->GetSkinManager()->AddTexture(g_MeshSkin, "bricka.dds", true, 1.0f, NULL, 0);	
	g_pDevice->MeshCreateTeapot(&g_Mesh);

	// Create cube
	HRESULT hr=Y_OK;
	VERTEX v[24];
	WORD   i[36];    
	memset(v, 0, sizeof(VERTEX)*24);
	memset(i, 0, sizeof(WORD)*36);
	YCOLOR d = {1.0f, 1.0f, 1.0f, 1.0f};
	YCOLOR a = {1.0f, 1.0f, 1.0f, 1.0f};
	YCOLOR s = {0.4f, 0.4f, 0.4f, 1.0f};

	g_pDevice->GetSkinManager()->AddSkin(&a, &d, &d, &s, 8.0f, &g_CubeSkin);
	g_pDevice->GetSkinManager()->AddTexture(g_CubeSkin, "crate.jpg",false,0,NULL,0);

	CreateCube(YVector(0,0,0), 10.0f, 10.0f, 10.0f, v, i, false);
	g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, g_CubeSkin , 24, 36, v, i, &g_Cube);
	return Y_OK;
} 

//-----------------------------------------------------------------------------
// Name: CreateCube
// Desc: Create a cube
//-----------------------------------------------------------------------------
void CreateCube(YVector vcP, float fW, float fH, float fD,
			VERTEX *pVerts, WORD *pIndis, bool bInv) {
      WORD     i[36] = {  3,  0,  1,   3,  1,  2,   // top
                       6,  7,  4,   6,  4,  5,   // right
                      10,  9,  8,  11, 10,  8,   // left
                      12, 13, 14,  14, 15, 12,   // back
                      18, 19, 16,  17, 18, 16,   // front
                      22, 23, 20,  20, 21, 22 }; // bottom

   WORD     iinv[36] = {  1,  0,  3,   2,  1,  3,   // top
                          4,  7,  6,   5,  4,  6,   // right
                          8,  9, 10,   8, 10, 11,   // left
                         14, 13, 12,  12, 15, 14,   // back
                         16, 19, 18,  16, 18, 17,   // front
                         20, 23, 22,  22, 21, 20 }; // bottom

   if (bInv) memcpy(pIndis, iinv, sizeof(WORD)*36);
   else      memcpy(pIndis,    i, sizeof(WORD)*36);

   memset(pVerts, 0, sizeof(LVERTEX)*24);

   // top rectangle
   pVerts[0].x = vcP.x - (fW / 2.0f);
   pVerts[0].y = vcP.y + (fH / 2.0f);
   pVerts[0].z = vcP.z - (fD / 2.0f);
   pVerts[0].tu = 0.0f;  pVerts[0].tv = 0.0f;
  

   pVerts[1].x = vcP.x - (fW / 2.0f);
   pVerts[1].y = vcP.y + (fH / 2.0f);
   pVerts[1].z = vcP.z + (fD / 2.0f);
   pVerts[1].tu = 0.0f;  pVerts[1].tv = 1.0f;
  
   pVerts[2].x = vcP.x + (fW / 2.0f);
   pVerts[2].y = vcP.y + (fH / 2.0f);
   pVerts[2].z = vcP.z + (fD / 2.0f);
   pVerts[2].tu = 1.0f;  pVerts[2].tv = 1.0f;

   pVerts[3].x = vcP.x + (fW / 2.0f);
   pVerts[3].y = vcP.y + (fH / 2.0f);
   pVerts[3].z = vcP.z - (fD / 2.0f);
   pVerts[3].tu = 1.0f;  pVerts[3].tv = 0.0f;

   if (bInv) pVerts[0].vcN[1] = -1.0f;
   else      pVerts[0].vcN[1] =  1.0f;
   pVerts[1].vcN[1] = pVerts[0].vcN[1];
   pVerts[2].vcN[1] = pVerts[0].vcN[1];
   pVerts[3].vcN[1] = pVerts[0].vcN[1];


   // right side rectanlge
   pVerts[4]    = pVerts[3];
   pVerts[4].tu = 1.0f;  
   pVerts[4].tv = 0.0f;

   pVerts[5]    = pVerts[2];
   pVerts[5].tu = 0.0f;  
   pVerts[5].tv = 0.0f;

   pVerts[6].x  = vcP.x + (fW / 2.0f);
   pVerts[6].y  = vcP.y - (fH / 2.0f);
   pVerts[6].z  = vcP.z + (fD / 2.0f);
   pVerts[6].tu = 0.0f;  pVerts[6].tv = 1.0f;

   pVerts[7].x  = vcP.x + (fW / 2.0f);
   pVerts[7].y  = vcP.y - (fH / 2.0f);
   pVerts[7].z  = vcP.z - (fD / 2.0f);
   pVerts[7].tu = 1.0f;  pVerts[7].tv = 1.0f;

   if (bInv) pVerts[4].vcN[0] = -1.0f;
   else      pVerts[4].vcN[0] =  1.0f;
   pVerts[5].vcN[0] = pVerts[4].vcN[0];
   pVerts[6].vcN[0] = pVerts[4].vcN[0];
   pVerts[7].vcN[0] = pVerts[4].vcN[0];


   // left side rectangle
   pVerts[8]    = pVerts[0];
   pVerts[8].tu = 0.0f;  
   pVerts[8].tv = 0.0f;

   pVerts[9]    = pVerts[1];
   pVerts[9].tu = 1.0f;  
   pVerts[9].tv = 0.0f;

   pVerts[10].x = vcP.x - (fW / 2.0f);
   pVerts[10].y = vcP.y - (fH / 2.0f);
   pVerts[10].z = vcP.z + (fD / 2.0f);
   pVerts[10].tu = 1.0f;  pVerts[10].tv = 1.0f;
   pVerts[11].x = vcP.x - (fW / 2.0f);
   pVerts[11].y = vcP.y - (fH / 2.0f);
   pVerts[11].z = vcP.z - (fD / 2.0f);
   pVerts[11].tu = 0.0f;  pVerts[11].tv = 1.0f;

   if (bInv) pVerts[8].vcN[0] =  1.0f;
   else      pVerts[8].vcN[0] = -1.0f;
   pVerts[9].vcN[0]  = pVerts[8].vcN[0];
   pVerts[10].vcN[0] = pVerts[8].vcN[0];
   pVerts[11].vcN[0] = pVerts[8].vcN[0];


   // back side rectanlge
   pVerts[12]    = pVerts[2];
   pVerts[12].tu = 1.0f; 
   pVerts[12].tv = 0.0f;

   pVerts[13]    = pVerts[1];
   pVerts[13].tu = 0.0f; 
   pVerts[13].tv = 0.0f;

   pVerts[14]    = pVerts[10];
   pVerts[14].tu = 0.0f; 
   pVerts[14].tv = 1.0f;

   pVerts[15]    = pVerts[6];
   pVerts[15].tu = 1.0f; 
   pVerts[15].tv = 1.0f;

   if (bInv) pVerts[12].vcN[2] = -1.0f;
   else      pVerts[12].vcN[2] =  1.0f;
   pVerts[13].vcN[2] = pVerts[12].vcN[2];
   pVerts[14].vcN[2] = pVerts[12].vcN[2];
   pVerts[15].vcN[2] = pVerts[12].vcN[2];


   // front side rectangle
   pVerts[16]    = pVerts[0];
   pVerts[16].tu = 1.0f; 
   pVerts[16].tv = 0.0f;

   pVerts[17]    = pVerts[3];
   pVerts[17].tu = 0.0f;  
   pVerts[17].tv = 0.0f;

   pVerts[18]    = pVerts[7];
   pVerts[18].tu = 0.0f; 
   pVerts[18].tv = 1.0f;

   pVerts[19]    = pVerts[11];
   pVerts[19].tu = 1.0f; 
   pVerts[19].tv = 1.0f;

   if (bInv) pVerts[16].vcN[2] =  1.0f;
   else      pVerts[16].vcN[2] = -1.0f;
   pVerts[17].vcN[2] = pVerts[16].vcN[2];
   pVerts[18].vcN[2] = pVerts[16].vcN[2];
   pVerts[19].vcN[2] = pVerts[16].vcN[2];


   // bottom side rectangle
   pVerts[20]    = pVerts[7];
   pVerts[20].tu = 1.0f; 
   pVerts[20].tv = 1.0f;

   pVerts[21]    = pVerts[6];
   pVerts[21].tu = 1.0f; 
   pVerts[21].tv = 0.0f;

   pVerts[22]    = pVerts[10];
   pVerts[22].tu = 0.0f;  
   pVerts[22].tv = 0.0f;

   pVerts[23]    = pVerts[11];
   pVerts[23].tu = 0.0f; 
   pVerts[23].tv = 1.0f;

   if (bInv) pVerts[20].vcN[1] =  1.0f;
   else      pVerts[20].vcN[1] = -1.0f;
   pVerts[21].vcN[1] = pVerts[20].vcN[1];
   pVerts[22].vcN[1] = pVerts[20].vcN[1];
   pVerts[23].vcN[1] = pVerts[20].vcN[1];

   

} // CreateCube


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