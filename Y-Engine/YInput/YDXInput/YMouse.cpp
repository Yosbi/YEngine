//-------------------------------------------------------------------------------
// YKeyboard.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: The implemetation of the keyboard class
//--------------------------------------------------------------------------
#include "YDXInput.h" // class definition


extern bool g_bLF;
const int BUFFER_SIZE = 16;


//-----------------------------------------------------------------------
// Name: YMouse
// Desc: Constructor
//-----------------------------------------------------------------------
YMouse::YMouse(LPDIRECTINPUT8 pDI, HWND hWnd, FILE* pLog) 
   { Create(pDI, hWnd, pLog); }

//-----------------------------------------------------------------------
// Name: YMouse
// Desc: Destructor
//-----------------------------------------------------------------------
YMouse::~YMouse(void) 
{ if (g_bLF)Log("releasing mouse"); Release(); }

//-----------------------------------------------------------------------
// Name: Init
// Desc: Initializes the mouse device
//-----------------------------------------------------------------------
HRESULT YMouse::Init() 
{
	//	Clear out structs for mouse buttons
	memset(m_bPressed,  0, sizeof(bool)*3);
	memset(m_bReleased, 0, sizeof(bool)*3);
	m_lX = m_lY = 0;

	m_rcCage.left   = -1;
	m_rcCage.right  = -1;
	m_rcCage.top    = -1;
	m_rcCage.bottom = -1;

	if (g_bLF) Log("crancking up mouse");
	if (FAILED(CrankUp(GUID_SysMouse, &c_dfDIMouse)))
		return Y_FAIL;

	// Activate event notification for the mouse
	if (!(m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL))) 
	{
		if (g_bLF) Log("Error: CreateEvent(Mouse) failed");
		return Y_FAIL;
	}
	if (FAILED( m_pDevice->SetEventNotification(m_hEvent))) 
	{
		if (g_bLF) Log("Error: SetEventNotification(Mouse) failed");
		return Y_FAIL;
	}

	// create buffered input for the mouse
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = BUFFER_SIZE;

	if (FAILED( m_pDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
	{
		if (g_bLF) Log("Error: SetProperty(Mouse) failed");
		return Y_FAIL;
	}

	// Acquire the device to make it work
	m_pDevice->Acquire();
	if (g_bLF)Log("mouse online");
	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: Update
// Desc: Update all input devices
//-----------------------------------------------------------------------
HRESULT YMouse::Update() 
{
	DIDEVICEOBJECTDATA od[BUFFER_SIZE];
	DWORD        dwNumElem = BUFFER_SIZE;

	bool bCage = (m_rcCage.right != -1);

	m_Delta.x = m_Delta.y = 0;
      
	// Try to get the data from the mouse
	if (FAILED(GetData(IDV_MOUSE, &od[0], &dwNumElem))) 
	{
		if (g_bLF) Log("error: GetData(Mouse) failed");
		return Y_FAIL;
	}

	// Clear those
	m_bReleased[0] = m_bReleased[1] = m_bReleased[2] = false;

	// Now we have 'dwNumElem' of mouse events
	for (DWORD i=0; i<dwNumElem; i++) 
	{
		switch (od[i].dwOfs) 
		{
			// MOVEMENT
			case DIMOFS_X: 
			{
				m_lX += od[i].dwData;
				m_Delta.x = od[i].dwData;

				if (bCage & (m_lX < m_rcCage.left))
					m_lX = m_rcCage.left;
				else if (bCage & (m_lX > m_rcCage.right))
					m_lX = m_rcCage.right;
			} break;

			case DIMOFS_Y: 
			{
				m_lY += od[i].dwData;
				m_Delta.y = od[i].dwData;
            
				if (bCage & (m_lY < m_rcCage.top))
					m_lY = m_rcCage.top;
				else if (bCage & (m_lY > m_rcCage.bottom))
					m_lY = m_rcCage.bottom;
			} break;

			// BUTTON STATES
			case DIMOFS_BUTTON0: 
			{
				if (od[i].dwData & 0x80) 
				{
					m_bPressed[0] = true;
				}
				else 
				{
					if (m_bPressed[0])
						m_bReleased[0] = true;

					m_bPressed[0] = false;
				}
			} break;

			case DIMOFS_BUTTON1: 
			{
				if (od[i].dwData & 0x80)
					m_bPressed[1] = true;
				else 
				{
					if (m_bPressed[1])
						m_bReleased[1] = true;

					m_bPressed[1] = false;
				}
			} break;

			case DIMOFS_BUTTON2: 
			{
				if (od[i].dwData & 0x80)
					m_bPressed[2] = true;
				else 
				{
					if (m_bPressed[2])
						m_bReleased[2] = true;

					m_bPressed[2] = false;
				}
			} break;
		}; // switch
	} // for

	return Y_OK;
}

