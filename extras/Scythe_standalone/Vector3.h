#pragma once
/*
*	Scythes basic Vector classes. Designed to load phs models and just stay out of your way.
*/
#include <math.h>
#include <cstddef>
#include <cstdlib>

namespace Scythe
{
class  Vector3;

typedef float Real;

class Vector3
{
public:
	Vector3(): x(0),y(0),z(0) {}
	Vector3(float* v): x(v[0]),y(v[1]),z(v[2]) {}

	Vector3(float a, float b, float c) {
		x = a;
		y = b;
		z = c;
	}

	const Vector3& operator=(const Vector3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	float& operator[](int i) { return (&x)[i]; }
	float  operator[](int i) const { return (&x)[i]; }
  
	Vector3 operator -() const { return Vector3(-x, -y, -z); }

	Vector3 operator +(const Vector3 & v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
 
	Vector3 operator -(const Vector3 & v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator *(float f) const {
		return Vector3(x * f, y * f, z * f);
	}

	Vector3& operator +=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& operator *=(const Vector3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector3& operator -=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	float getLength() const {
		return sqrt(x * x + y * y + z * z);
	}

	void crossProduct(const Vector3 &l, const Vector3 & r)
	{
		float a = (l.y * r.z) - (l.z * r.y);
		float b = (l.z * r.x) - (l.x * r.z);
		float c = (l.x * r.y) - (l.y * r.x);
		x = a;
		y = b;
		z = c;
	}

	void normalise()
	{
		float length = getLength();
		float f = 1.0f / length;
		x *= f;
		y *= f;
		z *= f;
	}

	float getLength()
	{
		return sqrt(x * x + y * y + z * z);
	}
 
	float x, y, z;

	float* array()
	{
		static float v[3];
		v[0]=x;
		v[1]=y;
		v[2]=z;
		return v;
	}
};

class Quat
{
public:
	Quat(){}
	Quat(float W, float X, float Y, float Z)
		: x(X), y(Y), z(Z), w(W)
	{}
	
	float w,x,y,z;

	Vector3 operator* (const Vector3& v) const
    {
		//  borrowed from Ogre ;)
		// nVidia SDK implementation
		Vector3 uv, uuv;
		Vector3 qvec(x, y, z);
		uv.crossProduct(qvec, v);
		uuv.crossProduct(qvec, uv);
		uv = uv * (2.0f * w);
		uuv = uuv * 2.0f;

		return v + uv + uuv;
    }
    Quat operator* (const Quat& rkQ) const
    {
		//  borrowed from Ogre ;)
        return Quat
        (
            w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
            w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
            w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
            w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
        );
    }
	Quat invert () const
    {
        float fNorm = w*w+x*x+y*y+z*z;
        if ( fNorm > 0.0 )
        {
            float fInvNorm = 1.0/fNorm;
            return Quat(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
        }
    }

};

class Matrix
{
public:
	Matrix()
	{
		m[0][0] = 1.0f;
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;

		m[1][0] = 0.0f;
		m[1][1] = 1.0f;
		m[1][2] = 0.0f;

		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = 1.0f;
	}
	Matrix(const float* v)
	{
		m[0][0] = v[0];
		m[0][1] = v[1];
		m[0][2] = v[2];

		m[1][0] = v[3];
		m[1][1] = v[4];
		m[1][2] = v[5];

		m[2][0] = v[6];
		m[2][1] = v[7];
		m[2][2] = v[8];
	}
    
	Matrix operator* (const Matrix& rkMatrix) const
    {
        Matrix kProd;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
            {
                kProd.m[iRow][iCol] =
                    m[iRow][0]*rkMatrix.m[0][iCol] +
                    m[iRow][1]*rkMatrix.m[1][iCol] +
                    m[iRow][2]*rkMatrix.m[2][iCol];
            }
        }
        return kProd;
    }
    Vector3 operator* (const Vector3& rkPoint) const
    {
        Vector3 kProd;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            kProd[iRow] =
                m[iRow][0]*rkPoint[0] +
                m[iRow][1]*rkPoint[1] +
                m[iRow][2]*rkPoint[2];
        }
        return kProd;
    }

   Matrix inverse () const
    {
        Matrix kInverse;
        Inverse(kInverse,0.0001f);
        return kInverse;
    }



private:
	bool Inverse (Matrix& rkInverse, Real fTolerance) const
    {
        // Invert a 3x3 using cofactors.  This is about 8 times faster than
        // the Numerical Recipes code which uses Gaussian elimination.

        rkInverse.m[0][0] = m[1][1]*m[2][2] -
            m[1][2]*m[2][1];
        rkInverse.m[0][1] = m[0][2]*m[2][1] -
            m[0][1]*m[2][2];
        rkInverse.m[0][2] = m[0][1]*m[1][2] -
            m[0][2]*m[1][1];
        rkInverse.m[1][0] = m[1][2]*m[2][0] -
            m[1][0]*m[2][2];
        rkInverse.m[1][1] = m[0][0]*m[2][2] -
            m[0][2]*m[2][0];
        rkInverse.m[1][2] = m[0][2]*m[1][0] -
            m[0][0]*m[1][2];
        rkInverse.m[2][0] = m[1][0]*m[2][1] -
            m[1][1]*m[2][0];
        rkInverse.m[2][1] = m[0][1]*m[2][0] -
            m[0][0]*m[2][1];
        rkInverse.m[2][2] = m[0][0]*m[1][1] -
            m[0][1]*m[1][0];

        Real fDet =
            m[0][0]*rkInverse.m[0][0] +
            m[0][1]*rkInverse.m[1][0]+
            m[0][2]*rkInverse.m[2][0];

        if ( abs(fDet) <= fTolerance )
            return false;

        Real fInvDet = 1.0/fDet;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
                rkInverse.m[iRow][iCol] *= fInvDet;
        }

        return true;
    }
public:

    Quat toQuat ()
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".
	Quat q;
        float fTrace = m[0][0]+m[1][1]+m[2][2];
        float fRoot;

        if ( fTrace > 0.0 )
        {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = sqrt(fTrace + 1.0f);  // 2w
            q.w = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;  // 1/(4w)
            q.x = (m[2][1]-m[1][2])*fRoot;
            q.y = (m[0][2]-m[2][0])*fRoot;
            q.z = (m[1][0]-m[0][1])*fRoot;
        }
        else
        {
            // |w| <= 1/2
            static int s_iNext[3] = { 1, 2, 0 };
            int i = 0;
			if ( m[1][1] > m[0][0] )
                i = 1;
			if ( m[2][2] > m[i][i] )
                i = 2;
            int j = s_iNext[i];
            int k = s_iNext[j];

			fRoot = sqrt(m[i][i]-m[j][j]-m[k][k] + 1.0f);
            float* apkQuat[3] = { &q.x, &q.y, &q.z };
            *apkQuat[i] = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;
			q.w = (m[k][j]-m[j][k])*fRoot;
			*apkQuat[j] = (m[j][i]+m[i][j])*fRoot;
			*apkQuat[k] = (m[k][i]+m[i][k])*fRoot;
        }
		return q;
    }
	void fromQuat(const Quat q)
    {
	    double sqw = q.w*q.w;
	    double sqx = q.x*q.x;
	    double sqy = q.y*q.y;
	    double sqz = q.z*q.z;

	    // invs (inverse square length) is only required if quaternion is not already normalised
	    double invs = 1;/// / (sqx + sqy + sqz + sqw);
		    m[0][0] = ( sqx - sqy - sqz + sqw)*invs ; // since sqw + sqx + sqy + sqz =1/invs*invs
	    m[1][1] = (-sqx + sqy - sqz + sqw)*invs ;
	    m[2][2] = (-sqx - sqy + sqz + sqw)*invs ;

	    double tmp1 = q.x*q.y;
	    double tmp2 = q.z*q.w;
	    m[1][0] = 2.0 * (tmp1 + tmp2)*invs ;
	    m[0][1] = 2.0 * (tmp1 - tmp2)*invs ;

	    tmp1 = q.x*q.z;
	    tmp2 = q.y*q.w;
	    m[2][0] = 2.0 * (tmp1 - tmp2)*invs ;
	    m[0][2] = 2.0 * (tmp1 + tmp2)*invs ;
	    tmp1 = q.y*q.z;
	    tmp2 = q.x*q.w;
	    m[2][1] = 2.0 * (tmp1 + tmp2)*invs ;
	    m[1][2] = 2.0 * (tmp1 - tmp2)*invs ;     
	    /*
        float fCos = cos(q.w);
        float fSin = sin(q.w);
        float fOneMinusCos = 1.0-fCos;
        float fX2 = q.x*q.x;
        float fY2 = q.y*q.y;
        float fZ2 = q.z*q.z;
        float fXYM = q.x*q.y*fOneMinusCos;
        float fXZM = q.x*q.z*fOneMinusCos;
        float fYZM = q.y*q.z*fOneMinusCos;
        float fXSin = q.x*fSin;
        float fYSin = q.y*fSin;
        float fZSin = q.z*fSin;

        m[0][0] = fX2*fOneMinusCos+fCos;
        m[0][1] = fXYM-fZSin;
        m[0][2] = fXZM+fYSin;
        m[1][0] = fXYM+fZSin;
        m[1][1] = fY2*fOneMinusCos+fCos;
        m[1][2] = fYZM-fXSin;
        m[2][0] = fXZM-fYSin;
        m[2][1] = fYZM+fXSin;
        m[2][2] = fZ2*fOneMinusCos+fCos;
	   */
    }
	Vector3 m[3];

	Matrix operator- (const Matrix& rkMatrix) const
	{
		Matrix kDiff;
		for (size_t iRow = 0; iRow < 3; iRow++)
		{
			for (size_t iCol = 0; iCol < 3; iCol++)
			{
				kDiff.m[iRow][iCol] = m[iRow][iCol] -
					rkMatrix.m[iRow][iCol];
			}
		}
		return kDiff;
	}
};

//Vec3 and Mat33 are the basic types used by scythe, that you will need to define
//here we just typedef to our own math classes
typedef Vector3 Vec3;
typedef Matrix Mat33;
}