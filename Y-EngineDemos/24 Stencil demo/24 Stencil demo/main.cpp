// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "../../../Y-Engine/Renderer & Math/YRenderer/YRenderer.h"
#include "../../../Y-Engine/Renderer & Math/YD3D/Y.h"
#include "main.h"       // prototypes and stuff
#include "../../../Y-Engine/Renderer & Math/YEMath/YEMath.h"
#include "../../../Y-Engine/YInput/YInput/YInput.h"
#include "../../../Y-Engine/YInput/YInput/YInputDevice.h"
#include <d3dx9.h>

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

UINT g_sFloor  = 0;
UINT g_sWall   = 0;
UINT g_sMirror = 0;
UINT g_sMirrorMask = 0;
UINT g_sTeapot = 0, g_sTeapotSkin = 0; 
UINT g_sShadowSkin = 0;

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
	if (!(hWnd = CreateWindowEx(NULL, g_szAppClass, "YEngine v0.1 - By Yosbi Alves(yosbito@gmail.com)",
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
	   
		if (FAILED(BuildAndSetFX())) 
		{
			MessageBox(g_hWnd, "Sorry, pixel shaders 2.0 required to run this demo.", "Y-Engine Report", MB_OK | MB_ICONERROR);
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
    g_pDevice->ActivateFX(1);

	Input(TimeElapsed);
	Render(TimeElapsed);  // ambient

	//g_pDevice->UseShaders(false);
	g_pDevice->DrawText(g_nFontID, 55, 20, 0, 200, 0, "Y-Engine Demo: Stencil mirror Demo");
	g_pDevice->DrawText(g_nFontID, 55, 55, 0, 200, 0, "By: Yosbi Alves (yosbito@gmail.com)");

	// flip backbuffer to front
	g_pDevice->EndRendering();
	return hr;
} 

void Input(float TimeElapsed)
{
	// get the imputs to control the camera
	g_pYInputDevice->Update();
	
	// rotation
	static POINT sump  = {0,0};
	static float fBackForw = 50.0f;
	POINT p = g_pYInputDevice->GetMouseDelta();
	sump.x += p.x;
	fBackForw += p.y;

	if (fBackForw < 3.0f)
		fBackForw =  3.0f;

	// up & down
	static float up = 5.0f;
	if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, YVK_W))
	{
		up +=0.1f;
		/*if (up > 50.0f)
			up = 50.0f;*/
	}
	if (g_pYInputDevice->IsPressed(IDV_KEYBOARD, YVK_S))
	{
		up -=0.1f;
		/*if (up < 5.0f)
			up = 5.0f;*/
	}

	YMatrix vmat;
	vmat.Identity();
	vmat.RotaY(-0.4f * ((sump.x * 3.14)/180));   
	g_pDevice->SetViewLookAt(YVector(0 ,up, 0.2f * (-fBackForw)) * vmat, YVector(0,0,0), YVector(0,1,0));

}

//-----------------------------------------------------------------------------
// Name: DrawShadow
// Desc: Draw the teapot shadow
//-----------------------------------------------------------------------------
void DrawShadow(YMatrix wmMatrix, YPlane Plane, YVector vcLight, UINT meshIndex)
{
	// First clean the stencil buffer
	g_pDevice->Clear(false, false, true);
	
	// Set the stencil states
	g_pDevice->SetStencilBufferMode(RS_STENCIL_ENABLE, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_FUNC_EQUAL, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_REF, 0X0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_MASK, 0Xffffff);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_WRITEMASK, 0Xffffff);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_ZFAIL_KEEP, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_FAIL_KEEP, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_PASS_INCR, 0);

	YMatrix S;
	S.Shadow(&Plane, &vcLight);
	/*YPlane Plane;
	Plane.m_vcN.x = 0.0f; Plane.m_vcN.y = 1.0f; Plane.m_vcN.z = 0.0f; Plane.m_fD = 0.0f;*/

	// Offset the shadow
	YMatrix offset;
	offset.Translate(0.0f, 0.01f, 0.0f);

	wmMatrix = wmMatrix * S * offset;
	g_pDevice->SetWorldTransform(&wmMatrix);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &wmMatrix, 0);
	
	YMatrix mat2;
	mat2.InverseOf(wmMatrix);
	mat2.TransposeOf(mat2);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat2, 0);
	g_pDevice->MeshRender(g_sTeapot, g_sShadowSkin);

	g_pDevice->SetStencilBufferMode(RS_STENCIL_DISABLE, 0);

}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Render the scene
//-----------------------------------------------------------------------------
HRESULT Render(float TimeElapsed)
{
	// Set the scene light
	YCOLOR  cLight	= {1.0f, 1.0f, 1.0f, 1.0f};
	YCOLOR  cLightspec	= {0.7f, 0.7f, 0.7f, 1.0f};
	YVector vcLight	= YVector(1.0f, 1.0f, -1.0f);
	vcLight.Normalize();

	g_pDevice->SetFXConstant(DAT_OTHER, "gLightVecW", &vcLight, sizeof(float) * 3);
	g_pDevice->SetFXConstant(DAT_OTHER, "gDiffuseLight", &cLight, sizeof(YCOLOR));
	g_pDevice->SetFXConstant(DAT_OTHER, "gSpecularLight", &cLightspec, sizeof(YCOLOR));
	
	// render floor
	YMatrix mat;
	mat.Identity();  
	g_pDevice->SetWorldTransform(&mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &mat, 0);
	
	mat.InverseOf(mat);
	mat.TransposeOf(mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat, 0);	
	
	g_pDevice->GetVertexManager()->Render(g_sFloor);

	// render wall
	mat.Identity();
	//mat.RotaX(3.1415f / 2); 
	//mat.Rota(3.1415f / 2, 3.1415f, 0);
	////mat.Translate(0,0.001f,0);
	g_pDevice->SetWorldTransform(&mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &mat, 0);
	
	mat.InverseOf(mat);
	mat.TransposeOf(mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat, 0);
	
	g_pDevice->SetBackfaceCulling(RS_CULL_NONE);
	g_pDevice->GetVertexManager()->Render(g_sWall);
	g_pDevice->SetBackfaceCulling(RS_CULL_CCW);	
	// Render teapot
	static YMatrix mTeapot;
	mTeapot.Identity();
	mTeapot.Translate(0.0f, 5.0f, -6.0f);
	g_pDevice->SetWorldTransform(&mTeapot);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &mTeapot, 0);
	
	YMatrix mat2;
	mat2.InverseOf(mTeapot);
	mat2.TransposeOf(mat2);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat2, 0);
	g_pDevice->MeshRender(g_sTeapot, g_sTeapotSkin);

	// Render the mirror
	mat.Identity();
	mat.Translate(0.0f, 0.0f, -0.03f);
	g_pDevice->SetWorldTransform(&mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &mat, 0);
	
	mat.InverseOf(mat);
	mat.TransposeOf(mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat, 0);
	
	g_pDevice->GetVertexManager()->Render(g_sMirror);

	// Draw the shadow //////////////////
	YPlane Plane;
	Plane.m_vcN.x = 0.0f; Plane.m_vcN.y = 1.0f; Plane.m_vcN.z = 0.0f; Plane.m_fD = 0.0f;
	DrawShadow(mTeapot, Plane, vcLight, g_sTeapot);
		
	// Draw mirrowed things
	DrawMirrowed();
	
	
	return Y_OK;
}
//-----------------------------------------------------------------------------
// Name: DrawMirrowed
// Desc:  Render mirror and mirrowed things
//-----------------------------------------------------------------------------
void DrawMirrowed()
{
	// First clean the stencil buffer
	g_pDevice->Clear(false, false, true);
	YMatrix mat;

	static YMatrix mTeapot;
	mTeapot.Identity();
	mTeapot.Translate(0.0f, 5.0f, -6.0f);

	YVector vcLight	= YVector(1.0f, 1.0f, -1.0f);
	vcLight.Normalize();

	// Set the render states to write in the stencil buffer
	g_pDevice->SetStencilBufferMode(RS_STENCIL_ENABLE, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_FUNC_ALWAYS, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_REF, 0x1);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_MASK, 0xffffffff);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_WRITEMASK, 0xffffffff);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_ZFAIL_KEEP, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_FAIL_KEEP, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_PASS_REPLACE, 0);

	// Render the mask mirror to stencil buffer
	mat.Identity();
	mat.Translate(0.0f, 0.0f, -0.04f);
	g_pDevice->SetWorldTransform(&mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &mat, 0);
	
	mat.InverseOf(mat);
	mat.TransposeOf(mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat, 0);

	g_pDevice->SetDepthBufferMode(RS_DEPTH_READONLY);
	g_pDevice->UseColorBuffer(false);	

	g_pDevice->GetVertexManager()->Render(g_sMirrorMask);

	g_pDevice->UseColorBuffer(true);
	g_pDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);

	// Stop writing in the stencil buffer
	g_pDevice->SetStencilBufferMode(RS_STENCIL_FUNC_EQUAL, 0);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_PASS_KEEP, 0);

	// Draw reflected teapot/////////////777
	// Build reflection transformation
	YMatrix R;
	YPlane  plane; plane.m_vcN = YVector(0.0f, 0.0f, 1.0f); plane.m_fD = 0.0f;
	R.Reflect(&plane);

	// Save the original teapot world matrix
	/*static YMatrix mTeapotR;*/

	// Add reflection transform
	mTeapot = mTeapot * R;

	// Reflect light vector also
	YVector vcLightR = vcLight;
	vcLightR = vcLightR * R;
	vcLightR.Normalize();
	g_pDevice->SetFXConstant(DAT_OTHER, "gLightVecW", &vcLightR, sizeof(float) * 3);

	// Disable depth buffer and render the reflected teapot
	g_pDevice->SetDepthBufferMode(RS_DEPTH_NONE);	
	g_pDevice->SetBackfaceCulling(RS_CULL_CW);

	// Render floor
	mat.Identity();
	mat = mat * R;
	g_pDevice->SetWorldTransform(&mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &mat, 0);
	
	mat.InverseOf(mat);
	mat.TransposeOf(mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat, 0);	
	
	g_pDevice->GetVertexManager()->Render(g_sFloor);

	// finally render the teapot
	g_pDevice->SetWorldTransform(&mTeapot);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorld", &mTeapot, 0);
	
	mat.InverseOf(mTeapot);
	mat.TransposeOf(mat);
	g_pDevice->SetFXConstant(DAT_MATRIX, "gWorldInverseTranspose", &mat, 0);
	g_pDevice->MeshRender(g_sTeapot, g_sTeapotSkin);

	// Clean buffers
	g_pDevice->SetDepthBufferMode(RS_DEPTH_READWRITE);
	g_pDevice->SetStencilBufferMode(RS_STENCIL_DISABLE, 0);
	g_pDevice->SetBackfaceCulling(RS_CULL_CCW);
	
}

