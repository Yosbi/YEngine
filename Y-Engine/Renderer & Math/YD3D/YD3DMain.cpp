//-------------------------------------------------------------------------------
// YD3DMain.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: Direct3d enum and initialization

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include "YD3D.h"
#include "Y.h"
extern bool g_bLF; // To know if i have to mantain a logs files

//-----------------------------------------------------------------------------
// Name: UseWindow
// Desc: If in windowed mode thid function lets switch between all available
//		child windows given as parameter array to the original init() call
//-----------------------------------------------------------------------------
HRESULT	YD3D::UseWindow( UINT nHwnd)
{
	LPDIRECT3DSURFACE9 pBack = NULL;

	if (!m_d3dpp.Windowed)
		return Y_OK;
	else if (nHwnd >= m_nNumhWnd)
		return Y_FAIL;

	// Try to get the right backbuffer
	if(FAILED( m_pChain[nHwnd]->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBack)))
	{
		if (g_bLF)Log("Error: GetBackBuffer() failed in UseWindow()");
		return Y_FAIL;
	}

	// Set him for the device
	m_pDevice->SetRenderTarget( 0, pBack);
	pBack->Release();
	m_nActivehWnd = nHwnd;
	return Y_OK;
}
//-----------------------------------------------------------------------------
// Name: BeginRendering
// Desc: Clear the scene and prepare the device for retrieving geometry
//-----------------------------------------------------------------------------
HRESULT	YD3D::BeginRendering(bool bClearPixel, bool bClearDepth, bool bClearStencil)
{
	DWORD dw = 0;

	// Is there anything to clear?
	if(bClearPixel || bClearDepth || bClearStencil)
	{
		if (bClearPixel)				 dw |= D3DCLEAR_TARGET;
		if (bClearDepth)				 dw |= D3DCLEAR_ZBUFFER;
		if (bClearStencil && m_bStencil) dw |= D3DCLEAR_STENCIL;

		if (FAILED(m_pDevice->Clear(0, NULL, dw, m_ClearColor, 1.0f, 0) ))
		{
			if (g_bLF)Log("Error: Clear in BeginRendering()\n ");
			return Y_FAIL;
		}
	}

	if (FAILED (m_pDevice->BeginScene()))
	{
		if (g_bLF)Log("Error: BeginScene in BeginRendering()\n ");
		return Y_FAIL;
	}

	// Set the effect technique
	m_pYFX[m_nActiveFX].FX->SetTechnique(m_pYFX[m_nActiveFX].hTech);

	// Begin the effect
	m_pYFX[m_nActiveFX].FX->Begin(&m_nPasses, 0);

	m_bIsSceneRunning = true;
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: Clear
// Desc: just clear the scene, only call this when Scene already begun
//-----------------------------------------------------------------------------
HRESULT YD3D::Clear(bool bClearPixel, bool bClearDepth, bool bClearStencil)
{
	DWORD dw = 0;

	// Is there anything to clear?
	if (bClearPixel)				 dw |= D3DCLEAR_TARGET;
	if (bClearDepth)				 dw |= D3DCLEAR_ZBUFFER;
	if (bClearStencil && m_bStencil) dw |= D3DCLEAR_STENCIL;

	// If is running the scene the stopit momentously
	if(m_bIsSceneRunning)
		m_pDevice->EndScene();

	if (FAILED(m_pDevice->Clear(0, NULL, dw, m_ClearColor, 1.0f, 0) ))
	{
		if (g_bLF)Log("Error: Clear in Clear()\n ");
		return Y_FAIL;
	}

	if(m_bIsSceneRunning)
		m_pDevice->BeginScene();

	return Y_OK;
}
//-----------------------------------------------------------------------------
// Name: EndRendering
// Desc: End Render operations and flip scene to frontbuffer
//-----------------------------------------------------------------------------
void YD3D::EndRendering()
{
	// flush all vertex buffers
	/*if (FAILED(m_pVertexManager->ForcedFlushAll()))
		if (g_bLF)Log("error: ForceFlushAll() from EndRendering() failed");
*/
	// End the effect
	m_pYFX[m_nActiveFX].FX->End();

	m_pDevice->EndScene();

	if (m_d3dpp.Windowed && (m_nNumhWnd > 0) ) 
	{
		if (FAILED(m_pChain[m_nActivehWnd]->Present(NULL, NULL, NULL, NULL, 0)))
			if (g_bLF)Log("error: Chain->Present() from EndRendering() failed");
	}
	else 
	{
		if (FAILED(m_pDevice->Present(NULL, NULL, NULL, NULL)))
			if (g_bLF)Log("error: Dev->Present() from EndRendering() failed");
	}

	m_bIsSceneRunning = false;
}

//-----------------------------------------------------------------------------
// Name: SetClearColor
// Desc: Change the color of the screen clearing operation
//-----------------------------------------------------------------------------
void YD3D::SetClearColor( float fRed, float fGreen, float fBlue)
{
	m_ClearColor = D3DCOLOR_COLORVALUE(fRed, fGreen, fBlue, 1.0f);
}

////-----------------------------------------------------------------------------
//// Name: FadeScreen
//// Desc: Fade the screen to ginven color by blending a semi-transparent quad
////		 over the whole backbuffer
////-----------------------------------------------------------------------------
//void YD3D::FadeScreen(float fR, float fG, float fB, float fA) 
//{
//	YENGINEMODE   OldMode;
//	LVERTEX       v[4];
//	bool          bChanged = false;
//	WORD          wI[6] = { 0, 1, 2, 0, 3, 1 };
//	DWORD         dwColor = D3DCOLOR_COLORVALUE(fR,fG,fB,fA);
//
//	if (m_bIsSceneRunning)
//		m_pDevice->EndScene();
//
//	// Actual skin gets invalid
//	SetActiveSkinID(MAX_ID);
//
//	v[0].x     = (float)m_dwWidth;  
//	v[0].y     = 0.0f;
//	v[0].z     = 1.0f;
//	v[0].tu    = 1.0f;
//	v[0].tv    = 0.0f;
//	v[0].Color = dwColor;
//
//	v[1].x     = 0.0f; 
//	v[1].y     = (float)m_dwHeight;
//	v[1].z     = 1.0f;
//	v[1].tu    = 0.0f;
//	v[1].tv    = 1.0f;
//	v[1].Color = dwColor;
//
//	v[2].x     = 0.0f; 
//	v[2].y     = 0.0f;
//	v[2].z     = 1.0f;
//	v[2].tu    = 0.0f;
//	v[2].tv    = 0.0f;
//	v[2].Color = dwColor;
//
//	v[3].x     = (float)m_dwWidth;
//	v[3].y     = (float)m_dwHeight;
//	v[3].z     = 1.0f;
//	v[3].tu    = 1.0f;
//	v[3].tv    = 1.0f;
//	v[3].Color = dwColor;
//
//	SetWorldTransform(NULL);
//	SetView3D(YVector(1,0,0), YVector(0,1,0), YVector(0,0,1), YVector(0,0,0));
//
//	UseShaders(false);
//	m_pDevice->SetTexture(0, NULL);
////   m_pDevice->SetStreamSource(0, NULL, 0, sizeof(LVERTEX));
////   m_pDevice->SetIndices(NULL);
//	m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
//	m_pDevice->SetFVF(FVF_LVERTEX);
//
//	// Check projection mode
//	if (m_Mode != EMD_TWOD) 
//	{
//		bChanged = true;
//		OldMode  = m_Mode;
//		SetMode(EMD_TWOD, m_nStage);
//	}
//
//	// Set material with alpha value
//	D3DMATERIAL9 mtrl;
//	memset(&mtrl, 0, sizeof(D3DMATERIAL9));
//	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
//	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
//	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
//	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
//	m_pDevice->SetMaterial(&mtrl);
//
//	// Activate alpha blending using alpha from material
//	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//
//	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//	m_pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
//	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_MODULATE);
//	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
//
//	// Dont need zbuffer
//	SetDepthBufferMode(RS_DEPTH_NONE);
//   
//	// Dont need cool shading, skip to gain speed
//	m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
//
//	m_pDevice->BeginScene();
//
//	if (FAILED(m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, wI, D3DFMT_INDEX16, v, sizeof(LVERTEX))))
//		if (g_bLF)Log("error: DrawPrimitiveUP()");
//
//	m_pDevice->EndScene();
//
//	if (bChanged) SetMode(OldMode, m_nStage);
//
//	m_pDevice->SetMaterial(&m_StdMtrl);
//
//	// Restore old d3d states
//	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_CURRENT);
//	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
//	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//	m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
//
//	SetDepthBufferMode(RS_DEPTH_READWRITE);
//
//	if (m_bIsSceneRunning)
//		m_pDevice->BeginScene();
//}

//-----------------------------------------------------------------------------
// Name: CreateFont
// Desc: Creates a D3DXFont object form a GDI font the caller submitted and
//		 returns its ID to the caller for later use.
//-----------------------------------------------------------------------------
HRESULT YD3D::CreateFont(int nWeight, bool bItalic, bool bUnderline, bool bStrike, DWORD dwSize, 
                            UINT *pID) 
 {
    HRESULT hr;
    HDC hDC;
    int nHeight;

    if (!pID) return Y_INVALIDPARAM;

    hDC = GetDC( NULL );
    nHeight = -MulDiv(dwSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    ReleaseDC(NULL, hDC);

    m_pFont = (LPD3DXFONT*)realloc(m_pFont, sizeof(LPD3DXFONT)*(m_nNumFonts+1));

    // build D3DX font from GDI font
    hr = D3DXCreateFont( m_pDevice, // Direct3D device
                         nHeight, nHeight/2, // font height and width
                         nWeight, // thickness, 0=default, 700=bold
                         1, // mipmap levels
                         bItalic, // italic style
                         DEFAULT_CHARSET, // character set
                         OUT_DEFAULT_PRECIS, // precision
                         DEFAULT_QUALITY, // quality
                         DEFAULT_PITCH | FF_DONTCARE, // pitch and family
                         "Arial", // font face (=font type)
                         &m_pFont[m_nNumFonts]); // address for new font

    if (SUCCEEDED(hr)) 
    {
       (*pID) = m_nNumFonts;
       m_nNumFonts++;
       return Y_OK;
    }
    else return Y_FAIL;
 }

//-----------------------------------------------------------------------------
// Name: DrawText
// Desc: Draws text using the previously created font object.
//-----------------------------------------------------------------------------
HRESULT YD3D::DrawText( UINT nID, int x, int y, UCHAR r, UCHAR g, UCHAR b, char *ch, ...) 
{
	RECT rc = { x, y, 0, 0 };
	char cch[1024];
	char *pArgs;

	// Put variables into the string
	pArgs = (char*) &ch + sizeof(ch);
	vsprintf(cch, ch, pArgs);

	if (nID >= m_nNumFonts) return Y_INVALIDPARAM;

	// Calculate actual size of the text
	m_pFont[nID]->DrawText(NULL, cch, -1, &rc, DT_SINGLELINE | DT_CALCRECT, 0);

	// Now draw the text 
	m_pFont[nID]->DrawText(NULL, cch, -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255,r,g,b));

	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: SetAmbientLight
// Desc: Set ambient light level to given values. If shaders are used the
//		 ambient color is stored in C4
//-----------------------------------------------------------------------------
void YD3D::SetAmbientLight(float fRed, float fGreen, float fBlue) 
{
	// Last chance check
	//m_pVertexManager->ForcedFlushAll();
	YCOLOR yc = { fRed, fGreen, fBlue, 1.0f };
	m_ycAmbientLight	= yc;
	m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hAmbientLight, &m_ycAmbientLight, sizeof(YCOLOR));	
	
	if(m_bIsSceneRunning)
		m_pYFX[m_nActiveFX].FX->CommitChanges();
}