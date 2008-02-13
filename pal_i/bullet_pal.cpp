
#include "bullet_pal.h"
#include "bullet_palVehicle.h"
#include "LinearMath/btScalar.h"

FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palBulletPhysics);

FACTORY_CLASS_IMPLEMENTATION(palBulletBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBulletSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBulletCapsuleGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBulletConvexGeometry);

FACTORY_CLASS_IMPLEMENTATION(palBulletBox);
FACTORY_CLASS_IMPLEMENTATION(palBulletSphere);
FACTORY_CLASS_IMPLEMENTATION(palBulletCapsule);
FACTORY_CLASS_IMPLEMENTATION(palBulletConvex);
FACTORY_CLASS_IMPLEMENTATION(palBulletCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palBulletStaticBox);
FACTORY_CLASS_IMPLEMENTATION(palBulletStaticSphere);
FACTORY_CLASS_IMPLEMENTATION(palBulletStaticCapsule);

FACTORY_CLASS_IMPLEMENTATION(palBulletOrientatedTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palBulletTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palBulletTerrainMesh);
FACTORY_CLASS_IMPLEMENTATION(palBulletTerrainHeightmap);

FACTORY_CLASS_IMPLEMENTATION(palBulletSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletRevoluteLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletPrismaticLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletGenericLink);

FACTORY_CLASS_IMPLEMENTATION(palBulletVehicle);

FACTORY_CLASS_IMPLEMENTATION(palBulletPSDSensor);
FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

btDynamicsWorld* g_DynamicsWorld = NULL;

palBulletPhysics::palBulletPhysics() {
	m_dynamicsWorld = 0;
}

const char* palBulletPhysics::GetVersion() {
	static char verbuf[256];
	int v = btGetVersion(); 
	sprintf(verbuf,"Bullet V%d.%d",v/100,v%100);
	return verbuf;
}

void palBulletPhysics::Init(Float gravity_x, Float gravity_y, Float gravity_z) {
	//old code, gee wasn't it nice back then:
	//m_dynamicsWorld = new btDiscreteDynamicsWorld();
	
	
	btBroadphaseInterface*	m_overlappingPairCache;
	btCollisionDispatcher*	m_dispatcher;
	btConstraintSolver*	m_solver;
#if 1
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	m_overlappingPairCache = new btAxisSweep3(worldMin,worldMax);
#else
	m_overlappingPairCache = new btSimpleBroadphase;
#endif
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(collisionConfiguration);
	m_solver = new btSequentialImpulseConstraintSolver();
	
	
//	m_dynamicsWorld = new btSimpleDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_solver);
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_solver,collisionConfiguration);

	m_dynamicsWorld->setGravity(btVector3(gravity_x,gravity_y,gravity_z));
	g_DynamicsWorld = m_dynamicsWorld;
}

void palBulletPhysics::Cleanup() {
}

void palBulletPhysics::Iterate(Float timestep) {
	if (m_dynamicsWorld) {
		//m_dynamicsWorld->stepSimulation(timestep,2);
		m_dynamicsWorld->stepSimulation(timestep,1,timestep);
		//m_dynamicsWorld->stepSimulation(timestep,1,timestep/2);
	}
}


///////////////
palBulletBodyBase::palBulletBodyBase() {
	m_pbtBody = NULL;
	m_pbtMotionState = NULL;
}

void palBulletBodyBase::SetMaterial(palMaterial *material) {
/*	for (unsigned int i=0;i<m_Geometries.size();i++) {
		palBulletGeometry *pbg = m_Geometries[i];
		pbg->m_pbtShape->s
	}*/
	palBodyBase::SetMaterial(material);
	m_pbtBody->setFriction(material->m_fStatic);
	m_pbtBody->setRestitution(material->m_fRestitution);
}

