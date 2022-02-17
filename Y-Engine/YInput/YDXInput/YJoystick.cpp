//-------------------------------------------------------------------------------
// YJoystick.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: The implemetation of the joystick class
//--------------------------------------------------------------------------
#include "YDXInput.h"		// class definition


extern bool g_bLF;

YJoystick *g_pThis=NULL;	// Pointer to this object
BOOL CALLBACK gEnumJoyCallback(const DIDEVICEINSTANCE* pInst, void* pUserData);


//-----------------------------------------------------------------------
// Name: YJoystick
// Desc: Constructor
//-----------------------------------------------------------------------
YJoystick::YJoystick(LPDIRECTINPUT8 pDI, HWND hWnd, FILE* pLog) 
{ Create(pDI, hWnd, pLog); }

//-----------------------------------------------------------------------
// Name: ~YJoystick
// Desc: Destructor
//-----------------------------------------------------------------------
YJoystick::~YJoystick() 
{ if (g_bLF) Log("releasing joystick"); Release(); }

//-----------------------------------------------------------------------
// Name: Init
// Desc: Initializes the joystick device
//-----------------------------------------------------------------------
HRESULT YJoystick::Init() 
{
	DIPROPRANGE diprg;
	DIDEVCAPS   diCaps;

	if (g_bLF) Log("Crancking up joystick");

	// Some inits
	memset(m_bPressed, 0, sizeof(m_bPressed));
	memset(m_bReleased, 0, sizeof(m_bReleased));
	m_bJoyFound = false;
	m_lX = m_lY = 0;
	g_pThis     = this;

	// First enumerate available joysticks
	m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)gEnumJoyCallback,
						&m_guid, DIEDFL_ATTACHEDONLY);

	// Nothing found? so there is no joy at all!
	if (!m_bJoyFound) 
	{
		if (g_bLF)Log("no joystick found");
		return Y_FAIL;
	}

	// Final settings
	diprg.diph.dwSize = sizeof(DIPROPRANGE);
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMin = -1000;
	diprg.lMax = +1000;

	diprg.diph.dwObj = DIJOFS_X;
	m_pDevice->SetProperty(DIPROP_RANGE, &diprg.diph);

	diprg.diph.dwObj = DIJOFS_Y;                  
	m_pDevice->SetProperty(DIPROP_RANGE, &diprg.diph);

	// Check number of buttons on this thing
	if (SUCCEEDED(m_pDevice->GetCapabilities(&diCaps)))
		m_dwNumBtns = diCaps.dwButtons;
	else
		m_dwNumBtns = 4;

	if (g_bLF) Log("joystick online (%d buttons, \"%s\")", m_dwNumBtns, m_Name);
	return Y_OK;
} 


//-----------------------------------------------------------------------
// Name: gEnumJoyCallback
// Desc: Callback function to look for a joystick in the system
//-----------------------------------------------------------------------
BOOL CALLBACK gEnumJoyCallback(const DIDEVICEINSTANCE* pInst, void* pUserData) 
{
	return g_pThis->EnumJoyCallback(pInst);
}

//-----------------------------------------------------------------------
// Name: EnumJoyCallback
// Desc: Called for every enumerated Joystick. If we manage to create a
//		 device interface for one of them, then stop the enumeration.
//-----------------------------------------------------------------------
BOOL YJoystick::EnumJoyCallback(const DIDEVICEINSTANCE *pInst) 
{
	// if found any then 'go create' it 
	if (SUCCEEDED(CrankUp(pInst->guidInstance, &c_dfDIJoystick))) 
	{
		m_bJoyFound = true;
		strcpy(m_Name, (char*)pInst->tszProductName);
		return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
} 

//-----------------------------------------------------------------------
// Name: Update
// Desc: Update all input devices
//-----------------------------------------------------------------------
HRESULT YJoystick::Update(void) 
{
	DIJOYSTATE js;
   
	// Poll the device, this might not be needed by
	// some joysticks but it won't hurt them, though
	m_pDevice->Poll();

	// Try to get the data from the Joystick
	if (FAILED(GetData(IDV_JOYSTICK, &js, NULL))) 
	{
		if (g_bLF) Log("error: GetData(Joystick) failed");
		return Y_FAIL;
	}

	// Get the button states
	for (DWORD i=0; i<m_dwNumBtns; i++) 
	{
		m_bReleased[i] = false;
		if (js.rgbButtons[i] & 0x80)
			m_bPressed[i] = true;
		else 
		{
			if (m_bPressed[i]) m_bReleased[i] = true;
			m_bPressed[i] = false;
		}
	}

	// Save the sticks position
	m_lX = js.lX;
	m_lY = js.lY;

	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: GetJoyDeflection
// Desc: Return the intensity of joystick deflection on both axes in the
//		 range from [-1.0f, +1.0f].
//-----------------------------------------------------------------------
HRESULT YJoystick::GetJoyDeflection(float *pfX, float *pfY) 
{
   (*pfX) = m_lX / 1000.0f;
   (*pfY) = m_lY / 1000.0f;
   return Y_OK;
}