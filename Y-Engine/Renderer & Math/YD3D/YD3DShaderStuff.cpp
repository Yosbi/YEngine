//-------------------------------------------------------------------------------
// YD3DShaderStuff.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: This file contains some functions used the setup of the shaders
//		such loading a shader
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Include
//--------------------------------------------------------------------------
#include "YD3D.h"
//#include "math.h"
extern bool g_bLF; // to know if the user wants the log of the engine

//-----------------------------------------------------------------------
// Name:CanDoShaderVersion
// Desc: Return true if the hw can do the shader version
//--------------------------------------------------------------------------
bool YD3D::CanDoShaderVersion(YSHADERVER shVer)
{
	D3DCAPS9 d3dCaps;
	if (FAILED(m_pDevice->GetDeviceCaps(&d3dCaps)))
	{
		if(g_bLF)Log("error: GetDeviceCaps() in CanDoShaderVersion failed");		
		return false;
	}
	switch(shVer)
	{
		// Case shader 1.1
		case SH_1_1:{
			if(d3dCaps.VertexShaderVersion < D3DVS_VERSION(1,1))
			{
				if(g_bLF)Log("warning: Vertex Shader Version < 1.1 => no support");
				return false;				
			}		
			else if(d3dCaps.PixelShaderVersion < D3DVS_VERSION(1,1))
			{
				if(g_bLF)Log("warning: Pixel Shader Version < 1.1 => no support");
				return false;
			}
			else
			{
				if(g_bLF)Log("HW Supports Shaders Version 1.1");
				return true;
			}
			} break;

			// Case shader 2.0
		case SH_2_0:{
			if(d3dCaps.VertexShaderVersion < D3DVS_VERSION(2,0))
			{
				if(g_bLF)Log("warning: Vertex Shader Version < 2.0 => no support");
				return false;				
			}	
			else if(d3dCaps.PixelShaderVersion < D3DVS_VERSION(2,0))
			{
				if(g_bLF)Log("warning: Pixel Shader Version < 2.0 => no support");
				return false;
			}
			else
			{
				if(g_bLF)Log("HW Supports Shaders Version 2.0");
				return true;
			}
			} break;

			// Case shader 3.0
		case SH_3_0:{
			if(d3dCaps.VertexShaderVersion < D3DVS_VERSION(3,0))
			{
				if(g_bLF)Log("warning: Vertex Shader Version < 3.0 => no support");
				return false;				
			}
			if(d3dCaps.PixelShaderVersion < D3DVS_VERSION(3,0))
			{
				if(g_bLF)Log("warning: Pixel Shader Version < 3.0 => no support");
				return false;
			}
			else
			{
				if(g_bLF)Log("HW Supports Shaders Version 3.0");
				return true;
			}
			} break;

		default:{
			if(g_bLF)Log("Unknow YSHADERVER");
			return false;
			}
	}	
}

//-----------------------------------------------------------------------
// Name:PrepareShaderStuff
// Desc: This function evaluates shader support and initializes everything
//		 i need to use shaders if they are available
//--------------------------------------------------------------------------
void YD3D::PrepareShaderStuff()
{
	// Vertex declarations
	D3DVERTEXELEMENT9 declPVertex[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 declVertex[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 declLVertex[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,    0}, 
		{ 0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 declCVertex[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1}, 
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl3TVertex[] =
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
		{ 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 1}, 
		{ 0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 2}, 
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 declTVertex[] = 
	{
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION, 0}, 
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,   0}, 
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD, 0}, 
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TANGENT,  0}, 
		D3DDECL_END() 
	};

	// Create declarations
   m_pDevice->CreateVertexDeclaration(declVertex,   &m_pDeclVertex);
   m_pDevice->CreateVertexDeclaration(declPVertex,  &m_pDeclPVertex);
   m_pDevice->CreateVertexDeclaration(declLVertex,  &m_pDeclLVertex);
   m_pDevice->CreateVertexDeclaration(declCVertex,  &m_pDeclCVertex);
   m_pDevice->CreateVertexDeclaration(decl3TVertex, &m_pDecl3TVertex);
   m_pDevice->CreateVertexDeclaration(declTVertex,  &m_pDeclTVertex);
   m_pDevice->SetFVF(NULL);

   if(g_bLF)Log("Use of shaders activated");
}

