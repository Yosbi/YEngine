//-------------------------------------------------------------------------------
// Y.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: This file is used to define macros like errors and
//		things that need to be globally know, and other stuff
//--------------------------------------------------------------------------
#ifndef Y_H
#define Y_H
#include <Windows.h>
#include "YEMath.h"
//-----------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------
#define MAX_ID 65535  // Max word value

////////////
// Returns
////////////
// everything went smooth
#define Y_OK                  S_OK

// just reports no errors
#define Y_CANCELED            0x82000000

// general error message
#define Y_FAIL                0x82000001

// specific error messages
#define Y_CREATEAPI           0x82000002 // Error creating api (Direct3D9)
#define Y_CREATEDEVICE        0x82000003 // Error creating device (Direct3DCreate9)
#define Y_INVALIDPARAM		  0x82000004
#define Y_NODEPTHSTENCIL      0x82000005
#define Y_INVALIDID           0x82000006 // Given id is not valid
#define Y_BUFFERSIZE          0x82000007 // Buffer size of a predefined array exeeded
#define Y_OUTOFMEMORY         0x82000008 // Out of memory
#define Y_FILENOTFOUND        0x82000009
#define Y_INVALIDFILE		  0x8200000a // If a invalid file has been introduced (like a 16 bit bitmap)
#define Y_BUFFERLOCK          0x8200000b // If any Direct3D resourse can not be locked
#define Y_NOSHADERSUPPORT     0x8200000c // If current hardware dont have shader support
#define Y_CREATEBUFFER		  0x8200000d // Error creating buffer
#define Y_FAILc               0x8200000e
#define Y_FAILd               0x8200000f

//-----------------------------------------------------------------------
// Enumerations
//-----------------------------------------------------------------------
//----------------------------------------------------------------------
// Name: YENGINEMODE
// Desc: Used to set the current mode of the engine e.g 2d, projection,
//		or orthogonal
//----------------------------------------------------------------------
typedef enum YENGINEMODE_TYPE
{
	EMD_PERSPECTIVE, // Perspective projection
	EMD_TWOD,		 // Worlds equals screen coodinates
	EMD_ORTHOGONAL	 // Orthogonal projection
}YENGINEMODE;

//-----------------------------------------------------------------------
// Vertex types
//-----------------------------------------------------------------------
// Just untransformed position
typedef struct PVERTEX_TYPE{
	float x, y, z;
} PVERTEX;

// Untransformed unlit vertex
typedef struct VERTEX_TYPE{
	float x, y, z;
	float vcN[3];
	float tu, tv;
} VERTEX;

// Untransformed and lit vertex
typedef struct LVERTEX_TYPE{
	float x, y, z;
	DWORD Color;
	float tu, tv;
} LVERTEX;

// Used for character animation
typedef struct CVERTEX_TYPE{
	float x, y, z;
	float vcN[3];
	float tu, tv;
	float fBone1, fWeigth1;
	float fBone2, fWeigth2;
} CVERTEX;

// Three textures coord pairs
typedef struct VERTEX3T_TYPE{
	float x, y, z;
	float vcN[3];
	float tu0, tv0;
	float tu1, tv1;
	float tu2, tv2;
} VERTEX3T;

// Like UU but with tangent vector
typedef struct TVERTEX_TYPE{
	float x, y, z;
	float vcN[3];
	float tu, tv;
	float vcU[3]; // Tangent
} TVERTEX;

//-----------------------------------------------------------------------
// Data types
//-----------------------------------------------------------------------

//----------------------------------------------------------------------
// Name: YVIEWPORT
// Desc: Basic viewport structure
//----------------------------------------------------------------------
typedef struct YVIEWPORT_TYPE
{
	DWORD X;	// Position of the upper left corner
	DWORD Y;
	DWORD Width;
	DWORD Height;
} YVIEWPORT;

//----------------------------------------------------------------------
// Name: YCOLOR
// Desc: Basic color structure
//----------------------------------------------------------------------
typedef struct YCOLOR_TYPE
{
	union
	{
		struct
		{
			float fR;
			float fG;
			float fB;
			float fA;
		};
		float c[4];
	};
} YCOLOR;

//----------------------------------------------------------------------
// Name: YMATERIAL
// Desc: Material structure
//----------------------------------------------------------------------
typedef struct YMATERIAL_TYPE
{
	YCOLOR cDiffuse;  // RGBA diffuse light
	YCOLOR cAmbient;  // RGBA ambient light
	YCOLOR cSpecular; // RGBA specular light
	YCOLOR cEmissive; // RGBA emissive light
	float  fPower;	  // Specular power
} YMATERIAL;

//----------------------------------------------------------------------
// Name: YTEXTURE
// Desc: Texture estructure
//----------------------------------------------------------------------
typedef struct YTEXTURE_TYPE
{
	float   fAlpha;		// Overall transparency value
	char   *chName;		// Texture filename
	void   *pData;		// Texture data
	YCOLOR *pClrKeys;	// Color key array
	DWORD   dwNum;		// Number of color keys
} YTEXTURE;

//----------------------------------------------------------------------
// Name: YSKIN
// Desc: Skin estructure, it uses a type of material, and up to a 
//		maximum of 8 textures
//----------------------------------------------------------------------
typedef struct YSKIN_TYPE
{
	bool bAlpha; // This skin uses transparency?
	UINT nMaterial; // The ordinal of the material in the array of materials
	UINT nTexture[8]; // Array of ordinals of texture referencing the array of textures
} YSKIN;


