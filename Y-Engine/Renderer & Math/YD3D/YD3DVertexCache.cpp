//-------------------------------------------------------------------------------
// YD3DVertexCache.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: Implementation of the YD3DVertexCacheManager and YD3DVertexCache

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "YD3DVertexCache.h" // Class definition
extern bool g_bLF;			 // to know if the user wants the log of the engine

//-----------------------------------------------------------------------------
// Name: Constructor(Manager)
// Desc: Initializes all the variables
//-----------------------------------------------------------------------------
YD3DVertexCacheManager::YD3DVertexCacheManager(YD3DSkinManager *pSkinMan, LPDIRECT3DDEVICE9 pDevice, YD3D *pYD3D, 
											   UINT nMaxVerts, UINT nMaxIndis, FILE *pLog)
{
	DWORD dwID			= 1;

	m_pSB				= NULL;
	//m_pIB				= NULL;
	m_nNumSB			= 0;
	m_nNumIB			= 0;

	m_pLog				= pLog;
	m_pDevice			= pDevice;
	m_pYD3D				= pYD3D;
	m_pSkinMan			= pSkinMan;
	m_dwActiveCache		= MAX_ID;
	m_dwActiveSB		= MAX_ID;
	m_dwActiveIB		= MAX_ID;
	m_dwActiveVertexDcl = MAX_ID;

	// Init dynamic buffers
	//for (int i = 0; i < NUM_CACHES; i++) 
	//{
	//	m_CachePS[i] = new YD3DVertexCache(nMaxVerts, nMaxIndis, sizeof(PVERTEX), pSkinMan, pDevice, this,
	//									   dwID++, FVF_PVERTEX, pLog);

	//	m_CacheUU[i] = new YD3DVertexCache(nMaxVerts, nMaxIndis, sizeof(VERTEX), pSkinMan, pDevice, this,
	//									   dwID++, FVF_VERTEX, pLog);
 //     
	//	m_CacheUL[i] = new YD3DVertexCache(nMaxVerts, nMaxIndis, sizeof(LVERTEX), pSkinMan, pDevice, this,
	//									   dwID++, FVF_LVERTEX, pLog);

	//	m_CacheCA[i] = new YD3DVertexCache(nMaxVerts, nMaxIndis, sizeof(CVERTEX), pSkinMan, pDevice, this,
	//									   dwID++, FVF_CVERTEX, pLog);

	//	m_Cache3T[i] = new YD3DVertexCache(nMaxVerts, nMaxIndis, sizeof(VERTEX3T), pSkinMan, pDevice, this,
	//									   dwID++, FVF_T3VERTEX, pLog);

	//	m_CacheTV[i] = new YD3DVertexCache(nMaxVerts, nMaxIndis, sizeof(TVERTEX), pSkinMan, pDevice, this,
	//									   dwID++, FVF_TVERTEX, pLog);
	//	} // for
	if(g_bLF)Log("online");
}

//-----------------------------------------------------------------------------
// Name: Destructor(Manager)
// Desc: Releases vertex cache arrays
//-----------------------------------------------------------------------------
YD3DVertexCacheManager::~YD3DVertexCacheManager()
{
	UINT n = 0;
	int	 i = 0;

	// Release static buffers
	if (m_pSB)
	{
		for (n = 0; n < m_nNumSB; n++)
		{
			if(m_pSB[n].pVB) // If vertex buffer
			{
				m_pSB[n].pVB->Release();
				m_pSB[n].pVB = NULL;
			}
			if(m_pSB[n].pIB) // If index buffer
			{
				m_pSB[n].pIB->Release();
				m_pSB[n].pIB = NULL;
			}
		}
		free(m_pSB);
		m_pSB = NULL;
	}

	// Release index buffers
	/*if (m_pIB)
	{
		for ( n = 0; n < m_nNumIB; n++)
		{
			if (m_pIB[n].pIB)
			{
				m_pIB[n].pIB->Release();
				m_pIB[n].pIB = NULL;
			}
		}
		free(m_pIB);
		m_pIB = NULL;
	}*/

	// free dynamic vertex caches
	/*for (i = 0; i < NUM_CACHES; i++)
	{
		if (m_CachePS[i])
		{
			delete m_CachePS[i];
			m_CachePS[i] = NULL;
		}

		if (m_CacheUU[i])
		{
			delete m_CacheUU[i];
			m_CacheUU[i] = NULL;
		}

		if (m_CacheUL[i])
		{
			delete m_CacheUL[i];
			m_CacheUL[i] = NULL;
		}

		if (m_CacheCA[i])
		{
			delete m_CacheCA[i];
			m_CacheCA[i] = NULL;
		}

		if (m_Cache3T[i])
		{
			delete m_Cache3T[i];
			m_Cache3T[i] = NULL;
		}

		if (m_CacheTV[i])
		{
			delete m_CacheTV[i];
			m_CacheTV[i] = NULL;
		}
	}*/
	if (g_bLF)Log("offline (ok)");
}

