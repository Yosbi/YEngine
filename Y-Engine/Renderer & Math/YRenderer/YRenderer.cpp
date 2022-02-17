//-----------------------------------------------------------------------
// YRenderer.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: This file has the implementation of the static lib used to create,
//		get a pointer to, and release a render device, it can be for now 
//		just D3D but in a future i will add OpenGl

#include "YRenderer.h"

//----------------------------------------------------------------------
// Name: YRenderer
// Desc: Constructor
//----------------------------------------------------------------------
YRenderer::YRenderer( HINSTANCE hInst)
{
	// Clear all required values
	m_hInst		= hInst;
	m_pDevice	= NULL;
	m_hDLL		= NULL;
}

//----------------------------------------------------------------------
// Name: ~YRenderer
// Desc: Destructor
//----------------------------------------------------------------------
YRenderer::~YRenderer()
{
	// Release all allocated resourses
	Release();
	// Release DLL
	if (m_hDLL) FreeLibrary(m_hDLL);
}

//----------------------------------------------------------------------
// Name: Release
// Desc: Release all allocated resourses sniffing for the method
//		ReleaseRenderDevice inside the DLL
//----------------------------------------------------------------------
void YRenderer::Release()
{
	// Declaration of a pointer to the method ReleaseRenderDevice
	RELEASERENDERDEVICE _ReleaseRenderDevice = 0;

	if (m_hDLL)
	{
		// Get the address of this method on the DLL
		_ReleaseRenderDevice = (RELEASERENDERDEVICE) GetProcAddress(m_hDLL, "ReleaseRenderDevice" );
	}
	// Call the dll release function
	if (m_pDevice)
	{
		if(FAILED( _ReleaseRenderDevice(&m_pDevice))) m_pDevice = NULL;
	}
}

//----------------------------------------------------------------------
// Name: CreateDevice
// Desc: Create the DLL object loading the appropiate DLL
//----------------------------------------------------------------------
HRESULT YRenderer::CreateDevice( const char *chAPI)
{
	char buffer[300];
	//MessageBox( NULL, "ENTROOOOOOOOOOOOOOOOOOOOOOO", "ENTROOOOOOOOOOOOOOOOOOOOOOO", MB_OK | MB_ICONERROR );
	// Decide which API should be used
	if ( strcmp(chAPI, "Direct3D") == 0)
	{
		m_hDLL = LoadLibrary("YD3D.dll");
		if (!m_hDLL)
		{
			MessageBox( NULL, "Loading YD3D.dll from lib failed.", "Y-Engine - Error", MB_OK | MB_ICONERROR );
			return E_FAIL;
		}
	}
	// If that API is not supported
	else
	{
		//_snprintf(buffer, 300, "API %s is not yet supported", chAPI);
		MessageBox( NULL, buffer, "Y-Engine - Error", MB_OK | MB_ICONERROR );
		return E_FAIL;
	}

	// Pointer to the method CreateRenderDevice nside the dll
	CREATERENDERDEVICE _CreateRenderDevice = NULL;

	// Get the pointer
	if(!(_CreateRenderDevice = (CREATERENDERDEVICE) GetProcAddress(m_hDLL, "CreateRenderDevice")))		
		return E_FAIL;

	// Call the dll function
	if( FAILED( _CreateRenderDevice(m_hDLL, &m_pDevice)))
	{
		MessageBox( NULL, "CreateRenderDevice() from lib failed", "Y-Engine - Error", MB_OK | MB_ICONERROR );
		return E_FAIL;
	}
	
	// Sucess!
	return S_OK;
}
