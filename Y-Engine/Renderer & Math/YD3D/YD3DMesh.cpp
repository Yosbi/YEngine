//-------------------------------------------------------------------------------
// YD3DWorldViewProj.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: This file contains some functions used
//		for the set up of the perspective, view  and world matrices
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Include
//--------------------------------------------------------------------------
#include "YD3D.h"
extern bool g_bLF; // to know if the user wants the log of the engine

//-----------------------------------------------------------------------------
// Name: MeshCreateTeapod
// Desc: Creates a teapod
//-----------------------------------------------------------------------------
HRESULT YD3D::MeshCreateTeapot(UINT *nMID)
{
	HRESULT hr = Y_OK;

	// Validate the num of meshes
	if (m_nNumMeshes + 1 == MAX_MESH)
	{
		if (g_bLF) Log("Error: max number of meshes reached");
		return Y_OUTOFMEMORY;
	}

	if (FAILED(D3DXCreateTeapot(m_pDevice, &m_pMesh[m_nNumMeshes], 0)))
	{
		if (g_bLF) Log("Error: teapot can not be created");
		return Y_FAIL;
	}

	// Set the texture coord to the sphere
	if (FAILED(hr = GenSphericalTexCoords(m_nNumMeshes)))
	{
		return hr;
	}

	if(nMID) *nMID = m_nNumMeshes;
	m_nNumMeshes++;
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: MeshCreateSphere
// Desc: Creates a sphere
//-----------------------------------------------------------------------------
HRESULT YD3D::MeshCreateSphere(float fRadius, UINT nSlices, UINT nStacks, UINT *nMID)
{
	HRESULT hr = Y_OK;

	// Validate the num of meshes
	if (m_nNumMeshes + 1 == MAX_MESH)
	{
		if (g_bLF) Log("Error: max number of meshes reached");
		return Y_OUTOFMEMORY;
	}
	
	if (FAILED(D3DXCreateSphere(m_pDevice, fRadius, nSlices, nStacks, &m_pMesh[m_nNumMeshes], 0)))
	{
		if (g_bLF) Log("Error: sphere can not be created");
		return Y_FAIL;
	}

	// Set the texture coord to the sphere
	if (FAILED(hr = GenSphericalTexCoords(m_nNumMeshes)))
	{
		return hr;
	}

	if(nMID) *nMID = m_nNumMeshes;
	m_nNumMeshes++;
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: MeshCreateCylinder
// Desc: Creates a cylinder
//-----------------------------------------------------------------------------
HRESULT YD3D::MeshCreateCylinder(float fRadius1, float fRadius2, float fLength, UINT nSlices, UINT nStacks, const char cSkinOrientation, UINT *nMID)
{
	HRESULT hr = Y_OK;

	// Validate the num of meshes
	if (m_nNumMeshes + 1 == MAX_MESH)
	{
		if (g_bLF) Log("Error: max number of meshes reached");
		return Y_OUTOFMEMORY;
	}
	
	if (FAILED(D3DXCreateCylinder(m_pDevice, fRadius1, fRadius2, fLength, nSlices, nStacks, &m_pMesh[m_nNumMeshes], 0)))
	{
		if (g_bLF) Log("Error: cylinder can not be created");
		return Y_FAIL;
	}

	// Set the texture coord to the cylinder
	if (FAILED(hr = genCylTexCoords(m_nNumMeshes, cSkinOrientation)))
	{
		return hr;
	}

	if(nMID) *nMID = m_nNumMeshes;
	m_nNumMeshes++;
	return Y_OK;
}
//-----------------------------------------------------------------------------
// Name: MeshRender
// Desc: Render a mesh
//-----------------------------------------------------------------------------
HRESULT YD3D::MeshRender(UINT nMID, UINT nSkinID)
{
	if (nMID > m_nNumMeshes)
		return Y_INVALIDID;


	int iT = 0;
	static YD3DSkinManager* pYSM = (YD3DSkinManager*)m_pSkinManager;

	if (nSkinID > pYSM->m_nNumSkins) 
		return Y_INVALIDID;

	// Set the static buffer to MAX_ID
	static YD3DVertexCacheManager* pYVCM = (YD3DVertexCacheManager*)GetVertexManager();
	if (pYVCM->m_dwActiveSB != MAX_ID)
		pYVCM->m_dwActiveSB = MAX_ID;

	// Set the skin
	// Is the device already using this skin?   
	if (GetActiveSkinID() != nSkinID) 
	{
		// Set material for device
		YSKIN *pSkin = &pYSM->m_pSkins[nSkinID];    
		
		// Set material for the FX
		YMATERIAL *pMat = &pYSM->m_pMaterials[pSkin->nMaterial];
		m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hDiffuseMat, &pMat->cDiffuse, sizeof(YCOLOR));
		m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hAmbientMat, &pMat->cAmbient, sizeof(YCOLOR));
		m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hEmissiveMat, &pMat->cEmissive, sizeof(YCOLOR));
		m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hSpecMat, &pMat->cSpecular, sizeof(YCOLOR));
		m_pYFX[m_nActiveFX].FX->SetFloat(m_pYFX[m_nActiveFX].hSpecPowMat, pMat->fPower);

		// Rrendering with textures?
		for (iT = 0; iT < 8; iT++) 
		{
			if (pSkin->nTexture[iT] != MAX_ID) 
				m_pYFX[m_nActiveFX].FX->SetTexture(m_pYFX[m_nActiveFX].hTex[iT], (LPDIRECT3DTEXTURE9)pYSM->m_pTextures[pSkin->nTexture[iT]].pData);
			else 
				break;
		} 
      
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
		SetActiveSkinID(nSkinID);
	} 

	//  Should I use additive rendering?
	if (UsesAdditiveBlending()) 
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	//  Should I use rendering to color buffer at all?
	else if (!UsesColorBuffer()) 
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	// Should I force alpha blending?
	/*else if(UsesAlphaBlending())
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}*/
	
	HRESULT hr = NULL;
	m_pDevice->SetRenderState(D3DRS_WRAP0, D3DWRAP_U | D3DWRAP_V);
	for (int i = 0; i < m_nPasses; i++)
	{
		//Begin the pass
		m_pYFX[m_nActiveFX].FX->BeginPass(i);

		// Render the mesh
		if(FAILED(hr = m_pMesh[nMID]->DrawSubset(0)))
		{
			if (g_bLF) Log("Error: can not render mesh %i", nMID);			
		}

		// End the pass
		m_pYFX[m_nActiveFX].FX->EndPass();
	}
	m_pDevice->SetRenderState(D3DRS_WRAP0, 0);
	
	if(FAILED(hr))
		return Y_FAIL;

	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: GenSphericalTexCoords
