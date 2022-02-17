// FILE: main.h 

#ifndef MAIN_H
#define MAIN_H

// D E F I N E S ///////////////////////////////////////////////////


// P R O T O T Y P E S /////////////////////////////////////////////

LRESULT   WINAPI MsgProc(HWND, UINT, WPARAM, LPARAM);
HRESULT   ProgramStartup(char *chAPI);
HRESULT   ProgramCleanup(void);
HRESULT   ProgramTick(void);
HRESULT   Render(int);
HRESULT   BuildAndSetShader(void);
HRESULT   BuildGeometry(void);
void      CreateCube(YVector,float,float,float,LVERTEX*,WORD*,bool);
TCHAR*    HrToStr(HRESULT hr);
void Log(char *chString, ...);

#endif