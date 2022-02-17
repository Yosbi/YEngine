//-----------------------------------------------------------------------
// YD3DVertexCache.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: This file contains datatypes and clases to implement a the 
// rendering part of the Y-Engine, it contains a vertex cache class and
// a vertex cache manager.

#ifndef YD3DVERTEXCACHE_H
#define YD3DVERTEXCACHE_H

//----------------------------------------------------------------------
// includes
//----------------------------------------------------------------------
#include <d3d9.h>
#include "Y.h"
#include "YD3D.h"
#include "YD3DSkinManager.h"

#define NUM_CACHES	50	// Number of dynamic caches in the manager
class YD3D;

//----------------------------------------------------------------------
// Name: YSTATICBUFFER
// Desc: Struct for static vertex/index data
//----------------------------------------------------------------------
typedef struct YSTATICBUFFER_TYPE
{
	int			nStride;
	UINT		nSkinID;
	bool		bIndis;
	int			nNumVerts;
	int			nNumIndis;
	int			nNumTris;
	YVERTEXID	VID;
	LPDIRECT3DVERTEXBUFFER9 pVB;
	LPDIRECT3DINDEXBUFFER9	pIB;
} YSTATICBUFFER;

//----------------------------------------------------------------------
// Name: YD3DVertexCacheManager
// Desc: Class to manage static and dynamic vertex bunches, optionally
//		 using indices during rendering process.
//----------------------------------------------------------------------
class YD3DVertexCacheManager : public YVertexCacheManager
{
	friend class YD3D;
public:
	// Cons/Dest
	YD3DVertexCacheManager(YD3DSkinManager *pSkinMan, LPDIRECT3DDEVICE9 pDevice, YD3D *pYD3D, 
						   UINT nMaxVerts, UINT nMaxIndis, FILE *pLog);
	~YD3DVertexCacheManager();

	// Public functions
	HRESULT		 CreateStaticBuffer (YVERTEXID VertexID, UINT nSkinID, UINT nVerts, UINT nIdis, const void *pVerts, 
									 const WORD *pIndis, UINT *pnID);
	HRESULT		 Render				(UINT nSBufferID);
	//YRENDERSTATE GetShadeMode		();

private:
	// Private functions	
	void		 Log				(char *, ...);
	HRESULT		 SetVertexDeclaration(DWORD VertexID);

	// Private member vars
	YD3DSkinManager		*m_pSkinMan;				// The skin manager
	LPDIRECT3DDEVICE9	 m_pDevice;					// the render device
	YD3D				*m_pYD3D;					// The YD3D class

	YSTATICBUFFER		*m_pSB;						// Array of Static vertex buffer
	//YINDEXBUFFER		*m_pIB;						// Array of Index buffer
	UINT				 m_nNumSB;					// Number of statics buffers saved
	UINT				 m_nNumIB;					// Number of index buffers saved
	//YD3DVertexCache     *m_CachePS[NUM_CACHES];		// Position only
 //   YD3DVertexCache     *m_CacheUU[NUM_CACHES];		// Untransformed Unlit
 //   YD3DVertexCache     *m_CacheUL[NUM_CACHES];		// Untransformed Lit
 //   YD3DVertexCache     *m_CacheCA[NUM_CACHES];		// Character animation
 //   YD3DVertexCache     *m_Cache3T[NUM_CACHES];		// Three textures
 //   YD3DVertexCache     *m_CacheTV[NUM_CACHES];		// uu with tanget
	DWORD				 m_dwActiveCache;			// The currently active cache
	DWORD				 m_dwActiveSB;				// The currently active static buffer
	DWORD				 m_dwActiveIB;				// The currently active index
	DWORD				 m_dwActiveVertexDcl;		// The currently active vertex declatation
	FILE				*m_pLog;
};

#endif