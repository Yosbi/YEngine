//-----------------------------------------------------------------------
// YRenderDevice.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: This file contain a set of abstract classes which provides an interface
//		from which render dlls can inherit. this secures availability
//		of the nesessary render functions

#pragma once
#ifndef YRENDERERDEVICE_H
#define YRENDERERDEVICE_H

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include <Windows.h>
#include <stdio.h>
#include "YEMath.h"
#include "Y.h"


//----------------------------------------------------------------------
// Definitions
//----------------------------------------------------------------------
#define MAX_3DHWND 8	// Defines max 8 child windows
#define MAX_SHADER 20	// Max shaders quantity

//----------------------------------------------------------------------
// Name: YSkinManager (class)
// Desc: YSkinManager is an abstract class which provides an interface
//		from which material managers can inherit. Those heirs need to 
//		implement a store for texture data in a way appropriate to the
//		API.
//----------------------------------------------------------------------
class YSkinManager
{
public:

	// Constructor/Destructor
	YSkinManager ()			 {};
	virtual ~YSkinManager () {};

	// Add a skin to the skin manager, you have to provide 
	// the id where the skin must be saved
	virtual HRESULT AddSkin ( const YCOLOR *pcAmbient,  const YCOLOR *pcDiffuse,
							  const YCOLOR *pcEmissive, const YCOLOR *pcSpecular,				 
							  float fSpecPower, UINT *pnSkinID) = 0;			

	// Add a texture to a skin
	virtual HRESULT	AddTexture ( UINT nSkinID, const char *chName, bool bAlpha, 
								float fAlpha, YCOLOR *cColorKeys, DWORD dwNumColorKeys) = 0;

	// Compares to materials and returns 1 if equal and 0 if not
	virtual bool	MaterialEqual ( const YMATERIAL *pMat0, const YMATERIAL * pMat1) = 0;	

	// Compares two color and returns 1 if equal
	virtual	bool	ColorEqual	(const YCOLOR *pCol0, const YCOLOR *pCol1) = 0;

protected:
	// Member Variables
	UINT		m_nNumSkins;	// Numbers (ids) of the skins
	UINT		m_nNumTextures; // Numbers (ids) of the textures
	UINT		m_nNumMaterials;// Numbers (ids) of the materials
	YSKIN	   *m_pSkins;		// Allocate mem for skins here
	YTEXTURE   *m_pTextures;	// Allocate mem for textures here
	YMATERIAL  *m_pMaterials;	// Allocate mem for materials here	

};
typedef class YSkinManager* LPYSKINMANAGER;

//----------------------------------------------------------------------
// Name: YVertexCacheManager (class)
// Desc: Interface Class to manage static and dynamic verteces
//----------------------------------------------------------------------
class YVertexCacheManager
{
public:

	// Constructor/Destructor
	YVertexCacheManager() {};
	virtual ~YVertexCacheManager() {};

	// Create a static buffer of vertexes, it returns in the parameter 
	// pnID the index of the buffer you create
	virtual HRESULT CreateStaticBuffer (YVERTEXID VertexID, UINT nSkinID, UINT nVerts, UINT nIdis, 
										const void *pVerts, const WORD *pIndis, UINT *pnID) = 0;

	// Render static buffers with the default given skin. Index buffers is optional.
	virtual HRESULT Render(UINT nSBufferID) = 0;

	// Returns the current shade mode
	virtual YRENDERSTATE GetShadeMode() = 0;


};
typedef class YVertexCacheManager* LPYVERTEXCACHEMANAGER;

//----------------------------------------------------------------------
// Name: YRenderer (class)
// Desc: The class of the static lib used to create, get a pointer, and
//		 release an object of a renderer dll
//----------------------------------------------------------------------
class YRenderDevice 
{
public:
	// Const and dest
			 YRenderDevice () {};
	virtual ~YRenderDevice () {};

	//---------------------------
	// Initialize and release
	//---------------------------

	// Initialize the engine, should first display dialog to select
	// graphics adapter and mode , and then start the 3D API
	virtual HRESULT Init		 (HWND hWnd, const HWND *hWnd3D, int nNumhWnd, bool bStencil, bool bSaveLog) = 0;

	// Release API specific stuff
	virtual void	Release () = 0;

