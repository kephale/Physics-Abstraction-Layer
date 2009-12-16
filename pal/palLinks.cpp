#include "palFactory.h"
#include <memory.h>

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

palSpringDesc::palSpringDesc()
: m_fDamper(0.0)
, m_fSpringCoef(0.0)
, m_fTarget(0.0)
{

}

void palSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
	m_pParent=parent;
	m_pChild=child;
	m_Type = PAL_LINK_SPHERICAL;
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

void palRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
	m_pParent=parent;
	m_pChild=child;

	m_fAxisX=axis_x;
	m_fAxisY=axis_y;
	m_fAxisZ=axis_z;
	//todo: infinity
	m_fLowerLimit=-999;
	m_fUpperLimit= 999;
	m_Type = PAL_LINK_REVOLUTE;

	if (m_pParent && m_pChild) {
		//Link_rel with the rotation matrix
		//Link_rel=(link_abs - parent_abs)*R

		palMatrix4x4 a_PAL = m_pParent->GetLocationMatrix();
		palMatrix4x4 b_PAL = m_pChild->GetLocationMatrix();

		//Transpose the matrix to get Normal rotation matrixes.
		palMatrix4x4 a, b;
		mat_transpose(&a, &a_PAL);
		mat_transpose(&b, &b_PAL);

		palVector3 link_rel;
		palVector3 translation;

		//link relative position with respect to the parent
		//Translation of absolute to relative		
		palVector3 posVec;
		m_pParent->GetPosition(posVec);

		translation._vec[0] = m_fPosX - posVec.x;
		translation._vec[1] = m_fPosY - posVec.y;
		translation._vec[2] = m_fPosZ - posVec.z;
			
		//Rotation
		vec_mat_mul(&link_rel,&a,&translation);
		m_fRelativePosX = link_rel.x;
		m_fRelativePosY = link_rel.y;
		m_fRelativePosZ = link_rel.z;
		m_pivotA.x = m_fRelativePosX;
		m_pivotA.y = m_fRelativePosY;
		m_pivotA.z = m_fRelativePosZ;

		m_pChild->GetPosition(posVec);
		//link relative position with respect to the child
		//Translation of absolute to relative
		translation._vec[0] = m_fPosX - posVec.x;
		translation._vec[1] = m_fPosY - posVec.y;
		translation._vec[2] = m_fPosZ - posVec.z;	
		//Rotation
		vec_mat_mul(&link_rel,&b,&translation);
		m_pivotB.x = link_rel.x;
		m_pivotB.y = link_rel.y;
		m_pivotB.z = link_rel.z;

		//Frames A and B: Bullet method

		//Axis
		palVector3 axis;
		palVector3 m_axisA;
		palVector3 m_axisB;

		vec_set(&axis,m_fAxisX,m_fAxisY,m_fAxisZ);
		vec_mat_mul(&m_axisA, &a, &axis);
		vec_mat_mul(&m_axisB, &b, &axis);

		//calc aFrame (see bullet for algo)
		palVector3 rbAxisA1;
		palVector3 rbAxisA2;

		mat_get_column(&a,&rbAxisA1,0);						//rbAxisA1
		Float projection = vec_dot(&m_axisA,&rbAxisA1);		//projection

		if (projection >=1-FLOAT_EPSILON  ) {
			mat_get_column(&a,&rbAxisA1,2);
			vec_mul(&rbAxisA1,-1);
			mat_get_column(&a,&rbAxisA2,1);
		} else if (projection <= -1+FLOAT_EPSILON  )  {
			mat_get_column(&a,&rbAxisA1,2);
			mat_get_column(&a,&rbAxisA2,1);
		} else {
			vec_cross(&rbAxisA2,&m_axisA,&rbAxisA1);
			vec_cross(&rbAxisA1,&rbAxisA2,&m_axisA);
		}

		//Set frameA
		mat_identity(&m_frameA);
		mat_set_translation(&m_frameA,m_pivotA.x,m_pivotA.y,m_pivotA.z);

		m_frameA._11 = rbAxisA1.x;
		m_frameA._21 = rbAxisA1.y;
		m_frameA._31 = rbAxisA1.z;
		m_frameA._12 = rbAxisA2.x;
		m_frameA._22 = rbAxisA2.y;
		m_frameA._32 = rbAxisA2.z;
		m_frameA._13 = m_axisA.x;
		m_frameA._23 = m_axisA.y;
		m_frameA._33 = m_axisA.z;

		//build frame B, see bullet for algo
		palQuaternion rArc;
		q_shortestArc(&rArc,&m_axisA,&m_axisB);

		palVector3 rbAxisB1;
		vec_q_rotate(&rbAxisB1,&rArc,&rbAxisA1);
		palVector3 rbAxisB2;
		vec_cross(&rbAxisB2,&m_axisB,&rbAxisB1);

		//now build frame B
		mat_identity(&m_frameB);
		mat_set_translation(&m_frameB,m_pivotB.x,m_pivotB.y,m_pivotB.z);
		m_frameB._11 = rbAxisB1.x;
		m_frameB._21 = rbAxisB1.y;
		m_frameB._31 = rbAxisB1.z;
		m_frameB._12 = rbAxisB2.x;
		m_frameB._22 = rbAxisB2.y;
		m_frameB._32 = rbAxisB2.z;
		m_frameB._13 = -m_axisB.x;
		m_frameB._23 = -m_axisB.y;
		m_frameB._33 = -m_axisB.z;
	}
}


