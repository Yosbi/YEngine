// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "YRenderer.h"  // our render interface
#include "Y.h"          // return values and stuff
#include "main.h"         // prototypes and stuff
#include "YEMath.h"
#include <cstdlib>		// to use the function random
#include <ctime>		// to get the random's seed

//include our library
#pragma comment(lib, "YRenderer.lib")
#pragma comment(lib, "YEMath.lib")

#define RANDOM_COLOR 0xFF000000 | ((rand() * 0xFFFFFF) / rand())

// windows stuff
HWND      g_hWnd  = NULL;
HINSTANCE g_hInst = NULL;
TCHAR     g_szAppClass[] = TEXT("FrameWorktest");

// application stuff
BOOL g_bIsActive = FALSE;
bool g_bDone     = false;

FILE *pLog=NULL;

UINT g_nFontID=0;

// renderer object
LPYRENDERER     g_pRenderer = NULL;
LPYRENDERDEVICE g_pDevice   = NULL;

UINT g_sRoom=0;

UINT g_Base[2] = { 0, 0 };



/**
 * WinMain function to get the thing started.
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow) {
   WNDCLASSEX	wndclass;
   HRESULT     hr;
   HWND		   hWnd;
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
	
   // Set the random's seed
   srand(time(0));

   // create window
   if (!(hWnd = CreateWindowEx(NULL, g_szAppClass,
	   		                   "YEngine v0.1 - By Yosbi Alves",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
                               WS_MINIMIZEBOX | WS_VISIBLE,
			 	                   GetSystemMetrics(SM_CXSCREEN)/2 -250,
                               GetSystemMetrics(SM_CYSCREEN)/2 -187,
				                   500, 375, NULL, NULL, hInst, NULL)))
      return 0;
   
   // dont show everything off yet
   ShowWindow(hWnd, SW_HIDE);
     
   g_hWnd  = hWnd;
   g_hInst = hInst;

   pLog = fopen("log_main.txt", "w");
  
   // try to start the engine
   if (FAILED( hr = ProgramStartup("Direct3D") )) {
      if (hr == Y_FAIL)
         fprintf(pLog, "error: ProgramStartup() failed\n");
      else if (hr == Y_CANCELED)
         fprintf(pLog, "error: ProgramStartup() canceled by user \n");
      
	  fflush(pLog);
      g_bDone = true;
   }
   // everything went smooth
   else {
      ShowWindow(hWnd, SW_SHOW);
	   
      if (FAILED(BuildAndSetShader())) {
         MessageBox(g_hWnd, 
                    "Sorry, pixel shaders 1.1 required to run this demo.", 
                    "ZFX Engine Report", 
                    MB_OK | MB_ICONERROR);
         g_bDone = true;
         }
	  
      if (FAILED( BuildGeometry() ))
         g_bDone = true;
      }

   YVector vR(1,0,0), vU(0,1,0), vD(0,-0,1), vP(0,0,-15);
   vD.Normalize();
   g_pDevice->SetView3D(vR,vU,vD,vP);
   g_pDevice->SetClearColor(1.0f,1.0f, 1.0f);

   // main loop
   while (!g_bDone) {
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
   } // WinMain
/*----------------------------------------------------------------*/


/**
 * MsgProc to deal with incoming windows messages.
 */
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
/*----------------------------------------------------------------*/
 

/**
 * Create a render device and stuff. 
 */
HRESULT ProgramStartup(char *chAPI) {
   // no opengl render device yet...
   if (strcmp(chAPI, "OpenGL")==0) return S_OK;

   // create a render objekt
   g_pRenderer = new YRenderer(g_hInst);
   
   // create a device for the chosen api
   if (FAILED( g_pRenderer->CreateDevice(chAPI) )) return E_FAIL;
   
   // get a pointer on that device
   g_pDevice = g_pRenderer->GetDevice();
   if (g_pDevice == NULL) return E_FAIL;

   // init render device
   if (FAILED( g_pDevice->Init(g_hWnd, NULL, 0, true, true ) )) {
      fprintf(pLog, "error: Init() failed in ProgramStartup()\n");
      return E_FAIL;
      }

   //g_pDevice->UseWindow(0);
   //if (!g_pDevice->UsesTextures())
   //{
	  // g_pDevice->UseTextures(true);
	  //// MessageBox(g_hWnd," Todo va bien", "Todo va bien", MB_OK | MB_ICONERROR);
   //}
   
   POINT ptRes; int fs=0;
   g_pDevice->GetResolution(&ptRes);

   // font size
   fs  = ptRes.x / 40;

   // prepare viewport stage
   g_pDevice->InitStage(0.8f, NULL, 0);

   if (FAILED( g_pDevice->CreateFont( 0, false, false,
                                     false, fs, &g_nFontID) )) {
      fprintf(pLog, "error: YRenderDevice::CreateFont failed\n");
      return Y_FAIL;
      }
   //g_pDevice->UseShaders(true);
   return Y_OK;
   } // ProgramStartup
