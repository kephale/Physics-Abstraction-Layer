#include "OpenTissue_PAL.h"

FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palOpenTissuePhysics);

FACTORY_CLASS_IMPLEMENTATION(palOpenTissueMaterialUnique);

FACTORY_CLASS_IMPLEMENTATION(palOpenTissueBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palOpenTissueSphereGeometry);
//FACTORY_CLASS_IMPLEMENTATION(palOpenTissueCylinderGeometry);

FACTORY_CLASS_IMPLEMENTATION(palOpenTissueBox);
FACTORY_CLASS_IMPLEMENTATION(palOpenTissueSphere);
//FACTORY_CLASS_IMPLEMENTATION(palOpenTissueCylinder);

FACTORY_CLASS_IMPLEMENTATION(palOpenTissueTerrainPlane);
//FACTORY_CLASS_IMPLEMENTATION(palOpenTissueTerrainMesh);
//FACTORY_CLASS_IMPLEMENTATION(palOpenTissueTerrainHeightmap);

//FACTORY_CLASS_IMPLEMENTATION(palOpenTissueSphericalLink);
//FACTORY_CLASS_IMPLEMENTATION(palOpenTissueRevoluteLink);
//FACTORY_CLASS_IMPLEMENTATION(palOpenTissuePrismaticLink);
FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

using namespace OpenTissue;
/*
set_position
set_orientation
set_velocity
set_spin
set_mass
get_position
get_orientation
get_velocity
get_spin

set_active
set_sleepy

set_material_idx

set_fixed

set_geometry
---
set_friction_coefficient
set_material_indices

friction_coefficient
normal_restitution
*/
#define X 0
#define Y 1
#define Z 2

OTTypes::configuration_type          g_configuration;
OTGravity g_gravity;
OTTypes::material_library_type        g_library;

static int g_materialcount = 1;
///////////////////////////////////////////////////////
palOpenTissueMaterialUnique::palOpenTissueMaterialUnique() {

}

void palOpenTissueMaterialUnique::Init(STRING name, const palMaterialDesc& desc) {
	palMaterialUnique::Init(name, desc);
	m_idx=g_materialcount;
	m_material.set_material_indices(m_idx,m_idx);
	m_material.set_friction_coefficient(m_fStatic);
	m_material.normal_restitution() = m_fRestitution;
	g_library.add(m_material);
	g_materialcount++;
}

palOpenTissuePhysics::palOpenTissuePhysics() {
}

void palOpenTissuePhysics::Init(palPhysicsDesc& desc) {
	palPhysics::Init(desc);
	g_configuration.clear();

	g_gravity.set_acceleration(vector3<OTReal>(m_fGravityX, m_fGravityY, m_fGravityZ));

	OTMaterial * default_material = g_library.default_material();
	default_material->set_friction_coefficient(0.25);
	default_material->normal_restitution()=0.25;
	g_configuration.set_material_library(g_library);

	m_simulator.init(g_configuration);
	//m_timestep = 0.01;
}

void palOpenTissuePhysics::Cleanup() {
}

const char* palOpenTissuePhysics::GetVersion() {
	return 0;
}

