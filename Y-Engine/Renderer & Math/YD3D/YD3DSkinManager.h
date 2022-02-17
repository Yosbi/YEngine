//-----------------------------------------------------------------------
// YD3DSkinManagerh
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: This file contains a class named SkinManager 

#ifndef YD3D_SKINMANAGER
#define YD3D_SKINMANAGER

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include "..\YRenderer\YRenderDevice.h" // Here is the manager interfase class
#include "Y.h"							// Return values and structs

//----------------------------------------------------------------------
// Name: YD3DSkinManager (class)
// Desc: Implements a skin manager to avoid materials, textures and
//		skins repeated in the memory
//----------------------------------------------------------------------
class YD3DSkinManager : public YSkinManager
{	
	friend class YD3DVertexCache;
	friend class YD3DVertexCacheManager;
	friend class YD3D;

public:
	// Constructor/Destructor
	YD3DSkinManager			(LPDIRECT3DDEVICE9 pDevice, FILE *pLog);
	~YD3DSkinManager		(void);

	HRESULT		AddSkin			( const YCOLOR *pcAmbient,  const YCOLOR *pcDiffuse,
								const YCOLOR *pcEmissive, const YCOLOR *pcSpecular,				
								float fSpecPower, UINT *pnSkinID);
	HRESULT		AddTexture		( UINT nSkinID, const char *chName, bool bAlpha, 
								float fAlpha, YCOLOR *cColorKeys, DWORD dwNumColorKeys);
	bool		MaterialEqual	( const YMATERIAL *pMat0, const YMATERIAL * pMat1);
	bool		ColorEqual		( const YCOLOR *pCol0, const YCOLOR *pCol1); 
	 

protected:
	void		Log				  (char *, ...);
	HRESULT		CreateTexture	  ( YTEXTURE *pTexture);							// Creates a texture
	HRESULT		SetAlphaKey		  (LPDIRECT3DTEXTURE9 *ppTexture, BYTE R, BYTE G,
									BYTE B, BYTE A);								// Set transparency value a espesific color in a texture
	HRESULT		SetTransparency	  (LPDIRECT3DTEXTURE9 *ppTexture, BYTE Alpha);		// Set overall transparency to a texture
	DWORD		MakeD3DColor	  ( BYTE R, BYTE G, BYTE B, BYTE A);				// Transforms a YCOLOR to a color of D3D

protected:
	// Variables
	HINSTANCE		  m_hDLL;		// Module handle to this DLL
	LPDIRECT3DDEVICE9 m_pDevice;	// Direct3d device
	FILE			 *m_pLog;
};

#endif // YD3D_SKINMANAGER