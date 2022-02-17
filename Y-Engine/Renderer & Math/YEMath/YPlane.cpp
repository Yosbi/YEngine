//-------------------------------------------------------------------------------
// YPlane.cpp
// Yosbi Alves
// yosbito@gmail.com
// Copyright (c) 2011
//-----------------------------------------------------------------------
// Desc: In this file is the definition of the clases, and methods
//		YPlane class
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------

#include "YEMath.h"     

//--------------------------------------------------------------------
// Name: Set()
// Desc: Set attributs
//--------------------------------------------------------------------
inline void YPlane::Set(const YVector &vcN, const YVector &vcP) {
   m_fD      = - ( vcN * vcP);
   m_vcN     = vcN;
   m_vcPoint = vcP;
}

//--------------------------------------------------------------------
// Name: Set()
// Desc: Set attributs
//--------------------------------------------------------------------
inline void YPlane::Set(const YVector &vcN, const YVector &vcP,
                          float fD) {
   m_vcN     = vcN;
   m_fD      = fD;
   m_vcPoint = vcP;
}

//--------------------------------------------------------------------
// Name: Set()
// Desc: Set attributs
//--------------------------------------------------------------------
inline void YPlane::Set(const YVector &v0, const YVector &v1, 
                          const YVector &v2) {
   YVector vcEdge1 = v1 - v0;
   YVector vcEdge2 = v2 - v0;

   m_vcN.Cross(vcEdge1, vcEdge2);
   m_vcN.Normalize();
   m_fD = - (m_vcN * v0);
   m_vcPoint = v0;
}

//--------------------------------------------------------------------
// Name: Distance()
// Desc: Calculate distance to point. Plane normal must be normalized.
//--------------------------------------------------------------------
inline float YPlane::Distance(const YVector &vcPoint) {
   return ( _fabs((m_vcN*vcPoint) - m_fD) );
}

//--------------------------------------------------------------------
// Name: Classify()
// Desc: Classify point to plane.
//-------------------------------------------------------------------- 
inline int YPlane::Classify(const YVector &vcPoint) {
   float f = (vcPoint * m_vcN) + m_fD;
   
   if (f >  0.00001) return YFRONT;
   if (f < -0.00001) return YBACK;
   return YPLANAR;
}

//--------------------------------------------------------------------
// Name: Classify()
// Desc: Classify polygon with respect to this plane
//-------------------------------------------------------------------- 
int YPlane::Classify(const YPolygon &Poly) {
   int NumFront=0, NumBack=0, NumPlanar=0;
   int nClass;

   // cast away const
   YPolygon *pPoly = ((YPolygon*)&Poly);

   int NumPoints = pPoly->GetNumPoints();

   // loop through all points
   for (int i=0; i < NumPoints; i++) {
      nClass = Classify( pPoly->m_pPoints[i] );
      
      if (nClass == YFRONT)     NumFront++;
      else if (nClass == YBACK) NumBack++;
      else {
         NumFront++;
         NumBack++;
         NumPlanar++;
         }
      } // for
   
   // all points are planar
   if (NumPlanar == NumPoints)
      return YPLANAR;
   // all points are in front of plane
   else if (NumFront == NumPoints)
      return YFRONT;
   // all points are on backside of plane
   else if (NumBack == NumPoints)
      return YBACK;
   // poly is intersecting the plane
   else
      return YCLIPPED;
} 

//--------------------------------------------------------------------
// Name: Clip()
// Desc: Clips a ray into two segments if it intersects the plane
//-------------------------------------------------------------------- 
bool YPlane::Clip(const YRay *_pRay, float fL, YRay *pF, YRay *pB) {

   YVector vcHit(0.0f,0.0f,0.0f);
   
   YRay *pRay = (YRay*)_pRay;

   // ray intersects plane at all?
   if ( !pRay->Intersects( *this, false, fL, NULL, &vcHit) ) 
      return false;

   int n = Classify( _pRay->m_vcOrig );

   // ray comes fron planes backside
   if ( n == YBACK ) {
      if (pB) pB->Set(pRay->m_vcOrig, pRay->m_vcDir);
      if (pF) pF->Set(vcHit, pRay->m_vcDir);
      }
   // ray comes from planes front side
   else if ( n == YFRONT ) {
      if (pF) pF->Set(pRay->m_vcOrig, pRay->m_vcDir);
      if (pB) pB->Set(vcHit, pRay->m_vcDir);
      }

   return true;
}

