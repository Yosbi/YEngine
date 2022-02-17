//-----------------------------------------------------------------------
// YD3D.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: This file contains all related functions, methods an data structs
//		to manipulate, initialize, shuts down among other things the Direct3D

#ifndef YD3D_H
#define YD3D_H

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <vector>
#include <tchar.h>
#include "..\YRenderer\YRenderDevice.h"
#include "Y.h"							// General header
#include "resource.h"					// The resourses ( the dialog )
#include "YD3DEnum.h"					// The enum class(es)
#include "YD3DSettingsDlg.h"			// The dialog box class
#include "YD3DSkinManager.h"			// The skin manager class
#include "YD3DVertexCache.h"
#include "../YEMath/YEMath.h"		// the math classes
#include <Vfw.h>

//#include <strsafe.h>
#pragma comment(lib, "vfw32.lib")  
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "../Debug/YEMath.lib")// I must change this for the release version

//----------------------------------------------------------------------
// Desc: Functions to export
//----------------------------------------------------------------------
//extern "C" _declspec(dllexport)  HRESULT CreateRenderDevice (HINSTANCE hDLL, LPYRENDERDEVICE *pInterface);
//extern "C" _declspec(dllexport)  HRESULT ReleaseRenderDevice (LPYRENDERDEVICE *pInterface);
//----------------------------------------------------------------------
// Name: DLLEntryPoint
// Desc: Is similar to WinMain()/main() functions, is used to initialize
//		the DLL
//----------------------------------------------------------------------
BOOL WINAPI	DllEntryPoint( HINSTANCE hDll, DWORD fdwReason, LPVOID lpvReserved );

#define LPDIRECT3DVDECL9 LPDIRECT3DVERTEXDECLARATION9

#define IDENTITY(m) {memset(&m,0,sizeof(D3DMATRIX));m._11=m._22=m._33=m._44=1.0f;}

//----------------------------------------------------------------------
// Name: YD3D (Class)
// Desc: Class definition for a Direct3D render device, implementing the
//		interface YRenderDevice
//----------------------------------------------------------------------
typedef struct YEFFECT_TYPE
{
	LPD3DXEFFECT	FX;			  // The effect data
	D3DXHANDLE		hTech;		  // Handle to the technique used for this effect
	D3DXHANDLE		hWVP;		  // Handle to the global var gWVP inside the effect;
	D3DXHANDLE		hDiffuseMat;  // Handle to the diffuse material var
	D3DXHANDLE		hAmbientMat;  // Handle to the ambient material var
	D3DXHANDLE		hEmissiveMat; // Handle to the emissive material var
	D3DXHANDLE		hSpecMat;	  // Handle to the specular material var
	D3DXHANDLE		hSpecPowMat;  // Handle to the specular power of the material
	D3DXHANDLE		hAmbientLight;// Handle to the general ambient light
	D3DXHANDLE		hEyePos;	  // Handle to the camera position in world space
	D3DXHANDLE		hTex[8];	  // Handle to the 7 textures
} YFX;

//----------------------------------------------------------------------
// Name: YD3D (Class)
// Desc: Class definition for a Direct3D render device, implementing the
//		interface YRenderDevice
//----------------------------------------------------------------------
class YD3D : public YRenderDevice
{
public:
	// Constructor and destructor
	YD3D ( HINSTANCE hDll);
	~YD3D( );

	//---------------------------
	// Initialize engine stuff
	//---------------------------
	HRESULT	Init			(HWND hWnd, const HWND *hWnd3D, int nNumhWnd, bool bStencil, bool bSaveLog);
	//BOOL CALLBACK DlgProc	(HWND, UINT, WPARAM, LPARAM);
	void	Release				 ( );

	//---------------------------
	// Rendering stuff
	//---------------------------
	HRESULT UseWindow			 (UINT nHwnd);	
	bool	IsRunning			 ( ) {return m_bRunning;}
	bool    IsSceneRunning		 ( ) {return m_bIsSceneRunning;}
	bool	IsWindowed			 ( ) {return m_bWindowed;}
	void	GetResolution		 (POINT *pPt) {pPt->x = m_dwWidth; pPt->y = m_dwHeight;}
	HRESULT BeginRendering		 (bool bClearPixel, bool bClearDepth, bool bClearStencil);
	HRESULT Clear				 (bool bClearPixel, bool bClearDepth, bool bClearStencil);
	void	EndRendering		 ( );
	void	SetClearColor		 (float fRed, float fGreen, float fBlue);
	//void	FadeScreen			 (float fR, float fG, float fB, float fA);
	HRESULT CreateFont			 (int nWeight, bool bItalic, bool bUnderline, bool bStrike, DWORD dwSize, UINT *pID);
	HRESULT DrawText			 (UINT nID, int x, int y, UCHAR r, UCHAR g, UCHAR b, char *ch, ...);