//-----------------------------------------------------------------------
// Name:CreateVShader
// Desc: Create a vertex shader object for the dirext3d device
//--------------------------------------------------------------------------
HRESULT YD3D::CreateVShader(const void *pData, UINT nSize, bool bLoadFromFile, bool bIsCompiled, UINT *pID)
{
	LPD3DXBUFFER pCode  = NULL;
	LPD3DXBUFFER pDebug = NULL;
	HRESULT		 hrC = Y_OK, hrA = Y_OK;
	DWORD		*pVS    = NULL;
	HANDLE		 hFile, hMap;
	
	// Is there space for one more??
	if (m_nNumVShaders >= (MAX_SHADER - 1))
	{
		if(g_bLF)Log("Error: max number of shaders reached in YD3D::CreateVShader");
		return Y_OUTOFMEMORY;
	}

	// (1): Already Assembled
	if (bIsCompiled)
	{
		// Already compiled from file
		if (bLoadFromFile)
		{
			hFile = CreateFile((LPCTSTR)pData, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				if (g_bLF)Log("Error: CreateFile() in YD3D::CreateVShader failed");
				return Y_FILENOTFOUND;
			}
			hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, 0);
			pVS = (DWORD*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
		}
		else // Already compiled (from datapointer)
			pVS = (DWORD*)pData;
	}
	else // (2) Still needs assembling
	{
		// Not yet compiled (from file)
		if (bLoadFromFile)
		{
			hrA = D3DXAssembleShaderFromFile((char*)pData, NULL, NULL, 0, &pCode, &pDebug);
		}
		 // not yet compiled (from datapointer)
		else 
		{
			hrA = D3DXAssembleShader((char*)pData, nSize-1, NULL, NULL, 0, &pCode, &pDebug);
		}

		// Track errors if any
		if (SUCCEEDED(hrA)) 
		{
			pVS = (DWORD*)pCode->GetBufferPointer();
        }
		else 
		{
			if(g_bLF)Log("Error: Assemble[Vertex]Shader[FromFile]() failed");
			if (hrA == D3DERR_INVALIDCALL)
				if(g_bLF)Log("INVALID_CALL");
			else if (hrA == D3DXERR_INVALIDDATA)
				if(g_bLF)Log("D3DXERR_INVALIDDATA");
			else if (hrA == E_OUTOFMEMORY)
				if(g_bLF)Log("E_OUTOFMEMORY");
			if (pDebug->GetBufferPointer())
				if(g_bLF)Log("Shader debugger says: %s", (char*)pDebug->GetBufferPointer());
			else 
				if(g_bLF)Log("no debug infos stored");

			return Y_FAIL;
		}
	}

	// create the shader object
	if (FAILED(hrC=m_pDevice->CreateVertexShader(pVS, &m_pVShader[m_nNumVShaders]))) 
	{
		if(g_bLF)Log("error: CreateVertexShader() failed");
		if (hrC==D3DERR_INVALIDCALL)
			if(g_bLF)Log("INVALID_CALL");
		else if (hrC==D3DERR_OUTOFVIDEOMEMORY)
			if(g_bLF)Log("D3DERR_OUTOFVIDEOMEMORY");
		else if (hrC==E_OUTOFMEMORY)
			if(g_bLF)Log("E_OUTOFMEMORY");

		return Y_FAIL;
	}

	 // store index to this shader
   if (pID) (*pID) = m_nNumVShaders;

   // free resources
   if (bIsCompiled && bLoadFromFile) 
   {
		UnmapViewOfFile(pVS);
		CloseHandle(hMap);
		CloseHandle(hFile);
   }
   
   m_nNumVShaders++;
   return Y_OK;
}

