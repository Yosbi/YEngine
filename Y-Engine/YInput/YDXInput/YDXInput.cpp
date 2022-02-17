//-------------------------------------------------------------------------------
// YDXInput.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: The implemetation of the imputdevice 
//--------------------------------------------------------------------------
#include "YDXInput.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


bool g_bLF = false;


//-----------------------------------------------------------
// DLL stuff implementation                                  
//-----------------------------------------------------------
BOOL WINAPI DllEntryPoint(HINSTANCE hDll, DWORD fdwReason, LPVOID lpvReserved) 
{
	switch(fdwReason) {
		// called when we attach to the DLL
		case DLL_PROCESS_ATTACH:
			/* dll init/setup stuff */
			break;
		case DLL_PROCESS_DETACH:
			/* dll shutdown/release stuff */
			break;
		default:
			break;
	};
   
   return TRUE;
} 

//-----------------------------------------------------------------------
// Name: CreateInputDevice
// Desc: Exported create function: Creates a new YInputDevice object
//-----------------------------------------------------------------------
HRESULT CreateInputDevice(HINSTANCE hDLL, YInputDevice **pDevice) 
{
	if(!*pDevice) 
	{
		*pDevice = new YDXInput(hDLL);
		return Y_OK;
	}
	return Y_FAIL;
}

//-----------------------------------------------------------------------
// Name: ReleaseInputDevice
// Desc: Exported release function: Realeses the given YInputDevice object.
//-----------------------------------------------------------------------
HRESULT ReleaseInputDevice(YInputDevice **pDevice) 
{
	if(!*pDevice) 
	{
		return Y_FAIL;
	}
	delete *pDevice;
	*pDevice = NULL;
	return Y_OK;
}

//-----------------------------------------------------------
// YDXInput class implementation                                
//-----------------------------------------------------------

//-----------------------------------------------------------------------
// Name: YDXInput
// Desc: Constructor
//-----------------------------------------------------------------------
YDXInput::YDXInput(HINSTANCE hDLL)
{
	m_hDLL        = hDLL;
	m_pDI         = NULL;
	m_pLog        = NULL;
	m_bRunning    = false;
	m_pKB         = NULL;
	m_pMouse      = NULL;
	m_pJoy        = NULL;
}

//-----------------------------------------------------------------------
// Name: YDXInput
// Desc: Destructor
//-----------------------------------------------------------------------
YDXInput::~YDXInput() 
{
   Release();
}


//-----------------------------------------------------------------------
// Name: Release
// Desc: Release all stuff.
//-----------------------------------------------------------------------
void YDXInput::Release() 
{
	if(g_bLF)Log("shutting down DirectInput");

	if (m_pKB) {
		delete m_pKB;
		m_pKB = NULL;
		}
   
	if (m_pMouse) {
		delete m_pMouse;
		m_pMouse = NULL;
		}

	if (m_pJoy) {
		delete m_pJoy;
		m_pJoy = NULL;
		}

	if (m_pDI) {
		m_pDI->Release();
		m_pDI = NULL;
		}

	if(g_bLF)Log("offline (ok)");
	if(g_bLF)fclose(m_pLog);
}