	//---------------------------
	// Rendering stuff
	//---------------------------
	
	// Switch swap chain to hwnd in array from init()
	virtual HRESULT UseWindow (UINT nHwnd) = 0;

	// Clear buffer and prepare for rendering
	virtual HRESULT BeginRendering (bool bClearPixel, bool bClearDepth, bool bClearStencil) = 0;

	// End rendering and flip pixel buffer to front
	virtual void	EndRendering () = 0;

	// Clear pixel, depth and stencil buffer
	virtual HRESULT Clear (bool bClearPixel, bool bClearDepth, bool bClearStencil) = 0;

	// Change background color
	virtual void	SetClearColor (float fRed, float fGreen, float fBlue) = 0;

	// Is well initialized the engine? / is the engine running?
	virtual bool	IsRunning () = 0;

	// Is running in windowed mode?
	virtual bool	IsWindowed () = 0;

	// Retrieve the current resolution
	virtual void	GetResolution (POINT *res) = 0;

	// Fade screen to given color with given intensity
    //virtual void	FadeScreen(float fR, float fG, float fB, float fA) = 0;

	// Type nWeight (0=default,700=bold), italic?, 
	// underline?, strike out?, size, return ID
	virtual HRESULT CreateFont(int nWeight, bool bItalic, bool bUnderline, bool bStrike, DWORD dwSize, UINT *pID)  = 0;

	// Draw text: font id, x, y, r, g, b, format string, variables
    virtual HRESULT DrawText(UINT nID, int x, int y, UCHAR r, UCHAR g, UCHAR b, char *ch, ...) = 0;

	//---------------------------
	// View and projection stuff
	//---------------------------

	// Set the world transformation matrix
	virtual void    SetWorldTransform(const YMatrix *mWorld) = 0;

	// Set the near and far clipping plane
	virtual void	SetClippingPlanes (float fNear, float fFar) = 0;

	// Field of view and viewport for stage n (0 to 3) this not will
	// activate any settings, use SetMode() to activate stage
	virtual HRESULT	InitStage(float fFOV, YVIEWPORT *pView, int nStage) = 0;

	// Set stage mode, 0 for perspective projection, 1 for orthogonal
	virtual HRESULT SetMode(YENGINEMODE Mode, int nStage) = 0;

	// View matrix from vRight, vUp, vDir, vPos
	virtual HRESULT SetView3D (const YVector &vcRight, const YVector &vcUp, const YVector &vcDir, const YVector &vcPos) = 0;

	// View matrix form position, fix point, worldup
	virtual HRESULT SetViewLookAt (const YVector &vcPos, const YVector &vcPoint, const YVector &vcWorldUp) = 0;

	//---------------------------
	// Shader stuff
	//---------------------------

	// Checks for support of the shader version, return true if the version is supported
	virtual bool CanDoShaderVersion(YSHADERVER shVer) = 0;

	// Create vertex shader from compiled or uncompiled file/memory
	virtual HRESULT CreateVShader	 (const void *pData, UINT nSize, bool bLoadFromFile, bool bIsCompiled, UINT *pID) = 0;

	// Create pixel shader from compiled or uncompiled file/memory
	virtual HRESULT CreatePShader	 (const void *pData, UINT nSize, bool bLoadFromFile, bool bIsCompiled, UINT *pID) = 0;

	// Use vertex shader
	virtual HRESULT ActivateVShader	 (UINT nID, YVERTEXID VertexID) = 0;

	// Use vertex shader
	virtual HRESULT ActivatePShader	 (UINT nID) = 0;

	// Is using shaders?
	/*virtual bool	UsesShaders		 () = 0;*/

	// Activate or deactivate shaders
	/*virtual void	UseShaders		 (bool b) = 0;*/

	// This hardware can do shaders?
	/*virtual bool    CanDoShaders     () = 0;*/

	// Sets constant value to the shaders, select which shader type
	// (pixel or vertex) and which type of data is the input. Vertex
	// shaders registers form 0-20 are reserved and create an error.
	// The nNum parameter is the quantity of that data type referencing
	// the pointer
	virtual HRESULT SetShaderConstant(YSHADERTYPE sht, YDATATYPE dat, UINT nReg, UINT nNum, const void *pData) = 0;