	//---------------------------
	// View and projection stuff
	//---------------------------
	void    SetWorldTransform	 (const YMatrix *mWorld);
	void	SetClippingPlanes	 (float fNear, float fFar);
	HRESULT	InitStage			 (float fFOV, YVIEWPORT *pView, int nStage);
	HRESULT SetMode				 (YENGINEMODE Mode, int nStage);
	HRESULT SetView3D			 (const YVector &vcRight, const YVector &vcUp, const YVector &vcDir, const YVector &vcPos);
	HRESULT SetViewLookAt(const YVector &vcPos, const YVector &vcPoint, const YVector &vcWorldUp);

	//---------------------------
	// Shader stuff
	//---------------------------
	HRESULT CreateVShader		 (const void *pData, UINT nSize, bool bLoadFromFile, bool bIsCompiled, UINT *pID);
	HRESULT CreatePShader		 (const void *pData, UINT nSize, bool bLoadFromFile, bool bIsCompiled, UINT *pID);
	HRESULT ActivateVShader		 (UINT nID, YVERTEXID VertexID);
	HRESULT ActivatePShader		 (UINT nID);
	/*bool	UsesShaders			 ( ) {return m_bUseShaders;}*/
	bool	CanDoShaderVersion	 (YSHADERVER shVer);	
	/*void	UseShaders			 (bool b);*/
	HRESULT	SetShaderConstant	 (YSHADERTYPE sht, YDATATYPE dat, UINT nReg, UINT nNum, const void *pData);
	UINT    GetActiveVShadID	 ( ) { return m_nActiveVShader; }
    UINT    GetActivePShadID	 ( ) { return m_nActivePShader; }

	//---------------------------
	// Effects stuff
	//---------------------------
	HRESULT CreateFX(const char *pData, const char *chTechnique, bool bDebug, UINT *pFXID);
	HRESULT SetFXConstant(YDATATYPE dat, const char* chName, const void *pData, UINT nBytes);
	HRESULT ActivateFX(UINT nID);
	void ChangeFXTechnique(UINT nFXID, const char *chTechnique);

	//---------------------------
	// SkinManager Stuff
	//---------------------------
	LPYSKINMANAGER GetSkinManager( ) {return m_pSkinManager;}
	UINT	GetActiveSkinID		 ( )	 {return m_nActiveSkin;}
	void	SetActiveSkinID		 (UINT nID) {m_nActiveSkin = nID;}

	//---------------------------
	// Vertex Cache Manager Stuff
	//---------------------------
	LPYVERTEXCACHEMANAGER GetVertexManager() {return m_pVertexManager;}

	//---------------------------
	// Render Estates
	//---------------------------
	void	UseColorBuffer		 (bool b);
	bool    UsesColorBuffer		 ( )  { return m_bColorBuffer; }
	void	SetDepthBufferMode	 (YRENDERSTATE rs);
	//HRESULT SetTextureStage		 (UCHAR n, YRENDERSTATE rs);
	//YRENDERSTATE GetShadeMode	 ( ) {return m_ShadeMode;}
	void	SetBackfaceCulling	 (YRENDERSTATE rs);
	void	SetStencilBufferMode (YRENDERSTATE rs, DWORD dw);
	//bool	UsesTextures		 ( ) { return m_bTextures; }
	void	UseAdditiveBlending	 (bool b);
	bool    UsesAdditiveBlending ( ) { return m_bAdditive; }
	/*void    UseAlphaBlending(bool b);
	bool	UsesAlphaBlending(void);*/
	//YCOLOR  GetWireColor		 ( ) { return m_clrWire; }
	//D3DTEXTUREOP GetTOP			 (UCHAR n) {return m_TOP[n];}

	//---------------------------
	// Light stuff
	//---------------------------
    void	SetAmbientLight		 (float fRed, float fGreen, float fBlue);

