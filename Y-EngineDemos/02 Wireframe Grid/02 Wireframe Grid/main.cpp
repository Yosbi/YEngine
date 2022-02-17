// FILE: main.cpp

#define WIN32_MEAN_AND_LEAN

#include "YRenderer.h"  // our render interface
#include "Y.h"          // return values and stuff
#include "main.h"         // prototypes and stuff
#include "YEMath.h"



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

   YVector vR(1,0,0), vU(0,1,0), vD(0,0,1), vP(0,0,-15);
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
                       "Y-Engine Demo: Wireframe Grid Demo");

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
   mat.RotaY(-0.4f);
   mat._42 -= 0.5f;
   mat._41 -= 1.5f;

   // circle ambient light color
   float fT = GetTickCount() / 1000.0f;
   float fR = 0.5f + 0.5f * sinf(fT*1.2f);
   float fG = 0.5f + 0.5f * sinf(fT*2.0f);
   float fB = 0.5f + 0.5f * sinf(fT*1.7f);

   g_pDevice->SetAmbientLight(fR, fG, fB);
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
	if (FAILED(g_pDevice->CreateFX("transform.fx", "TransformTech", true, NULL)))
		Log("no lo cargo :S");
 
   return Y_OK;
   } // BuildAndSetShader
/*----------------------------------------------------------------*/


/**
 * Create geometry needed for the scene along with textures and stuff.
 */
HRESULT BuildGeometry(void) {
   HRESULT hr=Y_OK;
   PVERTEX *v = NULL;
   
   int nIndis = 0;
   int nVerts = 0;
   WORD   *i  = NULL; 

  
  

  

   GenGrid(50, 50, 1.0f, 1.0f, YVector(0.0f, 0.0f, 0.0f),VID_PS, v, i, &nIndis, &nVerts);

  // for (int k = 0; k < nVerts;k++)
		//Log("vertex %f, %f, %f",  v[k].x, v[k].y, v[k].z);
  // for (int k = 0;k < nIndis; k++)
	 //  Log("vertex %i, %i, %i",  i[k], i[k], i[k]);

   return Y_OK;
   } // BuildGeometry
/*----------------------------------------------------------------*/
// Generate grid
void GenGrid(int numVertRows, int numVertCols, float dx, float dz, const YVector& center, YVERTEXID vID, 
				PVERTEX *verts, WORD *indices, int *nIndis, int *nVerts)
{
	*nVerts			= numVertRows*numVertCols;
	int numCellRows = numVertRows-1;
	int numCellCols = numVertCols-1;

	int numTris = numCellRows*numCellCols*2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;
	
	// Build vertices.	

	verts = new PVERTEX[*nVerts];	

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





	 YCOLOR d = { 1.0f, 0.0f, 0.0f, 1.0f };
	 YCOLOR c = { 1.0f, 1.0f, 1.0f, 1.0f };
	UINT    s=-99;
	 if(FAILED(g_pDevice->GetSkinManager()->AddSkin(&c, &c, &d, &c, 1, &s)))
   {}
	g_pDevice->GetVertexManager()->CreateStaticBuffer(
                           VID_PS, s, *nVerts, *nIndis,
                           verts, indices, &g_sRoom);
	delete [] verts;
	delete [] indices;
}


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