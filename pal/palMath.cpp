#include "palMath.h"
#include <stdio.h>
#include <memory.h>
#include <cfloat>
#include <ostream>

//(c) 2004 Adrian Boeing, Some code based from Mesa3d (C) 1999-2003  Brian Paul

Float clamp_angle(Float angle)
{
	 if (angle < -M_PI)
          angle += (Float)(2*M_PI);
     if (angle > M_PI)
          angle -= (Float)(2*M_PI);
     return angle;
}

Float diff_angle(Float a, Float b) {
	return clamp_angle(a-b);
}


void vec_set( palVector3 *v, Float x, Float y, Float z )
{
	if(!v) return;
	v->_vec[0] = x;
	v->_vec[1] = y;
	v->_vec[2] = z;
}

Float vec_mag2(const palVector3 *v ) {
	return v->x*v->x+v->y*v->y+v->z*v->z;
}

Float vec_mag(const palVector3 *v ) {
	return sqrt(vec_mag2(v));
}

void vec_norm(palVector3 *v) {
	Float a=vec_mag(v);
	if (a<0.00001) {
		v->x=0;
		v->y=0;
		v->z=0;
		return;
	}
	a=1/a;
	v->x*=a;
	v->y*=a;
	v->z*=a;
}

Float vec_dot(const palVector3 *a, const palVector3 *b) {
	return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

void vec_add(palVector3 *v, const palVector3 *a, const palVector3 *b) {
	v->x=a->x + b->x;
	v->y=a->y + b->y;
	v->z=a->z + b->z;
}

void vec_sub(palVector3 *v, const palVector3 *a, const palVector3 *b) {
	v->x=a->x - b->x;
	v->y=a->y - b->y;
	v->z=a->z - b->z;
}

void vec_mul(palVector3 *v, const Float a)
{
	v->x *= a;
	v->y *= a;
	v->z *= a;
}

void vec_mul(palVector4 *v, const Float a)
{
	v->x *= a;
	v->y *= a;
	v->z *= a;
	v->w *= a;
}

void vec_vec_mul(palVector3 *v, const palVector3 *a, const palVector3 *b) {
	v->x=a->x * b->x;
	v->y=a->y * b->y;
	v->z=a->z * b->z;
}

void vec_mat_mul(palVector3 *v, const palMatrix4x4 *a, const palVector3 *b) {
	float *m = (float *) a->_mat;
	v->x = m[0] * b->x + m[4] * b->y + m[8] * b->z;
	v->y = m[1] * b->x + m[5] * b->y + m[9] * b->z;
	v->z = m[2] * b->x + m[6] * b->y + m[10] * b->z;
}

palVector3 palVector3::operator+(const palVector3& b) const {
	palVector3 c;
	c.x = x + b.x;
	c.y = y + b.y;
	c.z = z + b.z;
	return c;
}

palVector3 palVector3::operator/(const Float& f) const {
	palVector3 result(x / f, y / f, z / f);
	return result;
}

palVector3 palVector3::operator-() const {
	palVector3 result(-x, -y, -z);
	return result;
}

void mat_scale(palMatrix4x4 *m, Float sx, Float sy, Float sz) {
	m->_11*=sx;
	m->_22*=sy;
	m->_33*=sz;
}

void mat_scale3x3(palMatrix4x4 *m, const palVector3 *s) {
	for (int j=0;j<3;j++)
		for (int i=0;i<3;i++) {
			m->_mat[i+j*4] *= s->_vec[i];
	}
}

void vec_mat_transform(palVector3 *v, const palMatrix4x4 *a, const palVector3 *b) {
	palVector3 x;
	vec_mat_mul(v,a,b);
	mat_get_translation(a,&x);
	vec_add(v,&x,v);
}

void vec_cross(palVector3 *v, const palVector3 *a, const palVector3 *b) {
	v->x=a->y*b->z - a->z*b->y;
	v->y=a->z*b->x - a->x*b->z;
	v->z=a->x*b->y - a->y*b->x;
}

void vec_const_mul(palVector3 *v, const palVector3 *a, const Float mult) {
	v->x=a->x*mult;
	v->y=a->y*mult;
	v->z=a->z*mult;
}

//check the liscence.txt file for the relevant ownership, this code is modified from Mesa3D

static Float Identity[16] = {
   1.0, 0.0, 0.0, 0.0,
   0.0, 1.0, 0.0, 0.0,
   0.0, 0.0, 1.0, 0.0,
   0.0, 0.0, 0.0, 1.0
};

void mat_identity( palMatrix4x4 *m) {
	memcpy(m->_mat,Identity,sizeof(Float)*16);
}

bool mat_is_identity(palMatrix4x4 *m) {
	bool result = true;
	for (int i=0;i<16;++i) {
		if (fabs(m->_mat[i] - Identity[i]) > FLT_EPSILON)
		{
			result = false;
			break;
		}
	}
	return result;
}

#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]

void mat_add(palMatrix4x4 *m, const palMatrix4x4 *a, const palMatrix4x4 *b) {
	for (int i=0;i<16;i++) {
		m->_mat[i] = a->_mat[i] + b->_mat[i];
	}
}

void mat_sub(palMatrix4x4 *m, const palMatrix4x4 *a, const palMatrix4x4 *b) {
	for (int i=0;i<16;i++) {
		m->_mat[i] = a->_mat[i] - b->_mat[i];
	}
}

void mat_multiply( palMatrix4x4 *pal_product, const palMatrix4x4 *pal_a, const palMatrix4x4 *pal_b )
{
	Float *product = pal_product->_mat;
	Float *a = (Float *) pal_a->_mat;
	Float *b = (Float *) pal_b->_mat;
   int i;
   for (i = 0; i < 4; i++) {
      const Float ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
      P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
      P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
      P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
      P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
   }
}

#define M(row,col)  m[(col<<2)+row]

void mat_rotate(palMatrix4x4 *pal_m, Float angle, Float x, Float y, Float z) {
	Float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c, s, c;

	palMatrix4x4 rot;
	Float *m=rot._mat;
	s = (Float) sin( angle * DEG2RAD );
	c = (Float) cos( angle * DEG2RAD );
	memcpy(m, Identity, sizeof(Float)*16);
    const Float mag = (Float) sqrt(x * x + y * y + z * z);

	if (mag <= 1.0e-4) {
		// no rotation, leave mat as-is
		return;
	}
	xx = x * x;
	yy = y * y;
	zz = z * z;
	xy = x * y;
	yz = y * z;
	zx = z * x;
	xs = x * s;
	ys = y * s;
	zs = z * s;
	one_c = 1.0F - c;

	// We already hold the identity-matrix so we can skip some statements
	M(0,0) = (one_c * xx) + c;
	M(0,1) = (one_c * xy) - zs;
	M(0,2) = (one_c * zx) + ys;

	M(1,0) = (one_c * xy) + zs;
	M(1,1) = (one_c * yy) + c;
	M(1,2) = (one_c * yz) - xs;

	M(2,0) = (one_c * zx) - ys;
	M(2,1) = (one_c * yz) + xs;
	M(2,2) = (one_c * zz) + c;

	mat_multiply(pal_m,pal_m,&rot);
//	mat_multiply(pal_m,&rot, pal_m);
}

void mat_translate( palMatrix4x4 *pal_m, Float x, Float y, Float z )
{
   Float *m = pal_m->_mat;
   m[12] += m[0] * x + m[4] * y + m[8]  * z;
   m[13] += m[1] * x + m[5] * y + m[9]  * z;
   m[14] += m[2] * x + m[6] * y + m[10] * z;
   m[15] += m[3] * x + m[7] * y + m[11] * z;
}

void mat_transpose(palMatrix4x4 *dest, const palMatrix4x4 *src )
{
	for (int j=0;j<4;j++)
		for (int i=0;i<4;i++) {
			dest->_mat[i+j*4] = src->_mat[j+i*4];
		}
}



#define SWAP_ROWS(a, b) { Float *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

/* Converted from MESA3d, original comments&credits:
 * Compute inverse of 4x4 transformation matrix.
 * Code contributed by Jacques Leroy jle@star.be
 * Return GL_TRUE for success, GL_FALSE for failure (singular matrix)
 */
bool mat_invert( palMatrix4x4 *dest, const palMatrix4x4 *src )
{
   const Float *m = src->_mat;
   Float *out = dest->_mat;
   Float wtmp[4][8];
   Float m0, m1, m2, m3, s;
   Float *r0, *r1, *r2, *r3;

   r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

   r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
   r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
   r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

   r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
   r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
   r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

   r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
   r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
   r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

   r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),
   r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
   r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

   /* choose pivot - or die */
   if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
   if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
   if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
   if (0.0 == r0[0])  return false;

   /* eliminate first variable     */
   m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
   s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
   s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
   s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
   s = r0[4];
   if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
   s = r0[5];
   if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
   s = r0[6];
   if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
   s = r0[7];
   if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

   /* choose pivot - or die */
   if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
   if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
   if (0.0 == r1[1])  return false;

   /* eliminate second variable */
   m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
   r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
   r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
   s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
   s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
   s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
   s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

   /* choose pivot - or die */
   if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
   if (0.0 == r2[2])  return false;

   /* eliminate third variable */
   m3 = r3[2]/r2[2];
   r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
   r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
   r3[7] -= m3 * r2[7];

   /* last check */
   if (0.0 == r3[3]) return false;

   s = 1.0F/r3[3];             /* now back substitute row 3 */
   r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

   m2 = r2[3];                 /* now back substitute row 2 */
   s  = 1.0F/r2[2];
   r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
   r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
   m1 = r1[3];
   r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
   r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
   m0 = r0[3];
   r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
   r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

   m1 = r1[2];                 /* now back substitute row 1 */
   s  = 1.0F/r1[1];
   r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
   r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
   m0 = r0[2];
   r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
   r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

   m0 = r0[1];                 /* now back substitute row 0 */
   s  = 1.0F/r0[0];
   r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
   r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

   MAT(out,0,0) = r0[4]; MAT(out,0,1) = r0[5],
   MAT(out,0,2) = r0[6]; MAT(out,0,3) = r0[7],
   MAT(out,1,0) = r1[4]; MAT(out,1,1) = r1[5],
   MAT(out,1,2) = r1[6]; MAT(out,1,3) = r1[7],
   MAT(out,2,0) = r2[4]; MAT(out,2,1) = r2[5],
   MAT(out,2,2) = r2[6]; MAT(out,2,3) = r2[7],
   MAT(out,3,0) = r3[4]; MAT(out,3,1) = r3[5],
   MAT(out,3,2) = r3[6]; MAT(out,3,3) = r3[7];

   return true;
}

