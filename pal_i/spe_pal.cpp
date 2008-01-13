#include "spe_pal.h"


FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palSPEPhysics);

//FACTORY_CLASS_IMPLEMENTATION(palSPECompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palSPEBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palSPESphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palSPECapsuleGeometry);
FACTORY_CLASS_IMPLEMENTATION(palSPEConvexGeometry);

FACTORY_CLASS_IMPLEMENTATION(palSPEBox);
FACTORY_CLASS_IMPLEMENTATION(palSPESphere);
FACTORY_CLASS_IMPLEMENTATION(palSPECapsule);
FACTORY_CLASS_IMPLEMENTATION(palSPEConvex);

FACTORY_CLASS_IMPLEMENTATION(palSPEStaticBox);

FACTORY_CLASS_IMPLEMENTATION(palSPETerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palSPETerrainMesh);

//FACTORY_CLASS_IMPLEMENTATION(palSPESphericalLink);

FACTORY_CLASS_IMPLEMENTATION(palSPEFluid);

FACTORY_CLASS_IMPLEMENTATION_END_GROUP;



LPSPEWORLD g_pWorld;

palSPEPhysics::palSPEPhysics() {
}

LPSPEWORLD palSPEPhysics::GetWorld() {
	return pWorld;
}

void palSPEPhysics::Init(Float gravity_x, Float gravity_y, Float gravity_z) {
	pWorld=CreateSPEWorld();
	g_pWorld = pWorld;
	pWorld->SetGravity(gravity_x,gravity_y,gravity_z);
}
void palSPEPhysics::Cleanup() {
}
const char* palSPEPhysics::GetVersion() {
	return 0;
}
void palSPEPhysics::Iterate(Float timestep) {
	pWorld->Update (timestep);
}

/////////////
palSPEGeometry::palSPEGeometry() {
	pShape = 0;
}

palSPEGeometry::~palSPEGeometry() {
	if (pShape)
		g_pWorld->ReleaseShape(pShape);
}

void palSPEGeometry::GenericCreate() {
	if (!pShape) {
	pShape=g_pWorld->CreateShape();
	SPERESULT r = pShape->Initialize ((BYTE *)GenerateMesh_Vertices(), sizeof(float)*3, GenerateMesh_Indices(), GetNumberOfIndices()/3 );
	if (!r) {
		SET_ERROR("Error creating geom");
	}
	}
}

/////////////
palSPEConvexGeometry::palSPEConvexGeometry() {
}

void palSPEConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	palConvexGeometry::Init(pos,pVertices,nVertices,mass);
	pShape=g_pWorld->CreateShape();
	SPERESULT r = pShape->Initialize((BYTE *)pVertices,sizeof(float)*3,nVertices);
	if (!r) {
		SET_ERROR("Error creating geom");
	}
}

palSPESphereGeometry::palSPESphereGeometry() {
}
void palSPESphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	pShape=g_pWorld->CreateShape();
	SPERESULT r = pShape->InitializeAsSphere(radius);
	if (!r) {
		SET_ERROR("Error creating geom");
	}
}

/////////////
palSPEBodyBase::palSPEBodyBase() {
	pBody = 0;
}

palSPEBodyBase::~palSPEBodyBase() {
	if (pBody)
		g_pWorld->DeleteRigidBody(pBody);
}

palMatrix4x4& palSPEBodyBase::GetLocationMatrix() {
	if (pBody)
		pBody->GetTransform (m_mLoc._mat);

	return m_mLoc;
}
void palSPEBodyBase::SetPosition(palMatrix4x4& location) {
	if (pBody)
		pBody->SetTransform(location._mat);
	else
		palBodyBase::SetPosition(location);
}
void palSPEBodyBase::SetMaterial(palMaterial *material) {

	pBody->SetFriction(material->m_fStatic);
	pBody->SetElasticity(material->m_fRestitution);
}

void palSPEBodyBase::BuildBody(Float fx, Float fy, Float fz, Float mass, bool dynamic) {
	palSPEGeometry *psg=dynamic_cast<palSPEGeometry *> (m_Geometries[0]);
	if (!psg) {
		SET_ERROR("No geometry");
	}

	psg->GenericCreate();

	pBody = g_pWorld->AddRigidBody (psg->pShape);
	pBody->SetPosition(fx,fy,fz);

	if (dynamic)
		pBody->SetMass(mass);

	pBody->SetBeStatic(!dynamic);
}

/////////////

palSPEBody::palSPEBody() {
}