//-----------------------------------------------------------------------------
// Name: BuildAndSetFX
// Desc: Load the FX to the engine
//-----------------------------------------------------------------------------
HRESULT BuildAndSetFX() 
{	
	 return  g_pDevice->CreateFX("dirLightTex.fx", "DirLightTexTech", true, NULL);		
} 

//-----------------------------------------------------------------------------
// Name: BuildGeometry
// Desc: Build the geometry to render
//-----------------------------------------------------------------------------
HRESULT BuildGeometry(void) 
{
	HRESULT hr=Y_OK;
	VERTEX vert[6];

	YCOLOR diff = { 1.0f, 1.0f, 1.0f, 1.0f };
	YCOLOR ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	YCOLOR spec = { 0.35f, 0.35f, 0.35f, 1.0f };
	float fSpec = 16.0f;
	UINT sk1 = -99;
	UINT sk2 = -99;
	UINT sk3 = -99;
	UINT sk4 = -99;

	// Generating floor
	vert[0].x = -7.5f; vert[0].y = 0.0f; vert[0].z = -10.0f;
	vert[0].vcN[0] = 0.0f; vert[0].vcN[1] = 1.0f; vert[0].vcN[2] = 0.0f;
	vert[0].tu = 0.0f; vert[0].tv = 4.0f;

	vert[1].x = -7.5f; vert[1].y = 0.0f; vert[1].z = 0.0f;
	vert[1].vcN[0] = 0.0f; vert[1].vcN[1] = 1.0f; vert[1].vcN[2] = 0.0f;
	vert[1].tu = 0.0f; vert[1].tv = 0.0f;

	vert[2].x = 7.5f; vert[2].y = 0.0f; vert[2].z = 0.0f;
	vert[2].vcN[0] = 0.0f; vert[2].vcN[1] = 1.0f; vert[2].vcN[2] = 0.0f;
	vert[2].tu = 4.0f; vert[2].tv = 0.0f;
	
	vert[3].x = -7.5f; vert[3].y = 0.0f; vert[3].z = -10.0f;
	vert[3].vcN[0] = 0.0f; vert[3].vcN[1] = 1.0f; vert[3].vcN[2] = 0.0f;
	vert[3].tu = 0.0f; vert[3].tv = 4.0f;

	vert[4].x = 7.5f; vert[4].y = 0.0f; vert[4].z = 0.0f;
	vert[4].vcN[0] = 0.0f; vert[4].vcN[1] = 1.0f; vert[4].vcN[2] = 0.0f;
	vert[4].tu = 4.0f; vert[4].tv = 0.0f;

	vert[5].x = 7.5f; vert[5].y = 0.0f; vert[5].z = -10.0f;
	vert[5].vcN[0] = 0.0f; vert[5].vcN[1] = 1.0f; vert[5].vcN[2] = 0.0f;
	vert[5].tu = 4.0f; vert[5].tv = 4.0f;

	g_pDevice->GetSkinManager()->AddSkin(&ambient, &diff, &diff, &spec, 8.0f, &sk1);
	g_pDevice->GetSkinManager()->AddTexture(sk1, "tiles.jpg", false, 1.0f, NULL, 0);	
	g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, sk1, 6, 0, vert, NULL, &g_sFloor);
	
	// Wall 
	vert[0].x = 7.5f; vert[0].y = 10.0f; vert[0].z = 0.0f;
	vert[0].vcN[0] = 0.0f; vert[0].vcN[1] = 1.0f; vert[0].vcN[2] = 0.0f;
	vert[0].tu = 0.0f; vert[0].tv = 4.0f;

	vert[1].x = 7.5f; vert[1].y = 0.0f; vert[1].z = 0.0f;
	vert[1].vcN[0] = 0.0f; vert[1].vcN[1] = 01.0f; vert[1].vcN[2] = 0.0f;
	vert[1].tu = 0.0f; vert[1].tv = 0.0f;

	vert[2].x = -7.5f; vert[2].y = 0.0f; vert[2].z = 0.0f;
	vert[2].vcN[0] = 0.0f; vert[2].vcN[1] = 1.0f; vert[2].vcN[2] = 0.0f;
	vert[2].tu = 4.0f; vert[2].tv = 0.0f;
	
	vert[3].x = 7.5f; vert[3].y = 10.0f; vert[3].z = 0.0f;
	vert[3].vcN[0] = 0.0f; vert[3].vcN[1] = 1.0f; vert[3].vcN[2] = 0.0f;
	vert[3].tu = 0.0f; vert[3].tv = 4.0f;

	vert[4].x = -7.5f; vert[4].y = 0.0f; vert[4].z = 0.0f;
	vert[4].vcN[0] = 0.0f; vert[4].vcN[1] = 1.0f; vert[4].vcN[2] = 0.0f;
	vert[4].tu = 4.0f; vert[4].tv = 0.0f;

	vert[5].x = -7.5f; vert[5].y = 10.0f; vert[5].z = 0.0f;
	vert[5].vcN[0] = 0.0f; vert[5].vcN[1] = 1.0f; vert[5].vcN[2] = 0.0f;
	vert[5].tu = 4.0f; vert[5].tv = 4.0f;

	g_pDevice->GetSkinManager()->AddSkin(&ambient, &diff, &diff, &spec, 8.0f, &sk2);
	g_pDevice->GetSkinManager()->AddTexture(sk2, "brick2.dds", false, 1.0f, NULL, 0);	
	g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, sk2, 6, 0, vert, NULL, &g_sWall);

	// Creating mirror////////////////////////////////////////////77	
	vert[0].x = -2.5f; vert[0].y = 0.0f; vert[0].z = 0.0f;
	vert[0].vcN[0] = 0.0f; vert[0].vcN[1] = 0.0f; vert[0].vcN[2] = -1.0f;
	vert[0].tu = 0.0f; vert[0].tv = 1.0f;

	vert[1].x = -2.5f; vert[1].y = 10.0f; vert[1].z = 0.0f;
	vert[1].vcN[0] = 0.0f; vert[1].vcN[1] = 0.0f; vert[1].vcN[2] = -1.0f;
	vert[1].tu = 0.0f; vert[1].tv = 0.0f;

	vert[2].x = 2.5f; vert[2].y = 10.0f; vert[2].z = 0.0f;
	vert[2].vcN[0] = 0.0f; vert[2].vcN[1] = 0.0f; vert[2].vcN[2] = -1.0f;
	vert[2].tu = 1.0f; vert[2].tv = 0.0f;
	
	vert[3].x = -2.5f; vert[3].y = 0.0f; vert[3].z = 0.0f;
	vert[3].vcN[0] = 0.0f; vert[3].vcN[1] = 0.0f; vert[3].vcN[2] = -1.0f;
	vert[3].tu = 0.0f; vert[3].tv = 1.0f;

	vert[4].x = 2.5f; vert[4].y = 10.0f; vert[4].z = 0.0f;
	vert[4].vcN[0] = 0.0f; vert[4].vcN[1] = 0.0f; vert[4].vcN[2] = -1.0f;
	vert[4].tu = 1.0f; vert[4].tv = 0.0f;

	vert[5].x = 2.5f; vert[5].y = 0.0f; vert[5].z = 0.0f;
	vert[5].vcN[0] = 0.0f; vert[5].vcN[1] = 0.0f; vert[5].vcN[2] = -1.0f;
	vert[5].tu = 1.0f; vert[5].tv = 1.0f;
	YCOLOR specM = { 0.7f, 0.7f, 0.7f, 1.0f };
	g_pDevice->GetSkinManager()->AddSkin(&ambient, &diff, &diff, &specM, fSpec, &sk3);
	g_pDevice->GetSkinManager()->AddTexture(sk3, "mirror.jpg", false, 1.0f, NULL, 0);
	g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, sk3, 6, 0, vert, NULL, &g_sMirror);

	// Creating mirror mask////////////////////////////////////////////77	
	vert[0].x = -2.06f; vert[0].y = 0.6f; vert[0].z = 0.0f;
	vert[0].vcN[0] = 0.0f; vert[0].vcN[1] = 0.0f; vert[0].vcN[2] = -1.0f;
	vert[0].tu = 0.0f; vert[0].tv = 0.0f;

	vert[1].x = -2.06f; vert[1].y = 9.4f; vert[1].z = 0.0f;
	vert[1].vcN[0] = 0.0f; vert[1].vcN[1] = 0.0f; vert[1].vcN[2] = -1.0f;
	vert[1].tu = 0.0f; vert[1].tv = 0.0f;

	vert[2].x = 2.06f; vert[2].y = 9.4f; vert[2].z = 0.0f;
	vert[2].vcN[0] = 0.0f; vert[2].vcN[1] = 0.0f; vert[2].vcN[2] = -1.0f;
	vert[2].tu = 0.0f; vert[2].tv = 0.0f;
	
	vert[3].x = -2.06f; vert[3].y = 0.6f; vert[3].z = 0.0f;
	vert[3].vcN[0] = 0.0f; vert[3].vcN[1] = 0.0f; vert[3].vcN[2] = -1.0f;
	vert[3].tu = 0.0f; vert[3].tv = 0.0f;

	vert[4].x = 2.06f; vert[4].y = 9.4f; vert[4].z = 0.0f;
	vert[4].vcN[0] = 0.0f; vert[4].vcN[1] = 0.0f; vert[4].vcN[2] = -1.0f;
	vert[4].tu = 0.0f; vert[4].tv = 0.0f;

	vert[5].x = 2.06f; vert[5].y = 0.6f; vert[5].z = 0.0f;
	vert[5].vcN[0] = 0.0f; vert[5].vcN[1] = 0.0f; vert[5].vcN[2] = -1.0f;
	vert[5].tu = 0.0f; vert[5].tv = 0.0f;

	g_pDevice->GetSkinManager()->AddSkin(&ambient, &diff, &diff, &specM, fSpec, &sk4);
	//g_pDevice->GetSkinManager()->AddTexture(sk3, "mirror.jpg", false, 1.0f, NULL, 0);
	g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, sk4, 6, 0, vert, NULL, &g_sMirrorMask);

	// Creating teapot
	spec.fR = spec.fG = spec.fB = 1.0f;
	g_pDevice->GetSkinManager()->AddSkin(&ambient, &diff, &diff, &spec, 16.0f, &g_sTeapotSkin);
	g_pDevice->GetSkinManager()->AddTexture(g_sTeapotSkin, "Texture_by_FE_Caruso.jpg", false, 1.0f, NULL, 0);
	g_pDevice->MeshCreateTeapot(&g_sTeapot);

	// Creating shadow skin
	// Create the 50% black transparent material
	YMATERIAL mat;
	mat.cAmbient.fR = 0.0f; mat.cAmbient.fG = 0.0f; mat.cAmbient.fB = 0.0f; mat.cAmbient.fA = 0.5f; 
	mat.cDiffuse.fR = 0.0f; mat.cDiffuse.fG = 0.0f; mat.cDiffuse.fB = 0.0f; mat.cDiffuse.fA = 0.5f; 
	mat.cEmissive.fR = 0.0f; mat.cEmissive.fG = 0.0f; mat.cEmissive.fB = 0.0f; mat.cEmissive.fA = 0.5f; 
	mat.cSpecular.fR = 0.0f; mat.cSpecular.fG = 0.0f; mat.cSpecular.fB = 0.0f; mat.cSpecular.fA = 0.5f; 
	mat.fPower = 1.0f;
	g_pDevice->GetSkinManager()->AddSkin(&mat.cAmbient, &mat.cDiffuse, &mat.cEmissive, &mat.cSpecular, mat.fPower, &g_sShadowSkin);


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

	float texScale = 0.2f; // scale the texture cood
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
			verts[k].tu = i * texScale;
			verts[k].tv = j * texScale;

			++k; // Next vertex
		}
	}

	// Set the texture coord

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

	YCOLOR diff = { 1.0f, 1.0f, 1.0f, 1.0f };
	YCOLOR ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	YCOLOR spec = { 0.4f, 0.4f, 0.4f, 1.0f };
	float fSpec = 8.0f;
	UINT z = -99;
	// Floor
	g_pDevice->GetSkinManager()->AddSkin(&ambient, &diff, &diff, &spec, fSpec, &z);
	g_pDevice->GetSkinManager()->AddTexture(z, "wood-floorboards.dds", false, 0.0f, NULL, 0);	
	g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, z, *nVerts, *nIndis, verts, indices, &g_sFloor);
	
	// Wall
	g_pDevice->GetSkinManager()->AddSkin(&ambient, &diff, &diff, &spec, fSpec, &z);
	g_pDevice->GetSkinManager()->AddTexture(z, "brick2.dds", false, 0.0f, NULL, 0);	
	g_pDevice->GetVertexManager()->CreateStaticBuffer(VID_UU, z, *nVerts, *nIndis, verts, indices, &g_sWall);

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