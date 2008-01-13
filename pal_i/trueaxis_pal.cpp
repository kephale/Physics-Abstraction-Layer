#include "TrueAxis_pal.h"

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. True Axis implementation.
		This enables the use of True Axis via PAL.

		Implementation
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.0.1 : 12/08/04 - Physics
	TODO:
		-get to 1.0 (ie: same as pal.h)
*/
FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisPhysics);

FACTORY_CLASS_IMPLEMENTATION(palTrueAxisBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisCylinderGeometry);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisConvexGeometry);

FACTORY_CLASS_IMPLEMENTATION(palTrueAxisConvex);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisBox);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisSphere);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisCylinder);

FACTORY_CLASS_IMPLEMENTATION(palTrueAxisOrientatedTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisTerrainMesh);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisTerrainHeightmap);

FACTORY_CLASS_IMPLEMENTATION(palTrueAxisSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palTrueAxisRevoluteLink);
FACTORY_CLASS_IMPLEMENTATION_END_GROUP

using namespace TA;

const float k_fWorldExtent = 2000.0f; 

palTrueAxisPhysics::palTrueAxisPhysics() {
}

const char* palTrueAxisPhysics::GetVersion() {
	static char verbuf[256];
	sprintf(verbuf,"TrueAxis V%d.%d.%d.%d",TA_VERSION/1000,(TA_VERSION/100)%10,(TA_VERSION/10)%10,TA_VERSION%10);
	return verbuf;
}

void palTrueAxisPhysics::Init(Float gravity_x, Float gravity_y, Float gravity_z) {
	TA::AABB aabb; 
    aabb.Initialise( 
        TA::Vec3(0.0f, 0.0f, 0.0f),                             // Center. 
        TA::Vec3(k_fWorldExtent, k_fWorldExtent, k_fWorldExtent));  // Extent 
    TA::Physics::CreateInstance( 
        aabb,                               // World bounding box. 
        TA::Vec3(gravity_x, gravity_y, gravity_z),       // Gravity. 
        TA::Physics::FLAG_XZ_COLLISION_GRID);            // Flags. 
    

}

void palTrueAxisPhysics::Iterate(Float timestep) {
	TA::Physics& physics = TA::Physics::GetInstance(); 
	physics.Update(timestep);
}

void palTrueAxisPhysics::Cleanup() {
	//go thru all objects, deleting them :P
    TA::Physics::DestroyInstance(); 
}

/*

	virtual void SetPosition(palMatrix4x4& location);
	virtual palMatrix4x4& GetLocationMatrix();

	virtual void SetForce(Float fx, Float fy, Float fz);
	virtual void GetForce(palVector3& force);

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void SetTorque(Float tx, Float ty, Float tz);
	virtual void GetTorque(palVector3& torque);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetMaterial(palMaterial *material);
	*/

palTrueAxisBody::palTrueAxisBody() {
	m_pDObj = NULL;
	m_nUserGroupID = -1;
	m_nUserGroupItemID = -1;
//	printf("TA::DynamicObject::CreateNew();\n");
}

palTrueAxisBody::~palTrueAxisBody() {
	if (m_pDObj) {
		Cleanup();
		TA::Physics& physics = TA::Physics::GetInstance(); 
		physics.RemoveDynamicObject(m_pDObj);
	}
}

void palTrueAxisBody::SetPosition(palMatrix4x4& location) {
	if (m_pDObj) {
	MFrame mf;
	mf.m33Rotation.SetToIdentity();
	mf.m33Rotation.Initialise((D3DXMATRIX *) location._mat);
	mf.v3Translation.x = location._41;
	mf.v3Translation.y = location._42;
	mf.v3Translation.z = location._43;
	m_pDObj->SetFrame(mf);
	}
}

palMatrix4x4&  palTrueAxisBody::GetLocationMatrix() {
	if (m_pDObj) {
	palMatrix4x4 m;
	TA::MFrame mf = m_pDObj->GetFrame();
	mf.GetAsD3DMatrix((D3DXMATRIX *)m._mat);
	m_mLoc = m;
	}
	return m_mLoc;
}

void palTrueAxisBody::SetActive(bool active) {
	if (active)
		m_pDObj->SetToMoving();
	else
		m_pDObj->SetToResting();
}

void palTrueAxisBody::SetMass(Float mass) {
	m_pDObj->SetMass(mass);
}

