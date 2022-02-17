//-------------------------------------------------------------------------------
// YEMath.h
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
//Desc: In this file is the definition of the clases, data types and methods
//		of the math that the Y-Engine use, it is part of the static math lib
//		implementing basic 3D math objects. I use c++ only in nessesary cases
//		because today every processor supports SSE
//--------------------------------------------------------------------------

#ifndef YEMATH_H
#define YEMATH_H

//-----------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------

#include <math.h> 
#include <stdio.h>
#include <memory.h>	//memset

//-------------------------------------------------------------------------
// Constants definitions
//--------------------------------------------------------------------------
const double YPI		= 3.14159265;
const double YPI2		= 1.5707963;
const double Y2PI		= 6.2831853;
const float  YG			= -32.174f; // ft/s^2
const float  YEPSILON	= 0.00001F;

//-------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------
#ifndef NULL
#define NULL 0
#endif

#define YFRONT	 0
#define YBACK	 1
#define YPLANAR	 2
#define YCLIPPED 3
#define YCULLED  4
#define YVISIBLE 5

//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------
float  _fabs(float f);
bool	YInitCPU();

//-----------------------------------------------------------------------
// Structs
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// Name: CPUINFO
// Desc: Is used to know the cap of the CPU (instructions sets, etc)
//-----------------------------------------------------------------------
typedef struct _CPUINFO
{
	bool bSSE;			// Streaming SIMD Extensions
	bool bSSE2;			// Streaming SIMD Extensions 2
	bool b3DNOW;		// 3DNow! (vendor independent)
	bool b3DNOWEX;		// 3DNow! (AMD specific extensions)
	bool bMMX;			// MMX support
	bool bMMXEX;		// MMX (AMD spesific extentions)
	bool bEXT;			// Extended features available
	char vendor[13];	// vendor name
	char name[48];		// CPU Name
}CPUINFO;

//-----------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------
class YVector;
class YMatrix;
class YObb;
class YAabb;
class YPlane;
class YQuat;
class YPolygon;

//-----------------------------------------------------------------------
// Classes
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
// Name: YVector
// Desc: Basic 4d vector class
//-----------------------------------------------------------------------
class __declspec(dllexport) YVector
{
public:
	// Costructor and destructor
	YVector() {x = 0, y = 0, z = 0, w = 1.0f;}
	YVector( float _x, float _y, float _z)
		{x = _x; y = _y; z = _z; w = 1.0f;}

	// Functions
	inline void		Set(float _x, float _y, float _z, float _w = 1.0f);		// Set the verctor values
	inline float	GetLength	 ();										// Length (magnitude)
	inline float	GetSqrLength () const;									// Square Length
	inline void		Negate		 ();										// Vector mult -1
	inline void		Normalize	 ();										// Normalize vector
	inline float	AngleWith	 (YVector &v);								// Angle in radians
	inline void		RotateWith	 (const YMatrix &m);						// Apply rotation with matrix
	inline void		InvRotateWith(const YMatrix &m);						// Apply inverse rotation with matrix
	inline void		Difference	 (const YVector &v1, const YVector &v2);	// Diference from v1 to v2

	void	operator += (const YVector &v);									// Operator +=
	void	operator -= (const YVector &v);									// Operator -=
	void	operator *= (float f);											// Scalar mult (scale vector)
	void	operator /= (float f);											// Scale down
	void	operator += (float f);											// Add scalar
	void	operator -= (float f);											// Substract scalar
	float	operator *  (const YVector &v) const;							// Dot product
	YVector operator *  (float f)		   const;							// Scale vector
	YVector operator /  (float f)		   const;							// Scalar divide
	YVector operator +  (float f)		   const;							// Add scalar
	YVector operator -  (float f)		   const;							// Substract scalar
	YQuat	operator *  (const YQuat &q)   const;							// Vector * quaternion
	YVector operator *  (const YMatrix &m) const;							// Vector-matrix product
	YVector operator +  (const YVector &v) const;							// Vector addition
	YVector operator -  (const YVector &v) const;							// Substraction

	inline void Cross(const YVector &v1, const YVector &v2);						// Cross product

	// Variables
	float x, y, z, w;		// Coordinate set

};

//-----------------------------------------------------------------------
// Name: YMatrix
// Desc: Basic matrix class
//-----------------------------------------------------------------------
class __declspec(dllexport) YMatrix {
   public:     

      // Constructor
      YMatrix(void) {}

