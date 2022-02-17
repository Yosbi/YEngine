//-------------------------------------------------------------------------------
// YQuat.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: In this file is the definition of the clases, and methods
//		YQuat class
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------

#include "YEMath.h"     

//--------------------------------------------------------------------
// Name: GetMagnitude()
// Desc: Get the mgnitude of a quaternion
//--------------------------------------------------------------------
float YQuat::GetMagnitude(void) {
   return sqrtf(x*x + y*y + z*z + w*w);
}

//--------------------------------------------------------------------
// Name: Normalize()
// Desc: Normalize quaternion
//--------------------------------------------------------------------
void YQuat::Normalize(void) {
   float m=sqrtf(x*x + y*y + z*z + w*w);
   if (m!=0) 
      { x /= m; y /= m; z /= m; w /= m; }
}

//--------------------------------------------------------------------
// Name: Conjugate()
// Desc: 
//--------------------------------------------------------------------
void YQuat::Conjugate(YQuat q) {
   x=-q.x;  y=-q.y;  z=-q.z;  w=q.w;
}

//--------------------------------------------------------------------
// Name: Rotate()
// Desc: 
//--------------------------------------------------------------------
void YQuat::Rotate(const YQuat &q1, const YQuat &q2) { 
   YQuat t = q1*q2*(~q1); 
   x=t.x; y=t.y; z=t.z; w=t.w; 
}

//--------------------------------------------------------------------
// Name: Rotate()
// Desc: 
//--------------------------------------------------------------------
YVector YQuat::Rotate(const YVector &v) { 
   YQuat t(x, y, z, w);
   YQuat r = t*v*(~t);
   return YVector(r.x, r.y, r.z); 
}

//--------------------------------------------------------------------
// Name: GetMatrix()
// Desc: Get a rotation matrix out of the quaternion
//--------------------------------------------------------------------
void YQuat::GetMatrix(YMatrix *pMat) {
   float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

   // set matrix to identity
   memset(pMat, 0, sizeof(YMatrix));
   pMat->_44 = 1.0f;

   x2 = x + x; 
   y2 = y + y; 
   z2 = z + z;

   xx = x * x2;   
   xy = x * y2;  
   xz = x * z2;

   yy = y * y2;   
   yz = y * z2;   
   zz = z * z2;

   wx = w * x2;   
   wy = w * y2;   
   wz = w * z2;

   pMat->_11 = 1.0f - (yy + zz);
   pMat->_12 = xy - wz;
   pMat->_13 = xz + wy;

   pMat->_21 = xy + wz;
   pMat->_22 = 1.0f - (xx + zz);
   pMat->_23 = yz - wx;

   pMat->_31 = xz - wy;
   pMat->_32 = yz + wx;
   pMat->_33 = 1.0f - (xx + yy);
} 

//--------------------------------------------------------------------
// Name: MakeFromEuler()
// Desc: Construct quaternion from Euler angles
//--------------------------------------------------------------------
void YQuat::MakeFromEuler(float fPitch, float fYaw, float fRoll) {
   float cX, cY, cZ, sX, sY, sZ, cYcZ, sYsZ, cYsZ, sYcZ;

   fPitch *= 0.5f;
   fYaw   *= 0.5f;
   fRoll  *= 0.5f;

   cX = cosf(fPitch);
   cY = cosf(fYaw);
   cZ = cosf(fRoll);

   sX = sinf(fPitch);
   sY = sinf(fYaw);
   sZ = sinf(fRoll);

   cYcZ = cY * cZ;
   sYsZ = sY * sZ;
   cYsZ = cY * sZ;
   sYcZ = sY * cZ;

   w = cX * cYcZ + sX * sYsZ;
   x = sX * cYcZ - cX * sYsZ;
   y = cX * sYcZ + sX * cYsZ;
   z = cX * cYsZ - sX * sYcZ;
   } 