/*----------------------------------------------------------------*/


/**
 * Release the render device and stuff.
 */
HRESULT ProgramCleanup(void) {
   if (g_pRenderer) {
      delete g_pRenderer;
      g_pRenderer = NULL;
      }

   if (pLog)
      fclose(pLog);

   return S_OK;
   } // ProgramCleanup
/*----------------------------------------------------------------*/


/**
 * Do one frame.
 */
HRESULT ProgramTick(void) {
   HRESULT hr = Y_FAIL;
   YMatrix mat;
   mat.Identity();
   //g_pDevice->UseShaders(true);
   
   // clear buffers and start scene
   g_pDevice->BeginRendering(true,true,true);
   
   Render(-1);  // ambient

   //g_pDevice->UseShaders(false);
   g_pDevice->DrawText(g_nFontID, 55, 20, 0, 200, 0, 
                       "Y-Engine Demo: Color cube effect");

   // flip backbuffer to front
   g_pDevice->EndRendering();
   return hr;
   } // Tick
/*----------------------------------------------------------------*/


/**
 * Renders the scene.
 */
HRESULT Render(int n) {
   YMatrix mat;
   mat.Identity();
   //mat.RotaX(-0.78f);
   mat.Rota(-0.78, 0.78, 0);
 /*  mat._42 -= 0.5f;
   mat._41 -= 1.5f;*/

   //// circle ambient light color
   //float fT = GetTickCount() / 1000.0f;
   //float fR = 0.5f + 0.5f * sinf(fT*1.2f);
   //float fG = 0.5f + 0.5f * sinf(fT*2.0f);
   //float fB = 0.5f + 0.5f * sinf(fT*1.7f);

   //g_pDevice->SetAmbientLight(fR, fG, fB);
   g_pDevice->SetWorldTransform(&mat);

   // ambient pass
  /* g_pDevice->ActivateVShader(g_Base[0], VID_TV);
   g_pDevice->ActivatePShader(g_Base[1]);*/
   g_pDevice->ActivateFX(1);

   return g_pDevice->GetVertexManager()->Render(g_sRoom);
   } // Render
/*----------------------------------------------------------------*/



HRESULT BuildAndSetShader(void) {
	
   //if (!g_pDevice->CanDoShaders()) return E_FAIL;
	if (FAILED(g_pDevice->CreateFX("color.fx", "TransformTech", true, NULL)))
		Log("no lo cargo :S");
  /* g_pDevice->CreateVShader("base.vsh", 292,
                            true, false, 
                            &g_Base[0]);
   
   g_pDevice->CreatePShader("base.psh", 130,
                            true, false, 
                            &g_Base[1]);*/
   //
   return Y_OK;
   } // BuildAndSetShader
/*----------------------------------------------------------------*/


/**
 * Create geometry needed for the scene along with textures and stuff.
 */
HRESULT BuildGeometry(void) {
   HRESULT hr=Y_OK;
   LVERTEX v[24];
   WORD    i[36];
   UINT    s=-99;

   memset(v, 0, sizeof(LVERTEX)*24);
   memset(i, 0, sizeof(WORD)*36);
   
   YCOLOR c = { 1.0f, 1.0f, 1.0f, 1.0f };
   YCOLOR d = { 1.0f, 0.0f, 0.0f, 1.0f };

   if(FAILED(g_pDevice->GetSkinManager()->AddSkin(&c, &c, &d, &c, 1, &s)))
   {}
   

  // g_pDevice->GetSkinManager()->AddTexture(s, "texture.bmp", false, 0, NULL, 0);
   // geometry for the room
   CreateCube(YVector(0,0,0), 10.0f, 7.0f, 10.0f, v, i, false);
   //test
		
		
    if (FAILED((g_pDevice->GetVertexManager()->CreateStaticBuffer(
                           VID_UL, s, 24, 36,
						   v, i, &g_sRoom))))
	{
		Log("fallo");
	}

	return Y_OK;
   } // BuildGeometry
