//-------------------------------------------------------------------------------
// YD3DSkinManager.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: Implementation of the YD3DSkinManager

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "YD3DSkinManager.h" // Class definition
extern bool g_bLF; // to know if the user wants the log of the engine

//-----------------------------------------------------------------------------
// Name: Constructor
// Desc: Initializes all the variables	
//-----------------------------------------------------------------------------
YD3DSkinManager::YD3DSkinManager( LPDIRECT3DDEVICE9 pDevice, FILE *pLog)
{
	m_nNumMaterials = 0;
	m_nNumTextures  = 0;
	m_nNumSkins		= 0;
	m_pMaterials	= NULL;
	m_pTextures		= NULL;
	m_pSkins		= NULL;
	m_pLog			= pLog;
	m_pDevice		= pDevice;
	
	if (g_bLF) Log("online");
}

//-----------------------------------------------------------------------------
// Name: Destructor
// Desc: Realease direct3d texture objects
//-----------------------------------------------------------------------------
YD3DSkinManager::~YD3DSkinManager()
{
	if ( m_pTextures)
	{
		for(UINT i = 0; i < m_nNumTextures; i++)
		{
			if(m_pTextures[i].pData) // If there are data in the texture
			{
				((LPDIRECT3DTEXTURE9)(m_pTextures[i].pData))->Release();
				m_pTextures[i].pData = NULL;
			}
			if(m_pTextures[i].chName) // If there is a name for the texture
			{
				delete m_pTextures[i].chName;
				m_pTextures[i].chName = NULL;
			}
			if(m_pTextures[i].pClrKeys) // If there are colors
			{
				delete [] m_pTextures[i].pClrKeys;
				m_pTextures[i].pClrKeys = NULL;
			}
		}
		free(m_pTextures);
		m_pTextures = NULL;
	}

	if (m_pMaterials) // If there are materials
	{
		free(m_pMaterials);
		m_pMaterials = NULL;
	}
	if (m_pSkins) // If there are skins
	{
		free(m_pSkins);
		m_pSkins = NULL;
	}
	if (g_bLF)Log("Offline (ok)");
}

//-----------------------------------------------------------------------------
// Name: AddSkin
// Desc: Creates a new skin object using the given material. Texture is set
//		to NULL till a texture is added to that skin.
//		The var npSkinID is an out var, a id to reference the new skin
//-----------------------------------------------------------------------------
HRESULT YD3DSkinManager::AddSkin( const YCOLOR *pcAmbient, const YCOLOR *pcDiffuse,
								  const YCOLOR *pcEmissive, const YCOLOR *pcSpecular,
								  float fSpecPower, UINT *pnSkinID)
{
	UINT	  nMat, n;
	bool	  bMat = false; // To know if the material is alreade in memory
	YMATERIAL mat;			// The new material

	// Allocate 50 new memory slot for the skin if needed
	if ( ( m_nNumSkins % 50) == 0 )
	{
		n = (m_nNumSkins + 50) * sizeof( YSKIN ); // Number registres to be allocated in memory
		m_pSkins = (YSKIN*) realloc(m_pSkins, n); // Reallocate the old data in the new memory block
		if (!m_pSkins) return Y_OUTOFMEMORY;	  // If cant be allocated then returns with error
	}

	mat.cAmbient	= *pcAmbient;
	mat.cDiffuse	= *pcDiffuse;
	mat.cEmissive	= *pcEmissive;
	mat.cSpecular	= *pcSpecular;
	mat.fPower		=  fSpecPower;

	// Check if already have a material in memory
	for (nMat = 0; nMat < m_nNumMaterials; nMat++)
	{
		if ( MaterialEqual(&mat, &m_pMaterials[nMat]) )
		{
			bMat = true;
			break;
		}
	}

	// If a material exist, store its id into the skin, otherwise create it
	if (bMat)
		m_pSkins[m_nNumSkins].nMaterial = nMat;
	else
	{		
		// Allocate 50 new memory slots for materials if needed
		if ( (m_nNumMaterials % 50) == 0 )
		{
			n = (m_nNumMaterials + 50) * sizeof(YMATERIAL);		  // Number registres to be allocated in memory
			m_pMaterials = (YMATERIAL*) realloc(m_pMaterials, n); // Reallocate the old data in the new memory block
			if(!m_pMaterials) return Y_OUTOFMEMORY;				  // If cant be allocated then returns with error
		}

		// Assing to the skin the last index of the array 
		// of material (where the material gona be saved)
		m_pSkins[m_nNumSkins].nMaterial = m_nNumMaterials;

		// Them proceed to salve the new material
		memcpy(&m_pMaterials[m_nNumMaterials], &mat, sizeof(YMATERIAL));
		m_nNumMaterials++;
	}

	// Set alpha value if needed
	if (mat.cDiffuse.fA < 1.0f)
		m_pSkins[m_nNumSkins].bAlpha = true;	
	else
		m_pSkins[m_nNumSkins].bAlpha = false;

	// Set the textures in MAX_ID to know that there is not texture in there
	for ( n = 0; n < 8; n++ )
	{
		m_pSkins[m_nNumSkins].nTexture[n] = MAX_ID; 
	}

	// Save the id of the skin in to the out var and incremetn the count of skins
	if (pnSkinID)(*pnSkinID) = m_nNumSkins; 
	m_nNumSkins++;

	return Y_OK;
}