//----------------------------------------------------------------------
// Enumerated types
//----------------------------------------------------------------------

typedef enum YSHADERVER_TYPE
{
	SH_1_1,		// Shader version 1.1
	SH_2_0,		// Shader version 2.0
	SH_3_0		// Shader version 3.0
} YSHADERVER;

//----------------------------------------------------------------------
// Name: YVERTEXID
// Desc: Definition of the vertex types in a enum
//----------------------------------------------------------------------
typedef enum YVERTEXID_TYPE
{
	VID_PS,		// Untransformed position only
	VID_UU,		// Untransformed and unlit vertex
	VID_UL,		// Untransformed and lit vertex
	VID_CA,		// Used for character animation
	VID_3T,		// Three texture coords pairs
	VID_TV		// Like UU but with tangent vector
} YVERTEXID;

//----------------------------------------------------------------------
// Name: YSHADERTYPE
// Desc: Definition of the shader types in a enum
//----------------------------------------------------------------------
typedef enum YSHADER_TYPE
{
	SHT_VERTEX,
	SHT_PIXEL
} YSHADERTYPE;

//----------------------------------------------------------------------
// Name: YDATATYPE
// Desc: Definition of the data types in a enum
//----------------------------------------------------------------------
typedef enum YDATATYPE_TYPE
{
	DAT_BOOL,	// Boolean
	DAT_INT,	// Integer
	DAT_FLOAT,	// Floating point
	DAT_MATRIX, // Matrix
	DAT_TEXTURE,// Texture
	DAT_VECTOR, // Vector
	DAT_OTHER	// Another data type
} YDATATYPE;

//----------------------------------------------------------------------
// Name: YRENDERSTATE
// Desc: Definition of all render states of the Y-Engine
//----------------------------------------------------------------------
typedef enum YRENDERSTATE_TYPE 
{
	RS_NONE,					// Just nothing
	RS_CULL_CW,					// Cull clockwise ordered triangles
	RS_CULL_CCW,				// Cull counter cw ordered triangles
	RS_CULL_NONE,				// Render front- and backsides
	RS_DEPTH_READWRITE,			// Read and write depth buffer
	RS_DEPTH_READONLY,			// Read but don't write depth buffer
	RS_DEPTH_NONE,				// No read or write with depth buffer
	RS_SHADE_POINTS,			// Render just vertices
	RS_SHADE_LINES,				// Render two verts as one line
	RS_SHADE_TRIWIRE,			// Render triangulated wire
	RS_SHADE_HULLWIRE,			// Render poly hull as polyline
	RS_SHADE_SOLID,				// Render solid polygons
	RS_TEX_ADDSIGNED,			// Texture stage operation
	RS_TEX_MODULATE,			// Texture stage operation
	RS_STENCIL_DISABLE,			// Stencilbuffer off
	RS_STENCIL_ENABLE,			// Stencilbuffer on
	RS_STENCIL_FUNC_ALWAYS,		// Stencil pass mode
	RS_STENCIL_FUNC_LESSEQUAL,	// Stencil pass mode
	RS_STENCIL_MASK,			// Stencil mask
	RS_STENCIL_WRITEMASK,		// Stencil write mask
	RS_STENCIL_REF,				// Reference value
	RS_STENCIL_FAIL_DECR,		// Stencil fail decrements
	RS_STENCIL_FAIL_INCR,		// Stencil fail increments
	RS_STENCIL_FAIL_KEEP,		// Stencil fail keeps
	RS_STENCIL_ZFAIL_DECR,		// Stencil pass but z fail decrements
	RS_STENCIL_ZFAIL_INCR,		// Stencil pass but z fail increments
	RS_STENCIL_ZFAIL_KEEP,		// Stencil pass but z fail keeps
	RS_STENCIL_PASS_DECR,		// Stencil and z pass decrements
	RS_STENCIL_PASS_INCR,		// Stencil and z pass increments
	RS_STENCIL_PASS_KEEP,		// Stencil and z pass keeps
	RS_DEPTHBIAS				// Bis value to add to depth value
} YRENDERSTATE;

//----------------------------------------------------------------------
// Name: YLIGHTID
// Desc: Definition of the types of light
//----------------------------------------------------------------------
typedef enum YLIGHTID_TYPE 
{
	LGT_DIRECTIONAL,  // Directional light source
	LGT_POINT,        // Point light source
	LGT_SPOT          // Spot light source
} YLIGHTID;

//----------------------------------------------------------------------
// Name: YLIGHT
// Desc: Definition of a light
//----------------------------------------------------------------------
typedef struct YLIGHT_TYPE 
{
	YLIGHTID   Type;           // Type of light
	YCOLOR     cDiffuse;       // RGBA diffuse light value
	YCOLOR     cSpecular;      // RGBA specular light value
	YCOLOR     cAmbient;       // RGBA ambient light value
	YVector    vcPosition;     // Light position
	YVector	   vcDirection;    // Light direction
	float      fRange;         // Range of light
	float      fTheta;         // Angle of spot light inner cone
	float      fPhi;           // Angle of spot light outer cone
	float      fAttenuation0;  // Change of intensity over distance
	float      fAttenuation1;  // Change of intensity over distance
} YLIGHT;

#endif // Y_H