//-----------------------------------------------------------------------
// Name: Init
// Desc: Initializes at least keyboard and mouse, otherwise returns error.
//		 If present joystick will also be initialized, but is not mandatory.
//-----------------------------------------------------------------------
HRESULT YDXInput::Init(HWND hWnd, const RECT *prcCage, bool bSaveLog) 
{
	HRESULT hr;

	// Opening log file
	if (bSaveLog)
	{
		m_pLog = fopen("Log_YInputDevice.txt", "w");		
		Log("calling initialization");
	}
    g_bLF		= bSaveLog;

	m_hWndMain	= hWnd;
	g_bLF       = bSaveLog;

	// Create main DirectInput object
	if (FAILED (hr = DirectInput8Create(m_hDLL, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL)) ) 
	{ 
		if(g_bLF) Log("Error: DirectInput8Create() failed"); 
		return Y_FAIL;
	} 

	// Create all input device objects
	m_pKB    = new YKeyboard(m_pDI, hWnd, m_pLog);
	m_pMouse = new YMouse(m_pDI, hWnd, m_pLog);
	m_pJoy   = new YJoystick(m_pDI, hWnd, m_pLog);


	// initialize all input device objects
	if (FAILED( m_pKB->Init() )) 
	{
		if (m_pKB) delete m_pKB;
		m_pKB = NULL;
		if(g_bLF) Log("error: Init(Keyboard) failed");
		return Y_FAIL;
	}
   
	if (FAILED( m_pMouse->Init() )) 
	{
		if (m_pMouse) delete m_pMouse;
		m_pMouse = NULL;
		if(g_bLF) Log("error: Init(Mouse) failed");
		return Y_FAIL;
	}

	if (prcCage) m_pMouse->SetCage(*prcCage);
   
	if (FAILED( m_pJoy->Init() )) 
	{
		if (m_pJoy) delete m_pJoy;
		m_pJoy = NULL;
		if(g_bLF) Log("warning: no joystick active");
		// no need to fail 
	}

	if(g_bLF) Log("initialized (online and ready)");
	m_bRunning = true;
	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: HasJoystick(
// Desc: Query if a Joystick is active and ready or not. Also returns
//		 Joytsicks name if any was found.
//-----------------------------------------------------------------------
bool YDXInput::HasJoystick(char *pJoyName) 
{
	if (m_pJoy) 
	{
		if (pJoyName) m_pJoy->GetName(pJoyName);
		return true;
	}
	return false;
} 

//-----------------------------------------------------------------------
// Name: Update
// Desc: Update all input devices
//-----------------------------------------------------------------------
HRESULT YDXInput::Update()
{
	HRESULT hr;

	if (!IsRunning()) return Y_FAIL;

	if (m_pKB) 
		if ( FAILED( hr=m_pKB->Update() ) )
			return hr;
	

	if (m_pMouse) 
		if ( FAILED( hr=m_pMouse->Update() ) )
			return hr;
		
	if (m_pJoy) 
		if ( FAILED( hr=m_pJoy->Update() ) )
			return hr;
		
	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: GetPosition
// Desc: If mouse or joystick return current position.
//-----------------------------------------------------------------------
HRESULT YDXInput::GetPosition(YINPUTDEV idType, POINT *pPt) 
{
	if (idType == IDV_MOUSE) 
	{
		m_pMouse->GetPosition(pPt);
		return Y_OK;
	}
	else if (idType==IDV_JOYSTICK) 
	{
		if (m_pJoy)
			m_pJoy->GetPosition(pPt);
		else 
		{
			(*pPt).x = 0;
			(*pPt).y = 0;
		}
		return Y_OK;
	}
	else return Y_INVALIDPARAM;
} 

//-----------------------------------------------------------------------
// Name: GetJoyDeflection
// Desc: If joystick, return current intensity on axes
//-----------------------------------------------------------------------
HRESULT YDXInput::GetJoyDeflection(float *pfX, float *pfY) 
{
	if (m_pJoy)
		return m_pJoy->GetJoyDeflection(pfX, pfY);
	else 
	{
		(*pfX) = 0.0f;
		(*pfY) = 0.0f;
		return Y_OK;
	}
}

//-----------------------------------------------------------------------
// Name: GetMouseDelta
// Desc: Return the change of mouse cursor since last call to Update().
//-----------------------------------------------------------------------
POINT YDXInput::GetMouseDelta() 
{ return m_pMouse->GetMouseDelta(); }

//-----------------------------------------------------------------------
// Name: IsPressed
// Desc: Ask about button state.
//-----------------------------------------------------------------------
bool YDXInput::IsPressed(YINPUTDEV idType, UINT nBtn) 
{
	if (idType == IDV_MOUSE)
		return m_pMouse->IsPressed(nBtn);
	else if (idType==IDV_KEYBOARD)
		return m_pKB->IsPressed(nBtn);
	else if ( (idType==IDV_JOYSTICK) && (m_pJoy) )
		return m_pJoy->IsPressed(nBtn);
	else 
		return false;
} 

//-----------------------------------------------------------------------
// Name: IsReleased
// Desc: Ask about button/key state for mouse, joytsick or keyboard
//-----------------------------------------------------------------------
bool YDXInput::IsReleased(YINPUTDEV idType, UINT nBtn) 
{
	if (idType == IDV_MOUSE)
		return m_pMouse->IsReleased(nBtn);
	else if (idType==IDV_KEYBOARD)
		return m_pKB->IsReleased(nBtn);
	else if ( (idType==IDV_JOYSTICK) && (m_pJoy) )
		return m_pJoy->IsReleased(nBtn);
	else 
		return false;
} 

//-----------------------------------------------------------------------
// Name: Log
// Desc: write outputstring to attribut outputstream if exists
//-----------------------------------------------------------------------
void YDXInput::Log(char *chString, ...) 
{
	if (g_bLF)
	{
		char ch[256];
		char *pArgs;
   
		pArgs = (char*) &chString + sizeof(chString);
		vsprintf(ch, chString, pArgs);
		fprintf(m_pLog, "[YDXInputDevice]: ");
		fprintf(m_pLog, ch);
		fprintf(m_pLog, "\n");
   
		fflush(m_pLog);
	}
} 



//-----------------------------------------------------------
// YDIDevice class implementation                          
//-----------------------------------------------------------
//-----------------------------------------------------------------------
// Name: Create
// Desc: Set basic attributes to pointers
//-----------------------------------------------------------------------
void YDIDevice::Create(LPDIRECTINPUT8 pDI, HWND hWnd, FILE* pLog)
{
	m_pLog    = pLog;
	m_hWnd    = hWnd;
	m_pDI     = pDI;
	m_pDevice = NULL;
}

//-----------------------------------------------------------------------
// Name: Release
// Desc: Release the object.
//-----------------------------------------------------------------------
void YDIDevice::Release(void) 
{
	if (m_pDevice) 
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	if(g_bLF) Log("input device offline (ok)");
}

//-----------------------------------------------------------------------
// Name: CrankUp
// Desc: Call all stuff DirectInput needs to initialize an input device.
//-----------------------------------------------------------------------
HRESULT YDIDevice::CrankUp(REFGUID rguid, LPCDIDATAFORMAT pdf) 
{
	DWORD dwFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

	// If device is already build destroy it
	if (m_pDevice) 
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	// 1. Step: create device
	if ( FAILED(m_pDI->CreateDevice(rguid, &m_pDevice, NULL))) 
	{
		if(g_bLF) Log("Error: CreateDevice failed");
		return Y_FAIL; 
	} 

	// Set the correct device data format
	if ( FAILED(m_pDevice->SetDataFormat(pdf))) 
	{
		if(g_bLF) Log("Error: SetDataFormat failed");
		return Y_FAIL; 
	}

	// Set the cooperation level with windows
	if ( FAILED(m_pDevice->SetCooperativeLevel(m_hWnd, dwFlags))) 
	{
		if(g_bLF) Log("Error: SetCoopLevel failed");
		return Y_FAIL; 
	}

	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: GetData
// Desc: Get the state or data from the device object.
//-----------------------------------------------------------------------
HRESULT YDIDevice::GetData(YINPUTDEV Type, void *pData, DWORD *pdwNum) 
{
	HRESULT hr=Y_FAIL;
	size_t size=0;
   
	// Is this a mouse?
	if (Type == IDV_MOUSE) 
	{
		size = sizeof(DIDEVICEOBJECTDATA);

		hr=m_pDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA*)pData, pdwNum, 0);
	}
	else 
	{
		if (Type==IDV_KEYBOARD) size = sizeof(char)*256;
		else size = sizeof(DIJOYSTATE);
		hr = m_pDevice->GetDeviceState(size, pData);
	}
 
	if (FAILED(hr)) 
	{
		// If lost or not yet acquired then acquire it at all costs
		if ( (hr==DIERR_NOTACQUIRED) || (hr==DIERR_INPUTLOST) ) 
		{
			hr = m_pDevice->Acquire();

			while (hr==DIERR_INPUTLOST)
				hr = m_pDevice->Acquire();
         
			// If another application is using this input device
			// have to give up and try next frame
			if (hr==DIERR_OTHERAPPHASPRIO) return Y_OK;
         
			// If got back device then try again to read data
			if (SUCCEEDED(hr)) 
			{
				if (Type == IDV_MOUSE)
					hr = m_pDevice->GetDeviceData(size, (DIDEVICEOBJECTDATA*)pData, pdwNum, 0);
				else 
					hr = m_pDevice->GetDeviceState(size, pData);
			}
			if (FAILED(hr)) return Y_FAIL;
		}
		else return Y_FAIL;
	}
	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: GetData
// Desc: Write outputstring to attribut outputstream if exists
//-----------------------------------------------------------------------
void YDIDevice::Log(char *chString, ...) 
{
	if (g_bLF)
	{
		char ch[256];
		char *pArgs;
   
		pArgs = (char*) &chString + sizeof(chString);
		vsprintf(ch, chString, pArgs);
		fprintf(m_pLog, "[YDXInputDevice]: ");
		fprintf(m_pLog, ch);
		fprintf(m_pLog, "\n");
   
		fflush(m_pLog);
	}
} 