//--------------------------------------------------------------------
// Name: GetEulers()
// Desc: Converts quaternion into euler angles
//--------------------------------------------------------------------
void YQuat::GetEulers(float *fPitch, float *fYaw, float *fRoll) {
   double   r11, r21, r31, r32, r33, r12, r13;
   double   q00, q11, q22, q33;
   double   tmp;

   q00 = w * w;
   q11 = x * x;
   q22 = y * y;
   q33 = z * z;

   r11 = q00 + q11 - q22 - q33;
   r21 = 2 * (x*y + w*z);
   r31 = 2 * (x*z - w*y);
   r32 = 2 * (y*z + w*x);
   r33 = q00 - q11 - q22 + q33;

   tmp = fabs(r31);
   if(tmp > 0.999999)
      {
      r12 = 2 * (x*y - w*z);
      r13 = 2 * (x*z + w*y);

      *fPitch = 0.0f;
      *fYaw   = (float)-((YPI/2) * r31/tmp);
      *fRoll  = (float)atan2(-r12, -r31*r13);
      }
   else {
      *fPitch = (float)atan2(r32, r33);
      *fYaw   = (float)asin(-r31);
      *fRoll  = (float)atan2(r21, r11);
      }
} 

//--------------------------------------------------------------------
// Name: operator /= 
// Desc: 
//--------------------------------------------------------------------
void YQuat::operator /= (float f) 
   { w /= f;  x /= f;  y /= f;  z /= f; }

//--------------------------------------------------------------------
// Name: operator /
// Desc: 
//--------------------------------------------------------------------
YQuat YQuat::operator / (float f) {
   return YQuat(x/f, y/f, z/f, w/f);
}

//--------------------------------------------------------------------
// Name: operator *=
// Desc: Multiplies a quaternion by a scalar value
//--------------------------------------------------------------------
void YQuat::operator *= (float f) 
   { w *= f;  x *= f;  y *= f;  z *= f; }

//--------------------------------------------------------------------
// Name: operator *
// Desc: Multiplies a quaternion by a scalar value
//--------------------------------------------------------------------
YQuat YQuat::operator * (float f) {
   return YQuat(x*f, y*f, z*f, w*f);
   }

//--------------------------------------------------------------------
// Name: operator +=
// Desc: Adds two quaternions
//--------------------------------------------------------------------
void YQuat::operator += (const YQuat &q) 
   { w += q.w;  x += q.x;  y += q.y;  z += q.z; }

YQuat YQuat::operator + (const YQuat &q) const {
   return YQuat(x+q.x, y+q.y, z+q.z, w+q.w);
}


//--------------------------------------------------------------------
// Name: operator *=
// Desc: multiplies two quaternions with *= operator
//--------------------------------------------------------------------
// multiplies two quaternions with *= operator
void YQuat::operator *= (const YQuat &q) {
   float _x, _y, _z, _w;

   _w = w*q.w - x*q.x - y*q.y - z*q.z;
   _x = w*q.x + x*q.w + y*q.z - z*q.y;
   _y = w*q.y + y*q.w + z*q.x - x*q.z;
   _z = w*q.z + z*q.w + x*q.y - y*q.x;

   x = _x;
   y = _y;
   z = _z;
   w = _w;
}

//--------------------------------------------------------------------
// Name: operator *
// Desc: Multiplies two quaternions with *= operator
// multiplies two quaternions with * operator
//--------------------------------------------------------------------
YQuat YQuat::operator * (const YQuat &q) const {
   YQuat qResult;
   
   qResult.w = w*q.w - x*q.x - y*q.y - z*q.z;
   qResult.x = w*q.x + x*q.w + y*q.z - z*q.y;
   qResult.y = w*q.y + y*q.w + z*q.x - x*q.z;
   qResult.z = w*q.z + z*q.w + x*q.y - y*q.x;

   return qResult;
}

//--------------------------------------------------------------------
// Name: operator *
// Desc: Multiplies a vector with a quaternion
//--------------------------------------------------------------------
YQuat YQuat::operator * (const YVector &v) const {
   return YQuat(  w*v.x + y*v.z - z*v.y,
                    w*v.y + z*v.x - x*v.z,
                    w*v.z + x*v.y - y*v.x,
                  -(x*v.x + y*v.y + z*v.z) );
}