void palRevoluteLink::GetPosition(palVector3& pos) {
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
	t0=m_fAxisX * torque;
	t1=m_fAxisY * torque;
	t2=m_fAxisZ * torque;
	palBody * pb =dynamic_cast<palBody *>(m_pParent);
	palBody * cb =dynamic_cast<palBody *>(m_pChild);
	if (pb)
		pb->ApplyTorque(t0,t1,t2);
	if (cb)
		cb->ApplyTorque(-t0,-t1,-t2);
}

void palRevoluteLink::ApplyAngularImpulse(Float torque) {
	palMatrix4x4 a = m_pParent->GetLocationMatrix();
	palVector3 axis;
	vec_set(&axis,m_fAxisX,m_fAxisY,m_fAxisZ);
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

Float palRevoluteLink::GetAngularVelocity() {
	palVector3 av1,av2,axis;
	palBody * pb =dynamic_cast<palBody *>(m_pParent);
	palBody * cb =dynamic_cast<palBody *>(m_pChild);
	vec_set(&av1,0,0,0);
	vec_set(&av2,0,0,0);
	if (pb)
		pb->GetAngularVelocity(av1);
	if (cb)
		cb->GetAngularVelocity(av2);
	axis.x=m_fAxisX;
	axis.y=m_fAxisY;
	axis.z=m_fAxisZ;
	Float rate;
	rate =vec_dot(&axis,&av1);
	rate-=vec_dot(&axis,&av2);
	return rate;
}

Float palRevoluteLink::GetAngle() {

	if (m_pParent==NULL) return 0.0f;
	if (m_pChild ==NULL) return 0.0f;

	palMatrix4x4 a_PAL,b_PAL;
	palMatrix4x4 a,b;
	a_PAL=m_pParent->GetLocationMatrix();
	b_PAL=m_pChild->GetLocationMatrix();

	mat_transpose(&a, &a_PAL);
	mat_transpose(&b, &b_PAL);

	palVector3 fac0;
	mat_get_column(&m_frameA,&fac0,0);
	palVector3 refAxis0;
	vec_mat_mul(&refAxis0,&a,&fac0);

	palVector3 fac1;
	mat_get_column(&m_frameA,&fac1,1);
	palVector3 refAxis1;
	vec_mat_mul(&refAxis1,&a,&fac1);

	palVector3 fbc1;
	mat_get_column(&m_frameB,&fbc1,1);
	palVector3 swingAxis;
	vec_mat_mul(&swingAxis,&b,&fbc1);

	Float d0 = vec_dot(&swingAxis,&refAxis0);
	Float d1 = vec_dot(&swingAxis,&refAxis1);
	return atan2(d0,d1);


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

void palPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
	m_pParent=parent;
	m_pChild=child;
	m_fAxisX=axis_x;
	m_fAxisY=axis_y;
	m_fAxisZ=axis_z;
	m_Type = PAL_LINK_PRISMATIC;
}

void palPrismaticLink::SetLimits(Float lower_limit, Float upper_limit) {
}


void palGenericLink::Init(palBodyBase *parent, palBodyBase *child, palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
		palVector3 linearLowerLimits,
		palVector3 linearUpperLimits,
		palVector3 angularLowerLimits,
		palVector3 angularUpperLimits) {
	m_pParent=parent;
	m_pChild=child;
	m_Type = PAL_LINK_GENERIC;

	memcpy(&m_frameA,&parentFrame,sizeof(palMatrix4x4));
	memcpy(&m_frameB,&childFrame,sizeof(palMatrix4x4));
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
