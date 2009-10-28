#include "jiggle_pal.h"

FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;

#if (JIGLIB_V >= 830)
FACTORY_CLASS_IMPLEMENTATION(palJiggleMaterialUnique);
#endif

FACTORY_CLASS_IMPLEMENTATION(palJiggleBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palJiggleSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palJiggleCylinderGeometry);

FACTORY_CLASS_IMPLEMENTATION(palJigglePhysics);

FACTORY_CLASS_IMPLEMENTATION(palJiggleBox);
FACTORY_CLASS_IMPLEMENTATION(palJiggleSphere);
FACTORY_CLASS_IMPLEMENTATION(palJiggleCylinder);

FACTORY_CLASS_IMPLEMENTATION(palJiggleOrientatedTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palJiggleTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palJiggleTerrainMesh);

FACTORY_CLASS_IMPLEMENTATION(palJiggleSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palJiggleRevoluteLink);
FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

using namespace JigLib;

// Our physics system
JigLib::tPhysicsSystem gPhysics;

// create the collision system on the heap since we might want to
// choose the type at run-time (from config)
JigLib::tCollisionSystem * gCollisionSystem;
/*
template <typename tBody, typename tVector> UpdateTracker {
};
*/


#if (JIGLIB_V >= 830)
static int g_materialcount = 1;

palJiggleMaterialUnique::palJiggleMaterialUnique() {
}

void palJiggleMaterialUnique::Init(PAL_STRING name,Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterialUnique::Init(name,static_friction,kinetic_friction,restitution);
	m_idx=g_materialcount;
	m_jProp = tMaterialProperties(m_fRestitution, m_fStatic, m_fKinetic);

	g_materialcount++;
}
#endif

palJigglePhysics::palJigglePhysics() {
}

JigLib::tPhysicsSystem* palJigglePhysics::JiggleGetPhysicsSystem() {
	return &gPhysics;
}

const char* palJigglePhysics::GetVersion() {
	static char verbuf[256];
	sprintf(verbuf,"JigLib V%d.%2d",JIGLIB_V/100,JIGLIB_V%100);
	return verbuf;
}

const char* palJigglePhysics::GetPALVersion() {
	static char verbuf[512];
	sprintf(verbuf,"PAL SDK V%d.%d.%d\nPAL JigLib V:%d.%d.%d\nFile: %s\nCompiled: %s %s\nModified:%s",
		PAL_SDK_VERSION_MAJOR,PAL_SDK_VERSION_MINOR,PAL_SDK_VERSION_BUGFIX,
		JIGLIB_PAL_SDK_VERSION_MAJOR,JIGLIB_PAL_SDK_VERSION_MINOR,JIGLIB_PAL_SDK_VERSION_BUGFIX,
		__FILE__,__TIME__,__DATE__,__TIMESTAMP__);
	return verbuf;
}


void palJigglePhysics::Init(palPhysicsDesc& desc) {
	palPhysics::Init(desc);
	gPhysics.SetGravity(tVector3(m_fGravityX, m_fGravityY, m_fGravityZ));
	gCollisionSystem = new tCollisionSystemBrute();
	gPhysics.SetCollisionSystem(gCollisionSystem);
// Danny added
	gPhysics.SetNumCollisionIterations(5);
	gPhysics.SetNumContactIterations(5);
	gPhysics.SetNumPenetrationRelaxationTimesteps(2);
	gPhysics.SetAllowedPenetration(0.001f);
	gPhysics.SetDoShockStep(false);
	gPhysics.SetCollToll(0.01f);
	gPhysics.SetSolverType(tPhysicsSystem::SOLVER_ACCUMULATED);

	gCollisionSystem->SetUseSweepTests(true);
}

void palJigglePhysics::Cleanup() {
	printf("TODO!\n");
}