void printPalVector(palVector3 & src) {
	printf("|%+5.3f\t%+5.3f\t%+5.3f\t|\n",src.x,src.y,src.z);
}

void printPalQuaternion(palQuaternion &src) {
	printf("|%+5.3f\t%+5.3f\t%+5.3f\t%+5.3f\t|\n",src.x,src.y,src.z,src.w);
}


void printPalMatrix(palMatrix4x4 & src){
	printf("|%3.3f\t%3.3f\t%3.3f\t%3.3f\t|\n|%3.3f\t%3.3f\t%3.3f\t%3.3f\t|\n|%3.3f\t%3.3f\t%3.3f\t%3.3f\t|\n|%3.3f\t%3.3f\t%3.3f\t%3.3f\t|\n\n",
				src._11, src._21, src._31, src._41,
				src._12, src._22, src._32, src._42,
				src._13, src._23, src._33, src._43,
				src._14, src._24, src._34, src._44);
}

void rotx( palMatrix4x4 *m, float t )
{
    float ct = cos(t);
	float st = sin(t);
	m->_22 =  ct;
    m->_32 = -st;
    m->_23 =  st;
    m->_33 =  ct;
}

void roty( palMatrix4x4 *m, float t )
{
    float ct = cos(t);
	float st = sin(t);
	m->_11 = ct;
    m->_31 = st;
    m->_13 = -st;
    m->_33 =  ct;
}

