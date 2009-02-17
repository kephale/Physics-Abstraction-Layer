#include "palFactory.h"

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
	if (m_pParent) {
		m_fRelativePosX = m_fPosX - m_pParent->m_fPosX;
		m_fRelativePosY = m_fPosY - m_pParent->m_fPosY;
		m_fRelativePosZ = m_fPosZ - m_pParent->m_fPosZ;
		if (m_pChild) {
			palMatrix4x4 a = m_pParent->GetLocationMatrix();
			palMatrix4x4 b = m_pChild->GetLocationMatrix();
		
			palVector3 m_axisA;
			vec_set(&m_pivotA,m_fPosX-a._41,m_fPosY-a._42,m_fPosZ-a._43);
			vec_set(&m_axisA,m_fAxisX,m_fAxisY,m_fAxisZ);
			//transform by A
			
			palVector3 outA;
			vec_mat_transform(&outA,&a,&m_pivotA);

			//inv transform by B
			palMatrix4x4 binv;
			mat_invert(&binv,&b);
			vec_mat_transform(&m_pivotB,&binv,&outA);

			//now have B pivot
			palVector3 m_axisB;
			vec_set(&m_axisB,m_fAxisX,m_fAxisY,m_fAxisZ);


			//m_pivotA
			//m_axisA
			//m_pivotB
			//m_axisB
#ifdef PRL_DEBUG
			PV(m_pivotA);
			PV(m_axisA);
			PV(m_pivotB);
			PV(m_axisB);
#endif

			//now build frames
			mat_identity(&m_frameA);
			//set origin A
			mat_set_translation(&m_frameA,m_pivotA.x,m_pivotA.y,m_pivotA.z);
			
			//calc aFrame (see bullet for algo)
			palVector3 rbAxisA1;
			palVector3 rbAxisA2;

			mat_get_column(&a,&rbAxisA1,0);

			Float projection = vec_dot(&rbAxisA1,&m_axisA);

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

#ifdef PRL_DEBUG
			PV(rbAxisA1);
			PV(rbAxisA2);
#endif
			
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
#ifdef PRL_DEBUG
			PQ(rArc);
#endif
			palVector3 rbAxisB1;
			vec_q_rotate(&rbAxisB1,&rArc,&rbAxisA1);
			palVector3 rbAxisB2;
			vec_cross(&rbAxisB2,&m_axisB,&rbAxisB1);

			//now build frame B
			mat_identity(&m_frameB);
			//set origin A
			mat_set_translation(&m_frameB,m_pivotB.x,m_pivotB.y,m_pivotB.z);
#ifdef PRL_DEBUG
			PV(rbAxisB1);
			PV(rbAxisB2);
#endif					
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
}

void palRevoluteLink::GetPosition(palVector3& pos) {
	palMatrix4x4 a = m_pParent->GetLocationMatrix();
	mat_translate(&a,m_fRelativePosX,m_fRelativePosY,m_fRelativePosZ);
	pos.x = a._41;
	pos.y = a._42;
	pos.z = a._43;
}

void palRevoluteLink::ApplyTorque(Float torque) {
	Float t0,t1,t2;
	t0=m_fAxisX * torque;
	t1=m_fAxisY * torque;
	t2=m_fAxisZ * torque;

	palBody * pbp = m_pParent->asBody();
	if (pbp)
		pbp->ApplyTorque(t0,t1,t2);
	palBody * pbc = m_pChild->asBody();
	if (pbc)
		pbc->ApplyTorque(-t0,-t1,-t2);
}

void palRevoluteLink::ApplyAngularImpulse(Float torque) {
	palMatrix4x4 a = m_pParent->GetLocationMatrix();
	palVector3 axis;
	vec_set(&axis,m_fAxisX,m_fAxisY,m_fAxisZ);
	palVector3 axisA;
	vec_mat_mul(&axisA,&a,&axis);
	vec_mul(&axisA,torque);
	
	palBody * pbp = m_pParent->asBody();
	if (pbp)
		pbp->ApplyAngularImpulse(axisA.x, axisA.y, axisA.z);
	palBody * pbc = m_pChild->asBody();
	if (pbc)
		pbc->ApplyAngularImpulse(-axisA.x,-axisA.y,-axisA.z);
}

Float palRevoluteLink::GetAngularVelocity() {
	palVector3 av1,av2,axis;
	vec_set(&av1,0,0,0);
	vec_set(&av2,0,0,0);

	palBody * pbp = m_pParent->asBody();
	if (pbp)
		pbp->GetAngularVelocity(av1);
	palBody * pbc = m_pChild->asBody();
	if (pbc)
		pbc->GetAngularVelocity(av2);

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
	palMatrix4x4 a,b;
	a=m_pParent->GetLocationMatrix();
	b=m_pChild->GetLocationMatrix();

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