void palBulletBodyBase::BuildBody(Float x, Float y, Float z, Float mass, bool dynamic, btCollisionShape *btShape) {


	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(x,y,z));

	m_pbtMotionState = new btDefaultMotionState(trans);

	btCollisionShape *pShape = 0;
	btVector3 localInertia(0,0,0);

	//no given shape? get from geom
	if (!btShape) {
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (m_Geometries[0]);
		pbtg->m_pbtShape->calculateLocalInertia(mass,localInertia);
		pShape = pbtg->m_pbtShape;
	}

	if (!dynamic) {
		mass = 0;
		if (btShape)
			pShape = btShape;
	}


	m_pbtBody = new btRigidBody(mass,m_pbtMotionState,pShape,localInertia);

	if (!dynamic)
		m_pbtBody->setCollisionFlags(m_pbtBody->getCollisionFlags()|btCollisionObject::CF_STATIC_OBJECT);
	g_DynamicsWorld->addRigidBody(m_pbtBody);
}


void palBulletBodyBase::SetPosition(palMatrix4x4& location) {
	if (m_pbtBody) {
		btTransform newloc;
		newloc.setFromOpenGLMatrix(location._mat);
			m_pbtBody->setWorldTransform(newloc);
	} else {
		palBodyBase::SetPosition(location);
	}
}

palMatrix4x4& palBulletBodyBase::GetLocationMatrix() {
	if (m_pbtBody) {
		m_pbtBody->getWorldTransform().getOpenGLMatrix(m_mLoc._mat);
	}
	return m_mLoc;
}
///////////////
palBulletBody::palBulletBody() {
}

palBulletBody::~palBulletBody() {
	if (m_pbtBody) {
		g_DynamicsWorld->removeRigidBody(m_pbtBody);
		Cleanup();
		delete m_pbtBody->getMotionState();
		delete m_pbtBody;
	}
}


void palBulletCompoundBody::SetPosition(palMatrix4x4& location) {
	if (m_pbtBody) {
#if 0
		btTransform newloc;
		newloc.setFromOpenGLMatrix(location._mat);
		m_pbtBody->getMotionState()->setWorldTransform(newloc);
#else
		palBulletBodyBase::SetPosition(location);
#endif
	} else {
		palBody::SetPosition(location);
	}
}

palMatrix4x4& palBulletCompoundBody::GetLocationMatrix() {
	if (m_pbtBody) {
		btTransform t;
		m_pbtBody->getMotionState()->getWorldTransform(t);
		t.getOpenGLMatrix(m_mLoc._mat);
	}
	return m_mLoc;
}

/*
#define ACTIVE_TAG 1
#define ISLAND_SLEEPING 2
#define WANTS_DEACTIVATION 3
#define DISABLE_DEACTIVATION 4
#define DISABLE_SIMULATION 5
*/
void palBulletBody::SetActive(bool active) {
	if (active) {
		m_pbtBody->setActivationState(ACTIVE_TAG);
		m_pbtBody->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		m_pbtBody->setActivationState(ISLAND_SLEEPING);
	}
}



void palBulletBody::ApplyForce(Float fx, Float fy, Float fz) {
	btVector3 force(fx,fy,fz);
	m_pbtBody->applyCentralForce(force);
}

void palBulletBody::ApplyTorque(Float tx, Float ty, Float tz) {
	btVector3 torque(tx,ty,tz);
	m_pbtBody->applyTorque(torque);
}

void palBulletBody::ApplyImpulse(Float fx, Float fy, Float fz) {
	btVector3 impulse(fx,fy,fz);
	m_pbtBody->applyCentralImpulse(impulse);
}

void palBulletBody::ApplyAngularImpulse(Float fx, Float fy, Float fz) {
	btVector3 impulse(fx,fy,fz);
	m_pbtBody->applyTorqueImpulse(impulse);
}

void palBulletBody::GetLinearVelocity(palVector3& velocity) {
	btVector3 vel = m_pbtBody->getLinearVelocity();
	velocity.x = vel.x();
	velocity.y = vel.y();
	velocity.z = vel.z();
}
void palBulletBody::GetAngularVelocity(palVector3& velocity) {
	btVector3 vel = m_pbtBody->getAngularVelocity();
	velocity.x = vel.x();
	velocity.y = vel.y();
	velocity.z = vel.z();
}

