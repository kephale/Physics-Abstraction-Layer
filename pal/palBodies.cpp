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

#include <memory.h>
#ifdef MEMDEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif
////////////////////////////////////////
palBody::palBody()
: m_fForceX(0.0)
, m_fForceY(0.0)
, m_fForceZ(0.0)
, m_fTorqueX(0.0)
, m_fTorqueY(0.0)
, m_fTorqueZ(0.0)
, m_fMass(0.0)
{
	m_pMaterial = NULL;
	memset(&m_mLoc,0,sizeof(palMatrix4x4));
	m_mLoc._11 = 1;
	m_mLoc._22 = 1;
	m_mLoc._33 = 1;
	m_mLoc._44 = 1;
}

void palBody::Cleanup() {
	palBodyBase::Cleanup();
}

void palBody::SetPosition(Float x, Float y, Float z) {
#if 0
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
	palMatrix4x4 loc = GetLocationMatrix();
	//don't reset position
	//memset(loc._mat,0,sizeof(palMatrix4x4));
	//loc._11=1; loc._22=1; loc._33=1; loc._44=1;
	loc._41=x;
	loc._42=y;
	loc._43=z;
	loc._44=1;
	SetPosition(loc);
#else
	palBodyBase::SetPosition(x,y,z);
#endif
}

void palBody::SetPosition(palMatrix4x4 &location) {
	palBodyBase::SetPosition(location);
//	m_mLoc = location;
}


void palBody::SetOrientation(Float roll, Float pitch, Float yaw) {

	palMatrix4x4 loc = GetLocationMatrix();

#if 0
	Float sinroll = (Float)sin(roll), cosroll = (Float)cos(roll);
	Float sinpitch = (Float)sin(pitch), cospitch = (Float)cos(pitch);
	Float sinyaw = (Float)sin(yaw), cosyaw = (Float)cos(yaw);

	loc._11= cosroll*cosyaw;
	loc._12= cosroll*sinyaw*sinpitch - sinroll*cospitch;
	loc._13= sinroll*sinpitch + cosroll*sinyaw*cospitch;
	loc._14= 0.0f;
	loc._21= sinroll*cosyaw;
	loc._22= cosroll*cospitch + sinroll*sinyaw*sinpitch;
	loc._23= sinroll*sinyaw*cospitch - cosroll*sinpitch;
	loc._24= 0.0f;
	loc._31= -sinyaw;
	loc._32= cosyaw*sinpitch;
	loc._33= cosyaw*cospitch;
	loc._34= 0.0f;
	//dont adjust position
	loc._44= 1.0f;
#else
	mat_set_rotation(&loc,roll,pitch,yaw);
#endif
	SetPosition(loc);
}

void palBody::SetPosition(Float x, Float y, Float z, Float roll, Float pitch, Float yaw) {
	palMatrix4x4 loc;
#if 0
	Float sinroll = (Float)sin(roll), cosroll = (Float)cos(roll);
	Float sinpitch = (Float)sin(pitch), cospitch = (Float)cos(pitch);
	Float sinyaw = (Float)sin(yaw), cosyaw = (Float)cos(yaw);




	loc._11= cosroll*cosyaw;
	loc._12= cosroll*sinyaw*sinpitch - sinroll*cospitch;
	loc._13= sinroll*sinpitch + cosroll*sinyaw*cospitch;
	loc._14= 0.0f;
	loc._21= sinroll*cosyaw;
	loc._22= cosroll*cospitch + sinroll*sinyaw*sinpitch;
	loc._23= sinroll*sinyaw*cospitch - cosroll*sinpitch;
	loc._24= 0.0f;
	loc._31= -sinyaw;
	loc._32= cosyaw*sinpitch;
	loc._33= cosyaw*cospitch;
	loc._34= 0.0f;
	loc._41= x;
	loc._42= y;
	loc._43= z;
	loc._44= 1.0f;
#else
	mat_set_translation(&loc,x,y,z);
	mat_set_rotation(&loc,roll,pitch,yaw);
#endif
	SetPosition(loc);

}



void palBody::ApplyImpulse(Float fx, Float fy, Float fz) {
	Float im = 1/m_fMass;

	palVector3 v;
	GetLinearVelocity(v);
	v.x += fx * im;
	v.y += fy * im;
	v.z += fz * im;
	SetLinearVelocity(v);
}

