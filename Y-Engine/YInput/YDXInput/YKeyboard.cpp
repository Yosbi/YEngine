//-------------------------------------------------------------------------------
// YKeyboard.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: The implemetation of the keyboard class
//--------------------------------------------------------------------------
#include "YDXInput.h"	// class definition


extern bool g_bLF;

//-----------------------------------------------------------------------
// Name: YKeyboard
// Desc: Constructor
//-----------------------------------------------------------------------
YKeyboard::YKeyboard(LPDIRECTINPUT8 pDI, HWND hWnd, FILE* pLog) 
   { Create(pDI, hWnd, pLog); }

//-----------------------------------------------------------------------
// Name: YKeyboard
// Desc: Destructor
//-----------------------------------------------------------------------
YKeyboard::~YKeyboard(void)
   { if (g_bLF) Log("releasing keyboard"); Release(); }

//-----------------------------------------------------------------------
// Name: Init
// Desc: Initializes the keyboard device.
//-----------------------------------------------------------------------
HRESULT YKeyboard::Init(void) 
{   
	if (g_bLF) Log("crancking up keyboard");
	if (FAILED(CrankUp(GUID_SysKeyboard, &c_dfDIKeyboard)))
		return Y_FAIL;

	// Clear out the structs
	memset(m_Keys, 0, sizeof(m_Keys));
	memset(m_KeysOld, 0, sizeof(m_KeysOld));

	// Acquire the device to make it work
	m_pDevice->Acquire();
	if (g_bLF)Log("keyboard online");
	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: Update
// Desc: Update the keyboard device.
//----------------------------------------------------------------------
HRESULT YKeyboard::Update(void) 
{
	// Copy keyboard state from last frame
	memcpy(m_KeysOld, m_Keys, sizeof(m_Keys));

	// Try to get the data from the keyboard
	if (FAILED(GetData(IDV_KEYBOARD, &m_Keys[0], NULL))) 
	{
		if (g_bLF)Log("Error: GetData(Keyboard) failed");
		return Y_FAIL;
	}
	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: IsPressed
// Desc: Check if the given key is pressed. Use WinAPI VK_ defines here.
//----------------------------------------------------------------------
bool YKeyboard::IsPressed(UINT nID) 
{
	// Translate virtual code to scan code
	if (m_Keys[nID] & 0x80)
		return true;
	return false;
}

//-----------------------------------------------------------------------
// Name: IsReleased
// Desc: Check if the given key is released. Use WinAPI VK_ defines here.
//----------------------------------------------------------------------
bool YKeyboard::IsReleased(UINT nID) 
{
	// Translate virtual code to scan code
	if ( (m_KeysOld[nID] & 0x80) && !(m_Keys[nID] & 0x80) )
		return true;
	return false;
} 