//--------------------------------------------------------------------
// Name: Clip()
// Desc: Intersection of two planes. If third parameter is given the line
//		of intersection will be calculated. (www.magic-software.com)
//-------------------------------------------------------------------- 
bool YPlane::Intersects(const YPlane &plane, YRay *pIntersection) {
   YVector vcCross;
   float     fSqrLength;
   
   // if crossproduct of normals 0 than planes parallel
   vcCross.Cross(this->m_vcN, plane.m_vcN);
   fSqrLength = vcCross.GetSqrLength();

   if (fSqrLength < 1e-08f) 
      return false;

   // find line of intersection
   if (pIntersection) {
      float fN00 = this->m_vcN.GetSqrLength();
      float fN01 = this->m_vcN * plane.m_vcN;
      float fN11 = plane.m_vcN.GetSqrLength();
      float fDet = fN00*fN11 - fN01*fN01;

      if (_fabs(fDet) < 1e-08f) 
         return false;

      float fInvDet = 1.0f/fDet;
      float fC0 = (fN11*this->m_fD - fN01*plane.m_fD) * fInvDet;
      float fC1 = (fN00*plane.m_fD - fN01*this->m_fD) * fInvDet;

      (*pIntersection).m_vcDir  = vcCross;
      (*pIntersection).m_vcOrig = this->m_vcN*fC0 + plane.m_vcN*fC1;
      }

   return true;
} 

//--------------------------------------------------------------------
// Name: Clip()
// Desc: Intersection of a plane with a triangle. If all vertices of the
//		triangle are on the same side of the plane, no intersection occured. 
//-------------------------------------------------------------------- 
bool YPlane::Intersects(const YVector &vc0, const YVector &vc1, 
                          const YVector &vc2) {
   int n = this->Classify(vc0);

   if ( (n == this->Classify(vc1)) && 
        (n == this->Classify(vc2)) )
      return false;
   return true;
}

//--------------------------------------------------------------------
// Name: Clip()
// Desc: Intersection with AABB. Search for AABB diagonal that is most
//		aligned to plane normal. Test its two vertices against plane.
//		(Möller/Haines, "Real-Time Rendering")
//--------------------------------------------------------------------  
bool YPlane::Intersects(const YAabb &aabb) {
   YVector Vmin, Vmax;

   // x component
   if (m_vcN.x >= 0.0f) {
      Vmin.x = aabb.vcMin.x;
      Vmax.x = aabb.vcMax.x;
      }
   else {
      Vmin.x = aabb.vcMax.x;
      Vmax.x = aabb.vcMin.x;
      }

   // y component
   if (m_vcN.y >= 0.0f) {
      Vmin.y = aabb.vcMin.y;
      Vmax.y = aabb.vcMax.y;
      }
   else {
      Vmin.y = aabb.vcMax.y;
      Vmax.y = aabb.vcMin.y;
      }
   
   // z component
   if (m_vcN.z >= 0.0f) {
      Vmin.z = aabb.vcMin.z;
      Vmax.z = aabb.vcMax.z;
      }
   else {
      Vmin.z = aabb.vcMax.z;
      Vmax.z = aabb.vcMin.z;
      }

   if ( ((m_vcN * Vmin) + m_fD) > 0.0f)
      return false;
   
   if ( ((m_vcN * Vmax) + m_fD) >= 0.0f)
      return true;
  
   return false;
   } 

//--------------------------------------------------------------------
// Name: Clip()
// Desc: Intersection with OBB. Same as obb culling to frustrum planes.
//-------------------------------------------------------------------- 
bool YPlane::Intersects(const YObb &obb) {
    float fRadius = _fabs( obb.fA0 * (m_vcN * obb.vcA0) ) 
                  + _fabs( obb.fA1 * (m_vcN * obb.vcA1) ) 
                  + _fabs( obb.fA2 * (m_vcN * obb.vcA2) );

    float fDistance = this->Distance(obb.vcCenter);
    return (fDistance <= fRadius);
}
