// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////


// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT   WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT   EngineStartup(char *chAPI);
HRESULT   InputStartup();
HRESULT   ProgramCleanup(void);
HRESULT   ProgramTick(void);
HRESULT   Render(float);
HRESULT   BuildAndSetFX(void);
HRESULT   BuildGeometry(void);
TCHAR*    HrToStr(HRESULT hr);
void      Input(float TimeElapsed);
void	  DrawMirrowed();
void	  DrawShadow(YMatrix mTeapot);
void	  Log(char *chString, ...);
void	  CreateCube(YVector vcP, float fW, float fH, float fD, VERTEX *pVerts, WORD *pIndis, bool bInv);
void	  GenGrid(int numVertRows, int numVertCols, float dx, float dz, const YVector& center, YVERTEXID vID,VERTEX *verts, WORD *indices, int *nIndis, int *nVerts);
void	  SetEngineFirstStates();

#endif