void palBulletBody::SetLinearVelocity(palVector3 velocity) {
	m_pbtBody->setLinearVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}

void palBulletBody::SetAngularVelocity(palVector3 velocity) {
	m_pbtBody->setAngularVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}



palBulletCompoundBody::palBulletCompoundBody() {
	;
}
void palBulletCompoundBody::Finalize() {
	SumInertia();
	btCompoundShape* compound = new btCompoundShape();
	for (VECTOR<palGeometry *>::size_type i=0;i<m_Geometries.size();i++) {
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (m_Geometries[i]);
		
		palMatrix4x4 m = pbtg->GetOffsetMatrix();//GetLocationMatrix();
		
		btTransform localTrans;
		localTrans.setFromOpenGLMatrix(m._mat);
	
		compound->addChildShape(localTrans,pbtg->m_pbtShape);
	}

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(m_fPosX,m_fPosY,m_fPosZ));

	btVector3 localInertia(m_fInertiaXX, m_fInertiaYY, m_fInertiaZZ);
	compound->calculateLocalInertia(m_fMass,localInertia);

	m_pbtMotionState = new btDefaultMotionState(trans);
	m_pbtBody = new btRigidBody(m_fMass,m_pbtMotionState,compound,localInertia);
	g_DynamicsWorld->addRigidBody(m_pbtBody);
}

palBulletGeometry::palBulletGeometry() {
	m_pbtShape = NULL;
}

palBulletGeometry::~palBulletGeometry() {
	if (m_pbtShape)
		delete m_pbtShape;
}

palBulletBoxGeometry::palBulletBoxGeometry() {
	m_pbtBoxShape = NULL;
}

void palBulletBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	m_pbtBoxShape = new btBoxShape(btVector3(width*(Float)0.5,height*(Float)0.5,depth*(Float)0.5));
	m_pbtShape = m_pbtBoxShape;
}

palBulletBox::palBulletBox() {
}

void palBulletBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	BuildBody(x,y,z,mass);
}

palBulletStaticBox::palBulletStaticBox() {
}

void palBulletStaticBox::Init(palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palStaticBox::Init(pos,width,height,depth);
	BuildBody(m_fPosX,m_fPosY,m_fPosZ,false,0);
	palBulletBodyBase::SetPosition(pos);
}

palBulletStaticSphere::palBulletStaticSphere() {
}

void palBulletStaticSphere::Init(palMatrix4x4 &pos, Float radius) {
	palStaticSphere::Init(pos,radius);
	BuildBody(m_fPosX,m_fPosY,m_fPosZ,false,0);
	palBulletBodyBase::SetPosition(pos);
}

palBulletStaticCapsule::palBulletStaticCapsule() {
}
	
void palBulletStaticCapsule::Init(Float x, Float y, Float z, Float radius, Float length) {
	palStaticCapsule::Init(x,y,z,radius,length);
	BuildBody(m_fPosX,m_fPosY,m_fPosZ,false,0);
}
	

palBulletSphereGeometry::palBulletSphereGeometry() {
	m_btSphereShape = NULL;
}

void palBulletSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	m_btSphereShape = new btSphereShape(radius); // this seems wrong!
	m_btSphereShape->setMargin(0.0f); //wwiiiieerrrddd
	m_pbtShape = m_btSphereShape;
	
}

palBulletCapsuleGeometry::palBulletCapsuleGeometry() {
	m_btCylinderShape = NULL;
}

void palBulletCapsuleGeometry::Init(palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	palCapsuleGeometry::Init(pos,radius,length,mass);
	m_btCylinderShape = new btCylinderShape (btVector3(radius,length,radius));
	m_pbtShape = m_btCylinderShape;
}


palBulletSphere::palBulletSphere() {
}

void palBulletSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	BuildBody(x,y,z,mass);
}

palBulletCapsule::palBulletCapsule() {
}

void palBulletCapsule::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palCapsule::Init(x,y,z,radius,length,mass);
	BuildBody(x,y,z,mass);
}

palBulletOrientatedTerrainPlane::palBulletOrientatedTerrainPlane() {
}

void palBulletOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size) {
	palOrientatedTerrainPlane::Init(x,y,z,nx,ny,nz,min_size);

	btVector3 normal(nx,ny,nz);
	normal.normalize();
	m_pbtPlaneShape = new btStaticPlaneShape(normal,CalculateD());

	BuildBody(x,y,z,0,false,m_pbtPlaneShape);
#if 0
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));
	btTransform final_groundTransform;
	final_groundTransform.setIdentity();
	final_groundTransform.setOrigin(btVector3(0,0,0));
//	final_groundTransform.setFromOpenGLMatrix(m_mLoc._mat);
	
	btVector3 localInertia(0,0,0);
	m_pbtMotionState = new btDefaultMotionState(final_groundTransform);
	m_pbtBody = new btRigidBody(0,m_pbtMotionState,m_pbtPlaneShape,localInertia);

	m_pbtBody->setCollisionFlags(m_pbtBody->getCollisionFlags()|btCollisionObject::CF_STATIC_OBJECT);
	g_DynamicsWorld->addRigidBody(m_pbtBody);
#endif
}


palBulletTerrainPlane::palBulletTerrainPlane() {
	m_pbtBody = NULL;
	m_pbtBoxShape = NULL;

}
void palBulletTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
	m_pbtBoxShape = new btBoxShape(btVector3(min_size*(Float)0.5,(Float)1,min_size*(Float)0.5));

	BuildBody(x,y-1,z,0,false,m_pbtBoxShape);
}

palBulletTerrainMesh::palBulletTerrainMesh() {

}
void palBulletTerrainMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palTerrainMesh::Init(x,y,z,pVertices,nVertices,pIndices,nIndices);

	btTriangleMesh* trimesh = new btTriangleMesh();
	int pi;
	for (int i=0;i<nIndices/3;i++) {
		pi = pIndices[i*3+0];
		btVector3 v0(	pVertices[pi*3+0],
						pVertices[pi*3+1],
						pVertices[pi*3+2]);
		pi = pIndices[i*3+1];
		btVector3 v1(	pVertices[pi*3+0],
						pVertices[pi*3+1],
						pVertices[pi*3+2]);
		pi = pIndices[i*3+2];
		btVector3 v2(	pVertices[pi*3+0],
						pVertices[pi*3+1],
						pVertices[pi*3+2]);
		trimesh->addTriangle(v0,v1,v2);
	}
	/*
	btTriangleIndexVertexArray* indexVertexArrays = new btTriangleIndexVertexArray(
		nIndices/3,
		const_cast<int *>(pIndices),
		sizeof(int)*3,
		nVertices,
		const_cast<float *>(pVertices),
		sizeof(Float)*3);
	*/
	//m_pbtTriMeshShape = new btBvhTriangleMeshShape(indexVertexArrays,true);

	 //new btConvexTriangleMeshShape(trimesh);
	m_pbtTriMeshShape = new btBvhTriangleMeshShape(trimesh,true);
/*
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(x,y,z));

	btVector3 localInertia(0,0,0);
//	m_pbtTriMeshShape->calculateLocalInertia(0,localInertia);

	m_pbtMotionState = new btDefaultMotionState(tr);
	m_pbtBody = new btRigidBody(0,m_pbtMotionState,m_pbtTriMeshShape,localInertia);

	g_DynamicsWorld->addRigidBody(m_pbtBody);
	*/
	BuildBody(x,y,z,0,false,m_pbtTriMeshShape);
}
/*
palMatrix4x4& palBulletTerrainMesh::GetLocationMatrix() {
	memset(&m_mLoc,0,sizeof(m_mLoc));
	m_mLoc._11=1;m_mLoc._22=1;m_mLoc._33=1;m_mLoc._44=1;
	m_mLoc._41=m_fPosX;
	m_mLoc._42=m_fPosY;
	m_mLoc._43=m_fPosZ;
	return m_mLoc;
}

void palBulletTerrainMesh::SetMaterial(palMaterial *material) {
	m_pbtBody->setFriction(material->m_fStatic);
	m_pbtBody->setRestitution(material->m_fRestitution);
}
*/
palBulletTerrainHeightmap::palBulletTerrainHeightmap() {
}