void palSPEBody::ApplyForce(Float fx, Float fy, Float fz){}
void palSPEBody::ApplyTorque(Float tx, Float ty, Float tz){}

void palSPEBody::ApplyImpulse(Float fx, Float fy, Float fz){}
void palSPEBody::ApplyAngularImpulse(Float fx, Float fy, Float fz){}

void palSPEBody::GetLinearVelocity(palVector3& velocity){
	SPEVector v = pBody->GetVelocity();
	velocity.x = v.x;
	velocity.y = v.y;
	velocity.z = v.z;
}

void palSPEBody::GetAngularVelocity(palVector3& velocity_rad){
	SPEVector v = pBody->GetAngularVelocity();
	velocity_rad.x = v.x;
	velocity_rad.y = v.y;
	velocity_rad.z = v.z;
}

void palSPEBody::SetLinearVelocity(palVector3 velocity){
	pBody->SetVelocity (velocity.x,velocity.y,velocity.z);
}

void palSPEBody::SetAngularVelocity(palVector3 velocity_rad){
	pBody->SetAngularVelocity(velocity_rad.x,velocity_rad.y,velocity_rad.z);
}

void palSPEBody::SetActive(bool active) {}

/////////////

palSPEBox::palSPEBox() {
}
void palSPEBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	BuildBody(x,y,z,mass,true);
}

palSPEStaticBox::palSPEStaticBox() {
}
void palSPEStaticBox::Init(palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palStaticBox::Init(pos,width,height,depth);
	BuildBody(m_fPosX,m_fPosY,m_fPosZ,0,false);
	SetPosition(pos);
}

palSPEStaticSphere::palSPEStaticSphere() {
}

void palSPEStaticSphere::Init(palMatrix4x4 &pos, Float radius) {
	palStaticSphere::Init(pos,radius);
	BuildBody(m_fPosX,m_fPosY,m_fPosZ,0,false);
	SetPosition(pos);
}

palSPEStaticCapsule::palSPEStaticCapsule() {
}

void palSPEStaticCapsule::Init(palMatrix4x4 &pos, Float radius, Float length) {
	palStaticCapsule::Init(pos,radius,length);
	BuildBody(m_fPosX,m_fPosY,m_fPosZ,0,false);
	SetPosition(pos);
}


palSPESphere::palSPESphere() {
}

void palSPESphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	BuildBody(x,y,z,mass,true);
}

palSPECapsule::	palSPECapsule() {
}

void palSPECapsule::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palCapsule::Init(x,y,z,radius,length,mass);
	BuildBody(x,y,z,mass,true);
}

palSPEConvex::palSPEConvex() {
}

void palSPEConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	BuildBody(x,y,z,mass,true);
}

palSPETerrainPlane::palSPETerrainPlane() {
}

void palSPETerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
	GenerateDefaultBoxGeom(0.5f);
	BuildBody(x,y,z,0,false);
}

palSPETerrainMesh::palSPETerrainMesh() {
}

void palSPETerrainMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palTerrainMesh::Init(x,y,z,pVertices,nVertices,pIndices,nIndices);

	LPSPESHAPE pShape;
	pShape=g_pWorld->CreateShape();
	pShape->InitializeForStatic((BYTE*)pVertices, sizeof(float)*3, (int*)pIndices, nIndices/3);
	//	BuildBody(x,y,z,0,false);
	pBody=g_pWorld->AddRigidBody (pShape);
	//pbody->SetBeStatic(true);
	pBody->SetPosition (x, y,z);

}


/////////////////////////////////

	palSPEFluid::palSPEFluid() {
		pFluid = 0;
	}
	void palSPEFluid::Init() {
		pFluid=g_pWorld->AddFluid(); // add a Fluid to SPEWorld
		pFluid->SetMaterial(SPE_WATER); // set material
	}
	void palSPEFluid::AddParticle(Float x, Float y, Float z, Float vx, Float vy, Float vz) {
		SPEVector velocity(vx, vy, vz);
		pFluid->AddParticle(SPEVector(x, y, z), velocity); // add particle to Fluid
	}
	int palSPEFluid::GetNumParticles() {
		return pFluid->GetNumParticles();
	}
	palVector3* palSPEFluid::GetParticlePositions() {
		pos.resize(pFluid->GetNumParticles());
		pFluid->GetParticlePosition(&pos[0],sizeof(palVector3));
		return &pos[0];
	}

	void palSPEFluid::Finalize() {
		;
	}