#if 0
void palTrueAxisBody::SetForce(Float fx, Float fy, Float fz) {
}

void palTrueAxisBody::GetForce(palVector3& force) {
}

void palTrueAxisBody::SetTorque(Float tx, Float ty, Float tz) {
}
void palTrueAxisBody::GetTorque(palVector3& torque) {
}
#endif


void palTrueAxisBody::ApplyImpulse(Float fx, Float fy, Float fz) {
	 m_pDObj->ApplyLinearImpulse (Vec3(fx,fy,fz));
}
void palTrueAxisBody::ApplyAngularImpulse(Float fx, Float fy, Float fz) {
	m_pDObj->ApplyAngularImpulse (Vec3(fx,fy,fz));
}


void palTrueAxisBody::GetLinearVelocity(palVector3& velocity) {
	Vec3 vel =m_pDObj->GetLinearVelocity ();
	velocity.x=vel.x;
	velocity.y=vel.y;
	velocity.z=vel.z;
}
void palTrueAxisBody::GetAngularVelocity(palVector3& velocity_rad) {
	Vec3 vel =m_pDObj->GetAngularVelocity ();
	velocity_rad.x=vel.x;
	velocity_rad.y=vel.y;
	velocity_rad.z=vel.z;
}

void palTrueAxisBody::SetLinearVelocity(palVector3 velocity) {
	m_pDObj->SetLinearVelocity(Vec3(velocity.x,velocity.y,velocity.z));
}
void palTrueAxisBody::SetAngularVelocity(palVector3 velocity) {
	m_pDObj->SetAngularVelocity(Vec3(velocity.x,velocity.y,velocity.z));
}

void palTrueAxisBody::SetMaterial(palMaterial *material) {
	m_pDObj->SetFriction(material->m_fStatic);
	m_pDObj->SetRestitution(material->m_fRestitution);
}

palTrueAxisGeometry::palTrueAxisGeometry() {
	m_pcoc = CollisionObjectCombo::CreateNew();
//	printf("CollisionObjectCombo::CreateNew();\n");
}

palTrueAxisGeometry::~palTrueAxisGeometry() {
	if (m_pcoc) {
		m_pcoc->Release();
	}
}

palTrueAxisSphereGeometry::palTrueAxisSphereGeometry() {
}

void palTrueAxisSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	//m_pcoc->InitialiseAsASphere(Vec3(pos._41,pos._42,pos._43),radius);
	m_pcoc->InitialiseAsASphere(Vec3(0,0,0),radius);
}

palTrueAxisBoxGeometry::palTrueAxisBoxGeometry() {
}

void palTrueAxisBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	m_pcoc->InitialiseAsABox(width, height, depth);
//	printf("InitialiseAsABox(width, height, depth);\n");
}


palTrueAxisCylinderGeometry::palTrueAxisCylinderGeometry() {
}

void palTrueAxisCylinderGeometry::Init(palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	palCapsuleGeometry::Init(pos,radius,length,mass);
	m_pcoc->InitialiseAsACapsule(Vec3(0,-length/2,0),Vec3(0,length/2,0),radius);
}

palTrueAxisConvexGeometry::palTrueAxisConvexGeometry() {
}

void palTrueAxisConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	palConvexGeometry::Init(pos,pVertices,nVertices,mass);
	TA::Vec3 *v = new TA::Vec3[nVertices];
	for (int i=0;i<nVertices;i++) {
		v[i].x = pVertices[i*3+0];
		v[i].y = pVertices[i*3+1];
		v[i].z = pVertices[i*3+2];
	}
	m_pcoc->InitialiseFromPointList(v,nVertices);
	delete [] v;
}
/////////////////////////////
/*
class palTrueAxisCompoundBody : public palCompoundBody, public palTrueAxisBody {
public:
	palTrueAxisCompoundBody();
	void Finalize();
protected:
	FACTORY_CLASS(palTrueAxisCompoundBody,palCompoundBody,TrueAxis,1)
};
palTrueAxisCompoundBody::palTrueAxisCompoundBody() {
}
palTrueAxisCompoundBody::Finalize() {
	//not included in TA since it can not set geom location&orientations
	SumInertia();

	palTrueAxisGeometry *ptag=dynamic_cast<palTrueAxisGeometry *> (m_Geometries[0]);

	CollisionObjectCombo* pCollisionObjectCombo = ptag->m_pcoc;
	for (int i=1;i<m_Geometries.size();i++) {
		ptag=dynamic_cast<palTrueAxisGeometry *> (m_Geometries[i]);
		pCollisionObjectCombo->AddCollisionObject(ptag->m_pcoc); //this would have to be 'simple'
	}
	BuildBody(m_fPosX,m_fPosY,m_fPosZ);
}
*/
void palTrueAxisBody::BuildBody(Float x, Float y, Float z) {
	m_pDObj = TA::DynamicObject::CreateNew();
	palTrueAxisGeometry *ptg=dynamic_cast<palTrueAxisGeometry *> (m_Geometries[0]);
	m_pDObj->Initialise(ptg->m_pcoc);
	TA::Physics& physics = TA::Physics::GetInstance(); 
	physics.AddDynamicObject(m_pDObj);
	palBody::SetPosition(x,y,z);
}

