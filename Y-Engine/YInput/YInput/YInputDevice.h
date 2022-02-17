//-------------------------------------------------------------------------------
// YInputDevice.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: The interface to get the events of the keyboard, mause and any joystick
//		without delay
//--------------------------------------------------------------------------

#ifndef YINPUTINTERFACE_H
#define YINPUTINTERFACE_H
#pragma once

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include "../../Renderer & Math/YD3D/Y.h"

//-----------------------------------------------------------------------
// Name: YInput(Class)
// Desc: Class used to get a object to the input module
//-----------------------------------------------------------------------
class YInputDevice 
{     
public:
			 YInputDevice(void) {};
	virtual ~YInputDevice(void) {};
      
    // Ini the module
	virtual HRESULT Init(HWND, const RECT*, bool)=0;
      
	// Release API specific stuff
	virtual void	Release(void)=0;
      
	// Is initialized?
	virtual bool	IsRunning(void)=0;

	// Joystick available?
	virtual bool	HasJoystick(char *pJoyName)=0;

	// Poll input devices
	virtual HRESULT	Update(void)=0;

	// Will work for mouse and joystick only
	virtual HRESULT GetPosition(YINPUTDEV, POINT*)=0;

	// For joysticks only (intensity range from -1 to +1)
	virtual HRESULT GetJoyDeflection(float*, float*)=0;

	// Get change in mouse position
	virtual POINT	GetMouseDelta(void)=0;

	// Will work for keyboard, mouse and joystick
	virtual bool	IsPressed(YINPUTDEV, UINT)=0;

	// Will work for keyboard, mouse and joystick
	virtual bool	IsReleased(YINPUTDEV, UINT)=0;

	protected:
	HWND       m_hWndMain;          // Application main window
	HINSTANCE  m_hDLL;              // Dll module handle
	bool       m_bRunning;          // After succesful initialization
	FILE      *m_pLog;              // Log file

};
typedef class YInputDevice *LPYINPUTDEVICE;


extern "C" 
{
	HRESULT CreateInputDevice(HINSTANCE hDLL, YInputDevice **pInterface);
	typedef HRESULT (*CREATEINPUTDEVICE)(HINSTANCE hDLL, YInputDevice **pInterface);
   
	HRESULT ReleaseInputDevice(YInputDevice **pInterface);
	typedef HRESULT (*RELEASEINPUTDEVICE)(YInputDevice **pInterface);
}

//-----------------------------------------------------------------------
// The next definitions is the codes of the buttons that this device uses
//-----------------------------------------------------------------------
// keyboard scan codes from dinput.h
#define YVK_ESCAPE          0x01
#define YVK_TAB             0x0F
#define YVK_SPACE           0x39
#define YVK_RETURN          0x1C
#define YVK_BACK            0x0E
#define YVK_CAPITAL         0x3A

#define YVK_MINUS           0x0C
#define YVK_EQUALS          0x0D
#define YVK_LBRACKET        0x1A
#define YVK_RBRACKET        0x1B
#define YVK_SEMICOLON       0x27
#define YVK_APOSTROPHE      0x28
#define YVK_GRAVE           0x29
#define YVK_BACKSLASH       0x2B
#define YVK_COMMA           0x33
#define YVK_PERIOD          0x34
#define YVK_SLASH           0x35

// CHARACTER KEYS
#define YVK_A               0x1E
#define YVK_S               0x1F
#define YVK_D               0x20
#define YVK_F               0x21
#define YVK_G               0x22
#define YVK_H               0x23
#define YVK_J               0x24
#define YVK_K               0x25
#define YVK_L               0x26
#define YVK_Z               0x2C
#define YVK_X               0x2D
#define YVK_C               0x2E
#define YVK_V               0x2F
#define YVK_B               0x30
#define YVK_N               0x31
#define YVK_M               0x32
#define YVK_Q               0x10
#define YVK_W               0x11
#define YVK_E               0x12
#define YVK_R               0x13
#define YVK_T               0x14
#define YVK_Y               0x15
#define YVK_U               0x16
#define YVK_I               0x17
#define YVK_O               0x18
#define YVK_P               0x19

// MAIN NUM KEYS
#define YVK_1               0x02
#define YVK_2               0x03
#define YVK_3               0x04
#define YVK_4               0x05
#define YVK_5               0x06
#define YVK_6               0x07
#define YVK_7               0x08
#define YVK_8               0x09
#define YVK_9               0x0A
#define YVK_0               0x0B

// FUNCTION KEYS
#define YVK_F1              0x3B
#define YVK_F2              0x3C
#define YVK_F3              0x3D
#define YVK_F4              0x3E
#define YVK_F5              0x3F
#define YVK_F6              0x40
#define YVK_F7              0x41
#define YVK_F8              0x42
#define YVK_F9              0x43
#define YVK_F10             0x44
#define YVK_F11             0x57
#define YVK_F12             0x58

// ON NUMPAD
#define YVK_NPPLUS          0x4E
#define YVK_NPMINUS         0x4A
#define YVK_NPDECIMAL       0x53
#define YVK_NPCOMMA         0xB3
#define YVK_NPDIVIDE        0xB5
#define YVK_NPMULTIPLY      0x37
#define YVK_NPENTER         0x9C
#define YVK_NUMLOCK         0x45
#define YVK_NP1             0x4F
#define YVK_NP2             0x50
#define YVK_NP3             0x51
#define YVK_NP4             0x4B
#define YVK_NP5             0x4C
#define YVK_NP6             0x4D
#define YVK_NP7             0x47
#define YVK_NP8             0x48
#define YVK_NP9             0x49
#define YVK_NP0             0x52

// CTRL, ALT, SHFT and WINDWS
#define YVK_RSHIFT          0x36
#define YVK_LSHIFT          0x2A
#define YVK_RCTRL           0x9D
#define YVK_LCTRL           0x1D
#define YVK_RALT            0xB8
#define YVK_LALT            0x38
#define YVK_LWIN            0xDB
#define YVK_RWIN            0xDC

// ON ARROW KEYPAD
#define YVK_UP              0xC8
#define YVK_DOWN            0xD0
#define YVK_LEFT            0xCB
#define YVK_RIGHT           0xCD
#define YVK_INSERT          0xD2
#define YVK_DELETE          0xD3
#define YVK_HOME            0xC7
#define YVK_END             0xCF
#define YVK_PGDOWN          0xD1
#define YVK_PGUP            0xC9

#define YVK_PAUSE           0xC5    /* Pause */
#define YVK_SCROLL          0x46    /* Scroll Lock */


#endif
