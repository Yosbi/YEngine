//-------------------------------------------------------------------------------
// YInput.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: Defines the static lib to get access to the dll of the imput module
//--------------------------------------------------------------------------
#ifndef YINPUT_H
#define YINPUT_H
#pragma once

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include "YInputDevice.h"

//-----------------------------------------------------------------------
// Name: YInput(Class)
// Desc: Class used to get a object to the input module
//-----------------------------------------------------------------------
class YInput 
{
public:
	YInput(HINSTANCE hInst);
	~YInput(void);
      
	HRESULT          CreateDevice(void);
	LPYINPUTDEVICE   GetDevice(void) { return m_pDevice; }
	HINSTANCE        GetModule(void) { return m_hDLL;    }
	void             Release(void);
      
private:
	YInputDevice    *m_pDevice;
	HINSTANCE        m_hInst;
	HMODULE          m_hDLL;
};

typedef class YInput *LPYINPUT;


#endif