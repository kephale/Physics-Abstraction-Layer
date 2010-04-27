#include "palFactory.h"
#include <memory.h>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <sstream>

/*
	Abstract:
		PAL - Physics Abstraction Layer.
		Implementation File (bodies)

	Author:
		Adrian Boeing
	Revision History:
		Version 0.1 :19/10/07 split from pal.cpp
	TODO:
*/

#ifdef MEMDEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

palLink::palLink()
    : m_Type(PAL_LINK_NONE), m_fPosX(0), m_fPosY(0), m_fPosZ(0), m_pParent(0), m_pChild(0)
{
}

palLink::palLink(palLinkType linkType)
    : m_Type(linkType), m_fPosX(0), m_fPosY(0), m_fPosZ(0), m_pParent(0), m_pChild(0)
{
}

palLink::~palLink()
{
}
 
void palLink::Init(palBodyBase *parent, palBodyBase *child)
{
    SetBodies(parent, child);
}

void palLink::SetBodies(palBodyBase *parent, palBodyBase *child)
{
    m_pParent = parent;
    m_pChild = child;
}

void palLink::Init(palBodyBase *parent, palBodyBase *child,
                   Float x, Float y, Float z) {
    SetBodies(parent, child);
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
}

std::string palLink::toString() const
{
    std::ostringstream oss;
	oss << "Link[type=" << m_Type << ",pos=(" << m_fPosX
        << "," << m_fPosY << "," << m_fPosZ <<
        "),parent=" << m_pParent << ",child=" << m_pChild
        << " ]";
    return oss.str();
}

palSpringDesc::palSpringDesc()
: m_fDamper(0.0)
, m_fSpringCoef(0.0)
, m_fTarget(0.0)
{
}

palSphericalLink::palSphericalLink()
    : palLink(PAL_LINK_SPHERICAL)
{
}

palSphericalLink::~palSphericalLink()
{
}

void palSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {
	m_fConeLimit=cone_limit_rad;
	m_fTwistLimit=cone_limit_rad;
}

/*
void palSphericalLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	m_fLowerLimit=lower_limit_rad;
	m_fUpperLimit=upper_limit_rad;
}

void palSphericalLink::SetTwistLimits(Float lower_limit_rad, Float upper_limit_rad) {
	m_fLowerTwistLimit=lower_limit_rad;
	m_fUpperTwistLimit=upper_limit_rad;
}*/

//#define PRL_DEBUG

#define PV(x) printf(#x);printPalVector(x);
#define PQ(x) printf(#x);printPalQuaternion(x);

palRevoluteLink::palRevoluteLink()
    : palLink(PAL_LINK_REVOLUTE), m_fLowerLimit(-FLT_MAX), m_fUpperLimit(FLT_MAX)
{
}

palRevoluteLink::~palRevoluteLink()
{
}

void palRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
    palLink::Init(parent, child, x, y, z);

	if (m_pParent && m_pChild) {
		//Link_rel with the rotation matrix
		//Link_rel=(link_abs - parent_abs)*R

		palMatrix4x4 a_PAL = m_pParent->GetLocationMatrix();
		palMatrix4x4 b_PAL = m_pChild->GetLocationMatrix();

		// Transpose each body's position matrix to get its world-to-body
		// rotation matrix:
		palMatrix4x4 a, b;
		mat_transpose(&a, &a_PAL);		// a <- a_PAL'
		mat_transpose(&b, &b_PAL);		// b <- b_PAL'

		palVector3 link_rel;
		palVector3 translation;
		palVector3 posVec;
		
		// Compute the position of the link with respect to the parent's
		// origin, in the parent's coordinate system:
		m_pParent->GetPosition(posVec);
		translation._vec[0] = m_fPosX - posVec.x;	// first in world coords
		translation._vec[1] = m_fPosY - posVec.y;
		translation._vec[2] = m_fPosZ - posVec.z;
		vec_mat_mul(&link_rel,&a,&translation);		// rotate into parent coords
		m_pivotA.x = m_fRelativePosX = link_rel.x;
		m_pivotA.y = m_fRelativePosY = link_rel.y;
		m_pivotA.z = m_fRelativePosZ = link_rel.z;

		// Compute the position of the link with respect to the child's
		// origin, in the child's coordinate system:
		m_pChild->GetPosition(posVec);
		translation._vec[0] = m_fPosX - posVec.x;	// first in world coords
		translation._vec[1] = m_fPosY - posVec.y;
		translation._vec[2] = m_fPosZ - posVec.z;	
		vec_mat_mul(&link_rel,&b,&translation);		// rotate into child coords
		m_pivotB.x = link_rel.x;
		m_pivotB.y = link_rel.y;
		m_pivotB.z = link_rel.z;

		// Define a hinge coordinate system by generating hinge-to-body
		// transforms for both parent and child bodies. The hinge
		// coordinate system's +Z axis coincides with the hinge axis, its
		// +X and +Y axes are perpendicular to the hinge axis, and its
		// origin is at the hinge's origin.

		palVector3 axis, m_axisA, m_axisB;

		vec_set(&axis, axis_x, axis_y, axis_z);
		vec_mat_mul(&m_axisA, &a, &axis);			// axis in parent coords

		m_fRelativeAxisX = m_axisA.x;
		m_fRelativeAxisY = m_axisA.y;
		m_fRelativeAxisZ = m_axisA.z;

		vec_mat_mul(&m_axisB, &b, &axis);			// axis in child coords

		// Build m_frameA, which transforms points from hinge coordinates
		// to parent (body A) coordinates.

		// Choose basis vectors for the hinge coordinate system wrt parent coords:
		palVector3 rbAxisA1( 1, 0, 0 ), rbAxisA2;
		const palVector3 Z_AXIS( 0, 0, 1 );

		Float projection = vec_dot( & m_axisA, & Z_AXIS );
		if (projection >= 1 - FLOAT_EPSILON) {
			// The hinge axis coincides with the parent's +Z axis.
			rbAxisA2 = palVector3( 0, 1, 0 );
		} else if (projection <= -1 + FLOAT_EPSILON) {
			// The hinge axis coincides with the parent's -Z axis.
			rbAxisA2 = palVector3( 0, -1, 0 );
		} else {
			// The hinge axis crosses the parent's Z axis.
			vec_cross( & rbAxisA2, & m_axisA, & Z_AXIS );
			vec_cross( & rbAxisA1, & rbAxisA2, & m_axisA );
			vec_norm( & rbAxisA1 );
			vec_norm( & rbAxisA2 );
		}
		vec_norm( & m_axisA );

		mat_identity(&m_frameA);
		mat_set_translation(&m_frameA,m_pivotA.x,m_pivotA.y,m_pivotA.z);

		// Put the basis vectors in the columns of m_frameA:
		m_frameA._11 = rbAxisA1.x;
		m_frameA._12 = rbAxisA1.y;
		m_frameA._13 = rbAxisA1.z;
		m_frameA._21 = rbAxisA2.x;
		m_frameA._22 = rbAxisA2.y;
		m_frameA._23 = rbAxisA2.z;
		m_frameA._31 = m_axisA.x;
		m_frameA._32 = m_axisA.y;
		m_frameA._33 = m_axisA.z;

		// Build m_frameB, which transforms points from hinge coordinates
		// to parent (body A) coordinates.

		// Transform m_frameA basis vectors rbAxisA1 and rbAxisA2 from parent
		// coords to child coords to get m_frameB basis vectors rbAxisB1 and
		// rbAxisB2:
		palVector3 rbAxisB1, rbAxisB2;
		{
			palVector3 tmp;
			vec_mat_mul( &tmp, &a_PAL, &rbAxisA1 );
			vec_mat_mul( &rbAxisB1, &b, &tmp );
			vec_mat_mul( &tmp, &a_PAL, &rbAxisA2 );
			vec_mat_mul( &rbAxisB2, &b, &tmp );
		}

		vec_norm(&rbAxisB1);
		vec_norm(&rbAxisB2);
		vec_norm(&m_axisB);

		mat_identity(&m_frameB);
		mat_set_translation(&m_frameB,m_pivotB.x,m_pivotB.y,m_pivotB.z);

		// Put the basis vectors in the columns of m_frameB:
		m_frameB._11 = rbAxisB1.x;
		m_frameB._12 = rbAxisB1.y;
		m_frameB._13 = rbAxisB1.z;
		m_frameB._21 = rbAxisB2.x;
		m_frameB._22 = rbAxisB2.y;
		m_frameB._23 = rbAxisB2.z;
		m_frameB._31 = m_axisB.x;
		m_frameB._32 = m_axisB.y;
		m_frameB._33 = m_axisB.z;
	}
}

palVector3 palRevoluteLink::GetAxis() const {
	palMatrix4x4 a = m_pParent->GetLocationMatrix();
	palVector3 axis;
	vec_set(&axis,m_fRelativeAxisX,m_fRelativeAxisY,m_fRelativeAxisZ);

	palVector3 axisWorld;
	vec_mat_mul(&axisWorld,&a,&axis);
	return axisWorld;
}