void rotz( palMatrix4x4 *m, float t )
{
    float ct = cos(t);
	float st = sin(t);
	m->_11 =  ct;
    m->_21 = -st;
    m->_12 =  st;
    m->_22 =  ct;
}

void mat_set_translation(palMatrix4x4 *m, Float x, Float y, Float z) {
	m->_41 = x;
	m->_42 = y;
	m->_43 = z;
}

void mat_get_translation(const palMatrix4x4 *m, palVector3 *v) {
	v->x = m->_41;
	v->y = m->_42;
	v->z = m->_43;
}

extern void mat_get_column(const palMatrix4x4 *m, palVector3 *v, const int col) {
	v->x = m->_mat[col+0];
	v->y = m->_mat[col+4];
	v->z = m->_mat[col+8];
}
extern void mat_get_row(const palMatrix4x4 *m, palVector3 *v, const int row) {
	int start = row * 4;
	v->x = m->_mat[start + 0];
	v->y = m->_mat[start + 1];
	v->z = m->_mat[start + 2];
}

#undef M
#define M m->_mat

void mat_set_rotation(palMatrix4x4 *m, Float rotX, Float rotY, Float rotZ) {
		Float cr = cos( rotX );
		Float sr = sin( rotX );
		Float cp = cos( rotY );
		Float sp = sin( rotY );
		Float cy = cos( rotZ );
		Float sy = sin( rotZ );

		M[0] = (Float)( cp*cy );
		M[1] = (Float)( cp*sy );
		M[2] = (Float)( -sp );

		Float srsp = sr*sp;
		Float crsp = cr*sp;

		M[4] = (Float)( srsp*cy-cr*sy );
		M[5] = (Float)( srsp*sy+cr*cy );
		M[6] = (Float)( sr*cp );

		M[8] = (Float)( crsp*cy+sr*sy );
		M[9] = (Float)( crsp*sy-sr*cy );
		M[10] = (Float)( cr*cp );
}