palTrueAxisConvex::palTrueAxisConvex() {
	m_pDObj = NULL;
}

void palTrueAxisConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	BuildBody(x,y,z);
	SetMass(mass);
}

palTrueAxisBox::palTrueAxisBox() {
}

void palTrueAxisBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	m_pDObj = NULL;
	palBox::Init(x,y,z,width,height,depth,mass);
	BuildBody(x,y,z);
	SetMass(mass);
}

palTrueAxisSphere::palTrueAxisSphere() {
}

void palTrueAxisSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	m_pDObj = NULL;
	palSphere::Init(x,y,z,radius,mass);
	BuildBody(x,y,z);
	SetMass(mass);
}


palTrueAxisCylinder::palTrueAxisCylinder() {
}

void palTrueAxisCylinder::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	m_pDObj = NULL;
	palCapsule::Init(x,y,z,radius,length,mass);
	BuildBody(x,y,z);
	SetMass(mass);
}

void palTrueAxisLink::DisableContacts(palTrueAxisBody *body0, palTrueAxisBody *body1) {
	int nActiveUserGroup = -1;
	if ((body0->m_nUserGroupID>=0) && (body1->m_nUserGroupID<0)) {
		nActiveUserGroup = body0->m_nUserGroupID;
	}
	if ((body0->m_nUserGroupID<0) && (body1->m_nUserGroupID>=0)) {
		nActiveUserGroup = body1->m_nUserGroupID;
	}
	if ((body0->m_nUserGroupID<0) && (body1->m_nUserGroupID<0)) {
		nActiveUserGroup = DynamicObject::CreateUserGroup();
	}
	if (nActiveUserGroup<0) {
		printf("ERROR!!\n");
	}

	body0->m_nUserGroupID=nActiveUserGroup;
	body1->m_nUserGroupID=nActiveUserGroup;
	body0->m_pDObj->SetUserGroup(nActiveUserGroup);
	body1->m_pDObj->SetUserGroup(nActiveUserGroup);
	body0->m_pDObj->SetUserGroupItemId((((int)body0)>>3)&31);
	body1->m_pDObj->SetUserGroupItemId((((int)body1)>>3)&31);
    
	body0->m_pDObj->DisallowCollisionWithUserGroupItemId((((int)body1)>>3)&31);
	body1->m_pDObj->DisallowCollisionWithUserGroupItemId((((int)body0)>>3)&31);
}
palTrueAxisSphericalLink::palTrueAxisSphericalLink() {
}


void TA_paldebug_printvector3(palVector3 *pv) {
	printf("%f %f %f\n",pv->x,pv->y,pv->z);
}

void palTrueAxisSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palSphericalLink::Init(parent,child,x,y,z);

	palTrueAxisBody *body0 = dynamic_cast<palTrueAxisBody *> (parent);
	palTrueAxisBody *body1 = dynamic_cast<palTrueAxisBody *> (child);
	palVector3 pos0;
	palVector3 pos1;
	body0->GetPosition(pos0);
	body1->GetPosition(pos1);
	palVector3 posl;
	posl.x=x; posl.y=y; posl.z=z;
	palVector3 lp0;
	palVector3 lp1;
	vec_sub(&lp0,&posl,&pos0);
	vec_sub(&lp1,&posl,&pos1);

	palVector3 dir0,dir1;
	vec_sub(&dir0,&pos1,&pos0);
	vec_norm(&dir0);
	vec_sub(&dir1,&pos0,&pos1);
	vec_norm(&dir1);
