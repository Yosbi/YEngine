//-------------------------------------------------------------------------------
// YVector.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: In this file is the definition of the clases, and methods
//		YVector class
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include "YEMath.h"


//--------------------------------------------------------------------
// Name: _fabs()
// Desc: Returns a float positive (absolut value)
//--------------------------------------------------------------------
float _fabs(float f) {  if(f < 0.0f) return -f ; return f;}

//--------------------------------------------------------------------
// Name: Set()
// Desc: Sets the vector with the values passed
//--------------------------------------------------------------------
inline void YVector::Set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

//--------------------------------------------------------------------
// Name: GetLength()
// Desc: returns the length of the vector(magnitude)
//--------------------------------------------------------------------
inline float YVector::GetLength()
{
	float f = 0;

	// Put w = 0,0 for the calc
	w = 0,0;
	__asm
	{
		mov		esi,	this		; vector u
		movups	xmm0,	[esi]		; first vextor in xmm0
		mulps   xmm0,	xmm0		; mul with 2nd vector
		movaps  xmm1,	xmm0		; copy result
		shufps	xmm1,	xmm1, 4Eh	; shuffle: f1, f0, f3, f2
		addps	xmm0,	xmm1		; add: f3+f1, f2+f0, f1+f3, f0+f2
		movaps  xmm1,	xmm0		; copy results
		shufps	xmm1,	xmm1, 11h	; shuffle f0+f2, f1+f3, f0+f2, f1+f3
		addps	xmm0,	xmm1		; add x,x,f0+f1+f2+f3, f0+f1+f2+f3
		sqrtss  xmm0,	xmm0		; sqroot from the firs value
		movss	f,		xmm0		; move the result of the first value to f
	}
	w = 1.0f;
	return f;
}

//--------------------------------------------------------------------
// Name: GetSqrLength()
// Desc: returns the length of the vector(magnitude) without sqrt
//--------------------------------------------------------------------
inline float YVector::GetSqrLength() const
{
	return (x*x, y*y, z*z);
}

//--------------------------------------------------------------------
// Name: Negate()
// Desc: returns a vector negated
//--------------------------------------------------------------------
inline void YVector::Negate()
{
	x = -x;
	y = -y;
	z = -z;

}

//--------------------------------------------------------------------
// Name: AngleWith()
// Desc: returns the angle between two vectors
//--------------------------------------------------------------------
inline float YVector::AngleWith(YVector &v)
{
	return (float) acos( ( (*this) * v ) / ( this->GetLength() * v.GetLength() ) );
}

//--------------------------------------------------------------------
// Name: Normalize()
// Desc: Normalize the vector
//--------------------------------------------------------------------
inline void YVector::Normalize()
{
	w = 0.0f;

	__asm
	{
		mov		esi,	this		; vector u
		movups	xmm0,	[esi]		; first vector in xmm0
		movaps	xmm2,	xmm0		; Copy the original vector
		mulps   xmm0,	xmm0		; mul with 2nd vector
		movaps  xmm1,	xmm0		; copy result
		shufps	xmm1,	xmm1, 4Eh	; shuffle: f1, f0, f3, f2
		addps	xmm0,	xmm1		; add: f3+f1, f2+f0, f1+f3, f0+f2
		movaps  xmm1,	xmm0		; copy results
		shufps	xmm1,	xmm1, 11h	; shuffle f0+f2, f1+f3, f0+f2, f1+f3
		addps	xmm0,	xmm1		; add x,x,f0+f1+f2+f3, f0+f1+f2+f3
		
		rsqrtps	xmm0,	xmm0		; recip. sqrt (faster than ss + shufps)
		mulps	xmm2,	xmm0		; mul by reciprocal the original vector
		movups	[esi],	xmm2		; bring back result
	
	}
	w = 1.0f;
}

//--------------------------------------------------------------------
// Name: Difference()
// Desc: Get the vector difference from v1 to v2
//--------------------------------------------------------------------
inline void	YVector::Difference(const YVector &v1, const YVector &v2)
{
	x = v2.x - v1.x;
	y = v2.y - v1.y;
	z = v2.z - v1.z;
	w = 1.0;
}