#define mat(row,col)  m->_mat[(col<<2)+row]

void mat_get_rotation(palMatrix4x4 *m, Float *protX, Float *protY, Float *protZ) {

		Float Y = -asin(mat(2,0));
		Float C = cos(Y);
		//Y *= GRAD_PI;

		Float rotx, roty, X, Z;

		if (fabs(C)>0.0005f)
		{
			rotx = mat(2,2) / C;
			roty = mat(2,1)  / C;
			X = atan2( roty, rotx );// * GRAD_PI;
			rotx = mat(0,0) / C;
			roty = mat(1,0) / C;
			Z = atan2( roty, rotx );// * GRAD_PI;
		}
		else
		{
			X  = 0.0f;
			rotx = mat(1,1);
			roty = -mat(0,1);
			Z  = atan2( roty, rotx );// * (f32)GRAD_PI;
		}

		// fix values that get below zero
		// before it would set (!) values to 360
		// that where above 360:
//		if (X < 0.0) X += 360.0;
//		if (Y < 0.0) Y += 360.0;
//		if (Z < 0.0) Z += 360.0;
		if (X < 0.0) X += (Float)M_PI*2;
		if (Y < 0.0) Y += (Float)M_PI*2;
		if (Z < 0.0) Z += (Float)M_PI*2;


		*protX = X;
		*protY = Y;
		*protZ = Z;
		//return vector3df((f32)X,(f32)Y,(f32)Z);
}

#undef SWAP_ROWS

////////////////////////////////////////////////////////

//based from bullet:
void q_set(palQuaternion *q, Float x, Float y, Float z, Float w) {
	q->x = x;
	q->y = y;
	q->z = z;
	q->w = w;

}
void q_inverse(palQuaternion *q) {
	q->x *=-1;
	q->y *=-1;
	q->z *=-1;
	//vs q->w *=-1; //?
}