/*
	TA_paldebug_printvector3(&pos0);
	TA_paldebug_printvector3(&pos1);

	printf("lp0:");
	TA_paldebug_printvector3(&lp0);
	printf("lp1:");
	TA_paldebug_printvector3(&lp1);

	body0->m_pDObj->AddJointTypeSocket(body1->m_pDObj,
		Vec3(lp0.x,lp0.y,lp0.z), //center 1
		Vec3(lp1.x,lp1.y,lp1.z), //center 2
		Vec3(dir0.x,dir0.y,dir0.z), //normal1
		Vec3(dir1.x,dir1.y,dir1.z), //normal2
		M_PI);
*/
	TA::Vec3 v3JointWorldPos(x,y,z);

	body0->m_pDObj->AddJointTypeSocket(body1->m_pDObj,
		v3JointWorldPos / body0->m_pDObj->GetFrame(),
		v3JointWorldPos / body1->m_pDObj->GetFrame(),
//		Vec3(lp0.x,lp0.y,lp0.z), //center 1
//		Vec3(lp1.x,lp1.y,lp1.z), //center 2

		Vec3(dir1.x,dir1.y,dir1.z), //normal2
		Vec3(dir0.x,dir0.y,dir0.z), //normal1
		
		(Float)M_PI);


	DisableContacts(body0,body1);
}

void palTrueAxisSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {

}

palTrueAxisRevoluteLink::palTrueAxisRevoluteLink() {

}

void palTrueAxisRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palTrueAxisBody *body0 = dynamic_cast<palTrueAxisBody *> (parent);
	palTrueAxisBody *body1 = dynamic_cast<palTrueAxisBody *> (child);
	TA::Vec3 v3JointWorldPos(x,y,z);

//	Mat33 jointOrientation;
//	jointOrientation.SetToLookAt(Vec3(0,-1,0), Vec3(-1,0,0));
	/*
	v3JointPos = v3LeftLegBottom;
	pULLeg->AddJoint(
		pLLLeg,
		v3JointPos / pULLeg->GetFrame(),
		v3JointPos / pLLLeg->GetFrame(),
		jointOrientation,
		EulerAngles(0.0f, 0.0f, 0.0f),
		EulerAngles(0.0f, k_fPi * 0.85f, 0.0f));
	*/
	body0->m_pDObj->AddJoint(body1->m_pDObj,
		v3JointWorldPos / body0->m_pDObj->GetFrame(),
		v3JointWorldPos / body1->m_pDObj->GetFrame(),
		body1->m_pDObj->GetFrame().m33Rotation,
//		jointOrientation,
		EulerAngles( 0,0,(Float)-M_PI),
		EulerAngles( 0,0,(Float) M_PI)
		);

	DisableContacts(body0,body1);
}

void palTrueAxisRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {

}

palTrueAxisTerrain::palTrueAxisTerrain() {
	m_pSObj = TA::StaticObject::CreateNew(); 
}

palMatrix4x4& palTrueAxisTerrain::GetLocationMatrix() {
	mat_identity(&m_mLoc);
	return m_mLoc;
}

palTrueAxisOrientatedTerrainPlane::palTrueAxisOrientatedTerrainPlane() {
}

void palTrueAxisOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size) {
	palOrientatedTerrainPlane::Init(x,y,z,nx,ny,nz,min_size);

	palVector3 ov1,ov2,ov3,ov4;
	vec_set(&ov1, min_size, 0, min_size);
	vec_set(&ov2,-min_size, 0, min_size);
	vec_set(&ov3,-min_size, 0,-min_size);
	vec_set(&ov4, min_size, 0,-min_size);

	palVector3 v1,v2,v3,v4;
	vec_mat_mul(&v1,&m_mLoc,&ov1);
	vec_mat_mul(&v2,&m_mLoc,&ov2);
	vec_mat_mul(&v3,&m_mLoc,&ov3);
	vec_mat_mul(&v4,&m_mLoc,&ov4);

	 TA::CollisionObjectAABBMesh* pStaticCollisionObject = 
        TA::CollisionObjectAABBMesh::CreateNew(); 
    pStaticCollisionObject->Initialise( 
        4,                              // Num vertices. 
        1,                              // Num polygons. 
        4);                             // Num polygon indices.    
   pStaticCollisionObject->AddVertex(TA::Vec3(v1.x,v1.y,v1.z)); 
    pStaticCollisionObject->AddVertex(TA::Vec3(v2.x,v2.y,v2.z)); 
    pStaticCollisionObject->AddVertex(TA::Vec3(v3.x,v3.y,v3.z)); 
    pStaticCollisionObject->AddVertex(TA::Vec3(v4.x,v4.y,v4.z)); 

    int pnPolygonIndexList[4] = { 0, 1, 2, 3 }; 
    pStaticCollisionObject->AddPolygon(4, pnPolygonIndexList); 
   pStaticCollisionObject->FinishedAddingGeometry(); 

    // Initialise the static object with the collision object. 
    m_pSObj->Initialise(pStaticCollisionObject); 
    pStaticCollisionObject->Release(); // We no long need the reference. 
    pStaticCollisionObject = 0; 

    // Add the static object to the simulation. 
	TA::Physics& physics = TA::Physics::GetInstance(); 
    physics.AddStaticObject(m_pSObj); 
}