//-----------------------------------------------------------------------------
// Name: CreateStaticBuffer(Manager)
// Desc: Create a static vertex/index buffer for the given data and returns
//		 a handle to that buffer for later rendering processes
//-----------------------------------------------------------------------------
HRESULT YD3DVertexCacheManager::CreateStaticBuffer(YVERTEXID VertexID, UINT nSkinID, UINT nVerts, 
                                 UINT nIndis, const void *pVerts, const WORD *pIndis, UINT *pnID) 
{
	HRESULT  hr;
	void    *pData;
	/*Log("usa shader %i", m_pYD3D->UsesShaders());*/
	if (m_nNumSB >= (MAX_ID-1)) return Y_OUTOFMEMORY;

	// Allocate memory for static buffers if needed
	if ( (m_nNumSB % 50) == 0) 
	{
		int n = (m_nNumSB + 50) * sizeof(YSTATICBUFFER);
		m_pSB = (YSTATICBUFFER*)realloc(m_pSB, n);
		if (!m_pSB) return Y_OUTOFMEMORY;
	}

	m_pSB[m_nNumSB].nNumVerts = nVerts;
	m_pSB[m_nNumSB].nNumIndis = nIndis;
	m_pSB[m_nNumSB].nSkinID   = nSkinID;
	m_pSB[m_nNumSB].pIB       = NULL;
	m_pSB[m_nNumSB].pVB       = NULL;
	m_pSB[m_nNumSB].VID		  = VertexID;

	// Get size and format of vertex
	switch (VertexID) {
		case VID_PS: { 
			m_pSB[m_nNumSB].nStride = sizeof(PVERTEX);
			} break;
		case VID_UU: { 
			m_pSB[m_nNumSB].nStride = sizeof(VERTEX);
			} break;
		case VID_UL: {
			m_pSB[m_nNumSB].nStride = sizeof(LVERTEX);
			} break;
		case VID_CA: {
			m_pSB[m_nNumSB].nStride = sizeof(CVERTEX);
			} break;
		case VID_3T: {
			m_pSB[m_nNumSB].nStride = sizeof(VERTEX3T);
			} break;
		case VID_TV: {
			m_pSB[m_nNumSB].nStride = sizeof(TVERTEX);
			} break;

		default: return Y_INVALIDID;
	} 

	// Create indexbuffer if needed
	if (nIndis > 0) 
	{
		m_pSB[m_nNumSB].bIndis   = true;
		m_pSB[m_nNumSB].nNumTris = int(nIndis / 3.0f);
		hr = m_pDevice->CreateIndexBuffer(nIndis * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
										  D3DPOOL_DEFAULT, &m_pSB[m_nNumSB].pIB, NULL);
		if (FAILED(hr)) 
		{
			if (g_bLF)Log("Error creating static index buffer");
			return Y_CREATEBUFFER;
		}

		// Fill the index buffer
		if (SUCCEEDED(m_pSB[m_nNumSB].pIB->Lock(0, 0, (void**)(&pData), 0))) 
		{
			memcpy(pData, pIndis, nIndis * sizeof(WORD));			
			m_pSB[m_nNumSB].pIB->Unlock();
		}
		else
		{
			if (g_bLF)Log("Error in lock call creating static index buffer");
			return Y_BUFFERLOCK;
		}
	}
	else 
	{
		m_pSB[m_nNumSB].bIndis   = false;
		m_pSB[m_nNumSB].nNumTris = int(nVerts / 3.0f);
		m_pSB[m_nNumSB].pIB      = NULL;
	}
	
	// Create vertex buffer	
	hr = m_pDevice->CreateVertexBuffer(nVerts * m_pSB[m_nNumSB].nStride, D3DUSAGE_WRITEONLY, 0,
									   D3DPOOL_DEFAULT, &m_pSB[m_nNumSB].pVB, NULL);
	if (FAILED(hr)) 
	{
		if (g_bLF)Log("Error creating static vertex buffer");
		return Y_CREATEBUFFER;
	}

	// Fill the vertex buffer
	if (SUCCEEDED(m_pSB[m_nNumSB].pVB->Lock(0, 0, (void**)(&pData), 0))) 
	{
		memcpy(pData, pVerts, nVerts*m_pSB[m_nNumSB].nStride);
		m_pSB[m_nNumSB].pVB->Unlock();
	}
	else
	{
		if (g_bLF)Log("Error in lock call creating static vertex buffer");
		return Y_BUFFERLOCK;
	}

	if (pnID)(*pnID) = m_nNumSB;
	m_nNumSB++;
	return Y_OK;
} 

