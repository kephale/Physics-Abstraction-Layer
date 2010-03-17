#ifndef PALMATH_H
#define PALMATH_H
//(c) 2004 Adrian Boeing, Some code based from Mesa3d (C) 1999-2003  Brian Paul

/*
	Abstract:
		PAL Maths	-	Physics Abstraction Layer.
						Very basic maths utilities
						Meant only to give some very basic functionality for certain limited physics packages
	Author:
		Adrian Boeing
	Revision History:
		Version 0.21: 10/07/08 vec_q_mul
		Version 0.2 : 23/10/07 palQuaternion
		Version 0.19: 22/06/07 Transpose
		Version 0.18: 22/11/06 Translation & orientation set
		Version 0.17: 03/12/04 Vector vector mul
		Version 0.16: 05/09/04 Vector const mul
		Version 0.15: 19/08/04 Matrix invert
		Version 0.14: 05/07/04 Matrix add,sub
		Version 0.13: 24/06/04 Matrix identity
		Version 0.12: 23/06/04 Vector dot, mat mul
		Version 0.11: 22/06/04 Vector mag & norm
		Version 0.1 : 10/06/04
	TODO:
		-infinity define
*/

#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG2RAD (M_PI/180.0)
#define RAD2DEG (180.0/M_PI)

typedef float Float;
//typedef double Float;
//need a define for infinity!
//#define MAX_FLOAT 3.402823466e+38

#define FLOAT_EPSILON  1.192092896e-07F  /* smallest such that 1.0+FLT_EPSILON != 1.0 */

struct palVector3 {
	static const unsigned int num_components = 3;

	palVector3(Float X = 0.0, Float Y = 0.0, Float Z = 0.0)
   : x(X)
   , y(Y)
   , z(Z)
	{
	}

	palVector3(const palVector3& toCopy)
        : x(toCopy.x), y(toCopy.y), z(toCopy.z) {
	}

	palVector3& operator=(const palVector3& toCopy) {
		if (this != &toCopy) {
			x = toCopy.x;
			y = toCopy.y;
			z = toCopy.z;
		}
		return *this;
	}

	union
	{
		struct
		{
			Float x, y, z;
		};
		Float _vec[3];
	};
	Float operator[] (size_t idx) const { return _vec[idx]; }
	Float& operator[] (size_t idx) { return _vec[idx]; }
};

struct palVector4 {
	static const unsigned int num_components = 4;

	palVector4(Float X = 0.0, Float Y = 0.0, Float Z = 0.0, Float W = 0.0)
	: x(X)
	, y(Y)
	, z(Z)
	, w(W)
	, n(*((palVector3*)this))
	{
	}

	palVector4(const palVector4& toCopy)
	: x(toCopy.x),
          y(toCopy.y),
          z(toCopy.z),
          w(toCopy.w),
          n(*((palVector3*)this))
	{
	}

	palVector4& operator=(const palVector4& toCopy) {
		if (this != &toCopy) {
			x = toCopy.x;
			y = toCopy.y;
			z = toCopy.z;
			w = toCopy.w;
		}
		return *this;
	}

	union
	{
		struct {
			Float x, y, z, w;
		};
		Float _q[4];
	};

	palVector3& n;

	Float operator[] (size_t idx) const { return _q[idx]; }
	Float& operator[] (size_t idx) { return _q[idx]; }
};

typedef palVector4 palQuaternion;
typedef palVector4 palPlane;

/*
typedef union {
	struct {
		Float _11, _12, _13;
		Float _21, _22, _23;
		Float _31, _32, _33;
	}
	Float _mat[3*3];
} palMatrix3x3;
*/
typedef union {
	struct {
		Float _11, _12, _13, _14;
		Float _21, _22, _23, _24;
		Float _31, _32, _33, _34;
		Float _41, _42, _43, _44;
	};
	Float _mat[4*4];
} palMatrix4x4;

extern Float clamp_angle(Float angle);
extern Float diff_angle(Float a, Float b);

/// Sets the value of a vector pointed to by v with the floating point values x, y, and z.
extern void  vec_set(palVector3 *v, Float x, Float y, Float z);
/// Squared magnitude/length of a vector (to avoid the square root)
extern Float vec_mag2(const palVector3 *v );
/// length of a vector.
extern Float vec_mag(const palVector3 *v);
/// Normalizes a vector.
extern void  vec_norm(palVector3 *v);
/// @return the dot product of two vectors.
extern Float vec_dot(const palVector3 *a, const palVector3 *b);
/// takes the cross product of a and b and stores it in the vector pointed to by v.
extern void  vec_cross(palVector3 *v, const palVector3 *a, const palVector3 *b);