palTrueAxisTerrainPlane::palTrueAxisTerrainPlane(){

}

void palTrueAxisTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	 TA::CollisionObjectAABBMesh* pStaticCollisionObject = 
        TA::CollisionObjectAABBMesh::CreateNew(); 
    pStaticCollisionObject->Initialise( 
        4,                              // Num vertices. 
        1,                              // Num polygons. 
        4);                             // Num polygon indices.    
   pStaticCollisionObject->AddVertex(TA::Vec3(min_size, 0, min_size)); 
    pStaticCollisionObject->AddVertex(TA::Vec3(-min_size, 0, min_size)); 
    pStaticCollisionObject->AddVertex(TA::Vec3(-min_size, 0, -min_size)); 
    pStaticCollisionObject->AddVertex(TA::Vec3(min_size, 0, -min_size)); 

    int pnPolygonIndexList[4] = { 0, 1, 2, 3 }; 
    pStaticCollisionObject->AddPolygon(4, pnPolygonIndexList); 
   pStaticCollisionObject->FinishedAddingGeometry(); 

    // Initialise the static object with the collision object. 
    m_pSObj->Initialise(pStaticCollisionObject); 
    pStaticCollisionObject->Release(); // We no long need the reference. 
    pStaticCollisionObject = 0; 

    // Add the static object to the simulation. 
	TA::Physics& physics = TA::Physics::GetInstance(); 
    physics.AddStaticObject(m_pSObj); 
    /*pStaticObject->Release(); // We no long need the reference. 
    pStaticObject = 0; */
}
palTrueAxisTerrainMesh::palTrueAxisTerrainMesh() {
}
void palTrueAxisTerrainMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	TA::CollisionObjectAABBMesh* pStaticCollisionObject = 
		TA::CollisionObjectAABBMesh::CreateNew(); 
	pStaticCollisionObject->Initialise( 
		nVertices,                              // Num vertices. 
		nIndices/3,                              // Num polygons. 
		nIndices);                             // Num polygon indices.    
	for (int i=0;i<nVertices*3;i+=3)
		pStaticCollisionObject->AddVertex(Vec3(pVertices[i], pVertices[i+1], pVertices[i+2])); 
	for (int i=0;i<nIndices;i+=3) {
//		printf("index: %d %d %d\n",pIndices[i],pIndices[i+1],pIndices[i+2]);
		int buf[3];
		buf[0]=pIndices[i+2];
		buf[1]=pIndices[i+1];
		buf[2]=pIndices[i+0];
		pStaticCollisionObject->AddPolygon(3, buf); 
	}
	pStaticCollisionObject->FinishedAddingGeometry(); 

	// Initialise the static object with the collision object. 
    m_pSObj->Initialise(pStaticCollisionObject); 
    pStaticCollisionObject->Release(); // We no long need the reference. 
    pStaticCollisionObject = 0; 

    // Add the static object to the simulation. 
	TA::Physics& physics = TA::Physics::GetInstance(); 
    physics.AddStaticObject(m_pSObj); 
}

palTrueAxisTerrainHeightmap::palTrueAxisTerrainHeightmap() {

}

