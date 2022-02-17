//-------------------------------------------------------------------------------
// YD3DShaderStuff.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: This file contains some misc functions, like the ones used to 
//		modificate the render states
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Include
//--------------------------------------------------------------------------
#include "YD3D.h"
//#include "math.h"
extern bool g_bLF; // to know if the user wants the log of the engine


//-----------------------------------------------------------------------
// Name: SetTextureStage
// Desc: Sets the texture operation on a determined stage
//--------------------------------------------------------------------------
//HRESULT YD3D::SetTextureStage(UCHAR n, YRENDERSTATE rs) {
//	D3DCAPS9 caps;
//
//	m_pDevice->GetDeviceCaps(&caps);
//	if (caps.MaxSimultaneousTextures < (n+1)) 
//		return Y_FAIL;
//
//	switch (rs) 
//	{
//		case RS_NONE:
//			m_TOP[n] = D3DTOP_DISABLE;
//			break;
//		case RS_TEX_ADDSIGNED:
//			m_TOP[n] = D3DTOP_ADDSIGNED;
//			break;
//		case RS_TEX_MODULATE:
//			m_TOP[n] = D3DTOP_MODULATE;
//			break;
//
//		default: break;
//	}
//
//
//	return Y_OK;
//} 

//-----------------------------------------------------------------------
// Name: SetBackfaceCulling
// Desc: Set the back face culling render state
//--------------------------------------------------------------------------
void YD3D::SetBackfaceCulling(YRENDERSTATE rs) {
	// clear out buffers prior to state changes
	//m_pVertexManager->ForcedFlushAll();

	if (rs == RS_CULL_CW) 
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	else if (rs == RS_CULL_CCW) 
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	else if (rs == RS_CULL_NONE) 
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

//-----------------------------------------------------------------------
// Name: SetStencilBufferMode
// Desc: Sets the Depth stencil buffer mode
//--------------------------------------------------------------------------
void YD3D::SetStencilBufferMode(YRENDERSTATE rs, DWORD dw) {
	// Clear out buffers prior to state changes
	//m_pVertexManager->ForcedFlushAll();

	switch (rs) 
	{
		// Switch on and off
		case RS_STENCIL_DISABLE:
			m_pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
			break;
		case RS_STENCIL_ENABLE:
			m_pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
			break;
		case RS_DEPTHBIAS:
			m_pDevice->SetRenderState(D3DRS_DEPTHBIAS, dw);

		// Function modes and values
		case RS_STENCIL_FUNC_ALWAYS:
			m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
			break;
		case RS_STENCIL_FUNC_LESSEQUAL:
			m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
			break;
		case RS_STENCIL_FUNC_EQUAL:
			m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
			break;
		case RS_STENCIL_MASK:
			m_pDevice->SetRenderState(D3DRS_STENCILMASK, dw);
			break;
		case RS_STENCIL_WRITEMASK:
			m_pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, dw);
			break;
		case RS_STENCIL_REF:
			m_pDevice->SetRenderState(D3DRS_STENCILREF, dw);
			break;

		// Stencil test fails modes
		case RS_STENCIL_FAIL_DECR:
			m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_DECR);
			break;
		case RS_STENCIL_FAIL_INCR:
			m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCR);
			break;
		case RS_STENCIL_FAIL_KEEP:
			m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
			break;

		// Stencil test passes but z test fails modes
		case RS_STENCIL_ZFAIL_DECR:
			m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR);
			break;
		case RS_STENCIL_ZFAIL_INCR:
			m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR);
			break;
		case RS_STENCIL_ZFAIL_KEEP:
			m_pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
			break;

		// Stencil test passes modes
		case RS_STENCIL_PASS_DECR:
			m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
			break;
		case RS_STENCIL_PASS_INCR:
			m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
			break;
		case RS_STENCIL_PASS_KEEP:
			m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
			break;
		case RS_STENCIL_PASS_REPLACE:
			m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
			break;
	} 
} 

//-----------------------------------------------------------------------
// Name: UseColorBuffer
// Desc: Enable or disable use of color buffer
//--------------------------------------------------------------------------
void YD3D::UseColorBuffer(bool b)
{ 
	if (b == m_bColorBuffer)
		return;
	// Clear out buffers prior to state changes
	/*m_pVertexManager->ForcedFlushAll();
	m_pVertexManager->InvalidateStates();*/

	m_bColorBuffer = b;

	if(!b)
	{
		m_bAlpha = false;
		m_bAdditive = false;
		/*m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR);*/
	}
	else 
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	}
	
}

//-----------------------------------------------------------------------
// Name: SetDepthBufferMode
// Desc: Activate depth buffer
//--------------------------------------------------------------------------
void YD3D::SetDepthBufferMode(YRENDERSTATE rs) 
{
	// clear out buffers prior to state changes
	//m_pVertexManager->ForcedFlushAll();

	if (rs == RS_DEPTH_READWRITE) 
	{
		m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	else if (rs == RS_DEPTH_READONLY)
	{
		m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

	else if (rs == RS_DEPTH_NONE)
	{
		m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}
}

//-----------------------------------------------------------------------
// Name: UseAdditiveBlending
// Desc: The user wants to render additive
//--------------------------------------------------------------------------
void YD3D::UseAdditiveBlending(bool b) 
{
	if (m_bAdditive == b) return;
	m_bAdditive = b;
	// clear all vertex caches
	/* m_pVertexManager->ForcedFlushAll();
	m_pVertexManager->InvalidateStates();*/
	if (b)
	{
		m_bAlpha = false;
		m_bColorBuffer = true;
	}
	else
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	}
}

//void YD3D::UseAlphaBlending(bool b)
//{
//	if (m_bAlpha == b) return;
//	m_bAlpha = b;
//	// clear all vertex caches
//	/* m_pVertexManager->ForcedFlushAll();
//	m_pVertexManager->InvalidateStates();*/
//	if (b)
//	{
//		m_bAdditive = false;
//		m_bColorBuffer = true;
//	}
//	else
//	{
//		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//	}
//}