	  // Functions
      inline void Identity(void);										// identity matrix
      inline void RotaX(float a);										// Rotation matrix arround x axis
      inline void RotaY(float a);										// Rotation matrix arround y axis
      inline void RotaZ(float a);										// Rotation matrix arround z axis
      inline void Rota(const YVector &vc);								// Rotation matrix arround x, y and z
      inline void Rota(float x, float y, float z);						// Rotation matrix arround x, y and z
      inline void RotaArbi(const YVector &vcAxis, float a);				// Rotation arround an arbitrary axis
      inline void ApplyInverseRota(YVector *pvc);						// Inverse rotation
      inline void Translate(float dx, float dy, float dz);				// Translate a certain distance
      inline void SetTranslation(YVector vc, bool EraseContent=false);	// Set translation values
	  inline YVector GetTranslation(void);								// Get the translation value

      inline void Billboard(YVector vcPos, YVector vcDir,
                            YVector vcWorldUp = YVector(0,1,0));		// Set billboard matrix
      inline void LookAt(YVector vcPos, YVector vcLookAt, 
                         YVector vcWorldUp = YVector(0,1,0));

      inline void TransposeOf(const YMatrix &m);						// transpose m, save result in this
      inline void InverseOf(const YMatrix &m);							// invert m, save result in this
      
      YMatrix operator * (const YMatrix &m)const;						// matrix multiplication
      YVector operator * (const YVector &vc)const;						// matrix vector multiplication

	  // Variables
	  float _11, _12, _13, _14;
      float _21, _22, _23, _24;
      float _31, _32, _33, _34;
      float _41, _42, _43, _44;
   };

//-----------------------------------------------------------------------
// Name: YRay
// Desc: Basic ray class
//-----------------------------------------------------------------------
class __declspec(dllexport) YRay {
   public:

	  // Constructor
      YRay(void) {}

	  // Functions
      inline void Set(YVector vcOrig, YVector vcDir);
      inline void DeTransform(const YMatrix &_m);				// Move to matrixspace

      bool Intersects(const YVector &vc0, const YVector &vc1,
                      const YVector &vc2, bool bCull,
                      float *t);
      bool Intersects(const YVector &vc0, const YVector &vc1,
                      const YVector &vc2, bool bCull,
                      float fL, float *t);
      bool Intersects(const YPlane &plane, bool bCull,
                      float *t, YVector *vcHit);       
      bool Intersects(const YPlane &plane, bool bCull,
                      float fL, float *t, YVector *vcHit);
      bool Intersects(const YAabb &aabb, float *t);
      bool Intersects(const YAabb &aabb, float fL, float *t);
      bool Intersects(const YObb &obb, float *t);
      bool Intersects(const YObb &obb, float fL, float *t);

	  // Variables
	   YVector m_vcOrig;  // Ray origin
       YVector m_vcDir;   // Ray direction
   }; 

//-----------------------------------------------------------------------
// Name: YPlane
// Desc: Basic plane class
//-----------------------------------------------------------------------
class __declspec(dllexport) YPlane {
   public:
     // Constructor
      YPlane(void) {}

	  // Functions
      inline void  Set(const YVector &vcN, const YVector &vcP);
      inline void  Set(const YVector &vcN, const YVector &vcP, float fD);
      inline void  Set(const YVector &v0,  const YVector &v1, const YVector &v2);
      inline float Distance(const YVector &vcPoint);
      inline int   Classify(const YVector &vcPoint);
      inline int   Classify(const YPolygon &Polygon);

      bool		   Clip(const YRay*, float, YRay*, YRay*);
      bool		   Intersects(const YVector &vc0, const YVector &vc1, const YVector &vc2);
      bool		   Intersects(const YPlane &plane, YRay *pIntersection);
      bool		   Intersects(const YAabb &aabb);
      bool		   Intersects(const YObb &obb);

	  // Variables
	  YVector m_vcN;       // Plane normal vector
      YVector m_vcPoint;   // Point on plane
      float   m_fD;        // Distance to origin

   };

//-----------------------------------------------------------------------
// Name: YObb
// Desc: Basic oriented bounding box class
//-----------------------------------------------------------------------
class __declspec(dllexport) YObb {
   public:
      // Constructor
      YObb(void) {}

	  // Functions 
      inline void DeTransform(const YObb &obb, const YMatrix &m);

      bool Intersects(const YRay &Ray, float *t);
      bool Intersects(const YRay &Ray, float fL, float *t);
      bool Intersects(const YObb &Obb);
      bool Intersects(const YVector &v0, const YVector &v1, const YVector &v2);
      int  Cull		 (const YPlane *pPlanes, int nNumPlanes);      