void vec_q_mul(palQuaternion *q, const palVector3 *a, const palQuaternion *b) {
	q->x = a->x * b->w + a->y * b->z - a->z * b->y;
	q->y = a->y * b->w + a->z * b->x - a->x * b->z;
	q->z = a->z * b->w + a->x * b->y - a->y * b->x;
	q->w =-a->x * b->x - a->y * b->y - a->z * b->z;
}

void q_vec_mul(palQuaternion *q, const palQuaternion *a, const palVector3 *b) {
	q->x = a->w * b->x + a->y * b->z - a->z * b->y;
	q->y = a->w * b->y + a->z * b->x - a->x * b->z;
	q->z = a->w * b->z + a->x * b->y - a->y * b->x;
	q->w =-a->x * b->x - a->y * b->y - a->z * b->z;
}



// Game Programming Gems 2.10. make sure a,b are normalized
void q_shortestArc(palQuaternion *q, const palVector3 *a, const palVector3 *b) {
	palVector3 c;
	vec_cross(&c,a,b);
	Float d = vec_dot(a,b);

	Float s =sqrt((1+d)*2);
	Float rs=1/s;
	q->x = c.x*rs;
	q->y = c.y*rs;
	q->z = c.z*rs;
	q->w = s*(Float)0.5;
}

void vec_q_rotate(palVector3 *v, const palQuaternion *a, const palVector3 *b) {
	palQuaternion q;
	q_vec_mul(&q,a,b);
	palQuaternion ia;
	memcpy(ia._q,a->_q,sizeof(palQuaternion));
	q_inverse(&ia);
	palQuaternion result;
	q_q_mul(&result,&q,&ia);
	vec_set(v,result.x,result.y,result.z);
}

void q_q_mul(palQuaternion *q, const palQuaternion *a, const palQuaternion *b) {
	q->x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
	q->y = a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z;
	q->z = a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x;
	q->w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
}

void plane_normalize(palPlane *p) {
	Float m = vec_mag(&p->n );
	Float im=1/m;
	p->x*=im;
	p->y*=im;
	p->z*=im;
	p->w*=im;
}

void plane_transform(palPlane *p,const palMatrix4x4 *a, const palPlane *b) {
	palVector3 PtOnPlane;
	vec_const_mul(&PtOnPlane,&b->n,b->w);
	palVector3 newN;
	palVector3 newP;
	vec_mat_mul(&newN,a,&b->n);
//	vec_mat_mul(&newP,a,&PtOnPlane);
	vec_mat_transform(&newP,a,&PtOnPlane);
	plane_create(p,&newN,&newP);
}

void plane_create(palPlane *p, const palVector3 *normal, const palVector3 *point) {
	memcpy(&p->n,normal,sizeof(palVector3));
	p->w = -vec_dot(&(p->n),point);
}

void plane_create(palPlane *p, const palVector3 *v1, const palVector3 *v2, const palVector3 *v3) {
	palVector3 v21,v31;
	vec_sub(&v21,v2,v1);
	vec_sub(&v31,v3,v1);
	vec_cross(&(p->n),&v21,&v31);
	p->w = -vec_dot(&(p->n),v1);
}

Float plane_distance(const palPlane *plane, const palVector3 *point) {
	Float mag = vec_mag(&plane->n);
	Float top = plane->n.x * point->x + plane->n.y * point->y + plane->n.z * point->z + plane->w;
	return top/mag;
}

//////////////////////////////////////////////////////////////////////
// output helpers
//

std::ostream& operator<<(std::ostream &os, const palVector3& v)
{
    os << "(" << v.x << "," << v.y << "," << v.z << ")";
    return os;
}

std::ostream& operator<<(std::ostream &os, const palMatrix4x4& m)
{
    os << "[";
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int i = c * 4 + r;	// elements are in column-major order
            os << m._mat[i];
            if (c != 3) {
                os << ", ";
            }
        }
        if (r != 3)  {
            os << " ; ";
        }        
    }
    os << "]";
    return os;
    
}