void palRevoluteLink::GetPosition(palVector3& pos) const {
	//Convert link_rel to the global coordinate system
	//Link_abs=(Link_rel * R_Inv) - parent_abs

	//Transpose the matrix to get Normal rotation matrixes.
	palMatrix4x4 a_PAL = m_pParent->GetLocationMatrix();
	palMatrix4x4 a; 										//R
	mat_transpose(&a, &a_PAL);

	palMatrix4x4 a_inv;										//R_Inv
	palVector3 link_rel;
	palVector3 link_abs;

	link_rel.x =m_fRelativePosX;
	link_rel.y =m_fRelativePosY;
	link_rel.z =m_fRelativePosZ;

	bool isInverted = mat_invert(&a_inv,&a);
	if(!isInverted)
		return;

	vec_mat_mul(&link_abs,&a_inv,&link_rel);
	palVector3 posVec;
	m_pParent->GetPosition(posVec);
	pos.x = link_abs.x + posVec.x;
	pos.y = link_abs.y + posVec.y;
	pos.z = link_abs.z + posVec.z;

}

void palRevoluteLink::ApplyTorque(Float torque) {
	Float t0,t1,t2;
	palVector3 axis = GetAxis();
	t0= axis.x * torque;
	t1= axis.y * torque;
	t2= axis.z * torque;
	palBody * pb =dynamic_cast<palBody *>(m_pParent);
	palBody * cb =dynamic_cast<palBody *>(m_pChild);
	if (pb)
		pb->ApplyTorque(t0,t1,t2);
	if (cb)
		cb->ApplyTorque(-t0,-t1,-t2);
}

void palRevoluteLink::ApplyAngularImpulse(Float torque) {
	palMatrix4x4 a = m_pParent->GetLocationMatrix();
	palVector3 axis = GetAxis();
	palVector3 axisA;
	vec_mat_mul(&axisA,&a,&axis);
	vec_mul(&axisA,torque);

	palBody * pb =dynamic_cast<palBody *>(m_pParent);
	palBody * cb =dynamic_cast<palBody *>(m_pChild);
	if (pb)
		pb->ApplyAngularImpulse(axisA.x, axisA.y, axisA.z);
	if (cb)
		cb->ApplyAngularImpulse(-axisA.x,-axisA.y,-axisA.z);
}

Float palRevoluteLink::GetAngularVelocity() const {
	palVector3 av1,av2,axis;
	palBody *pb =dynamic_cast<palBody *>(m_pParent);
	palBody *cb =dynamic_cast<palBody *>(m_pChild);
	vec_set(&av1,0,0,0);
	vec_set(&av2,0,0,0);
	if (pb)
		pb->GetAngularVelocity(av1);
	if (cb)
		cb->GetAngularVelocity(av2);

	axis = GetAxis();

	Float rate;
	rate =vec_dot(&axis,&av1);
	rate-=vec_dot(&axis,&av2);
	return rate;
}

Float palRevoluteLink::GetAngle() const {

	if (m_pParent==NULL) return 0.0f;
	if (m_pChild ==NULL) return 0.0f;

	//palMatrix4x4 a_PAL,b_PAL;
	palMatrix4x4 a,b;
	a=m_pParent->GetLocationMatrix();
	b=m_pChild->GetLocationMatrix();

	palVector3 fac0;
	mat_get_row(&m_frameA,&fac0,0);
	palVector3 refAxis0;
	vec_mat_mul(&refAxis0,&a,&fac0);

	palVector3 fac1;
	mat_get_row(&m_frameA,&fac1,1);
	palVector3 refAxis1;
	vec_mat_mul(&refAxis1,&a,&fac1);

	palVector3 fbc1;
	mat_get_row(&m_frameB,&fbc1,1);
	palVector3 swingAxis;
	vec_mat_mul(&swingAxis,&b,&fbc1);

	Float d0 = vec_dot(&swingAxis,&refAxis0);
	Float d1 = vec_dot(&swingAxis,&refAxis1);
	return std::atan2(d0,d1);
#if 0 //this method does not do +/-, just positive :(
	palVector3 pp,cp;
	m_pParent->GetPosition(pp);
	m_pChild->GetPosition(cp);
//	printf("pp:");
//	printvector(&pp);
//	printf("cp:");
//	printvector(&cp);
	palVector3 linkpos;
	linkpos.x=m_fRelativePosX;
	linkpos.y=m_fRelativePosY;
	linkpos.z=m_fRelativePosZ;
//	printf("lp:");
//	printvector(&linkpos);
	palVector3 newlp;
	vec_mat_mul(&newlp,&a,&linkpos);
	vec_add(&newlp,&newlp,&pp);
//	printf("nlp:");
//	printvector(&newlp);
	palVector3 la,lb;
	vec_sub(&la,&pp,&newlp);
	vec_sub(&lb,&cp,&newlp);
//	la = pp;
//	lb = cp;
	vec_norm(&la);
	vec_norm(&lb);
//	printvector(&la);
//	printvector(&lb);
	Float dot=vec_dot(&la,&lb);
	Float mag=vec_mag(&la)*vec_mag(&lb);
	return Float(acos(dot/mag));
#endif
}

void palRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	m_fLowerLimit=lower_limit_rad;
	m_fUpperLimit=upper_limit_rad;
}


std::string palRevoluteLink::toString() const
{
    std::ostringstream oss;
    oss << palLink::toString()
		<< "[angle=" << GetAngle()
		<< ",limits=(" << m_fLowerLimit << "," << m_fUpperLimit << ")"
		<< ",omega=" << GetAngularVelocity()
        << ",axis=" << GetAxis()
		<< "]";
    return oss.str();
}

palPrismaticLink::palPrismaticLink()
    : palLink(PAL_LINK_PRISMATIC)
{
}


palPrismaticLink::~palPrismaticLink()
{
}

void palPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
    palLink::Init(parent, child, x, y, z);
	m_fAxisX=axis_x;
	m_fAxisY=axis_y;
	m_fAxisZ=axis_z;
}

void palPrismaticLink::SetLimits(Float lower_limit, Float upper_limit) {
}


palGenericLink::palGenericLink()
    : palLink(PAL_LINK_GENERIC)
{
}


palGenericLink::~palGenericLink()
{
}

void palGenericLink::Init(palBodyBase *parent, palBodyBase *child, palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
		palVector3 linearLowerLimits,
		palVector3 linearUpperLimits,
		palVector3 angularLowerLimits,
		palVector3 angularUpperLimits) {
    palLink::Init(parent, child);

	memcpy(&m_frameA,&parentFrame,sizeof(palMatrix4x4));
	memcpy(&m_frameB,&childFrame,sizeof(palMatrix4x4));
}


palRigidLink::palRigidLink()
    : palLink(PAL_LINK_RIGID)
{
}

palRigidLink::~palRigidLink()
{
}

void palRigidLink::Init(palBodyBase *parent, palBodyBase *child)
{
    palVector3 parentPos, childPos;
	parent->GetPosition(parentPos);
	child->GetPosition(childPos);
	palVector3 center = (parentPos + childPos) / 2;
	palLink::Init(parent, child, center.x, center.y, center.z);
}

#if 0
void palSphericalLink::GenericInit(palBody *pb0, palBody *pb1, void *param) {
	/*Float p[3];
	va_list args;
	va_start( args, param);

	void *ptr;
	char *szParam;

	p[0]=atof( (char *)param );
	for (int i=1;i<3;i++) {
		ptr = va_arg( args, void *);
		szParam = (char *)ptr;
		p[i]=atof(szParam);
	}
	this->Init(pb0,pb1,p[0],p[1],p[2]);*/
	float *p = static_cast<float *>(param);
	this->Init(pb0,pb1,p[0],p[1],p[2]);
}


void palRevoluteLink::GenericInit(palBody *pb0, palBody *pb1, void *param) {
/*	Float p[6];
	va_list args;
	va_start( args, param);

	void *ptr;
	char *szParam;

	p[0]=atof( (char *)param );
	for (int i=1;i<6;i++) {
		ptr = va_arg( args, void *);
		szParam = (char *)ptr;
		p[i]=atof(szParam);
	}*/
	float *p = static_cast<float *>(param);
	this->Init(pb0,pb1,p[0],p[1],p[2], p[3],p[4],p[5]);
}

void palPrismaticLink::GenericInit(palBody *pb0, palBody *pb1, void *param) {
	/*Float p[6];
	va_list args;
	va_start( args, param);

	void *ptr;
	char *szParam;

	p[0]=atof( (char *)param );
	for (int i=1;i<6;i++) {
		ptr = va_arg( args, void *);
		szParam = (char *)ptr;
		p[i]=atof(szParam);
	}*/
	float *p = static_cast<float *>(param);
	this->Init(pb0,pb1,p[0],p[1],p[2], p[3],p[4],p[5]);
}

#endif