void palTrueAxisTerrainHeightmap::Init(Float px, Float py, Float pz, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap) {
	palTerrainHeightmap::Init(px,py,pz,width,depth,terrain_data_width,terrain_data_depth,pHeightmap);
	int iTriIndex;
	float fTerrainX, fTerrainZ;
	int x,z;

	int nv=m_iDataWidth*m_iDataDepth;
	int ni=(m_iDataWidth-1)*(m_iDataDepth-1)*2*3;

	Float *v = new Float[nv*3];
	int *ind = new int[ni];

	// Set the vertex values
	fTerrainZ = -m_fDepth/2;
	for (z=0; z<m_iDataDepth; z++)
	{
		fTerrainX = -m_fWidth/2;
		for (x=0; x<m_iDataWidth; x++)
		{
			v[(x + z*m_iDataWidth)*3+0]=fTerrainX;
			v[(x + z*m_iDataWidth)*3+1]=pHeightmap[x+z*m_iDataWidth];
			v[(x + z*m_iDataWidth)*3+2]=fTerrainZ;
		fTerrainX += (m_fWidth / (m_iDataWidth-1));
		}
		fTerrainZ += (m_fDepth / (m_iDataDepth-1));
	}

	iTriIndex = 0;
	int xDim=m_iDataWidth;
	int yDim=m_iDataDepth;
	int y;
	for (y=0;y < yDim-1;y++)
	for (x=0;x < xDim-1;x++) {
		ind[iTriIndex*3+0]=(y*xDim)+x;
		ind[iTriIndex*3+1]=(y*xDim)+xDim+x;
		ind[iTriIndex*3+2]=(y*xDim)+x+1;
		// Move to the next triangle in the array
		iTriIndex += 1;
		
		ind[iTriIndex*3+0]=(y*xDim)+x+1;
		ind[iTriIndex*3+1]=(y*xDim)+xDim+x;
		ind[iTriIndex*3+2]=(y*xDim)+x+xDim+1;
		// Move to the next triangle in the array
		iTriIndex += 1;
	}
	palTrueAxisTerrainMesh::Init(px,py,pz,v,nv,ind,ni);

	delete [] v;
	delete [] ind;
}
/*
	// Get placement height
			Collision collision = Physics::GetInstance().TestLineForCollision(
				v3GroundPos + k_v3UnitY * 1000.0f,
				v3GroundPos -k_v3UnitY * 2000.0f,
				Physics::FLAG_ALL_NON_DYNAMIC_OBJECTS);

			if (collision.CollisionOccurred())
				v3GroundPos = collision.v3Position;*/

palTrueAxisPSDSensor::palTrueAxisPSDSensor() {

}

void palTrueAxisPSDSensor::Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range) {
	palPSDSensor::Init(body,x,y,z,dx,dy,dz,range);
	palVector3 pos;
	body->GetPosition(pos);
	m_fRelativePosX = m_fPosX - pos.x;
	m_fRelativePosY = m_fPosY - pos.y;
	m_fRelativePosZ = m_fPosZ - pos.z;
}

Float palTrueAxisPSDSensor::GetDistance() {
	float pos0[3];
	float pos1[3];
	float PosX,PosY,PosZ;

	palMatrix4x4 m;
	m=m_pBody->GetLocationMatrix();

	PosX=m._41;
	PosY=m._42;
	PosZ=m._43;

	palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();
	palMatrix4x4 out;

	mat_identity(&m);
	mat_translate(&m,m_fRelativePosX,m_fRelativePosY,m_fRelativePosZ);
	mat_multiply(&out,&bodypos,&m);
	pos0[0]=out._41;
	pos0[1]=out._42;
	pos0[2]=out._43;

	mat_identity(&m);
	mat_translate(&m,m_fAxisX,m_fAxisY,m_fAxisZ);
	mat_multiply(&out,&bodypos,&m);

	palVector3 newaxis;
	newaxis.x=out._41-bodypos._41;
	newaxis.y=out._42-bodypos._42;
	newaxis.z=out._43-bodypos._43;
	vec_norm(&newaxis);


	pos1[0]=pos0[0]+newaxis.x*m_fRange;
	pos1[1]=pos0[1]+newaxis.y*m_fRange;
	pos1[2]=pos0[2]+newaxis.z*m_fRange;
	
	Vec3 start(pos0[0],pos0[1],pos0[2]);
	Vec3 end(pos1[0],pos1[1],pos1[2]);
	Collision collision = Physics::GetInstance().TestLineForCollision(
		start,
		end,
		Physics::FLAG_ALL_OBJECTS);
	if (collision.CollisionOccurred()) {
		Vec3 vdist = start - collision.GetPosition();
		return vdist.GetMagnitude();
	}
	return m_fRange;
}