void palJigglePhysics::Iterate(Float timestep) {
     gPhysics.Integrate(timestep);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palJiggleBody :: palJiggleBody () {
	m_pjBody = NULL;
}

void palJiggleBody::SetPosition(palMatrix4x4& loc) {
	if (!m_pjBody) return;
/*
	tVector3 pos;
	pos.Set(loc._41,loc._42,loc._43);
	m_pjBody->SetPosition(pos);*/
#if 0
	tVector3 pos;
	pos.Set(loc._41,loc._42,loc._43);
	m_pjBody->SetPosition(pos);

	tMatrix33 rot;

	rot[0][0] = loc._11;
	rot[1][0] = loc._21;
	rot[2][0] = loc._31;

	rot[0][1] = loc._12;
	rot[1][1] = loc._22;
	rot[2][1] = loc._32;

	rot[0][2] = loc._13;
	rot[1][2] = loc._23;
	rot[2][2] = loc._33;

	m_pjBody->SetOrientation(rot);
#endif //no idea why this doesn't work - broken only since jiglib release 0.7

}

palMatrix4x4& palJiggleBody::GetLocationMatrix() {
	if (!m_pjBody) return m_mLoc;
	palMatrix4x4 &Loc=m_mLoc;

	JigLib::tVector3 pos;
	JigLib::tMatrix33 rot;

	pos = m_pjBody->GetPosition();
	rot = m_pjBody->GetOrientation();

	Loc._11 = rot[0][0];
	Loc._21 = rot[1][0];
	Loc._31 = rot[2][0];
	Loc._41 = pos[0];

	Loc._12 = rot[0][1];
	Loc._22 = rot[1][1];
	Loc._32 = rot[2][1];
	Loc._42 = pos[1];

	Loc._13 = rot[0][2];
	Loc._23 = rot[1][2];
	Loc._33 = rot[2][2];
	Loc._43 = pos[2];

	Loc._14 = 0;
	Loc._24 = 0;
	Loc._34 = 0;
	Loc._44 = 1.0f;

	return m_mLoc;
}

bool palJiggleBody::IsActive()
{
	return m_pjBody->IsActive();
}

void palJiggleBody::SetActive(bool active) {
	if (active)
		m_pjBody->SetActive();
	else
		m_pjBody->SetInactive();
}

#if 0
void palJiggleBody::SetForce(Float fx, Float fy, Float fz) {
	m_pjBody->SetForce(tVector3(fx,fy,fz));
}

void palJiggleBody::GetForce(palVector3& force) {
	tVector3 f=m_pjBody->GetForce();
	force.x=f.x;
	force.y=f.y;
	force.z=f.z;
}
void palJiggleBody::SetTorque(Float tx, Float ty, Float tz) {
	m_pjBody->SetTorque(tVector3(tx,ty,tz));
}
void palJiggleBody::GetTorque(palVector3& torque) {
	tVector3 f=m_pjBody->GetTorque();
	torque.x=f.x;
	torque.y=f.y;
	torque.z=f.z;
}

void palJiggleBody::ApplyForce(Float fx, Float fy, Float fz) {
	tVector3 f(fx,fy,fz);
	m_pjBody->AddWorldForce(f);
}

void palJiggleBody::ApplyTorque(Float fx, Float fy, Float fz) {
	tVector3 f(fx,fy,fz);
	m_pjBody->AddWorldTorque(f);
}
#endif

void palJiggleBody::GetLinearVelocity(palVector3& velocity) {
	tVector3 f=m_pjBody->GetVelocity();
	velocity.x=f.x;
	velocity.y=f.y;
	velocity.z=f.z;
}

void palJiggleBody::GetAngularVelocity(palVector3& velocity_rad) {
	tVector3 f=m_pjBody->GetAngVel();
	velocity_rad.x=f.x;
	velocity_rad.y=f.y;
	velocity_rad.z=f.z;
}

void palJiggleBody::SetLinearVelocity(palVector3 velocity) {
	m_pjBody->SetVelocity(tVector3(velocity.x,velocity.y,velocity.z));
}
void palJiggleBody::SetAngularVelocity(palVector3 velocity) {
	m_pjBody->SetAngVel(tVector3(velocity.x,velocity.y,velocity.z));
}
/*
   void ApplyBodyImpulse(const tVector3 & impulse);
    void ApplyBodyImpulse(const tVector3 & impulse, const tVector3 & pos);
    void ApplyBodyAngImpulse(const tVector3 & angImpulse);*/

void palJiggleBody::ApplyImpulse(Float fx, Float fy, Float fz) {
	m_pjBody->ApplyBodyImpulse(tVector3(fx,fy,fz));
}

void palJiggleBody::ApplyAngularImpulse(Float fx, Float fy, Float fz) {
	m_pjBody->ApplyBodyAngImpulse(tVector3(fx,fy,fz));
}

void palJiggleBody::SetMaterial(palMaterial *material) {
	unsigned int i;
	for (i=0;i<m_Geometries.size();i++) {
		palJiggleGeometry *pjg = dynamic_cast<palJiggleGeometry *>(m_Geometries[i]);
		if (pjg) {
		//	printf("setting %f %f %f",material->m_fRestitution,material->m_fStatic, material->m_fKinetic);
#if (JIGLIB_V < 830)
			pjg->m_pjSkin->SetElasticity(material->m_fRestitution);
			pjg->m_pjSkin->SetFriction(material->m_fStatic, material->m_fKinetic);
#else
//			palJiggleMaterialUnique *jmat = dynamic_cast<palJiggleMaterialUnique *>(material);
//			pjg->m_pjSkin->SetMaterialProperties(jmat->m_idx,jmat->m_jProp);
//			pjg->m_pjSkin->SetMaterialProperties(0,jmat->m_jProp);
			pjg->m_pjSkin->SetMaterialProperties(0, tMaterialProperties(material->m_fRestitution, material->m_fStatic, material->m_fKinetic));
#endif
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palJiggleGeometry::palJiggleGeometry() {
	m_pjSkin = NULL;
}

palJiggleBoxGeometry::palJiggleBoxGeometry() {
#if (JIGLIB_V < 830)
	m_pjBoxSkin = NULL;
#endif
}

void palJiggleBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	JigLib::tVector3 sides;
	sides.Set(width,height,depth);
#if (JIGLIB_V < 830)
	m_pjBoxSkin = new tBoxSkin;

	m_pjBoxSkin->SetBoxesLocal(tBox(-0.5f * sides,
		tMatrix33::Identity(),
		sides));
		#else
	m_pjBoxSkin = new tCollisionSkin;

  m_pjBoxSkin->AddPrimitive(tBox(-0.5f * sides,
                            tMatrix33::Identity(),
                            sides), tMaterialTable::UNSET,
                            tMaterialProperties(0, 1, 1));

		#endif


	m_pjSkin = m_pjBoxSkin;
	palJiggleBody *pjb=dynamic_cast<palJiggleBody *>(m_pBody);


	m_pjSkin->SetOwner(pjb->JiggleGetBody());
	//SetProperties(0.6f, 0.8f, 0.6f);
	pjb->JiggleGetBody()->SetCollisionSkin(m_pjSkin);

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palJiggleSphereGeometry::palJiggleSphereGeometry() {
	m_pjSphereSkin = NULL;
}

void palJiggleSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
#if (JIGLIB_V < 830)
	m_pjSphereSkin = new tSphereSkin;
		m_pjSphereSkin->SetSpheresLocal(tSphere(tVector3(0.0f), radius));
	#else
		m_pjSphereSkin = new tCollisionSkin;
		tVector3 sides(radius, radius, radius);
 m_pjSphereSkin->AddPrimitive(tSphere(tVector3(0.0f), radius), tMaterialTable::UNSET);
  m_pjSphereSkin->SetMaterialProperties(0, tMaterialProperties(0, 1, 1));
	#endif
	m_pjSkin = m_pjSphereSkin;



	palJiggleBody *pjb=dynamic_cast<palJiggleBody *>(m_pBody);
	m_pjSkin->SetOwner(pjb->JiggleGetBody());
  	pjb->JiggleGetBody()->SetCollisionSkin(m_pjSkin);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palJiggleCylinderGeometry::palJiggleCylinderGeometry() {
	m_pjCapsuleSkin = NULL;
}

void palJiggleCylinderGeometry::Init(palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	palCapsuleGeometry::Init(pos,radius,length,mass);


	tMatrix33 orient;
	orient = RotationMatrix(90.0f, tVector3(0,0,1));

#if (JIGLIB_V < 830)
	m_pjCapsuleSkin = new tCapsuleSkin;

	//orient = orient.Identity();

	m_pjCapsuleSkin->SetCapsulesLocal(tCapsule( tVector3(0.0f, -0.5f * length, 0.0f), orient, radius, length));
	#else
 m_pjCapsuleSkin->AddPrimitive(tCapsule(orient * tVector3(-0.5f * length, 0.0f, 0.0f), orient, radius, length),
    tMaterialTable::UNSET);

	#endif

	m_pjSkin = m_pjCapsuleSkin;

	palJiggleBody *pjb=dynamic_cast<palJiggleBody *>(m_pBody);
	m_pjSkin->SetOwner(pjb->JiggleGetBody());
  	pjb->JiggleGetBody()->SetCollisionSkin(m_pjSkin);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palJiggleBox::palJiggleBox() {
}

void palJiggleBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	m_pjBody = new tBody;

	palBox::Init(x,y,z,width,height,depth,mass);

	JigLib::tVector3 sides;
	sides.Set(width,height,depth);

	palJiggleBoxGeometry *pjg=dynamic_cast<palJiggleBoxGeometry *> (m_Geometries[0]);
/*	pjg->m_pjBoxSkin->SetOwner(m_pjBody);
	m_pjBody->SetCollisionSkin(pjg->m_pjBoxSkin);
*/
	m_pjBody->SetMass(mass);

  tScalar Ixx = (1.0f / 12.0f) * m_pjBody->GetMass() *
    (Sq(sides.y) + Sq(sides.z));
  tScalar Iyy = (1.0f / 12.0f) * m_pjBody->GetMass() *
    (Sq(sides.x) + Sq(sides.z));
  tScalar Izz = (1.0f / 12.0f) * m_pjBody->GetMass() *
    (Sq(sides.x) + Sq(sides.y));

	m_pjBody->SetBodyInertia(Ixx, Iyy, Izz);

	tVector3 pos;
	pos.Set(x,y,z);

	m_pjBody->SetPosition(pos);

	m_pjBody->EnableBody();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palJiggleSphere::palJiggleSphere() {
}

void palJiggleSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	m_pjBody = new tBody;
	palSphere::Init(x,y,z,radius,mass);

	//palJiggleSphereGeometry *pjg=dynamic_cast<palJiggleBoxGeometry *> (m_Geometries[0]);

	m_pjBody->SetMass(mass);
	tScalar I = 0.2f * m_pjBody->GetMass() * radius;
	m_pjBody->SetBodyInertia(I, I, I);

	tVector3 pos;
	pos.Set(x,y,z);
	m_pjBody->SetPosition(pos);
	m_pjBody->EnableBody();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palJiggleCylinder::palJiggleCylinder() {
}

void palJiggleCylinder::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	m_pjBody = new tBody;
	palCapsule::Init(x,y,z,radius,length,mass);

	m_pjBody->SetMass(mass);

	palJiggleCylinderGeometry *pjg=dynamic_cast<palJiggleCylinderGeometry *> (m_Geometries[0]);
	if (pjg) {
		m_pjBody->SetBodyInertia(pjg->m_fInertiaXX, pjg->m_fInertiaYY, pjg->m_fInertiaZZ);
	}

	tVector3 pos;
	pos.Set(x,y,z);
	m_pjBody->SetPosition(pos);
	m_pjBody->EnableBody();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palJiggleTerrainPlane::palJiggleTerrainPlane() {

}
void palJiggleTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
		tPlane plane(tVector3(0,1,0), tVector3(x,y,z));

		#if (JIGLIB_V < 830)
	m_pjPlaneSkin = new tPlaneSkin;

	m_pjPlaneSkin->SetPlanesLocal(plane);
	#else
	m_pjPlaneSkin = new tCollisionSkin;
  m_pjPlaneSkin->AddPrimitive(plane, tMaterialTable::UNSET);
	#endif
  	gCollisionSystem->AddCollisionSkin(m_pjPlaneSkin);
}

void palJiggleTerrainPlane::InitND(Float nx,Float ny, Float nz, Float d) {
	tPlane plane(tVector3(nx,ny,nz), d);
#if (JIGLIB_V < 830)
	m_pjPlaneSkin = new tPlaneSkin;
	m_pjPlaneSkin->SetPlanesLocal(plane);
#else
	m_pjPlaneSkin = new tCollisionSkin;
	m_pjPlaneSkin->AddPrimitive(plane, tMaterialTable::UNSET);
#endif
	gCollisionSystem->AddCollisionSkin(m_pjPlaneSkin);
}

palMatrix4x4& palJiggleTerrainPlane::GetLocationMatrix() {
	memset(&m_mLoc,0,sizeof(m_mLoc));
	m_mLoc._11=1;m_mLoc._22=1;m_mLoc._33=1;m_mLoc._44=1;
	m_mLoc._41=m_fPosX;
	m_mLoc._42=m_fPosY;
	m_mLoc._43=m_fPosZ;
	return m_mLoc;
}

palJiggleOrientatedTerrainPlane::palJiggleOrientatedTerrainPlane() {
}

void palJiggleOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size) {
	palOrientatedTerrainPlane::Init(x,y,z,nx,ny,nz,min_size);

	tPlane plane(tVector3(nx,ny,nz), tVector3(x,y,z));
	m_pjPlaneSkin = new tCollisionSkin;
	m_pjPlaneSkin->AddPrimitive(plane, tMaterialTable::UNSET);
	gCollisionSystem->AddCollisionSkin(m_pjPlaneSkin);
}

void palJiggleTerrainPlane::SetMaterial(palMaterial *material) {
#if (JIGLIB_V < 830)
	m_pjPlaneSkin->SetElasticity(material->m_fRestitution);
	m_pjPlaneSkin->SetFriction(material->m_fStatic, material->m_fKinetic);
#else
	m_pjPlaneSkin->SetMaterialProperties(0, tMaterialProperties(material->m_fRestitution, material->m_fStatic, material->m_fKinetic));
#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palJiggleTerrainMesh::palJiggleTerrainMesh() {
}

void palJiggleTerrainMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palTerrainMesh::Init(x,y,z,pVertices,nVertices,pIndices,nIndices);


	//tPlane plane(tVector3(0,1,0), tVector3(x,y,z));
	//m_pjPlaneSkin->SetPlanesLocal(plane);
	//gCollisionSystem->AddCollisionSkin(m_pjPlaneSkin);
	    int maxTrianglesPerCell = 4;
    tScalar minCellSize = 1.0f;
	std::vector<tVector3> vertices;
	int i;
	for (i=0;i<nVertices;i++) {
		vertices.push_back(tVector3(pVertices[i*3+0],pVertices[i*3+1],pVertices[i*3+2]));
	}
	std::vector<tTriangleVertexIndices> triangleVertexIndices;
	for (i=0;i<nIndices/3;i++) {
		triangleVertexIndices.push_back(tTriangleVertexIndices(pIndices[i*3+0],pIndices[i*3+1],pIndices[i*3+2]));
	}
		#if (JIGLIB_V < 830)
	m_pjMeshSkin = new tStaticMeshSkin;
	   m_pjMeshSkin->CreateMesh(&vertices[0], vertices.size(),
                         &triangleVertexIndices[0], triangleVertexIndices.size(),
                         maxTrianglesPerCell, minCellSize);
#else
m_pjMeshSkin = new tCollisionSkin;
 tTriangleMesh mesh;
  mesh.CreateMesh(&vertices[0], vertices.size(),
                  &triangleVertexIndices[0], triangleVertexIndices.size(),
                  maxTrianglesPerCell, minCellSize);
  m_pjMeshSkin->AddPrimitive(mesh, tMaterialTable::UNSET);
  m_pjMeshSkin->SetMaterialProperties(0, tMaterialProperties(0, 1, 1));
#endif

	gCollisionSystem->AddCollisionSkin(m_pjMeshSkin);
}

palMatrix4x4& palJiggleTerrainMesh::GetLocationMatrix() {
	memset(&m_mLoc,0,sizeof(m_mLoc));
	m_mLoc._11=1;m_mLoc._22=1;m_mLoc._33=1;m_mLoc._44=1;
	m_mLoc._41=m_fPosX;
	m_mLoc._42=m_fPosY;
	m_mLoc._43=m_fPosZ;
	return m_mLoc;
}
void palJiggleTerrainMesh::SetMaterial(palMaterial *material) {
#if 0
	m_pjMeshSkin->SetElasticity(material->m_fRestitution);
	m_pjMeshSkin->SetFriction(material->m_fStatic, material->m_fKinetic);
	#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palJiggleSphericalLink::palJiggleSphericalLink() {
m_pjConstraint = NULL;
}

void palJiggleSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palSphericalLink::Init(parent,child,x,y,z);
	palJiggleBody *body0 = dynamic_cast<palJiggleBody *> (parent);
	palJiggleBody *body1 = dynamic_cast<palJiggleBody *> (child);

	//disable intercollisions
	body0->JiggleGetBody()->GetCollisionSkin()->
      GetNonCollidables().push_back(body1->JiggleGetBody()->GetCollisionSkin());
    body1->JiggleGetBody()->GetCollisionSkin()->
      GetNonCollidables().push_back(body0->JiggleGetBody()->GetCollisionSkin());

	palMatrix4x4 a = parent->GetLocationMatrix();
	palMatrix4x4 b = child->GetLocationMatrix();

	Float diffX = a._41 - b._41;
	Float diffY = a._42 - b._42;
	Float diffZ = a._43 - b._43;
	printf("THIS CODE IS WRONG, IT SHOULD BE TAKING INTO ACCOUNT THE x y z POSITION\n");
	//make a new constraint
	m_pjConstraint = new tConstraintPoint(
        body0->JiggleGetBody(), tVector3(-diffX*0.5f, -diffY*0.5f, -diffZ*0.5f),
        body1->JiggleGetBody(), tVector3(diffX*0.5f, diffX*0.5f, diffZ*0.5f),
        0.0f,
        0.0f);
     m_pjConstraint->EnableConstraint();
      //mConstraints.push_back(constraint);
}

void palJiggleSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {
}

palJiggleRevoluteLink::palJiggleRevoluteLink() {
	m_pjHinge = NULL;
}

void palJiggleRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);

	palJiggleBody *body0 = dynamic_cast<palJiggleBody *> (parent);
	palJiggleBody *body1 = dynamic_cast<palJiggleBody *> (child);

	//disable intercollisions
	body0->JiggleGetBody()->GetCollisionSkin()->
      GetNonCollidables().push_back(body1->JiggleGetBody()->GetCollisionSkin());
    body1->JiggleGetBody()->GetCollisionSkin()->
      GetNonCollidables().push_back(body0->JiggleGetBody()->GetCollisionSkin());

	m_pjHinge = new tHingeJoint;

	palMatrix4x4 a = parent->GetLocationMatrix();
	palMatrix4x4 b = child->GetLocationMatrix();
	Float diffX = x-a._41;

	float radius=m_fRelativePosX;

      m_pjHinge->Initialise(body0->JiggleGetBody(), body1->JiggleGetBody(),
                        tVector3(axis_x,axis_y,axis_z),
                        tVector3(diffX, 0.0f, 0.0f),
                        radius,
                        90.0f, 90.0f,
                        0.0f,
                        0.1f);
      m_pjHinge->EnableHinge();
}

void palJiggleRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {

}