void palOpenTissuePhysics::Iterate(Float timestep) {
	m_timestep = timestep;
	m_simulator.run(m_timestep);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palOpenTissueBodyBase::palOpenTissueBodyBase() {
	m_potBody = 0;
}

palOpenTissueBodyBase::~palOpenTissueBodyBase() {

}

void palOpenTissueBodyBase::SetPosition(palMatrix4x4& loc) {
	if (!m_potBody)
		return;
	vector3<OTReal> r;
	r = vector3<OTReal>(loc._41, loc._42, loc._43);

	matrix3x3<OTReal> m;
	m = matrix3x3<OTReal>(
		loc._11,loc._12,loc._13,
		loc._21,loc._22,loc._23,
		loc._31,loc._32,loc._33);

	quaternion<OTReal> Q(m);
//	Q.identity();

	m_potBody->set_position(r);
	m_potBody->set_orientation(Q);
}

palMatrix4x4& palOpenTissueBodyBase::GetLocationMatrix() {
	if (!m_potBody)
		return m_mLoc;
	vector3<OTReal> r;
	matrix3x3<OTReal> m;
	m_potBody->get_position(r);
	m_potBody->get_orientation(m);
	m_mLoc._41 = r(X);
	m_mLoc._42 = r(Y);
	m_mLoc._43 = r(Z);

	m_mLoc._11 = m(0,0);
	m_mLoc._12 = m(1,0);
	m_mLoc._13 = m(2,0);

	m_mLoc._21 = m(0,1);
	m_mLoc._22 = m(1,1);
	m_mLoc._23 = m(2,1);

	m_mLoc._31 = m(0,2);
	m_mLoc._32 = m(1,2);
	m_mLoc._33 = m(2,2);

	return m_mLoc;
}
void palOpenTissueBodyBase::SetMaterial(palMaterial *material) {
	if (!m_potBody)
		return;
	palOpenTissueMaterialUnique	*pm = dynamic_cast<palOpenTissueMaterialUnique *>(material);
	if (pm)
		m_potBody->set_material_idx(pm->m_idx);
}

void palOpenTissueBodyBase::BuildBody(Float x, Float y, Float z, Float mass, bool dynamic) {
	m_potBody = new OTBody;
	m_potBody->set_position(vector3<OTReal>(x,y,z));
	if (dynamic) {
		m_potBody->attach(&g_gravity);
		m_potBody->set_mass(mass);
		matrix3x3<OTReal> I;
		I= diag(1.0);
		m_Geometries[0]->CalculateInertia();
		I(0,0) = m_Geometries[0]->m_fInertiaXX;
		I(1,1) = m_Geometries[0]->m_fInertiaYY;
		I(2,2) = m_Geometries[0]->m_fInertiaZZ;
		m_potBody->set_inertia_bf(I);
	} else {
		m_potBody->set_fixed(true);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palOpenTissueBody::palOpenTissueBody() {

}

bool palOpenTissueBody::IsActive()
{
	return m_potBody->is_active();
}

void palOpenTissueBody::SetActive(bool active){
	m_potBody->set_active(active);
	m_potBody->set_sleepy(!active);
}

void palOpenTissueBody::SetForce(Float fx, Float fy, Float fz) {
}
void palOpenTissueBody::GetForce(palVector3& force) {
}
void palOpenTissueBody::ApplyImpulse(Float fx, Float fy, Float fz) {
}
void palOpenTissueBody::ApplyAngularImpulse(Float fx, Float fy, Float fz) {
}
void palOpenTissueBody::SetTorque(Float tx, Float ty, Float tz){
}
void palOpenTissueBody::GetTorque(palVector3& torque) {
}

void palOpenTissueBody::GetLinearVelocity(palVector3& v) {
	vector3<OTReal> r;
	m_potBody->get_velocity(r);
	v.x = r(X);
	v.y = r(Y);
	v.z = r(Z);
}
void palOpenTissueBody::GetAngularVelocity(palVector3& v) {
	vector3<OTReal> r;
	m_potBody->get_spin(r);
	v.x = r(X);
	v.y = r(Y);
	v.z = r(Z);
}

void palOpenTissueBody::SetLinearVelocity(palVector3 v) {
	m_potBody->set_velocity(vector3<OTReal>(v.x,v.y,v.z));
}
void palOpenTissueBody::SetAngularVelocity(palVector3 v) {
	m_potBody->set_spin(vector3<OTReal>(v.x,v.y,v.z));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


palOpenTissueBoxGeometry::palOpenTissueBoxGeometry() {
}
void palOpenTissueBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	m_potBox = new OTBox;
	matrix3x3<double> R;
	R = diag(1.0);
	m_potBox->set(vector3<OTReal>(0,0,0),R,vector3<OTReal>(width*0.5,height*0.5,depth*0.5));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palOpenTissueSphereGeometry::palOpenTissueSphereGeometry() {
}
void palOpenTissueSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	m_potSphere = new OTSphere;
	m_potSphere->set(vector3<OTReal>(pos._41,pos._42,pos._43),radius);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palOpenTissueBox::palOpenTissueBox() {
}
void palOpenTissueBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);

	BuildBody(x,y,z,mass,true);

	palOpenTissueBoxGeometry *pbtg=dynamic_cast<palOpenTissueBoxGeometry *> (m_Geometries[0]);
	m_potBody->set_geometry(*(pbtg->m_potBox));

	g_configuration.add(m_potBody);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palOpenTissueSphere::palOpenTissueSphere() {

}
void palOpenTissueSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);

	BuildBody(x,y,z,mass,true);

	palOpenTissueSphereGeometry *pbtg=dynamic_cast<palOpenTissueSphereGeometry  *> (m_Geometries[0]);
	m_potBody->set_geometry(*(pbtg->m_potSphere));

	g_configuration.add(m_potBody);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palOpenTissueTerrainPlane::palOpenTissueTerrainPlane() {
}
void palOpenTissueTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	matrix3x3<OTReal> R;
	R = diag(1.0);
	m_potBody = new OTBody;
	m_floorbox.set(vector3<OTReal>(0,-0.5f,0),R,vector3<OTReal>(min_size,1.0,min_size));
	m_potBody->set_fixed(true);
	m_potBody->set_geometry(m_floorbox);
	g_configuration.add(m_potBody);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
palOpenTissueSphericalLink::palOpenTissueSphericalLink() {
}
void palOpenTissueSphericalLink::Init(palBody *parent, palBody *child, Float x, Float y, Float z) {
/*
	OTTypes::socket_type            m_socket_A;
	OTTypes::socket_type            m_socket_B;
	OTTypes::ball_type m_ball;
*/

	palSphericalLink::Init(parent,child,x,y,z);
	palOpenTissueBody *body0 = dynamic_cast<palOpenTissueBody *> (parent);
	palOpenTissueBody *body1 = dynamic_cast<palOpenTissueBody *> (child);

	palMatrix4x4 a = parent->GetLocationMatrix();
	palMatrix4x4 b = child->GetLocationMatrix();

	vector3<OTReal> pivotInA(x-a._41,y-a._42,z-a._43);
	vector3<OTReal> pivotInB(x-b._41,y-b._42,z-b._43);
	//btVector3 pivotInB = body1->m_pbtBody->getCenterOfMassTransform().inverse()(body0->m_pbtBody->getCenterOfMassTransform()(pivotInA)) ;

	m_socket_A.init(*(body0->m_potBody),OTTypes::coordsys(pivotInA, quaternion<OTReal>()));
	m_socket_B.init(*(body1->m_potBody),OTTypes::coordsys(pivotInB, quaternion<OTReal>()));

	m_ball.connect(m_socket_A,m_socket_B);
	m_ball.set_FPS(1.0/0.01f);
	m_ball.set_ERP(0.8);
	g_configuration.add(&m_ball);
}

void palOpenTissueSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {
}
#endif