//-----------------------------------------------------------------------------
// Name: SetVertexDeclaration
// Desc: Sets the vertex declaration
//-----------------------------------------------------------------------------
HRESULT YD3DVertexCacheManager::SetVertexDeclaration(DWORD VertexID)
{
	// check
	if (m_dwActiveVertexDcl == VertexID)
		return Y_OK;
	else
	{
		// clear all vertex caches
//	m_pVertexManager->ForcedFlushAll();
////   m_pVertexMan->InvalidateStates();	
	}

	switch (VertexID) 
	{
		case VID_PS: { 
			if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclPVertex)))
				return Y_FAIL;
			} break;
		case VID_UU: { 
			if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclVertex)))
				return Y_FAIL;
			} break;
		case VID_UL: {
			if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclLVertex)))
				return Y_FAIL;
			} break;
		case VID_CA: {
			if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclCVertex)))
				return Y_FAIL;
			} break;
		case VID_3T: {
			if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDecl3TVertex)))
				return Y_FAIL;
			} break;
		case VID_TV: {
			if (FAILED(m_pDevice->SetVertexDeclaration(m_pYD3D->m_pDeclTVertex)))
				return Y_FAIL;
			} break;
		default: return Y_INVALIDID;
	} 

	// Set the currently used vertex declaration
	m_dwActiveVertexDcl = VertexID;
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: Render(Manager)
// Desc: Render buffers with the given skin. Index buffers is optional.
//-----------------------------------------------------------------------------
HRESULT YD3DVertexCacheManager::Render(UINT nID) 
{
	HRESULT hr = Y_OK;
	int		iT = 0;

	if (!m_pYD3D->IsSceneRunning())
	{
		if (g_bLF) Log("Error: can not render because the scene is not running call begin()");
		return Y_FAIL;
	}

	//YRENDERSTATE sm = m_pYD3D->GetShadeMode();

	// Active cache gets invalid
	m_dwActiveCache = MAX_ID;

	if (nID >= m_nNumSB) 
	{
		if (g_bLF)Log("error: invalid static buffer ID");
		return Y_INVALIDPARAM;
	}
	

	// Activate buffers if not already active
	if (m_dwActiveSB != nID) 
	{		
		// Index buffer used?
		if (m_pSB[nID].bIndis) m_pDevice->SetIndices(m_pSB[nID].pIB);
		// Set the vertex buffer
		m_pDevice->SetStreamSource(0, m_pSB[nID].pVB, 0, m_pSB[nID].nStride);
		// Set the vertex declaration
		if(FAILED(SetVertexDeclaration(m_pSB[nID].VID)))
		{
			if(g_bLF)Log("Error: SetVertexDeclaration");
			return Y_FAIL;
		}
		m_dwActiveSB = nID;
	}
	// There is maybe just another indexbuffer active
	else if (m_dwActiveIB != MAX_ID) 
	{
		if (m_pSB[nID].bIndis) m_pDevice->SetIndices(m_pSB[nID].pIB);
		m_dwActiveIB = MAX_ID;
	}


	// Is the device already using this skin?   
	if (m_pYD3D->GetActiveSkinID() != m_pSB[nID].nSkinID) 
	{
		// Set material for device
		YSKIN *pSkin = &m_pSkinMan->m_pSkins[m_pSB[nID].nSkinID];      
		
		// Set material for the FX
		YMATERIAL *pMat = &m_pSkinMan->m_pMaterials[pSkin->nMaterial];
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetValue(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hDiffuseMat, &pMat->cDiffuse, sizeof(YCOLOR));
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetValue(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hAmbientMat, &pMat->cAmbient, sizeof(YCOLOR));
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetValue(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hEmissiveMat, &pMat->cEmissive, sizeof(YCOLOR));
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetValue(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hSpecMat, &pMat->cSpecular, sizeof(YCOLOR));
		m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetFloat(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hSpecPowMat, pMat->fPower);
		
		// Set texture for device
		for (iT = 0; iT < 8; iT++) 
		{
			if (pSkin->nTexture[iT] != MAX_ID) 
				m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->SetTexture(m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].hTex[iT], (LPDIRECT3DTEXTURE9)m_pSkinMan->m_pTextures[pSkin->nTexture[iT]].pData);
			else 
				break;
		} 
		m_pDevice->SetTextureStageState(iT, D3DTSS_COLOROP, D3DTOP_DISABLE);
				
      
		// Set alpha states for device
		if (pSkin->bAlpha) 
		{		
			m_pDevice->SetRenderState(D3DRS_ALPHAREF, 15);
			m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		}
		else 
		{
			m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
		// Skin will change now
		m_pYD3D->SetActiveSkinID(m_pSB[nID].nSkinID);
	} 

	
	//  Should I use additive rendering?
	if (m_pYD3D->UsesAdditiveBlending()) 
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	//  Should I use rendering to color buffer at all?
	else if (!m_pYD3D->UsesColorBuffer()) 
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	//// Should I force alpha blending?
	//else if(m_pYD3D->UsesAlphaBlending())
	//{
	//	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//}


	// Indexed primitive
	if (m_pSB[nID].bIndis)
	{

		for (int i = 0; i < m_pYD3D->m_nPasses; i++)
		{
			//Begin the pass
			m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->BeginPass(i);
			 	
			// Render
			hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,	0, m_pSB[nID].nNumVerts, 0, m_pSB[nID].nNumTris);

			// End the pass
			m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->EndPass();
		}
		
	}
	// Non-indexed primitive
	else 
	{

		for (int i = 0; i < m_pYD3D->m_nPasses; i++)
		{
			m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->BeginPass(i);

			// Render content			
			hr = m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_pSB[nID].nNumTris);

			// End the pass
			m_pYD3D->m_pYFX[m_pYD3D->m_nActiveFX].FX->EndPass();
		}
		
	}

	return hr;
	
} 
////-----------------------------------------------------------------------------
//// Name: GetShadeMode(Manager)
//// Desc: Return shade mode
////-----------------------------------------------------------------------------
//YRENDERSTATE YD3DVertexCacheManager::GetShadeMode()
//{
//	return m_pYD3D->GetShadeMode();
//}

//-----------------------------------------------------------------------------
// Name: Log(Manager)
// Desc: Write outputstring to attribut outputstream if exists
//-----------------------------------------------------------------------------
void YD3DVertexCacheManager::Log(char *chString, ...) 
{

	char ch[256];
	char *pArgs;
   
	pArgs = (char*) &chString + sizeof(chString);
	vsprintf(ch, chString, pArgs);
	fprintf(m_pLog, "[YD3DVCacheManager]: ");
	fprintf(m_pLog, ch);
	fprintf(m_pLog, "\n");
   
	if (g_bLF)
		fflush(m_pLog);
}