void palBody::ApplyAngularImpulse(Float ix, Float iy, Float iz) {
	palVector3 ii;
	vec_set(&ii,ix,iy,iz);

	palVector3 invInertia;
	vec_set(&invInertia,1/m_Geometries[0]->m_fInertiaXX,
						1/m_Geometries[0]->m_fInertiaYY,
						1/m_Geometries[0]->m_fInertiaZZ);


	palMatrix4x4 pos;
	pos=this->GetLocationMatrix();

	palMatrix4x4 inertiaScaled;
	inertiaScaled=this->GetLocationMatrix();
	mat_scale3x3(&inertiaScaled,&invInertia);

	palMatrix4x4 posT;
	mat_transpose(&posT,&pos);

	palMatrix4x4 inertiaWorld;
	mat_multiply(&inertiaWorld,&inertiaScaled,&posT);

	palVector3 angularvel;
	vec_mat_mul(&angularvel,&inertiaWorld,&ii);
//	printPalVector(angularvel);

	palVector3 v;
	GetAngularVelocity(v);
	palVector3 sum;
	vec_add(&sum,&angularvel,&v);
	SetAngularVelocity(sum);
}

void palBody::ApplyImpulseAtPosition(Float px, Float py, Float pz, Float ix, Float iy, Float iz) {
	ApplyImpulse(ix,iy,iz);

	palVector3 f,q,p,bpos,tadd;
	f.x=ix; f.y=iy; f.z=iz;
	p.x=px; p.y=py; p.z=pz;
	GetPosition(bpos);
	vec_sub(&q,&p,&bpos);
	vec_cross(&tadd,&q,&f);
	ApplyAngularImpulse(tadd.x,tadd.y,tadd.z);
}

void palBody::ApplyForceAtPosition(Float px, Float py, Float pz, Float fx, Float fy, Float fz) {
	//based off code from ODE
	ApplyForce(fx,fy,fz);
	palVector3 f,q,p,bpos,tadd;
	f.x=fx; f.y=fy; f.z=fz;
	p.x=px; p.y=py; p.z=pz;
	GetPosition(bpos);
	vec_sub(&q,&p,&bpos);
	vec_cross(&tadd,&q,&f);
	ApplyTorque(tadd.x,tadd.y,tadd.z);
}


void palBody::ApplyForce(Float fx, Float fy, Float fz) {
	Float ts=PF->GetActivePhysics()->GetLastTimestep();
	ApplyImpulse(fx*ts,fy*ts,fz*ts);
}

void palBody::ApplyTorque(Float tx, Float ty, Float tz) {
	Float ts=PF->GetActivePhysics()->GetLastTimestep();
	ApplyAngularImpulse(tx*ts,ty*ts,tz*ts);
}

palVector3 palBody::CalcInertiaSum(float& summedMass)
{
	palVector3 pv;
	pv.x = 0.0;
	pv.y = 0.0;
	pv.z = 0.0;
	for (unsigned int i=0;i<m_Geometries.size();i++) {
		palVector3 gpos;
		palVector3 pos;
		palGeometry* geom = m_Geometries[i];
		geom->GetPosition(gpos);
		palMatrix4x4 loc = GetLocationMatrix();
		pos.x=loc._41; pos.y=loc._42; pos.z=loc._43;
		palVector3 d;
		vec_sub(&d,&gpos,&pos);
		Float distance = vec_mag(&d);
		Float massD2 = geom->GetMass() * distance * distance;
		pv.x+=geom->m_fInertiaXX + massD2;
		pv.y+=geom->m_fInertiaYY + massD2;
		pv.z+=geom->m_fInertiaZZ + massD2;
		summedMass+=geom->GetMass();
	}

	return pv;
}


#if 0
void palBody::SetForce(Float fx, Float fy, Float fz) {
	m_fForceX = fx;
	m_fForceY = fy;
	m_fForceZ = fz;
}


void palBody::AddForce(Float fx, Float fy, Float fz) {
	palVector3 force;
	GetForce(force);
	SetForce(force.x+fx,force.y+fy,force.z+fz);
}


void palBody::SetTorque(Float tx, Float ty, Float tz) {
	m_fTorqueX = tx;
	m_fTorqueY = ty;
	m_fTorqueZ = tz;
}

void palBody::AddTorque(Float tx, Float ty, Float tz) {
	palVector3 torque;
	GetTorque(torque);
	SetTorque(torque.x+tx,torque.y+ty,torque.z+tz);
}
#endif

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void palCompoundBody::Init(Float x, Float y, Float z) {
	palBody::SetPosition(x,y,z);
	m_Type = PAL_BODY_COMPOUND;
}

void palCompoundBody::Finalize() {
	SumInertia();
	Finalize(m_fMass,m_fInertiaXX,m_fInertiaYY,m_fInertiaZZ);
}

void palCompoundBody::SumInertia() {
	Float summedMass;
	palVector3 inertia = CalcInertiaSum(summedMass);
	m_fMass = summedMass;
	m_fInertiaXX = inertia.x;
	m_fInertiaYY = inertia.y;
	m_fInertiaZZ =inertia.z;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



void palBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	palBoxBase::Init(m,width,height,depth,mass);
	m_fMass = mass;
	m_Type = PAL_BODY_BOX;
}

