//-------------------------------------------------------------------------------
// YD3DEffectsStuff.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: This file contains some The implementation of the D3D effects 
//		frameworks that encapsulates pixel and vertex shaders
//--------------------------------------------------------------------------

//-----------------------------------------------------------------------
// Include
//--------------------------------------------------------------------------
#include "YD3D.h"
//#include "math.h"
extern bool g_bLF; // to know if the user wants the log of the engine


//-----------------------------------------------------------------------
// Name: CreateEffect
// Desc: Create a vertex shader object for the dirext3d device
//--------------------------------------------------------------------------
HRESULT YD3D::CreateFX(const char *pData, const char *chTechnique, bool bDebug, UINT *pFXID)
{
	// Check if the max number of effects has been created
	if (m_nNumFX >= (MAX_FX - 1))
	{
		if(g_bLF)Log("Error: max number of effects reached in YD3D::CreateEffect");
		return Y_OUTOFMEMORY;
	}

	
	// Init the YEFFECT
	m_pYFX[m_nNumFX].hTech		 = NULL;
	m_pYFX[m_nNumFX].hWVP		 = NULL;
	m_pYFX[m_nNumFX].hDiffuseMat = NULL;
	m_pYFX[m_nNumFX].hAmbientMat = NULL;
	m_pYFX[m_nNumFX].FX			 = NULL;

	LPD3DXBUFFER pDebug			 = NULL;
	DWORD		 dwFlag			 = 0;
	HRESULT		 hr				 = NULL;

	// The user want to debug?
	if (pDebug)
		dwFlag = D3DXSHADER_DEBUG;
	
	// Create the effect
	if (FAILED(hr = D3DXCreateEffectFromFile(m_pDevice, pData, 0, 0, dwFlag, 0, &m_pYFX[m_nNumFX].FX, &pDebug)))
	{
		if(g_bLF)Log("error: CreateEffect() failed");

		if (hr==D3DERR_INVALIDCALL)
			if(g_bLF)Log("INVALID_CALL");

		else if (hr == D3DXERR_INVALIDDATA)
			if(g_bLF)Log("D3DXERR_INVALIDDATA");

		else if (hr==D3DERR_OUTOFVIDEOMEMORY)
			if(g_bLF)Log("D3DERR_OUTOFVIDEOMEMORY");

		else if (hr==E_OUTOFMEMORY)
			if(g_bLF)Log("E_OUTOFMEMORY");

		else if (hr ==  D3DERR_NOTFOUND)
			if(g_bLF)Log("D3DERR_NOTFOUND");

		if (pDebug && bDebug)
			if(g_bLF)Log("Effect debugger says: %s", (char*)pDebug->GetBufferPointer());
		else 
			if(g_bLF)Log("no debug infos stored");

		return Y_FAIL;
	}	
	
	// Get the technique handle
	m_pYFX[m_nNumFX].hTech = m_pYFX[m_nNumFX].FX->GetTechniqueByName(chTechnique);	
	/*if (!m_pYFX[m_nNumFX].hTech)
	{
		Log("Could not find tech %s", chTechnique);
		return Y_INVALIDPARAM;
	}*/

	// Get the basic parameters handles
	m_pYFX[m_nNumFX].hWVP		 = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gWVP");			// Handle to the WVP combo matrix
	/*if (!m_pYFX[m_nNumFX].hWVP)
		Log("error2");*/

	m_pYFX[m_nNumFX].hDiffuseMat = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gDiffuseMtrl");  // Handle to diffuse material component
	/*if (!m_pYFX[m_nNumFX].hDiffuseMat)
		Log("error3");*/
	
	m_pYFX[m_nNumFX].hAmbientMat = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gAmbientMtrl");  // Handle to ambient material component
	/*if (!m_pYFX[m_nNumFX].hDiffuseMat)
		Log("error3");*/

	m_pYFX[m_nNumFX].hEmissiveMat = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gEmissiveMtrl");  // Handle to ambient material component
	/*if (!m_pYFX[m_nNumFX].hDiffuseMat)
		Log("error3");*/

	m_pYFX[m_nNumFX].hSpecMat = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gSpecularMtrl");  // Handle to specular material component
	/*if (!m_pYFX[m_nNumFX].hDiffuseMat)
		Log("error3");*/

	m_pYFX[m_nNumFX].hSpecPowMat = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gSpecularPowerMtrl");  // Handle to specular power material component
	/*if (!m_pYFX[m_nNumFX].hDiffuseMat)
		Log("error3");*/

	m_pYFX[m_nNumFX].hAmbientLight = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gAmbientLight");  // Handle to general ambient light
	/*if (!m_pYFX[m_nNumFX].hDiffuseMat)
		Log("error3");*/

	m_pYFX[m_nNumFX].hEyePos = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gEyePosW");  // Handle to the camera position in world space
	/*if (!m_pYFX[m_nNumFX].hDiffuseMat)
		Log("error3");*/

	// Setting the textures handles
	m_pYFX[m_nNumFX].hTex[0] = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gTex1");
	m_pYFX[m_nNumFX].hTex[1] = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gTex2");
	m_pYFX[m_nNumFX].hTex[2] = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gTex3");
	m_pYFX[m_nNumFX].hTex[3] = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gTex4");
	m_pYFX[m_nNumFX].hTex[4] = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gTex5");
	m_pYFX[m_nNumFX].hTex[5] = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gTex6");
	m_pYFX[m_nNumFX].hTex[6] = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gTex7");
	m_pYFX[m_nNumFX].hTex[7] = m_pYFX[m_nNumFX].FX->GetParameterByName(0, "gTex8");

	// Set the return id
	if (pFXID) (*pFXID) = m_nNumFX;

	// Add a effect
	m_nNumFX++;	
	return Y_OK;	
}

