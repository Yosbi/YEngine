//-------------------------------------------------------------------------------
// YDXInput.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: The implemetation of the imputdevice 
//--------------------------------------------------------------------------
#ifndef YDXINPUT_H
#define YDXINPUT_H
#pragma once

#define DIRECTINPUT_VERSION 0x0800

//-----------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------
#include "../YInput/YInputDevice.h"
#include <dinput.h>

//-----------------------------------------------------------------------
// Fordward class declarations
//--------------------------------------------------------------------------
class YKeyboard;
class YMouse;
class YJoystick;

//-----------------------------------------------------------------------
// DLL entrypoint function
//--------------------------------------------------------------------------
BOOL WINAPI DllEntryPoint( HINSTANCE hDll, DWORD fdwReason, LPVOID lpvRserved );

//-----------------------------------------------------------------------
// Name: YInput(Class)
// Desc: Class definition for a input device implementing the YInputDevice
//-----------------------------------------------------------------------
class YDXInput : public YInputDevice 
{
public:
	YDXInput(HINSTANCE hDLL);
	~YDXInput();
      
	// initialize the engine stuff
	HRESULT Init(HWND, const RECT*, bool);
     
	// Interface functions
	void    Release();
	bool    IsRunning() { return m_bRunning; }
	bool    HasJoystick(char *pJoyName);
	HRESULT Update();
      
	// Query input data and states
	HRESULT GetPosition(YINPUTDEV idType, POINT *pPt);
	HRESULT GetJoyDeflection(float*, float*);
	POINT   GetMouseDelta();
	bool    IsPressed(YINPUTDEV idType, UINT nBtn);
	bool    IsReleased(YINPUTDEV idType, UINT nBtn);
      

private:
	// write to log file
    void Log(char *, ...);

    LPDIRECTINPUT8  m_pDI; 
    YKeyboard      *m_pKB;
    YMouse         *m_pMouse;
    YJoystick      *m_pJoy;  
};

//-----------------------------------------------------------------------
// Name: ZFXDIDevice(Class)
// Desc: Joystick, mause and kb general class
//-----------------------------------------------------------------------
class YDIDevice 
{
   public:
				YDIDevice() { }
      virtual  ~YDIDevice() { }

      // base class methods
      virtual void    Create(LPDIRECTINPUT8, HWND, FILE*);
      virtual void    Release();
      virtual HRESULT CrankUp(REFGUID rguid, LPCDIDATAFORMAT lpdf);
      
      // Accessor-Methods
      virtual void    GetPosition(POINT *pPoint) { (*pPoint).x = m_lX; (*pPoint).y = m_lY; }

      // virtual methods for heirs
      virtual HRESULT Init()  =0;
      virtual HRESULT Update()=0;

   protected:
      virtual HRESULT GetData(YINPUTDEV Type, void *pData, DWORD *dwNum);

      LPDIRECTINPUTDEVICE8  m_pDevice;
      LPDIRECTINPUT8        m_pDI;
      HWND                  m_hWnd;
      long                  m_lX;
      long                  m_lY;
      FILE                 *m_pLog;
      void Log(char *, ...);
   }; 

//-----------------------------------------------------------------------
// Name: YKeyboard(Class)
// Desc: Keyboard Device
//-----------------------------------------------------------------------
class YKeyboard : public YDIDevice 
{
public:
			 YKeyboard(LPDIRECTINPUT8, HWND, FILE*);
    virtual ~YKeyboard();

    HRESULT Init();
    HRESULT Update();

    bool    IsPressed(UINT nID);
    bool    IsReleased(UINT nID);

private:
    char  m_Keys[256];
    char  m_KeysOld[256];
};

//-----------------------------------------------------------------------
// Name: YMouse(Class)
// Desc: Mouse Device
//-----------------------------------------------------------------------
class YMouse : public YDIDevice 
{
public:
			YMouse(LPDIRECTINPUT8, HWND, FILE*);
    virtual ~YMouse();

    HRESULT Init();
    HRESULT Update();

    void  SetCage(RECT rcCage) { m_rcCage = rcCage; }
    POINT GetMouseDelta() { return m_Delta; }

    bool IsPressed(UINT nBtn) 
        { if (nBtn<3) return m_bPressed[nBtn]; return false; }
    bool IsReleased(UINT nBtn)
        { if (nBtn<3) return m_bReleased[nBtn]; return false; }

private:
    HANDLE     m_hEvent;
    RECT       m_rcCage;
    bool       m_bPressed[3];
    bool       m_bReleased[3];
    POINT      m_Delta;
};

//-----------------------------------------------------------------------
// Name: YJoystick(Class)
// Desc: Joystick / Gamepad Device
//-----------------------------------------------------------------------
class YJoystick : public YDIDevice 
{
public:
			 YJoystick(LPDIRECTINPUT8, HWND, FILE*);
    virtual ~YJoystick();

    HRESULT Init();
    HRESULT Update();

    bool JoystickFound() { return m_bJoyFound; }
    BOOL EnumJoyCallback(const DIDEVICEINSTANCE *pInst);
      
    void GetName(char *pJoyName) { memcpy(pJoyName, m_Name, sizeof(char)*256); }

    bool IsPressed(UINT nBtn) { if (nBtn<m_dwNumBtns) return m_bPressed[nBtn]; return false; }

    bool IsReleased(UINT nBtn) { if (nBtn<m_dwNumBtns) return m_bReleased[nBtn]; return false; }

    HRESULT GetJoyDeflection(float *pfX, float *pfY);

private:
    GUID       m_guid;
    char       m_Name[256];
    bool       m_bJoyFound;
    bool       m_bPressed[24];
    bool       m_bReleased[24];
    DWORD      m_dwNumBtns;
}; 

#endif