void palBox::GenericInit(palMatrix4x4 &pos, void *param_array) {
	Float *p=(Float *)param_array;
	printf("generic init of the box now! loc: %f %f %f, dim:%f %f %f %f\n",pos._41,pos._42,pos._43,p[0],p[1],p[2],p[3]);
	Init(pos._41,pos._42,pos._43,p[0],p[1],p[2],p[3]);
	//SetPosition(pos);
}

void palConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	palConvexBase::Init(m,pVertices,nVertices,mass);
	m_fMass = mass;
	m_Type = PAL_BODY_CONVEX;
}

void palConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	palConvexBase::Init(m,pVertices,nVertices,pIndices,nIndices,mass);
	m_fMass = mass;
	m_Type = PAL_BODY_CONVEX;
}

void palCapsule::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	palCapsuleBase::Init(m,radius,length,mass);
	m_fMass = mass;
	m_Type = PAL_BODY_CAPSULE;
}

void palSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	palSphereBase::Init(m,radius,mass);
	m_fMass = mass;
	m_Type = PAL_BODY_SPHERE;
}

palGenericBody::palGenericBody(){
   m_Type = PAL_BODY_GENERIC;
	m_eDynType = PALBODY_DYNAMIC;
	m_fMass = 1.0;
	m_fInertiaXX = 1.0;
	m_fInertiaYY = 1.0;
	m_fInertiaZZ = 1.0;
	m_bInitialized = false;
}


void palGenericBody::Init(palMatrix4x4 &pos) {
	SetPosition(pos);

	for (unsigned i=0; i<m_Geometries.size(); ++i) {
		palGeometry* pGeom = m_Geometries[i];
		SetGeometryBody(pGeom);
		pGeom->ReCalculateOffset(); //recalculate the local offset now that we can reference the body
	}
	Float summedMass = 0.0f;
	palVector3 inertia = CalcInertiaSum(summedMass);
	SetInertia(inertia.x, inertia.y, inertia.z);
	m_bInitialized = true;
}

void palGenericBody::SetDynamicsType(palDynamicsType dynType) {
	m_eDynType = dynType;
}

void palGenericBody::SetMass(Float mass) {
	m_fMass = mass;
}

void palGenericBody::SetInertia(Float Ixx, Float Iyy, Float Izz) {
	m_fInertiaXX = Ixx;
	m_fInertiaYY = Iyy;
	m_fInertiaZZ = Izz;
}

void palGenericBody::GetInertia(Float& Ixx, Float& Iyy, Float& Izz)
{
   Ixx = m_fInertiaXX;
   Iyy = m_fInertiaYY;
   Izz = m_fInertiaZZ;
}

//void palGenericBody::SetCenterOfMass(palMatrix4x4& loc) {
//	m_mCOM = loc;
//}

unsigned int palGenericBody::GetNumGeometries() {
	return m_Geometries.size();
}

void palGenericBody::ConnectGeometry(palGeometry* pGeom) {
	if (pGeom == NULL) return;

	m_Geometries.push_back(pGeom);
	InternalConnectGeometry(pGeom);
}

struct CompareGeom {
	bool operator() (palGeometry* pGeom) {
		return pGeom == m_pGeomToCompare;
	}

	palGeometry* m_pGeomToCompare;
};

void palGenericBody::RemoveGeometry(palGeometry* pGeom)
{
	if (pGeom == NULL || pGeom->m_pBody != this) return;

	CompareGeom compFunc;
	compFunc.m_pGeomToCompare = pGeom;
	m_Geometries.erase(std::remove_if(m_Geometries.begin(), m_Geometries.end(), compFunc),
				m_Geometries.end());

	InternalDisconnectGeometry(pGeom);
}

void palGenericBody::InternalConnectGeometry(palGeometry* pGeom) {
	if (m_bInitialized)
	{
		SetGeometryBody(pGeom);
		pGeom->ReCalculateOffset(); //recalculate the local offset now that we can reference the body
		Float summedMass;
		palVector3 inertia = CalcInertiaSum(summedMass);
		SetInertia(inertia.x, inertia.y, inertia.z);
	}
}

void palGenericBody::InternalDisconnectGeometry(palGeometry* pGeom) {
	if (m_bInitialized) {
		ClearGeometryBody(pGeom);
		Float summedMass;
		palVector3 inertia = CalcInertiaSum(summedMass);
		SetInertia(inertia.x, inertia.y, inertia.z);
	}
}

const PAL_VECTOR<palGeometry *>& palGenericBody::GetGeometries() {
	return m_Geometries;
}