// Desc: Generates tex coods for a spherical like object
//-----------------------------------------------------------------------------
HRESULT YD3D::GenSphericalTexCoords(UINT nMesh)
{
	D3DVERTEXELEMENT9	elements[64];
    UINT				numElements = 0;
	HRESULT				hr = NULL;

	m_pDeclVertex->GetDeclaration(elements, &numElements);

    ID3DXMesh* temp = 0;
    if (FAILED(m_pMesh[nMesh]->CloneMesh(D3DXMESH_SYSTEMMEM, elements, m_pDevice, &temp)))
	{
		if (g_bLF) Log("Error: can not clone mesh to set mesh texture coords");
		return Y_FAIL;
	}

	m_pMesh[nMesh]->Release();    

    // Now generate texture coordinates for each vertex.
    VERTEX * vertices = 0;
    if (FAILED(temp->LockVertexBuffer(0, (void**)&vertices)))
	{
		if (g_bLF) Log("Error: can not lock buffer to set mesh texture coords 1");
		return Y_BUFFERLOCK;
	}

    for(UINT i = 0; i < temp->GetNumVertices(); i++)
    {
        // Convert to spherical coordinates.
		D3DXVECTOR3 p(vertices[i].x,vertices[i].y, vertices[i].z);

        float theta = atan2f(p.z, p.x);
        float phi   = acosf(p.y / sqrtf(p.x*p.x+p.y*p.y+p.z*p.z));

        // Phi and theta give the texture coordinates, but are
        // not in the range [0, 1], so scale them into that range.

        float u = theta / (2.0f*D3DX_PI);
        float v = phi   / D3DX_PI;

        // Save texture coordinates.

		vertices[i].tu = u;
		vertices[i].tv = v;
    }
    temp->UnlockVertexBuffer();

    // Clone back to a hardware mesh.
    if (FAILED(temp->CloneMesh(D3DXMESH_MANAGED | D3DXMESH_WRITEONLY, elements, m_pDevice, &m_pMesh[nMesh])))
	{
		if (g_bLF) Log("Error: can not lock buffer to set mesh texture coords 2");
		temp->Release();
		return Y_BUFFERLOCK;
	}

	temp->Release();
	
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: genCylTexCoords
// Desc: Generates tex coods for a cylindrical like object
//-----------------------------------------------------------------------------
HRESULT YD3D::genCylTexCoords(UINT nMesh, const char cTextOrientation)
{
	D3DVERTEXELEMENT9 elements[64];
	UINT numElements = 0;
	m_pDeclVertex->GetDeclaration(elements, &numElements);

	ID3DXMesh* temp = 0;
	if (FAILED(m_pMesh[nMesh]->CloneMesh(D3DXMESH_SYSTEMMEM, elements, m_pDevice, &temp)))
	{
		if (g_bLF) Log("Error: can not clone mesh to set mesh texture coords");
		return Y_FAIL;
	}

	m_pMesh[nMesh]->Release();

	// Now generate texture coordinates for each vertex.
	VERTEX* vertices = 0;
	 if (FAILED(temp->LockVertexBuffer(0, (void**)&vertices)))
	{
		if (g_bLF) Log("Error: can not lock buffer to set mesh texture coords 1");
		return Y_BUFFERLOCK;
	}

	// We need to get the height of the cylinder onto which we are
	// projecting the vertices. That height depends on the axis on
	// which the client has specified that the cylinder lies. The
	// height is determined by finding the height of the bounding
	// cylinder on the specified axis.

	D3DXVECTOR3 maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	D3DXVECTOR3 minPoint(FLT_MAX, FLT_MAX, FLT_MAX);

	for(UINT i = 0; i < temp->GetNumVertices(); ++i)
	{

		D3DXVec3Maximize(&maxPoint, &maxPoint, &D3DXVECTOR3(vertices[i].x,vertices[i].y,vertices[i].z));
		D3DXVec3Minimize(&minPoint, &minPoint, &D3DXVECTOR3(vertices[i].x,vertices[i].y,vertices[i].z));
	}

	float a = 0.0f;
	float b = 0.0f;
	float h = 0.0f;
	if ((cTextOrientation == 'x')||(cTextOrientation == 'X'))
	{
		a = minPoint.x;
		b = maxPoint.x;
		h = b-a;
	}		
	if ((cTextOrientation == 'y')||(cTextOrientation == 'Y'))
	{
		a = minPoint.y;
		b = maxPoint.y;
		h = b-a;
	}
	if ((cTextOrientation == 'z')||(cTextOrientation == 'Z'))
	{
		a = minPoint.z;
		b = maxPoint.z;
		h = b-a;
		
	}

	// Iterate over each vertex and compute its texture coordinate.
	for(UINT i = 0; i < temp->GetNumVertices(); ++i)
	{
		// Get the coordinates along the axes orthogonal to the
		// axis with which the cylinder is aligned.

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		if ((cTextOrientation == 'x')||(cTextOrientation == 'X'))
		{
			x = vertices[i].y;
			z = vertices[i].z;
			y = vertices[i].x;
		}
		if ((cTextOrientation == 'y')||(cTextOrientation == 'Y'))
		{
			x = vertices[i].x;
			z = vertices[i].z;
			y = vertices[i].y;
		}
		if ((cTextOrientation == 'z')||(cTextOrientation == 'Z'))
		{
			x = vertices[i].x;
			z = vertices[i].y;
			y = vertices[i].z;
		}
		

		// Convert to cylindrical coordinates.

		float theta = atan2f(z, x);
		float y2    = y - b; // Transform [a, b]-->[-h, 0]

		// Transform theta from [0, 2*pi] to [0, 1] range and
		// transform y2 from [-h, 0] to [0, 1].

		float u = theta / (2.0f*D3DX_PI);
		float v = y2 / -h;

		// Save texture coordinates.
		vertices[i].tu = u;
		vertices[i].tv = v;
	}

	temp->UnlockVertexBuffer();

	 // Clone back to a hardware mesh.
    if (FAILED(temp->CloneMesh(D3DXMESH_MANAGED | D3DXMESH_WRITEONLY, elements, m_pDevice, &m_pMesh[nMesh])))
	{
		if (g_bLF) Log("Error: can not lock buffer to set mesh texture coords 2");
		temp->Release();
		return Y_BUFFERLOCK;
	}

	temp->Release();

	return Y_OK;

}