/*----------------------------------------------------------------*/


/**
 * Create geometry for a cube, invert face orientation if needed.
 */
void CreateCube(YVector vcP, float fW, float fH, float fD,
                LVERTEX *pVerts, WORD *pIndis, bool bInv) {
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
   pVerts[0].Color = (DWORD)RANDOM_COLOR;

   pVerts[1].x = vcP.x - (fW / 2.0f);
   pVerts[1].y = vcP.y + (fH / 2.0f);
   pVerts[1].z = vcP.z + (fD / 2.0f);
   pVerts[1].tu = 0.0f;  pVerts[1].tv = 1.0f;
   pVerts[1].Color = (DWORD)RANDOM_COLOR;

   pVerts[2].x = vcP.x + (fW / 2.0f);
   pVerts[2].y = vcP.y + (fH / 2.0f);
   pVerts[2].z = vcP.z + (fD / 2.0f);
   pVerts[2].tu = 1.0f;  pVerts[2].tv = 1.0f;
   pVerts[2].Color = (DWORD)RANDOM_COLOR;

   pVerts[3].x = vcP.x + (fW / 2.0f);
   pVerts[3].y = vcP.y + (fH / 2.0f);
   pVerts[3].z = vcP.z - (fD / 2.0f);
   pVerts[3].tu = 1.0f;  pVerts[3].tv = 0.0f;
   pVerts[3].Color = (DWORD)RANDOM_COLOR;

  /* if (bInv) pVerts[0].vcN[1] = -1.0f;
   else      pVerts[0].vcN[1] =  1.0f;
   pVerts[1].vcN[1] = pVerts[0].vcN[1];
   pVerts[2].vcN[1] = pVerts[0].vcN[1];
   pVerts[3].vcN[1] = pVerts[0].vcN[1];*/


   // right side rectanlge
   pVerts[4]    = pVerts[3];
   pVerts[4].tu = 1.0f;  
   pVerts[4].tv = 0.0f;
   pVerts[4].Color = (DWORD)RANDOM_COLOR;

   pVerts[5]    = pVerts[2];
   pVerts[5].tu = 0.0f;  
   pVerts[5].tv = 0.0f;
   pVerts[5].Color = (DWORD)RANDOM_COLOR;

   pVerts[6].x  = vcP.x + (fW / 2.0f);
   pVerts[6].y  = vcP.y - (fH / 2.0f);
   pVerts[6].z  = vcP.z + (fD / 2.0f);
   pVerts[6].tu = 0.0f;  pVerts[6].tv = 1.0f;
   pVerts[6].Color = (DWORD)RANDOM_COLOR;

   pVerts[7].x  = vcP.x + (fW / 2.0f);
   pVerts[7].y  = vcP.y - (fH / 2.0f);
   pVerts[7].z  = vcP.z - (fD / 2.0f);
   pVerts[7].tu = 1.0f;  pVerts[7].tv = 1.0f;
   pVerts[7].Color = (DWORD)RANDOM_COLOR;

   /*if (bInv) pVerts[4].vcN[0] = -1.0f;
   else      pVerts[4].vcN[0] =  1.0f;
   pVerts[5].vcN[0] = pVerts[4].vcN[0];
   pVerts[6].vcN[0] = pVerts[4].vcN[0];
   pVerts[7].vcN[0] = pVerts[4].vcN[0];
*/

   // left side rectangle
   pVerts[8]    = pVerts[0];
   pVerts[8].tu = 0.0f;  
   pVerts[8].tv = 0.0f;
   pVerts[8].Color = (DWORD)RANDOM_COLOR;

   pVerts[9]    = pVerts[1];
   pVerts[9].tu = 1.0f;  
   pVerts[9].tv = 0.0f;
   pVerts[9].Color = (DWORD)RANDOM_COLOR;

   pVerts[10].x = vcP.x - (fW / 2.0f);
   pVerts[10].y = vcP.y - (fH / 2.0f);
   pVerts[10].z = vcP.z + (fD / 2.0f);
   pVerts[10].tu = 1.0f;  pVerts[10].tv = 1.0f;
   pVerts[10].Color =(DWORD)RANDOM_COLOR;

   pVerts[11].x = vcP.x - (fW / 2.0f);
   pVerts[11].y = vcP.y - (fH / 2.0f);
   pVerts[11].z = vcP.z - (fD / 2.0f);
   pVerts[11].tu = 0.0f;  pVerts[11].tv = 1.0f;
   pVerts[11].Color = (DWORD)RANDOM_COLOR;

   /*if (bInv) pVerts[8].vcN[0] =  1.0f;
   else      pVerts[8].vcN[0] = -1.0f;
   pVerts[9].vcN[0]  = pVerts[8].vcN[0];
   pVerts[10].vcN[0] = pVerts[8].vcN[0];
   pVerts[11].vcN[0] = pVerts[8].vcN[0];*/


   // back side rectanlge
   pVerts[12]    = pVerts[2];
   pVerts[12].tu = 1.0f; 
   pVerts[12].tv = 0.0f;
   pVerts[12].Color = (DWORD)RANDOM_COLOR;

   pVerts[13]    = pVerts[1];
   pVerts[13].tu = 0.0f; 
   pVerts[13].tv = 0.0f;
   pVerts[13].Color = (DWORD)RANDOM_COLOR;

   pVerts[14]    = pVerts[10];
   pVerts[14].tu = 0.0f; 
   pVerts[14].tv = 1.0f;
   pVerts[14].Color = (DWORD)RANDOM_COLOR;

   pVerts[15]    = pVerts[6];
   pVerts[15].tu = 1.0f; 
   pVerts[15].tv = 1.0f;
   pVerts[15].Color = (DWORD)RANDOM_COLOR;

  /* if (bInv) pVerts[12].vcN[2] = -1.0f;
   else      pVerts[12].vcN[2] =  1.0f;
   pVerts[13].vcN[2] = pVerts[12].vcN[2];
   pVerts[14].vcN[2] = pVerts[12].vcN[2];
   pVerts[15].vcN[2] = pVerts[12].vcN[2];*/


   // front side rectangle
   pVerts[16]    = pVerts[0];
   pVerts[16].tu = 1.0f; 
   pVerts[16].tv = 0.0f;
   pVerts[16].Color = (DWORD)RANDOM_COLOR;

   pVerts[17]    = pVerts[3];
   pVerts[17].tu = 0.0f;  
   pVerts[17].tv = 0.0f;
   pVerts[17].Color = (DWORD)RANDOM_COLOR;

   pVerts[18]    = pVerts[7];
   pVerts[18].tu = 0.0f; 
   pVerts[18].tv = 1.0f;
   pVerts[18].Color = (DWORD)RANDOM_COLOR;

   pVerts[19]    = pVerts[11];
   pVerts[19].tu = 1.0f; 
   pVerts[19].tv = 1.0f;
   pVerts[19].Color = (DWORD)RANDOM_COLOR;

   /*if (bInv) pVerts[16].vcN[2] =  1.0f;
   else      pVerts[16].vcN[2] = -1.0f;
   pVerts[17].vcN[2] = pVerts[16].vcN[2];
   pVerts[18].vcN[2] = pVerts[16].vcN[2];
   pVerts[19].vcN[2] = pVerts[16].vcN[2];*/


   // bottom side rectangle
   pVerts[20]    = pVerts[7];
   pVerts[20].tu = 1.0f; 
   pVerts[20].tv = 1.0f;
   pVerts[20].Color = (DWORD)RANDOM_COLOR;

   pVerts[21]    = pVerts[6];
   pVerts[21].tu = 1.0f; 
   pVerts[21].tv = 0.0f;
   pVerts[21].Color = (DWORD)RANDOM_COLOR;

   pVerts[22]    = pVerts[10];
   pVerts[22].tu = 0.0f;  
   pVerts[22].tv = 0.0f;
   pVerts[22].Color = (DWORD)RANDOM_COLOR;

   pVerts[23]    = pVerts[11];
   pVerts[23].tu = 0.0f; 
   pVerts[23].tv = 1.0f;
   pVerts[23].Color = (DWORD)RANDOM_COLOR;

  /* if (bInv) pVerts[20].vcN[1] =  1.0f;
   else      pVerts[20].vcN[1] = -1.0f;
   pVerts[21].vcN[1] = pVerts[20].vcN[1];
   pVerts[22].vcN[1] = pVerts[20].vcN[1];
   pVerts[23].vcN[1] = pVerts[20].vcN[1];*/

   

   } // CreateCube
/*----------------------------------------------------------------*/


/**
 * Convert error code to generic text string.
 */
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
   } // HrToStr
/*----------------------------------------------------------------*/


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