//-----------------------------------------------------------------------
// Name:CreatePShader
// Desc: Create a pixel shader object for the dirext3d device
//--------------------------------------------------------------------------
HRESULT YD3D::CreatePShader(const void *pData, UINT nSize, bool bLoadFromFile, bool bIsCompiled, UINT *pID) 
{
	LPD3DXBUFFER  pCode=NULL;
	LPD3DXBUFFER  pDebug=NULL;
	HRESULT       hrC=Y_OK, hrA=Y_OK;
	DWORD        *pPS=NULL;
	HANDLE        hFile, hMap;

	// do I have space for one more?
	if (m_nNumPShaders >= (MAX_SHADER-1))
		return Y_OUTOFMEMORY;

	// (1): ALREADY ASSEMBLED
	if (bIsCompiled) 
	{
		// already compiled (from file)
		if (bLoadFromFile) 
		{
			hFile = CreateFile((LPCTSTR)pData, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile == INVALID_HANDLE_VALUE) {
				if(g_bLF)Log("Error: CreateFile() in ZFXD3D::CreatePShader failed");
				return Y_FILENOTFOUND;
			}
		hMap = CreateFileMapping(hFile,0,PAGE_READONLY, 0,0,0);
		pPS = (DWORD*)MapViewOfFile(hMap,FILE_MAP_READ,0,0,0);
		}
		// already compiled (from datapointer)
		else { pPS = (DWORD*)pData; }
	} 

	// (2): STILL NEEDS ASSEMBLING
	else 
	{
		// not yet compiled (from file)
		if (bLoadFromFile) 
		{
			hrA = D3DXAssembleShaderFromFile((char*)pData, NULL, NULL, 0, &pCode, &pDebug);
		}
		// not yet compiled (from datapointer)
		else 
		{
			hrA = D3DXAssembleShader((char*)pData, nSize-1,	NULL, NULL, 0, &pCode, &pDebug);
		}

		// track errors if any
		if (SUCCEEDED(hrA)) 
		{
			pPS = (DWORD*)pCode->GetBufferPointer();
		}
		else 
		{
			if(g_bLF)Log("Error: Assemble[Pixel]Shader[FromFile]() failed");
			if (hrA == D3DERR_INVALIDCALL)
				if(g_bLF)Log("INVALID_CALL");
			else if (hrA == D3DXERR_INVALIDDATA)
				if(g_bLF)Log("D3DXERR_INVALIDDATA");
			else if (hrA == E_OUTOFMEMORY)
				if(g_bLF)Log("E_OUTOFMEMORY");
			if (pDebug->GetBufferPointer())
				if(g_bLF)Log("Shader debugger says: %s", (char*)pDebug->GetBufferPointer());
			else 
				if(g_bLF)Log("no debug infos stored");

			return Y_FAIL;
		}
	}
	// create the shader object
	hrC = m_pDevice->CreatePixelShader(pPS, &m_pPShader[m_nNumPShaders]);
	if (FAILED(hrC))
	{
		if(g_bLF)Log("Error: CreatePixelShader() failed");
		if (hrC == D3DERR_INVALIDCALL)
			if(g_bLF)Log("INVALID_CALL");
		else if (hrC == D3DERR_OUTOFVIDEOMEMORY)
			if(g_bLF)Log("D3DERR_OUTOFVIDEOMEMORY");
		else if (hrC == E_OUTOFMEMORY)
			if(g_bLF)Log("E_OUTOFMEMORY");

		return Y_FAIL;
	}

	// store index to this shader
	if (pID) (*pID) = m_nNumPShaders;

	// free resources
	if (bIsCompiled && bLoadFromFile) 
	{
		UnmapViewOfFile(pPS);
		CloseHandle(hMap);
		CloseHandle(hFile);
	}
	
	m_nNumPShaders++;
	return Y_OK;
} // CreatePShader

