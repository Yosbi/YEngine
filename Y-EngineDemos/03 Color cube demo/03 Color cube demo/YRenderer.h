//-----------------------------------------------------------------------
// YRenderer.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: This file has the definition of the static lib used to create,
//		get a pointer to, and release a render device, it can be for now 
//		just D3D but in a future i will add OpenGl

#pragma once
#ifndef YRENDERER_H
#define YRENDERER_H

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "YRenderDevice.h"
#include <stdio.h>


//----------------------------------------------------------------------
// Name: YRenderer (class)
// Desc: the class of the static lib used to create, get a pointer, and
//		release an object of a renderer dll
//----------------------------------------------------------------------
class YRenderer
{
public:
	// Constructor and destructor
	YRenderer( HINSTANCE hInst );
	~YRenderer();

	// Public functions
	HRESULT			CreateDevice(const char *chAPI);
	LPYRENDERDEVICE	GetDevice() { return m_pDevice; }
	HINSTANCE		GetModule() { return m_hDLL; }	
	void			Release  ();

private:
	// Private variables
	LPYRENDERDEVICE m_pDevice;	// Pointer to the object of the DLL casted to the interface type (YRendererDevice)
	HINSTANCE		m_hInst;	// Instance of the app
	HMODULE			m_hDLL;		// Handle of the loaded DLL
};
typedef class YRenderer *LPYRENDERER;

#endif //YRENDERER_H