	// Retrieves the current vertex shader active
	virtual UINT    GetActiveVShadID(void) = 0;

	// Retrieves the current vertex shader active
    virtual UINT    GetActivePShadID(void) = 0;

	//---------------------------
	// Effects stuff
	//---------------------------
	virtual HRESULT CreateFX(const char *pData, const char *chTechnique, bool bDebug, UINT *pFXID) = 0;
	virtual HRESULT SetFXConstant(YDATATYPE dat, const char* chName, const void *pData, UINT nBytes) = 0;
	virtual HRESULT ActivateFX(UINT nID) = 0;

	//---------------------------
	// Skin manager stuff
	//---------------------------

	// Get the skin manager
	virtual LPYSKINMANAGER GetSkinManager() = 0;

	// Get the index of the currently active skin
	virtual UINT	GetActiveSkinID	 ( ) = 0;

	// Set an skin to render
	virtual void	SetActiveSkinID	 (UINT nID) = 0;

	//---------------------------
	// Vertex Cache Manager stuff
	//---------------------------

	// Get the vertex manager
	virtual LPYVERTEXCACHEMANAGER	GetVertexManager () = 0;

	//---------------------------
	// Manage Render States
	//---------------------------

	// Sets the texture operation on a determined stage the posible values are
	// RS_TEX_ADDSIGNED, RS_TEX_MODULATE
	virtual HRESULT SetTextureStage(UCHAR n, YRENDERSTATE rs) = 0;

	// Retrieves the current shade mode in use
	virtual YRENDERSTATE GetShadeMode() = 0;

	// Return true if are using textures
	virtual bool	UsesTextures(void) = 0;

	// Returns the color of the wireframe
	virtual YCOLOR  GetWireColor() = 0;

	//---------------------------
	// Light stuff
	//---------------------------

	// Set ambient light level
    virtual void SetAmbientLight(float fRed, float fGreen, float fBlue) = 0;

protected:
	HWND					m_hWndMain;			// The application main window
	HWND					m_hWnd[MAX_3DHWND]; // 3D render window handle
	UINT					m_nNumhWnd;			// Number of hWnd in the m_hWnd array
	UINT					m_nActivehWnd;		// which one is active ( 0 = main hwnd)
	HINSTANCE				m_hDLL;				// DLL module handle
	DWORD					m_dwWidth;			// Screen width
	DWORD					m_dwHeight;			// Screen Height
	bool					m_bWindowed;		// Windowed mode?
	bool					m_bRunning;			// After succesfull initialization
	//bool					m_bUseShaders;      // shaders or fixed function pipeline
	//bool					m_bCanDoShaders;    // Hw can do shaders version 1.1
	bool					m_bColorBuffer;		// Use color buffer
	bool					m_bAdditive;        // Use additive blending rendering
	bool					m_bTextures;        // use textures?
	FILE				   *m_pLog;				// Log file
	LPYSKINMANAGER			m_pSkinManager;		// Pointer to use the skin manager
	LPYVERTEXCACHEMANAGER	m_pVertexManager;	// Pointer to use the vertex cache manager
	YCOLOR					m_clrWire;          // Color for wireframe rendering
	YRENDERSTATE			m_ShadeMode;        // Wireframe rendering?


	// View and projection vars
	float					m_fNear;			// Near Plane
	float					m_fFar;				// Far Plane
	YENGINEMODE				m_Mode;				// 2D, 3D ...
	int						m_nStage;			// Stage (0-3)
	YVIEWPORT				m_VP[4];			// View ports

};
typedef class YRenderDevice *LPYRENDERDEVICE;

//----------------------------------------------------------------------
// Desc: This are methods to export from the dll and typedef pointers
//		to be able to do the exportation
//----------------------------------------------------------------------
extern "C"
{
	HRESULT CreateRenderDevice (HINSTANCE hDLL, LPYRENDERDEVICE *pInterface);
	typedef HRESULT (*CREATERENDERDEVICE)(HINSTANCE hDLL, LPYRENDERDEVICE *pInterface);

	HRESULT ReleaseRenderDevice (LPYRENDERDEVICE *pInterface);
	typedef HRESULT (*RELEASERENDERDEVICE)(LPYRENDERDEVICE *pInterface);
}
#endif //YRENDERERDEVICE_H