//--------------------------------------------------------------------
// Name: Cross()
// Desc: Build cross prodict of two vectors.
//--------------------------------------------------------------------
inline void YVector::Cross( const YVector &v1, const YVector &v2)
{
	__asm
	{
		mov		esi,	v1		; moving vectors to the registers
		mov		edi,	v2

		movups	xmm0,	[esi]
		movups	xmm1,	[edi]
		movaps	xmm2,	xmm0
		movaps	xmm3,	xmm1

		shufps	xmm0,	xmm0,	0xC9
		shufps	xmm1,	xmm1,	0xD2
		mulps	xmm0,	xmm1

		shufps	xmm2,	xmm2,	0xD2
		shufps	xmm3,	xmm3,	0xC9
		mulps	xmm2,	xmm3

		subps	xmm0,	xmm2

		mov		esi,	this
		movups	[esi],	xmm0
	}
	w = 1.0f;
}

//--------------------------------------------------------------------
// Name: RotateWith()
// Desc: Rotate the vector with the given matrix, ignoring translation
//		 information in matrix if they are present
//--------------------------------------------------------------------
inline void YVector::RotateWith(const YMatrix &m) 
{
	// Applying rotational part of matrix only
	float _x = x * m._11 + y * m._21 + z * m._31;
	float _y = x * m._12 + y * m._22 + z * m._32;
	float _z = x * m._13 + y * m._23 + z * m._33;
	x = _x;   y = _y;   z = _z;
}

//--------------------------------------------------------------------
// Name: InvRotateWith()
// Desc: Rotate the vector with the inverse rotation part of the given
//		 matrix, ignoring translation of the matrix if present.
//--------------------------------------------------------------------
inline void YVector::InvRotateWith(const YMatrix &m) 
{

	// Using transposed matrix
	float _x = x * m._11 + y * m._12 + z * m._13;
	float _y = x * m._21 + y * m._22 + z * m._23;
	float _z = x * m._31 + y * m._32 + z * m._33;
	x = _x;   y = _y;   z = _z;
} 

//--------------------------------------------------------------------
// Name: InvRotateWith()
// Desc: Add two vectors together. Note this is faster than '+' due to lack
//		 of additional constructor and return.
//--------------------------------------------------------------------
void YVector::operator += (const YVector &v) 
{
	x += v.x;   y += v.y;   z += v.z;
}

//--------------------------------------------------------------------
// Name: InvRotateWith()
// Desc: Add two vectors together. Note this is slower than '+=' due to 
//		 additional constructor and return.
//--------------------------------------------------------------------


YVector YVector::operator + (const YVector &v) const
{
	return YVector(x+v.x, y+v.y, z+v.z);
}


void YVector::operator -= (const YVector &v) 
{
	x -= v.x;   y -= v.y;   z -= v.z;
}


YVector YVector::operator - (const YVector &v) const 
{
	return YVector(x-v.x, y-v.y, z-v.z);
}


void YVector::operator *= (float f) 
{
	x *= f;   y *= f;   z *= f;
}

void YVector::operator /= (float f) 
{
    x /= f;   y /= f;   z /= f;
}

YVector YVector::operator * (float f) const 
{
   return YVector(x*f, y*f, z*f);
}

YVector YVector::operator / (float f) const 
{
    return YVector(x/f, y/f, z/f);
}

void YVector::operator += (float f) {
    x += f;   y += f;   z += f;
}

void YVector::operator -= (float f) 
{
    x -= f;   y -= f;   z -= f;
}

YVector YVector::operator + (float f) const 
{
    return YVector(x+f, y+f, z+f);
}

YVector YVector::operator - (float f) const 
{
    return YVector(x-f, y-f, z-f);
}


float YVector::operator * (const YVector &v) const 
{
	return (v.x*x + v.y*y + v.z*z);

}


YQuat YVector::operator * (const YQuat &q) const 
{
   return YQuat(  q.w*x + q.z*y - q.y*z,
                    q.w*y + q.x*z - q.z*x,
                    q.w*z + q.y*x - q.x*y,
                  -(q.x*x + q.y*y + q.z*z) );
}


YVector YVector::operator * (const YMatrix &m) const 
{
	YVector vcResult;


	vcResult.x = x*m._11 + y*m._21 + z*m._31 + m._41;
	vcResult.y = x*m._12 + y*m._22 + z*m._32 + m._42;
	vcResult.z = x*m._13 + y*m._23 + z*m._33 + m._43;
	vcResult.w = x*m._14 + y*m._24 + z*m._34 + m._44;
   
	vcResult.x = vcResult.x/vcResult.w;
	vcResult.y = vcResult.y/vcResult.w;
	vcResult.z = vcResult.z/vcResult.w;
	vcResult.w = 1.0f;

   return vcResult;
}