//-----------------------------------------------------------------------------
// Name: AddTexture
// Desc: Add a texture to a given skin. BMP is only loaded if not used yet
//		can alse be used to set alpha channels on textures for alpha keys and/or
//		overall transparency. Set bool to true in both cases
//-----------------------------------------------------------------------------
HRESULT YD3DSkinManager::AddTexture(UINT nSkinID, const char *chName, bool bAlpha,
									float fAlpha, YCOLOR *cColorKeys, DWORD dwNumColorKeys)
{
	YTEXTURE *pYTex = NULL;
	HRESULT	  hr;
	UINT	  nTex, n;
	bool	  bTex = false; // To know if the texture has been introduced before

	// If this is a valid skin 
	if (nSkinID >= m_nNumSkins) return Y_INVALIDID;

	// Is all the 8 textures for this skin already set?
	if ( m_pSkins[nSkinID].nTexture[7] != MAX_ID)
	{
		if (g_bLF)Log("Error: AddTextures() failed, all 8 stages set");
		return Y_BUFFERSIZE;
	}		

	// Is the texture already loaded in memory?
	for (nTex = 0; nTex < m_nNumTextures; nTex++)
	{
		if (strcmp(chName, m_pTextures[nTex].chName) == 0)
		{
			bTex = true;
			break;
		}
	}

	// Setting the alpha blending to the skin if the texture has alpha blending
	if (bAlpha)
		m_pSkins[nSkinID].bAlpha = bAlpha;
	

	// If the texture is not loaded
	if (!bTex)
	{
		// Allocate 50 new memory slots for the texture if needed
		if( (m_nNumTextures % 50) == 0 )
		{
			n = (m_nNumTextures + 50) * sizeof(YTEXTURE);		  // Number registres to be allocated in memory
			m_pTextures = (YTEXTURE*) realloc(m_pTextures, n);    // Reallocate the old data in the new memory block
			if(!m_pTextures)
			{
				if (g_bLF)Log("Error: AddTexture() failed, realloc()");
				return Y_OUTOFMEMORY; // If cant be allocated then returns with error
			}
		}
		
		// Init vars of the texture
		m_pTextures[m_nNumTextures].pClrKeys = NULL;
		m_pTextures[m_nNumTextures].dwNum	 = 0;

		// If it not use alphablending
		if (!bAlpha) m_pTextures[m_nNumTextures].fAlpha = 1.0;


		// Save the texture name
		m_pTextures[m_nNumTextures].chName = new char[strlen(chName) + 1];
		memcpy(m_pTextures[m_nNumTextures].chName, chName, strlen(chName) + 1);

		// Create d3d texture from that pointer
		hr = CreateTexture(&m_pTextures[m_nNumTextures]);
		if (FAILED(hr))
		{
			if (g_bLF)Log("Error: CreateTexture() failed");
			return hr;
		}

		// Add alpha values if needed
		if (bAlpha)
		{
			// Set dummy pointer
			pYTex = &m_pTextures[m_nNumTextures];
			
			// Remind information
			pYTex->fAlpha = fAlpha;
			pYTex->dwNum = dwNumColorKeys;
			pYTex->pClrKeys = new YCOLOR[dwNumColorKeys];
			memcpy(pYTex->pClrKeys, cColorKeys, sizeof(YCOLOR) * dwNumColorKeys);

			// Set D3D dummy pointer
			LPDIRECT3DTEXTURE9 pTex = (LPDIRECT3DTEXTURE9)m_pTextures[m_nNumTextures].pData;
			
			// Set alpha keys first to specific colors
			for(DWORD dw = 0; dw < dwNumColorKeys; dw++)
			{
				hr = SetAlphaKey( &pTex, BYTE(cColorKeys[dw].fR * 255), BYTE(cColorKeys[dw].fG * 255), 
								  BYTE(cColorKeys[dw].fB * 255), BYTE(cColorKeys[dw].fA * 255) );
				if (FAILED(hr)) 
				{					
					if (hr == Y_INVALIDPARAM)
					{
						if (g_bLF) 
							Log("Warning: SetAlphaKey() failed because texture format is not A8R8G8B8");
					}
					else
					{
						if (g_bLF)Log("Error: SetAlphaKey() failed");
						return hr;
					}
				}
			}

			// Now general transparency
			if (fAlpha < 1.0f)
			{
				hr = SetTransparency(&pTex, BYTE(fAlpha * 255));
				if (FAILED(hr)) 
				{
					if (hr == Y_INVALIDPARAM)
					{
						if (g_bLF) 
							Log("Warning: SetTransparency() failed because texture format is not A8R8G8B8 ");
					}
					else
					{
						if (g_bLF)Log("Error: SetTransparency() failed");
						return hr;
					}
				}
			}
		}// If bAlpha

		// Save id and add to count
		nTex = m_nNumTextures;
		m_nNumTextures++;
	}// if not texture

	// Puting texture id into the skin reference to a texture array
	for (int i = 0; i < 8; i++)
	{
		if (m_pSkins[nSkinID].nTexture[i] == MAX_ID)
		{
			m_pSkins[nSkinID].nTexture[i] = nTex;
			break;
		}
	}
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: ColorEqual
// Desc: Compares two YCOLORS and returns 1 if equal
//-----------------------------------------------------------------------------
inline bool YD3DSkinManager::ColorEqual(const YCOLOR *pCol0, const YCOLOR *pCol1)
{
	if ( (pCol0->fA == pCol1->fA) && (pCol0->fB == pCol1->fB) && (pCol0->fG == pCol1->fG) && (pCol0->fR == pCol1->fR) )
		return true;
	return false;
}

//-----------------------------------------------------------------------------
// Name: MaterialEqual
// Desc: Compares two YMATERIALs and returns 1 if equal
//-----------------------------------------------------------------------------
bool YD3DSkinManager::MaterialEqual( const YMATERIAL *pMat0, const YMATERIAL *pMat1)
{
	if( ColorEqual(&pMat0->cAmbient, &pMat1->cAmbient) && ColorEqual(&pMat0->cDiffuse, &pMat1->cDiffuse) &&
		ColorEqual(&pMat0->cEmissive, &pMat1->cEmissive) && ColorEqual(&pMat0->cSpecular, &pMat1->cSpecular) &&
		(&pMat0->fPower == &pMat1->fPower) )
		return true;
	return false;
}

//-----------------------------------------------------------------------------
// Name: CreateTexture
// Desc: Creates a D3DTexture object and loads the image data from disc
//-----------------------------------------------------------------------------
HRESULT YD3DSkinManager::CreateTexture(YTEXTURE *pTexture)
{
	HRESULT hr = NULL;
	pTexture->pData = NULL;

	// Create the texture
	if (FAILED(hr = D3DXCreateTextureFromFile(m_pDevice, pTexture->chName, (LPDIRECT3DTEXTURE9*)&pTexture->pData  )))
	{
		if (hr == D3DERR_NOTFOUND)
		{
			if (g_bLF)Log("Error: Cannot open texture \"%s\"", pTexture->chName);
			return Y_FILENOTFOUND;
		}
		if (hr == D3DERR_INVALIDCALL)
			if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (D3DERR_INVALIDCALL)");
		else if (hr == D3DERR_OUTOFVIDEOMEMORY)
			if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (D3DERR_OUTOFVIDEOMEMORY)");
		else if (hr == E_OUTOFMEMORY)
			if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (E_OUTOFMEMORY)");
		else if (hr == D3DOK_NOAUTOGEN)
			if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (D3DOK_NOAUTOGEN)");
		else if (hr == D3DXERR_INVALIDDATA)
			if (g_bLF) Log("Error: IDirect3DDevice::CreateTexture failed (D3DXERR_INVALIDDATA)");
		else if (hr == D3DERR_NOTAVAILABLE)
			if (g_bLF) Log("Error: IDirect3DDevice::CreateTexture failed (D3DERR_NOTAVAILABLE)");
		else if (hr == D3DERR_INVALIDCALL)
			if (g_bLF) Log("Error: IDirect3DDevice::CreateTexture failed (D3DERR_INVALIDCALL)");
		else
			if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (unknown)");

		return Y_FAIL;
	}
	//// OLD
	//D3DLOCKED_RECT	d3dRect;
	//D3DFORMAT		d3dFmt;
	//DIBSECTION		dibS; // To know al the data of the bitmap
	//int				lineWidth;
	//void		   *pMemory = NULL;
	//HRESULT			hr;
	//
	//// Load the bitmap
	//HBITMAP	hBMP = (HBITMAP)LoadImage(NULL, pTexture->chName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	//if (!hBMP) 
	//{
	//	if (g_bLF)Log("Error: Cannot open texture \"%s\"", pTexture->chName);
	//	return Y_FILENOTFOUND;
	//}
	//// Get dib section
	//GetObject(hBMP, sizeof(DIBSECTION), &dibS);

	//// Supports only 24 bits bitmaps
	//if (dibS.dsBmih.biBitCount != 24)
	//{
	//	DeleteObject(hBMP);
	//	if (g_bLF)Log("Error: texture is not 24 bit \"s\"", pTexture->chName);
	//	return Y_INVALIDFILE;
	//}

	//d3dFmt = D3DFMT_A8R8G8B8;

	//long  lWidth   = dibS.dsBmih.biWidth;
	//long  lHeight  = dibS.dsBmih.biHeight;
	//BYTE *pBMPBits = (BYTE*)dibS.dsBm.bmBits;
	//
	//// Build d3d texture object
	//hr = m_pDevice->CreateTexture(lWidth, lHeight, 1, 0, d3dFmt, D3DPOOL_MANAGED,
	//							 (LPDIRECT3DTEXTURE9*)(&(pTexture->pData)), NULL);
	//if (FAILED(hr)) 
	//{
	//	if (hr == D3DERR_INVALIDCALL)
	//		if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (D3DERR_INVALIDCALL)");
	//	else if (hr == D3DERR_OUTOFVIDEOMEMORY)
	//		if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (D3DERR_OUTOFVIDEOMEMORY)");
	//	else if (hr == E_OUTOFMEMORY)
	//		if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (E_OUTOFMEMORY)");
	//	else if (hr == D3DOK_NOAUTOGEN)
	//		if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (D3DOK_NOAUTOGEN)");
	//	else
	//		if (g_bLF)Log("Error: IDirect3DDevice::CreateTexture failed (unknown)");

	//	return Y_FAIL;
	//}

	//// Get a dummy pointer to handle the date in a easyer way
	//LPDIRECT3DTEXTURE9 pTex = ((LPDIRECT3DTEXTURE9)pTexture->pData);

	//if ( FAILED(pTex->LockRect(0, &d3dRect, NULL, 0)) )
	//{
	//	if (g_bLF)Log("Error: cannot lock texture to copy pixels \"%s\"", pTexture->chName);
	//	return Y_BUFFERLOCK;
	//}
	//
	//lineWidth = d3dRect.Pitch >> 2; // 32 bit
	//pMemory = (DWORD*)d3dRect.pBits;	
	//
	//// Copy each pixel 
	//for (int cy = 0; cy < lHeight; cy++)
	//{
	//	for (int cx = 0; cx < lWidth; cx++)
	//	{			
	//		DWORD Color = 0xff000000; // Set the alpha value to 1
	//		int	  i     = (cy * lWidth + cx) * 3;
	//		memcpy(&Color, &pBMPBits[i], sizeof(BYTE) * 3);   // Save the color of the bitmap into the custom color
	//		((DWORD*)pMemory)[cx + (cy * lineWidth)] = Color; // Save the color into the d3dRect memory						
	//	}	
	//}

	//pTex->UnlockRect(0); // Unlock the resourse
	//DeleteObject(hBMP);  // I dont need anymore the bitmap

	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: SetAlphaKey
// Desc: Sets the alpha channel of all pixels with 
//		given RGB value to the amount of transparency especified, Make 
//		sure the corresponding RGB values in the texture have alpha value 
//		set to 1.0f, so set all alpha key prior to calling SetTransparency. 
//		The range ofvalues of RGBA vars is 0 to 255
//-----------------------------------------------------------------------------
HRESULT YD3DSkinManager::SetAlphaKey(LPDIRECT3DTEXTURE9 *ppTexture, BYTE R,
									 BYTE G, BYTE B, BYTE A)
{
	D3DSURFACE_DESC d3dDesc; // Descryption of the surface
	D3DLOCKED_RECT	d3dRect;
	DWORD			dwKey, Color;

	// Security check: Must be ARGB format
	(*ppTexture)->GetLevelDesc(0, &d3dDesc);
	if(d3dDesc.Format != D3DFMT_A8R8G8B8)
		return Y_INVALIDPARAM;

	// Calculate the key value
	dwKey = MakeD3DColor( R, G, B, 255);

	// Calculate new color tto set for key pixels
	if(A > 0) Color = MakeD3DColor( R, G, B, A);
	else	  Color = MakeD3DColor( 0, 0, 0, A);

	if(FAILED((*ppTexture)->LockRect( 0, &d3dRect, NULL, 0)) )
		return Y_BUFFERLOCK;

	// Overwrite all key pixels with new color valur
	for (DWORD y = 0; y < d3dDesc.Height; y++)
	{
		for (DWORD x = 0; x < d3dDesc.Width; x++)
		{
			if( ((DWORD*)d3dRect.pBits)[d3dDesc.Width * y + x] == dwKey )
				((DWORD*)d3dRect.pBits)[d3dDesc.Width * y + x] = Color;
		}
	}

	(*ppTexture)->UnlockRect(0);

	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name:MakeD3DColor
// Desc: Calculates a 32 bit ARGB value for use with d3d for a given RGBA
//		 color values rangin from 0 to 255
//-----------------------------------------------------------------------------
DWORD YD3DSkinManager::MakeD3DColor(BYTE R, BYTE G, BYTE B, BYTE A)
{
	return (A << 24) | (R << 16) | (G << 8) | B;
}

//-----------------------------------------------------------------------------
// Name: SetTransparency
// Desc: Sets all pixels of the texture to the amount of transparency
//		 (overall transparency)
//-----------------------------------------------------------------------------
HRESULT YD3DSkinManager::SetTransparency(LPDIRECT3DTEXTURE9 *ppTexture, UCHAR Alpha)
{
	D3DSURFACE_DESC	d3dDesc;
	D3DLOCKED_RECT	d3dRect;
	DWORD			Color;
	WORD			A, R, G, B;

	(*ppTexture)->GetLevelDesc(0, &d3dDesc); // Obtain the description of the texture loaded

	// Must be an ARGB format
	if (d3dDesc.Format != D3DFMT_A8R8G8B8)
		return Y_INVALIDPARAM;

	if(FAILED((*ppTexture)->LockRect(0, &d3dRect, NULL, 0)))
		return Y_BUFFERLOCK;

	// Loop through all pixels
	for (DWORD y = 0; y < d3dDesc.Height; y++)
	{
		for (DWORD x = 0; x < d3dDesc.Width; x++)
		{
			// Get color from this pixel
			Color = ((DWORD*)d3dRect.pBits)[d3dDesc.Width * y + x];

			// Calculate ARGB values from pixel color
			A = (UCHAR)( (Color & 0xff000000) >> 24);
			R = (UCHAR)( (Color & 0x00ff0000) >> 16);
			G = (UCHAR)( (Color & 0x0000ff00) >> 8 );
			B = (UCHAR)( (Color & 0x000000ff) >> 0 );

			// Only set new alpha value if old value is greater
			if (A >= Alpha)
				A = Alpha;

			// Putting new color value for this pixel
			((DWORD*)d3dRect.pBits)[d3dDesc.Width * y + x] = MakeD3DColor( R, G, B, A);
		}
	}
	(*ppTexture)->UnlockRect(0);
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: Log
// Desc: Write output string to if this attribut do exist
//-----------------------------------------------------------------------------
void YD3DSkinManager::Log(char *chString, ...)
{
	if (g_bLF)
	{
		char ch[256];
		char *pArgs;

		pArgs = (char*) &chString + sizeof(chString);
		vsprintf(ch, chString, pArgs);
		fprintf(m_pLog, "[YD3DSkinManager]: ");
		fprintf(m_pLog, ch);
		fprintf(m_pLog, "\n");
		
		fflush(m_pLog);
	}
}