   private:
      void ObbProj(const YObb &Obb, const YVector &vcV, float *pfMin, float *pfMax);
      void TriProj(const YVector &v0, const YVector &v1, 
                   const YVector &v2, const YVector &vcV, 
                   float *pfMin, float *pfMax);

	  // Variables
	public:
	  float   fA0,   fA1,  fA2; // half axis length
      YVector vcA0, vcA1, vcA2; // box axis
      YVector vcCenter;         // centerpoint
   };

//-----------------------------------------------------------------------
// Name: YAabb
// Desc: Basic axis aligned bounding box class
//-----------------------------------------------------------------------
class __declspec(dllexport) YAabb {
   public:
     
      // Constructor
      YAabb(void) {}

	  // Functions
      YAabb(YVector vcMin, YVector vcMax);

      void Construct(const YObb *pObb);                  // build from obb
      int  Cull(const YPlane *pPlanes, int nNumPlanes);  

      // normals pointing outwards
      void GetPlanes(YPlane *pPlanes);

      bool Contains	 (const YRay &Ray, float fL);
      bool Intersects(const YRay &Ray, float *t);
      bool Intersects(const YRay &Ray, float fL, float *t);
      bool Intersects(const YAabb &aabb);
      bool Intersects(const YVector &vc0);

	  // Variables
	  YVector vcMin, vcMax; // box extreme points
      YVector vcCenter;     // centerpoint

   };

//-----------------------------------------------------------------------
// Name: YPolygon
// Desc: Basic polygon class
//-----------------------------------------------------------------------
class __declspec(dllexport) YPolygon {
   friend class ZFXPlane;        // access for easier classifying

   public:
	  // Constructor/Destructor
      YPolygon(void);
      ~YPolygon(void);

	  // Functions    

      void          Set(const YVector *pPoints, int nNumP,
                        const unsigned int *pIndis, int nNumI);

      void          Clip(const YPlane &Plane, YPolygon *pFront, YPolygon *pBack);
      void          Clip(const YAabb &aabb);
      int           Cull(const YAabb &aabb);

      void          CopyOf( const YPolygon &Poly );

      void          SwapFaces(void);

      bool          Intersects(const YRay &Ray, bool, float *t);
      bool          Intersects(const YRay &Ray, bool, float fL, float *t);

      int           GetNumPoints(void)      { return m_NumP;    }
      int           GetNumIndis(void)       { return m_NumI;    }
      YVector*      GetPoints(void)         { return m_pPoints; }
      unsigned int* GetIndices(void)        { return m_pIndis;  }
      YPlane        GetPlane(void)          { return m_Plane;   }
      YAabb         GetAabb(void)           { return m_Aabb;    }
      unsigned int  GetFlag(void)           { return m_Flag;    }
      void          SetFlag(unsigned int n) { m_Flag = n;       }

      // DEBUG ONLY
      void			Print(FILE*);
	  void			CalcBoundingBox(void);

	  // Variables
   private:
      YPlane         m_Plane;    // plane which poly lies in

      int            m_NumP;     // number of points
      int            m_NumI;     // number of indices
      YAabb          m_Aabb;     // bounding box
      unsigned int   m_Flag;     // whatever you want it to be
	  
   public:
	  YVector      *m_pPoints;  // list of points
      unsigned int *m_pIndis;   // index list

      
   }; 

//-----------------------------------------------------------------------
// Name: YOPolygon
// Desc: Basic quaternion class
//-----------------------------------------------------------------------
class __declspec(dllexport) YQuat {
   public:
	  // Constructors
      YQuat(void) { x=0.0f, y=0.0f, z=0.0f, w=1.0f; }
      YQuat(float _x, float _y, float _z, float _w)
         { x=_x; y=_y; z=_z; w=_w; }

	  // Functions
      void  MakeFromEuler(float fPitch, float fYaw, float fRoll);
      void  Normalize();
      void  Conjugate(YQuat q);
      void  GetEulers(float *fPitch, float *fYaw, float *fRoll);
      void  GetMatrix(YMatrix *m);
      float GetMagnitude(void);


      void  operator /= (float f);
      YQuat operator /  (float f);

      void  operator *= (float f);
      YQuat operator *  (float f);

      YQuat operator *  (const YVector &v) const;

      YQuat operator *  (const YQuat &q) const;
      void  operator *= (const YQuat &q);

      void  operator += (const YQuat &q);
      YQuat operator +  (const YQuat &q) const;

      YQuat operator~(void) const { return YQuat(-x, -y, -z, w); }

      void  Rotate(const YQuat &q1, const YQuat &q2);

      YVector Rotate(const YVector &v);

	  // Variables
	  float x, y, z, w;
   }; 

#endif //YEMATH_H