//-----------------------------------------------------------------------
// Name: UseShaders
// Desc: This function evaluates shader support and initializes everything
//		 i need to use shaders if they are available
//--------------------------------------------------------------------------
//void YD3D::UseShaders(bool b) 
//{ 
//	if (!m_bCanDoShaders) return;
//
//	if (m_bUseShaders == b) return;
//
//	// clear all vertex caches
//	/*m_pVertexManager->ForcedFlushAll();
//	m_pVertexManager->InvalidateStates();*/
//
//	m_bUseShaders = b;
//
//	// deactivate use of shaders
//	if (!m_bUseShaders) 
//	{
//		m_pDevice->SetVertexShader(NULL);
//		m_pDevice->SetPixelShader(NULL);
//		m_pDevice->SetVertexDeclaration(NULL);
//	}
//	else
//	{
//		m_pDevice->SetFVF(0);
//	}
//} // UseShaders

//-----------------------------------------------------------------------
// Name: ActivateVShader
// Desc: Activate a prior create vertex shader for the render device
//--------------------------------------------------------------------------
HRESULT YD3D::ActivateVShader(UINT nID, YVERTEXID VertexID) 
{
	if (nID >= m_nNumVShaders) return Y_INVALIDID;

	// clear all vertex caches
//	m_pVertexManager->ForcedFlushAll();
////   m_pVertexMan->InvalidateStates();

	if (FAILED(m_pDevice->SetVertexShader(m_pVShader[nID])))
		return Y_FAIL;

	m_nActiveVShader = nID;
	//m_bUseShaders	 = true;
	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: ActivatePShader
// Desc: Activate a prior create pixel shader for the render device
//--------------------------------------------------------------------------
HRESULT YD3D::ActivatePShader(UINT nID) 
{
	HRESULT hr;	
	if (nID >= m_nNumPShaders) return Y_INVALIDID;

	// clear out buffers prior to state changes
	//m_pVertexManager->ForcedFlushAll();
	//// m_pVertexMan->InvalidateStates();

	hr = m_pDevice->SetPixelShader(m_pPShader[nID]);

	if (FAILED(hr))
		return Y_FAIL;

	m_nActivePShader = nID;
	//m_bUseShaders	 = true;

	return Y_OK;
} 

//-----------------------------------------------------------------------
// Name: SetShaderConstant
// Desc: Sets constant value to the shaders, select which shader type
//		 (pixel or vertex) and which type of data is the input. Vertex
//		 shaders registers form 0-20 are reserved and create an error
//--------------------------------------------------------------------------
HRESULT YD3D::SetShaderConstant(YSHADERTYPE sht, YDATATYPE dat, UINT nReg, UINT nNum, const void *pData) 
{
	if (sht == SHT_VERTEX) 
	{
		if (nReg < 20) return Y_INVALIDPARAM;

		switch (dat) 
		{
			case DAT_BOOL:
				m_pDevice->SetVertexShaderConstantB(nReg, (BOOL*)pData, nNum);
				break;
			case DAT_INT:
				m_pDevice->SetVertexShaderConstantI(nReg, (int*)pData, nNum);
				break;
			case DAT_FLOAT:
				m_pDevice->SetVertexShaderConstantF(nReg, (float*)pData, nNum);
				break;
			default: return Y_FAIL;
		}
	}
	else 
	{
		switch (dat) 
		{
			case DAT_BOOL:
				m_pDevice->SetPixelShaderConstantB(nReg, (BOOL*)pData, nNum);
				break;
			case DAT_INT:
				m_pDevice->SetPixelShaderConstantI(nReg, (int*)pData, nNum);
				break;
			case DAT_FLOAT:
				m_pDevice->SetPixelShaderConstantF(nReg, (float*)pData, nNum);
				break;
			default: return Y_FAIL;
		} 
	}
	return Y_OK;
}