	//---------------------------
	// Mesh stuff
	//---------------------------
	HRESULT MeshCreateTeapot(UINT *nMID);
	HRESULT MeshCreateSphere(float fRadius, UINT nSlices, UINT nStacks, UINT *nMID);
	HRESULT MeshCreateCylinder(float fRadius1, float fRadius2, float fLength, UINT nSlices, UINT nStacks, const char cSkinOrientation, UINT *nMID);
	HRESULT MeshRender(UINT nMID, UINT nSkinID);



private:
	// Private functions
	HRESULT Go					 ( );										// Start API with values from dialog box
	void	Prepare2D			 ( );										// Prepare 2D projection matrix
	void	Log					 (char*, ...);								// Write to log file
	HRESULT	CalcPerspProjMatrix	 (float fFOV, float fAspect, D3DMATRIX *m);	// Set the perspective proyection matrix
	HRESULT FirstTimeInit		 ();										// Init the first time the engine with defaults values
	void	PrepareShaderStuff	 ( );										// Init shaders stuff
	void	CalcViewProjMatrix	 ( );										// Calc combo matrix
	void	CalcWorldViewProjMatrix( );										// Calc combo matrix
	HRESULT	GenSphericalTexCoords(UINT nMesh);								// Set tex coord to an spherical object
	HRESULT genCylTexCoords		 (UINT nMesh, const char cTextOrientation); // Set tex coord to an cylindrical object

private:
	// Private variables
	YD3DSettings			m_D3DSettings;			// The settings used to initialize D3D
	LPDIRECT3D9				m_pD3D;					// DIrect3D object
	LPDIRECT3DDEVICE9		m_pDevice;				// Direct3D device object
	LPDIRECT3DSWAPCHAIN9	m_pChain[MAX_3DHWND];	// Swap chain array, to change between to render
	D3DPRESENT_PARAMETERS	m_d3dpp;				// Present parameters struct
	D3DCOLOR				m_ClearColor;			// Color to clean the back buffer
	D3DMATERIAL9            m_StdMtrl;              // Standard material
	LPD3DXFONT			   *m_pFont;                // Font objects
    UINT					m_nNumFonts;            // Number of fonts
	bool					m_bIsSceneRunning;		// To know if the scene is runing or not
	bool					m_bStencil;				// To know if the user is using stencil
	//bool					m_bTextures;			// Use textures?
	UINT					m_nActiveSkin;          // Skin currently active
	YCOLOR					m_ycAmbientLight;		// The scene ambient light
	YVector					m_vcEyePos;				// The camera position
	D3DTEXTUREOP			m_TOP[8];				// Active texture operations

	// View and projection vars
	D3DMATRIX 				m_mView2D,				// View matrix 2D
							m_mView3D,				// View matrix 3D
							m_mProj2D,				// Projection 2D
							m_mProjP[4],			// Projection persp
							m_mProjO[4],			// Projection orthog
							m_mWorld,				// World transformation
							m_mViewProj,			// Combo matrix
							m_mWorldViewProj;		// Combo matrix

	// Mesh variables
	static const int		MAX_MESH = 50;			// Max number of meshes
	LPD3DXMESH   		    m_pMesh[MAX_MESH];		// Array of meshes
	UINT					m_nNumMeshes;			// Number of meshes

public:
	// Shaders variables
	LPDIRECT3DVDECL9		m_pDeclPVertex;			// Just untransformed position
	LPDIRECT3DVDECL9		m_pDeclVertex;			// Untransformed unlit vertex
	LPDIRECT3DVDECL9		m_pDeclLVertex;			// Untransformed and lit vertex
	LPDIRECT3DVDECL9		m_pDeclCVertex;			// Used for character animation
	LPDIRECT3DVDECL9		m_pDecl3TVertex;		// Three textures coord pairs
	LPDIRECT3DVDECL9		m_pDeclTVertex;			// Like UU but with tangent vector

	LPDIRECT3DVERTEXSHADER9 m_pVShader[MAX_SHADER];	// Vertex shaders
	LPDIRECT3DPIXELSHADER9  m_pPShader[MAX_SHADER];	// Pixel shaders
	UINT					m_nNumVShaders;			// Current num of Vertex shaders in the array
	UINT					m_nNumPShaders;			// Current num of Pixel shaders in the array

	UINT					m_nActiveVShader;		// Currently active vertex shader
	UINT					m_nActivePShader;		// Currently active pixel shader

	// Effects variables
	static const int		MAX_FX = 50;			// Max number of effects
	YFX						m_pYFX[MAX_FX];			// Array of effects
	UINT					m_nNumFX;				// Num of effects
	UINT					m_nActiveFX;			// The current active effect
	UINT					m_nPasses;				// Number of passes of the current effect

};

#endif //YD3D_H