extern void vec_add(palVector3 *v, const palVector3 *a, const palVector3 *b); //v=a+b;
extern void vec_sub(palVector3 *v, const palVector3 *a, const palVector3 *b); //v=a-b;
extern void vec_mul(palVector3 *v, const Float a);
extern void vec_mul(palVector4 *v, const Float a);
extern void vec_vec_mul(palVector3 *v, const palVector3 *a, const palVector3 *b); //v=a*b
extern void vec_mat_mul(palVector3 *v, const palMatrix4x4 *a, const palVector3 *b); //v=a*b
extern void vec_mat_transform(palVector3 *v, const palMatrix4x4 *a, const palVector3 *b); //v=basis(a)*b+origin(a)
extern void vec_const_mul(palVector3 *v, const palVector3 *a, const Float mult);

extern void plane_normalize(palPlane *p);
extern void plane_transform(palPlane *p,const palMatrix4x4 *a, const palPlane *b);
extern void plane_create(palPlane *p, const palVector3 *normal, const palVector3 *point);
extern void plane_create(palPlane *p, const palVector3 *v1, const palVector3 *v2, const palVector3 *v3);
extern Float plane_distance(const palPlane *plane, const palVector3 *point);

extern bool mat_is_identity(palMatrix4x4 *m);
extern void mat_identity(palMatrix4x4 *m);
extern void mat_add(palMatrix4x4 *m, const palMatrix4x4 *a, const palMatrix4x4 *b); //v=a+b
extern void mat_sub(palMatrix4x4 *m, const palMatrix4x4 *a, const palMatrix4x4 *b); //v=a-b

extern void mat_transpose( palMatrix4x4 *dest, const palMatrix4x4 *src );
extern void mat_scale(palMatrix4x4 *m, Float sx, Float sy, Float sz);
extern void mat_scale3x3(palMatrix4x4 *m, const palVector3 *s);
extern void mat_set_translation(palMatrix4x4 *m, Float x, Float y, Float z); //sets _41,_42,_43
extern void mat_get_translation(const palMatrix4x4 *m, palVector3 *v);
extern void mat_get_column(const palMatrix4x4 *m, palVector3 *v, const int col);
extern void mat_get_row(const palMatrix4x4 *m, palVector3 *v, const int row);
//from irrlicht:
extern void mat_set_rotation(palMatrix4x4 *m, Float rotX, Float rotY, Float rotZ);
extern void mat_get_rotation(palMatrix4x4 *m, Float *protX, Float *protY, Float *protZ);
//from mesa:
extern void mat_multiply( palMatrix4x4 *m, const palMatrix4x4 *a, const palMatrix4x4 *b );
extern void mat_rotate( palMatrix4x4 *m, Float angle, Float x, Float y, Float z);
extern void mat_translate( palMatrix4x4 *m, Float x, Float y, Float z);
extern bool mat_invert( palMatrix4x4 *dest, const palMatrix4x4 *src );
//based from bullet:
extern void q_set(palQuaternion *q, Float x, Float y, Float z, Float w);
extern void vec_q_mul(palQuaternion *q, const palVector3 *a, const palQuaternion *b);
extern void q_vec_mul(palQuaternion *q, const palQuaternion *a, const palVector3 *b);
extern void q_q_mul(palQuaternion *q, const palQuaternion *a, const palQuaternion *b);
extern void q_inverse(palQuaternion *q);
extern void q_shortestArc(palQuaternion *q, const palVector3 *a, const palVector3 *b);
extern void vec_q_rotate(palVector3 *v, const palQuaternion *a, const palVector3 *b);

extern void printPalQuaternion(palQuaternion &src);
//from thorsten
extern void printPalVector(palVector3 &src);
extern void printPalMatrix(palMatrix4x4 & src);
extern void rotx( palMatrix4x4 *m, float t );
extern void roty( palMatrix4x4 *m, float t );
extern void rotz( palMatrix4x4 *m, float t );

//what a waste:
template <typename T> class std_matrix {
public:
	void Resize(int x, int y) {
		mat.resize(y);
		for (unsigned int i=0;i<mat.size();i++)
			mat[i].resize(x);
	}
	T Get(int x, int y) {
		return mat[y][x];
	}
	void Set(int x, int y, T t) {
		mat[y][x]=t;
	}
	void GetDimensions(int &x, int &y) {
		y=(int)mat.size();
		if (y>0)
			x=(int)mat[0].size();
		else
			x=0;
	}
private:
	std::vector<std::vector<T> > mat;
};

#endif