void palBulletTerrainHeightmap::Init(Float px, Float py, Float pz, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap) {
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
	palBulletTerrainMesh::Init(px,py,pz,v,nv,ind,ni);

	delete [] v;
	delete [] ind;
}


palBulletSphericalLink::palBulletSphericalLink() {
	m_btp2p = NULL;
}


void palBulletSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palSphericalLink::Init(parent,child,x,y,z);
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);
	
	palMatrix4x4 a = parent->GetLocationMatrix();
	palMatrix4x4 b = child->GetLocationMatrix();
	
	btVector3 pivotInA(x-a._41,y-a._42,z-a._43);
	btVector3 pivotInB = body1->m_pbtBody->getCenterOfMassTransform().inverse()(body0->m_pbtBody->getCenterOfMassTransform()(pivotInA)) ;

	m_btp2p = new btPoint2PointConstraint(*(body0->m_pbtBody),*(body1->m_pbtBody),pivotInA,pivotInB);
	g_DynamicsWorld->addConstraint(m_btp2p,true);
}

void palBulletSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {

}

palBulletRevoluteLink::palBulletRevoluteLink() {
	m_btHinge = NULL;
}

void palBulletRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);

	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);
	palMatrix4x4 a = parent->GetLocationMatrix();
	palMatrix4x4 b = child->GetLocationMatrix();
	
	btVector3 pivotInA(x-a._41,y-a._42,z-a._43);
	btVector3 pivotInB = body1->m_pbtBody->getCenterOfMassTransform().inverse()(body0->m_pbtBody->getCenterOfMassTransform()(pivotInA)) ;

//	printPalVector(m_pivotA);
//	printPalVector(m_pivotB);

	btVector3 axisInA(axis_x,axis_y,axis_z);
	//btVector3 axisInB = body1->m_pbtBody->getCenterOfMassTransform().inverse()(body0->m_pbtBody->getCenterOfMassTransform()(axisInA)) ;
//	btVector3 axisInB = (body1->m_pbtBody->getCenterOfMassTransform().getBasis().inverse()*(body1->m_pbtBody->getCenterOfMassTransform().getBasis() * axisInA));
	btVector3 axisInB = axisInA;
	m_btHinge = new btHingeConstraint(*(body0->m_pbtBody),*(body1->m_pbtBody),
		pivotInA,pivotInB,axisInA,axisInB);
		/*
btHingeConstraint (btRigidBody &rbA, btRigidBody &rbB, const btVector3 &pivotInA, const btVector3 &pivotInB, 
btVector3 &axisInA, btVector3 &axisInB)
		*/
	g_DynamicsWorld->addConstraint(m_btHinge,true);
}

void palBulletRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {

}

palBulletPrismaticLink::palBulletPrismaticLink() {
	m_btSlider = NULL;

}

void palBulletPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palPrismaticLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);
	
	btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInB = btTransform::getIdentity();
		

	btGeneric6DofConstraint* m_btSlider = new btGeneric6DofConstraint(*(body0->m_pbtBody),*(body1->m_pbtBody),
		frameInA,frameInB,true);
btVector3 lowerSliderLimit = btVector3(btScalar(-1e30),0,0);
btVector3 hiSliderLimit = btVector3(btScalar(1e30),0,0);
		m_btSlider->setLinearLowerLimit(lowerSliderLimit);
		m_btSlider->setLinearUpperLimit(hiSliderLimit);


	g_DynamicsWorld->addConstraint(m_btSlider);
}
//////////////////////////////

void palBulletConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	palConvexGeometry::Init(pos,pVertices,nVertices,mass);
	m_pbtConvexShape = new btConvexHullShape(pVertices,nVertices,sizeof(Float)*3);
	m_pbtShape = m_pbtConvexShape;
}

palBulletConvex::palBulletConvex() {
}

void palBulletConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	BuildBody(x,y,z,mass);
}

