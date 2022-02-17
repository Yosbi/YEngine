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
#include "math.h"  // For the sqrt function
extern bool g_bLF; // to know if the user wants the log of the engine

//-----------------------------------------------------------------------------
// Name: SetView3D
// Desc: Set camera as view matrix
//-----------------------------------------------------------------------------
HRESULT YD3D::SetView3D(const YVector &vcRight, const YVector &vcUp, const YVector &vcDir, const YVector &vcPos)
{
	if (!m_bRunning) return E_FAIL;

	m_mView3D._14 = m_mView3D._21 = m_mView3D._34 = 0.0f;
	m_mView3D._44 = 1.0f;

	m_mView3D._11 = vcRight.x;
	m_mView3D._21 = vcRight.y;
	m_mView3D._31 = vcRight.z;
	m_mView3D._41 = - (vcRight*vcPos);

	m_mView3D._12 = vcUp.x;
	m_mView3D._22 = vcUp.y;
	m_mView3D._32 = vcUp.z;
	m_mView3D._42 = - (vcUp*vcPos);

	m_mView3D._13 = vcDir.x;
	m_mView3D._23 = vcDir.y;
	m_mView3D._33 = vcDir.z;
	m_mView3D._43 = - (vcDir*vcPos);	

	// Set the eye position to the FX
	m_vcEyePos = vcPos;
	m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hEyePos, &m_vcEyePos, sizeof(float) * 3);	

	// CommitChanges is called in CalcWorldViewMatrix()

	// view changed so recalculate combomatrix
	CalcViewProjMatrix();
	CalcWorldViewProjMatrix();
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: SetViewLookAt
// Desc: Calculates look at matrix fo given point and calls SetView3D
//		 to activate the look matrix
//-----------------------------------------------------------------------------
HRESULT YD3D::SetViewLookAt(const YVector &vcPos, const YVector &vcPoint, const YVector &vcWorldUp)
{
	YVector vcDir,	// Camera direction
			vcTemp, // Helper
			vcUp;	// Camera up vector

	vcDir      = vcPoint - vcPos;
	vcDir.Normalize();

	// Calculate the up vector
	float fDot = vcWorldUp * vcDir;
	
	vcTemp	   = vcDir * fDot;
	vcUp	   = vcWorldUp - vcTemp;
	float fL   = vcUp.GetLength();

	// If length si too small take normal y axis as up vector
	if (fL < 1e-6f)
	{
		YVector vcY(0.0f, 1.0f, 0.0f);

		vcTemp = vcDir * vcDir.y;
		vcUp   = vcY - vcTemp;

		fL     = vcUp.GetLength();

		// If too small again take z axis as up vector
		if(fL < 1e-6f)
		{
			vcY.Set(0.0f, 0.0f, 1.0f);

			vcTemp = vcDir * vcDir.z;
			vcUp   = vcY - vcTemp;

			// If still too small (...)
			fL     = vcUp.GetLength();

			if(fL < 1e-6f) return Y_FAIL;
		}

	}

	// Normalize vector
	vcUp /= fL;

	// Build right vector using cross product
	YVector vcRight;
	vcRight.Cross(vcUp, vcDir);

	// Build final matrix and set for device 
	return SetView3D(vcRight, vcUp, vcDir, vcPos);

}

//-----------------------------------------------------------------------------
// Name: SetClippingPlanes
// Desc: Set the near and far clippings panes of the frustum
//-----------------------------------------------------------------------------
void YD3D::SetClippingPlanes(float fNear, float fFar)
{
	m_fNear = fNear;
	m_fFar  = fFar;

	if (m_fNear <= 0.0f)
		m_fNear = 0.01f;

	if (m_fFar <= 1.0f)
		m_fFar = 1.00f;
   
	if (m_fNear >= m_fFar) 
	{
		m_fNear = m_fFar;
		m_fFar  = m_fNear + 1.0f;
	}

	// change 2D projection and view
	Prepare2D();

	// change orthogonal projection
	float Q = 1.0f / (m_fFar-m_fNear);
	float X = m_fNear/(m_fNear-m_fFar);
	m_mProjO[0]._33 = m_mProjO[1]._33 = Q;
	m_mProjO[2]._33 = m_mProjO[3]._33 = Q;
	m_mProjO[0]._43 = m_mProjO[1]._43 = X;
	m_mProjO[2]._43 = m_mProjO[3]._43 = X;

	// change perspective projection
	Q *= m_fFar;
	X = -Q * m_fNear;
	m_mProjP[0]._33 = m_mProjP[1]._33 = Q;
	m_mProjP[2]._33 = m_mProjP[3]._33 = Q;
	m_mProjP[0]._43 = m_mProjP[1]._43 = X;
	m_mProjP[2]._43 = m_mProjP[3]._43 = X;


	// Projection changed, Recalculate the combos
	CalcViewProjMatrix();
	CalcWorldViewProjMatrix();
}