//-----------------------------------------------------------------------
// Name: SetEffectConstant
// Desc: Sets a constant to the currently used effect
//--------------------------------------------------------------------------
void YD3D::ChangeFXTechnique(UINT nFXID, const char *chTechnique)
{
	if(m_bIsSceneRunning)
	{		// End the fx
		m_pYFX[m_nActiveFX].FX->End();
	}
		
	// Changing tech
	m_pYFX[nFXID].hTech = m_pYFX[nFXID].FX->GetTechniqueByName(chTechnique);

	if(m_bIsSceneRunning)
	{
		// Begin the FX
		// Set the effect technique
		m_pYFX[m_nActiveFX].FX->SetTechnique(m_pYFX[m_nActiveFX].hTech);

		// Begin the effect
		m_pYFX[m_nActiveFX].FX->Begin(&m_nPasses, 0);
	}
}

//-----------------------------------------------------------------------
// Name: SetEffectConstant
// Desc: Sets a constant to the currently used effect
//--------------------------------------------------------------------------
HRESULT YD3D::SetFXConstant(YDATATYPE dat, const char* chName, const void *pData, UINT nBytes)
{	
	D3DXHANDLE hData = NULL;
	HRESULT	   hr	 = NULL;

	// Getting the handle of the constant
	hData = m_pYFX[m_nActiveFX].FX->GetParameterByName(0, chName);
	
	// Set the data
	switch(dat)
	{
	case DAT_FLOAT:{
		hr = m_pYFX[m_nActiveFX].FX->SetFloat(hData, *((float*)pData));		
		break;
		}

	case DAT_BOOL:{
		hr = m_pYFX[m_nActiveFX].FX->SetBool(hData, *((bool*)pData));
		break;		
		}
		
	case DAT_INT:{
		hr = m_pYFX[m_nActiveFX].FX->SetInt(hData, *((int*)pData));
		break;		
		}

	case DAT_TEXTURE:{
		hr = m_pYFX[m_nActiveFX].FX->SetTexture(hData, (LPDIRECT3DBASETEXTURE9)pData);
		break;		
		}

	case DAT_VECTOR:{
		hr = m_pYFX[m_nActiveFX].FX->SetVector(hData, (D3DXVECTOR4*)pData);
		break;		
		}

	case DAT_MATRIX:{
		hr = m_pYFX[m_nActiveFX].FX->SetMatrix(hData, (D3DXMATRIX*)pData);
		break;
		}	

	case DAT_OTHER:{
		hr = m_pYFX[m_nActiveFX].FX->SetValue(hData, pData, nBytes);
		break;
		}	

	default: return Y_INVALIDPARAM;
		
	}

	// Check for error
	if (FAILED(hr))
	{
		if (g_bLF)Log("Cannot set the constant %s in the current effect", chName); 
		return Y_FAIL;		
	}

	// If the scene is running must to commit this change
	if (m_bIsSceneRunning)	
		m_pYFX[m_nActiveFX].FX->CommitChanges();

	return Y_OK;
}

//-----------------------------------------------------------------------
// Name: ActivateEffect
// Desc: Set a effect to render
//--------------------------------------------------------------------------
HRESULT YD3D::ActivateFX(UINT nID)
{	
	// See if the effect is valid
	if (nID >= m_nNumFX)
		return Y_INVALIDID;	

	static YD3DSkinManager* pYSM = (YD3DSkinManager*)m_pSkinManager;

	if (m_nActiveFX != nID)
	{
		if(m_bIsSceneRunning)
		{
			// End the last fx
			m_pYFX[m_nActiveFX].FX->End();

			// Begin the next
			// Set the effect technique
			m_pYFX[nID].FX->SetTechnique(m_pYFX[nID].hTech);

			// Begin the effect
			m_pYFX[nID].FX->Begin(&m_nPasses, 0);
		}
		// Set this effect as active
		m_nActiveFX = nID;

		//------------------------Set the effect and set the basic parameters---------------------------
		// WVP matrix
		m_pYFX[m_nActiveFX].FX->SetMatrix(m_pYFX[m_nActiveFX].hWVP, (D3DXMATRIX*)&m_mWorldViewProj); 

		// Set material for the FX
		if (m_nActiveSkin < MAX_ID)
		{
			if (pYSM->m_nNumSkins > 0)
			{
				UINT	   nSkinID	= GetActiveSkinID();		
				YSKIN	  *pSkin	= &pYSM->m_pSkins[nSkinID];		
				YMATERIAL *pMat		= &pYSM->m_pMaterials[pSkin->nMaterial];
				m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hDiffuseMat, &pMat->cDiffuse, sizeof(YCOLOR));
				m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hAmbientMat, &pMat->cAmbient, sizeof(YCOLOR));
				m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hEmissiveMat, &pMat->cEmissive, sizeof(YCOLOR));
				m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hSpecMat, &pMat->cSpecular, sizeof(YCOLOR));
				m_pYFX[m_nActiveFX].FX->SetFloat(m_pYFX[m_nActiveFX].hSpecPowMat, pMat->fPower);

			}
		}

		// Set the ambient light
		m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hAmbientLight, &m_ycAmbientLight, sizeof(YCOLOR));	
		m_pYFX[m_nActiveFX].FX->SetValue(m_pYFX[m_nActiveFX].hEyePos, &m_vcEyePos, sizeof(float) * 3);	

		if(m_bIsSceneRunning)
			m_pYFX[m_nActiveFX].FX->CommitChanges();

	}

	return Y_OK;
}