//////////////////////////////

palBulletPSDSensor::palBulletPSDSensor() {
	;
}

void palBulletPSDSensor::Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range) {
	palPSDSensor::Init(body,x,y,z,dx,dy,dz,range);
	palVector3 pos;
	body->GetPosition(pos);
	m_fRelativePosX = m_fPosX - pos.x;
	m_fRelativePosY = m_fPosY - pos.y;
	m_fRelativePosZ = m_fPosZ - pos.z;
}
#if 0
#include <GL/gl.h>
#pragma comment (lib, "opengl32.lib")
#endif
Float palBulletPSDSensor::GetDistance() {
	btVector3 from;
	btVector3 to;
	palMatrix4x4 m;
	palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();
	palMatrix4x4 out;

	mat_identity(&m);
	mat_translate(&m,m_fRelativePosX,m_fRelativePosY,m_fRelativePosZ);
	mat_multiply(&out,&bodypos,&m);
	from.setX(out._41);
	from.setY(out._42);
	from.setZ(out._43);

	mat_identity(&m);
	mat_translate(&m,m_fAxisX,m_fAxisY,m_fAxisZ);
	mat_multiply(&out,&bodypos,&m);

	palVector3 newaxis;
	newaxis.x=out._41-bodypos._41;
	newaxis.y=out._42-bodypos._42;
	newaxis.z=out._43-bodypos._43;
	vec_norm(&newaxis);


	to.setX(from.x()+newaxis.x*m_fRange);
	to.setY(from.y()+newaxis.y*m_fRange);
	to.setZ(from.z()+newaxis.z*m_fRange);

	btCollisionWorld::ClosestRayResultCallback rayCallback(from,to);
#if 0
glBegin(GL_POINTS);
glVertex3f(from.x(), from.y(), from.z());
glEnd( );

glBegin(GL_LINES);
glVertex3f(from.x(), from.y(), from.z());
glVertex3f(to.x(), to.y(), to.z());
glEnd( );
#endif

	g_DynamicsWorld->rayTest(from, to, rayCallback);
	if (rayCallback.HasHit())
	{
		
		btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
		if (body)
		{
			return m_fRange*rayCallback.m_closestHitFraction;
	/*		btVector3	m_hitPointInWorld;
			btVector3	m_hitNormalInWorld;
			btScalar	m_distFraction;
			result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
			result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
			result.m_hitNormalInWorld.normalize();
			result.m_distFraction = rayCallback.m_closestHitFraction;
			*/
			
		}
	} 
	return m_fRange;
}


palBulletGenericLink::palBulletGenericLink() {
}
void palBulletGenericLink::Init(palBody *parent, palBody *child, 
								palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
								palVector3 linearLowerLimits, palVector3 linearUpperLimits,
								palVector3 angularLowerLimits, palVector3 angularUpperLimits)
{

	palBulletBody *body0 = dynamic_cast<palBulletBody *> (parent);
	palBulletBody *body1 = dynamic_cast<palBulletBody *> (child);

	btTransform frameInA, frameInB;
	frameInA.setFromOpenGLMatrix(parentFrame._mat);
	frameInB.setFromOpenGLMatrix(childFrame._mat);

	btGeneric6DofConstraint* genericConstraint = new btGeneric6DofConstraint(
		*(body0->m_pbtBody),*(body1->m_pbtBody),
		frameInA,frameInB,true);

	genericConstraint->setLinearLowerLimit(btVector3(linearLowerLimits.x,linearLowerLimits.y,linearLowerLimits.z));
	genericConstraint->setLinearUpperLimit(btVector3(linearUpperLimits.x,linearUpperLimits.y,linearUpperLimits.z));
	genericConstraint->setAngularLowerLimit(btVector3(angularLowerLimits.x,angularLowerLimits.y,angularLowerLimits.z));
	genericConstraint->setAngularUpperLimit(btVector3(angularUpperLimits.x,angularUpperLimits.y,angularUpperLimits.z));

	g_DynamicsWorld->addConstraint(genericConstraint);
}