//-----------------------------------------------------------------------------
// Name: InitStage
// Desc: Calculate perspective and orthogonal projection matrix for given
//		 stage using given values. This will not activate any settings
//-----------------------------------------------------------------------------
HRESULT YD3D::InitStage(float fFOV, YVIEWPORT *pView, int nStage)
{	
	float fAspect;
	bool  bOwnRect=false;
   
	if (!pView) 
	{
		YVIEWPORT vpOwn = { 0, 0, m_dwWidth, m_dwHeight};
		memcpy(&m_VP[nStage], &vpOwn, sizeof(RECT));
	}
	else
		memcpy(&m_VP[nStage], pView, sizeof(RECT));

	if ( (nStage>3) || (nStage<0) ) nStage=0;

	fAspect = ((float)(m_VP[nStage].Height)) / (m_VP[nStage].Width);

	// Perspective projection matrix
	if (FAILED(this->CalcPerspProjMatrix(fFOV, fAspect, &m_mProjP[nStage])))
		return Y_FAIL;

	// orthogonal projection matrix
	memset(&m_mProjO[nStage], 0, sizeof(float)*16); 
	m_mProjO[nStage]._11 =  2.0f/m_VP[nStage].Width;
	m_mProjO[nStage]._22 =  2.0f/m_VP[nStage].Height;
	m_mProjO[nStage]._33 =  1.0f/(m_fFar-m_fNear);

	m_mProjO[nStage]._43 =  m_fNear/(m_fNear-m_fFar);
	m_mProjO[nStage]._44 =  1.0f;

	// If changing the current stage recalculate the combos
	if (nStage == m_nStage)
	{
		CalcViewProjMatrix();
		CalcWorldViewProjMatrix();
	}

	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: SetMode
// Desc: Set mode for projection and view for given stage
//-----------------------------------------------------------------------------
HRESULT YD3D::SetMode(YENGINEMODE Mode, int nStage)
{
	D3DVIEWPORT9 d3dVP;

	if (!m_bRunning) return E_FAIL;
	if ((nStage > 3) || (nStage < 0)) nStage=0;
   
	if (m_Mode != Mode)
		m_Mode  = Mode;

	// we change fundamentals settings so fush all content
	//m_pVertexMan->ForcedFlushAll();

	m_nStage = nStage;

	// set viewport
	d3dVP.X      = m_VP[nStage].X;
	d3dVP.Y      = m_VP[nStage].Y;
	d3dVP.Width  = m_VP[nStage].Width;
	d3dVP.Height = m_VP[nStage].Height;
	d3dVP.MinZ   = 0.0f;
	d3dVP.MaxZ   = 1.0f;

	// If 2D mode set orthogonal projection and view
	if (Mode==EMD_TWOD) 
	{
		if (FAILED(m_pDevice->SetViewport(&d3dVP)))
			return Y_FAIL;		
	}

	// Perspective or orthogonal projection
	else 
	{
		if (FAILED(m_pDevice->SetViewport(&d3dVP)))
			return Y_FAIL;
		
		
	}

	CalcViewProjMatrix();
	CalcWorldViewProjMatrix();
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: SetWorldTransform
// Desc: Sets the world transformation matrix
//-----------------------------------------------------------------------------
void YD3D::SetWorldTransform(const YMatrix *mWorld)
{
	// last chance check
   //m_pVertexMan->ForcedFlushAll();

   // set class attribute 'world matrix'
	if (!mWorld) 
	{
		YMatrix m; m.Identity();
		memcpy(&m_mWorld, &m, sizeof(D3DMATRIX)); 
	}
	else
		memcpy(&m_mWorld, mWorld, sizeof(D3DMATRIX)); 

	// recalculate connected values
	CalcWorldViewProjMatrix();	
}

//-----------------------------------------------------------------------------
// Name: Prepare2D
// Desc: Calculate orthogonal projection and view matrix to render 
//		 vertices whos world-coordinates are given in screen space
//-----------------------------------------------------------------------------
void YD3D::Prepare2D()
{
	// Set matrices to indentify
	memset(&m_mProj2D, 0, sizeof(float) * 16);
	memset(&m_mView2D, 0, sizeof(float) * 16);
	m_mView2D._11 = m_mView2D._22 = m_mView2D._33 = m_mView2D._44 = 1;

	// Orthogonal projection matrix
	m_mProj2D._11 = 2.0f / (float)m_dwWidth;
	m_mProj2D._22 = 2.0f / (float)m_dwHeight;
	m_mProj2D._33 = 1.0f / (m_fFar - m_fNear);
	m_mProj2D._43 = -m_fNear * (1.0f / (m_fFar - m_fNear));
	m_mProj2D._44 = 1.0f;

	// Set 2D view matrix
	float tx, ty, tz;
	tx = -((int)m_dwWidth) + m_dwWidth * 0.5f;
	ty = (float)m_dwHeight - m_dwHeight * 0.5;
	tz = m_fNear + 0.1f;

	m_mView2D._22 = -1.0f;
	m_mView2D._41 = tx;
	m_mView2D._42 = ty;
	m_mView2D._43 = tz;
}

//-----------------------------------------------------------------------------
// Name: CalcPerspProjMatrix
// Desc: Calculate perspective 3D projection matrix. The FOV is the field of 
//		 horizontal view.
//-----------------------------------------------------------------------------
HRESULT	YD3D::CalcPerspProjMatrix(float fFOV, float fAspect, D3DMATRIX *m)
{
	if(fabs(m_fFar - m_fNear) < 0.01f)
		return Y_FAIL;

	float sinFOV2 = sinf(fFOV/2);

	if(fabs(sinFOV2) < 0.01f)
		return Y_FAIL;

	float cosFOV2 = cosf(fFOV/2);

	float w = fAspect * (cosFOV2 / sinFOV2);
	float h =   1.0f  * (cosFOV2 / sinFOV2);
	float Q = m_fFar / (m_fFar - m_fNear);

	memset(m, 0, sizeof(D3DMATRIX));
	(*m)._11 = w;
	(*m)._22 = h;
	(*m)._33 = Q;
	(*m)._34 = 1.0f;
	(*m)._43 = -Q*m_fNear;
	
	return Y_OK;
}

//-----------------------------------------------------------------------------
// Name: CalcViewProjMatrix
// Desc: ReCalculate view-projection combo matrix each time one of them is 
//		 changing to mantain integrity
//-----------------------------------------------------------------------------
void YD3D::CalcViewProjMatrix()
{
	YMatrix *pA;
	YMatrix *pB;

	// 2D, perspective or orthogonal mode
	if (m_Mode == EMD_TWOD) 
	{
		pA = (YMatrix*)&m_mProj2D;
		pB = (YMatrix*)&m_mView2D;
		
	}
	else 
	{
		pB = (YMatrix*)&m_mView3D;

		if (m_Mode == EMD_PERSPECTIVE)
			pA = (YMatrix*)&(m_mProjP[m_nStage]);
		else
			pA = (YMatrix*)&(m_mProjO[m_nStage]);
	}

	YMatrix *pM = (YMatrix*)&m_mViewProj;
	(*pM) = (*pB) * (*pA);
}

//-----------------------------------------------------------------------------
// Name: CalcWorldViewProjMatrix
// Desc: ReCalculate world-view-projection combo matrix each time one of them is 
//		 changing to mantain integrity
//-----------------------------------------------------------------------------
void YD3D::CalcWorldViewProjMatrix()
{
	YMatrix *pProj;
	YMatrix *pView;
	YMatrix *pWorld;

	pWorld = (YMatrix*)&m_mWorld;

	// 2D, perspective or orthogonal mode
	if (m_Mode == EMD_TWOD)
	{
		pProj = (YMatrix*)&m_mProj2D;
		pView = (YMatrix*)&m_mView2D;
	}
	else 
	{
		pView = (YMatrix*)&m_mView3D;

		if (m_Mode == EMD_PERSPECTIVE)
			pProj = (YMatrix*)&(m_mProjP[m_nStage]);
		else
			pProj = (YMatrix*)&(m_mProjO[m_nStage]);
	}

	YMatrix *pCombo = (YMatrix*)&m_mWorldViewProj;
	(*pCombo) = ((*pWorld) * (*pView)) * (*pProj);

	// Set transform as constant to device
	m_pYFX[m_nActiveFX].FX->SetMatrix(m_pYFX[m_nActiveFX].hWVP, (D3DXMATRIX*)pCombo);

	if(m_bIsSceneRunning)
			m_pYFX[m_nActiveFX].FX->CommitChanges();
	 
	
}

