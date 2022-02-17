//-------------------------------------------------------------------------------
// YInput.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: Implementation of the class YInput
//--------------------------------------------------------------------------
#include "YInput.h"

//-----------------------------------------------------------------------
// Name: YInput(Const)
// Desc: Constructor
//-----------------------------------------------------------------------
YInput::YInput(HINSTANCE hInst) 
{
	m_hInst   = hInst;
	m_pDevice = NULL;
	m_hDLL    = NULL;
}

//-----------------------------------------------------------------------
// Name: ~YInput(Dest)
// Desc: Destructor
//-----------------------------------------------------------------------
YInput::~YInput(void) 
{ 
	Release(); 
}

//-----------------------------------------------------------------------
// Name: CreateDevice
// Desc: Create the dll objects. This functions loads the appropriate dll.
//-----------------------------------------------------------------------
HRESULT YInput::CreateDevice(void) 
{
   // load the DLL containing interface implementation
   m_hDLL = LoadLibraryEx("YDXInput.dll",NULL,0);
   if(!m_hDLL) 
   {
      MessageBox(NULL, "Loading YDXInput.dll from lib failed.", "Y-Engine - error", MB_OK | MB_ICONERROR);
      return E_FAIL;
   }
   
   CREATEINPUTDEVICE _CreateInputDevice = 0;
   HRESULT hr;
   
   // Function pointer to dll's 'CreateInputDevice' function
   _CreateInputDevice = (CREATEINPUTDEVICE)GetProcAddress(m_hDLL, "CreateInputDevice");

   // Call dll's create function
   hr = _CreateInputDevice(m_hDLL, &m_pDevice);
   if(FAILED(hr))
   {
		MessageBox(NULL, "CreateInputDevice() from lib failed.", "Y-Engine - error", MB_OK | MB_ICONERROR);
		m_pDevice = NULL;
		return E_FAIL;
   }   
   return S_OK;
} 

//-----------------------------------------------------------------------
// Name: Release
// Desc: Clean up dll objects
//-----------------------------------------------------------------------
void YInput::Release(void) 
{
   RELEASEINPUTDEVICE _ReleaseInputDevice = 0;
   HRESULT hr;
   
   if (m_hDLL) 
   {
		// function pointer to dll 'ReleaseInputDevice' function
		_ReleaseInputDevice = (RELEASEINPUTDEVICE)GetProcAddress(m_hDLL, "ReleaseInputDevice");
   }
   // call dll's release function
   if (m_pDevice) 
   {
		hr = _ReleaseInputDevice(&m_pDevice);
		if(FAILED(hr))		
			m_pDevice = NULL;		
   }
} 
