// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////


// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT   WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT   EngineStartup(char *chAPI);
HRESULT   ProgramCleanup(void);
HRESULT   ProgramTick(void);
HRESULT   Render(float, float);
HRESULT   BuildAndSetFX(void);
HRESULT   BuildGeometry(void);
TCHAR*    HrToStr(HRESULT hr);
void	  Log(char *chString, ...);
void	  GenGrid(int numVertRows, int numVertCols, float dx, float dz, const YVector& center, YVERTEXID vID,VERTEX *verts, WORD *indices, int *nIndis, int *nVerts);
void	  SetEngineFirstStates();
HRESULT	  RenderGrid(float TimeElapsed);
HRESULT   RenderCylinder(float TimeElapsed);
HRESULT   RenderSphere(float TimeElapsed);

#endif