#define BULLET_SINGLETHREAD
#ifndef BULLET_SINGLETHREAD
#define USE_PARALLEL_DISPATCHER 1
#else
#undef USE_PARALLEL_DISPATCHER
#endif

#include "bullet_pal.h"
#include "bullet_palVehicle.h"
#include "bullet_palCharacter.h"
#include "LinearMath/btScalar.h"

#ifndef OS_WINDOWS
#define USE_PTHREADS
#endif

#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"

#ifdef USE_PARALLEL_DISPATCHER
#include <BulletMultiThreaded/SpuGatheringCollisionDispatcher.h>
#include <BulletMultiThreaded/PlatformDefinitions.h>

#ifdef USE_LIBSPE2
#include "BulletMultiThreaded/SpuLibspe2Support.h"
#elif defined (WIN32)
#include "BulletMultiThreaded/Win32ThreadSupport.h"
#include "BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.h"

#elif defined (USE_PTHREADS)

#include <BulletMultiThreaded/PosixThreadSupport.h>
#include <BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.h>

#else
//other platforms run the parallel code sequentially (until pthread support or other parallel implementation is added)
#include "BulletMultiThreaded/SequentialThreadSupport.h"
#include "BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.h"
#endif //USE_LIBSPE2

#ifdef USE_PARALLEL_SOLVER
#include "BulletMultiThreaded/SpuParallelSolver.h"
#include "BulletMultiThreaded/SpuSolverTask/SpuParallellSolverTask.h"
#endif //USE_PARALLEL_SOLVER

#endif//USE_PARALLEL_DISPATCHER

FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palBulletPhysics);

FACTORY_CLASS_IMPLEMENTATION(palBulletBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBulletSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBulletCapsuleGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBulletConvexGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBulletConcaveGeometry);

FACTORY_CLASS_IMPLEMENTATION(palBulletBox);
FACTORY_CLASS_IMPLEMENTATION(palBulletSphere);
FACTORY_CLASS_IMPLEMENTATION(palBulletCapsule);
FACTORY_CLASS_IMPLEMENTATION(palBulletConvex);
FACTORY_CLASS_IMPLEMENTATION(palBulletCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palBulletGenericBody);

FACTORY_CLASS_IMPLEMENTATION(palBulletStaticBox);
FACTORY_CLASS_IMPLEMENTATION(palBulletStaticSphere);
FACTORY_CLASS_IMPLEMENTATION(palBulletStaticCapsule);
FACTORY_CLASS_IMPLEMENTATION(palBulletStaticConvex);
FACTORY_CLASS_IMPLEMENTATION(palBulletStaticCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palBulletOrientatedTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palBulletTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palBulletTerrainMesh);
FACTORY_CLASS_IMPLEMENTATION(palBulletTerrainHeightmap);

FACTORY_CLASS_IMPLEMENTATION(palBulletSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletRevoluteLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletPrismaticLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletGenericLink);

FACTORY_CLASS_IMPLEMENTATION(palBulletVehicle);

FACTORY_CLASS_IMPLEMENTATION(palBulletCharacterController);

FACTORY_CLASS_IMPLEMENTATION(palBulletPSDSensor);

FACTORY_CLASS_IMPLEMENTATION(palBulletAngularMotor);

FACTORY_CLASS_IMPLEMENTATION(palBulletPatchSoftBody);
FACTORY_CLASS_IMPLEMENTATION(palBulletTetrahedralSoftBody);

FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

btDynamicsWorld* g_DynamicsWorld = NULL;

#include <iostream>

struct CustomOverlapFilterCallback: public btOverlapFilterCallback
{
	virtual ~CustomOverlapFilterCallback()
	{}
	// return true when pairs need collision
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const {
		palBulletPhysics* physics = static_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics());

		unsigned long proxy0GroupBits = proxy0->m_collisionFilterGroup;
      unsigned long proxy1GroupBits = proxy1->m_collisionFilterGroup;

		palGroup p0Group = convert_to_pal_group(proxy0GroupBits);
      palGroup p1Group = convert_to_pal_group(proxy1GroupBits);

		// if it hasn't been set, then default to colliding.
		size_t maskVectorSize = physics->m_CollisionMasks.size();
		if (maskVectorSize <= size_t(p0Group)
					|| maskVectorSize <= size_t(p1Group)) {
			return true;
		}

		unsigned long proxy0Mask = physics->m_CollisionMasks[p0Group];
		unsigned long proxy1Mask = physics->m_CollisionMasks[p1Group];

		bool collides = ((proxy0GroupBits & proxy1Mask) != 0) && ((proxy1GroupBits & proxy0Mask) != 0);
		return collides;
	}
};


void palBulletPhysics::SetGroupCollision(palGroup a, palGroup b, bool enabled) {
	unsigned long bits = convert_group(a);
	unsigned long other_bits = convert_group(b);

	if (m_CollisionMasks.size() < size_t(std::max(a, b)))
	{
		m_CollisionMasks.resize(std::max(a,b), ~0);
	}

	if (enabled) {
		m_CollisionMasks[a] = m_CollisionMasks[a] | other_bits;
		m_CollisionMasks[b] = m_CollisionMasks[b] | bits;
	} else {
		m_CollisionMasks[a] = m_CollisionMasks[a] & ~other_bits;
		m_CollisionMasks[b] = m_CollisionMasks[b] & ~bits;
	}
}

void palBulletPhysics::SetCollisionAccuracy(Float fAccuracy) {
	;//
}

void palBulletPhysics::RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit) {

	btVector3 from(x,y,z);
	btVector3 dir(dx,dy,dz);
	btVector3 to = from + dir * range;
	btCollisionWorld::ClosestRayResultCallback rayCallback(from,to);

	g_DynamicsWorld->rayTest(from, to, rayCallback);
	if (rayCallback.hasHit())
	{
		hit.Clear();
		hit.SetHitPosition(rayCallback.m_hitPointWorld.x(),rayCallback.m_hitPointWorld.y(),rayCallback.m_hitPointWorld.z());
		hit.SetHitNormal(rayCallback.m_hitNormalWorld.x(),rayCallback.m_hitNormalWorld.y(),rayCallback.m_hitNormalWorld.z());
		hit.m_bHit = true;
		hit.m_fDistance = range*rayCallback.m_closestHitFraction;

		btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
		if (body)
		{
			hit.m_pBody = static_cast<palBodyBase *>(body->getUserPointer());
		}
	}
}

struct palBulletCustomResultCallback : public btCollisionWorld::RayResultCallback
{
   palBulletCustomResultCallback(const btVector3& rayFromWorld,const btVector3& rayToWorld, btScalar range,
            palRayHitCallback& callback, palGroupFlags groupFilter)
	: m_rayFromWorld(rayFromWorld)
	, m_rayToWorld(rayToWorld)
	, m_range(range)
	, m_callback(callback)
	, m_groupFilter(groupFilter)
	, m_lastFraction(1.0)
	{
	}

	btVector3       m_rayFromWorld;//used to calculate hitPointWorld from hitFraction
	btVector3       m_rayToWorld;
	btScalar        m_range;
	palRayHitCallback& m_callback;
	palGroupFlags   m_groupFilter;
	btScalar        m_lastFraction;

	virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
	{
      m_closestHitFraction = rayResult.m_hitFraction;
      m_collisionObject = rayResult.m_collisionObject;

      btVector3 hitNormalWorld, hitPointWorld;
		if (normalInWorldSpace) {
			hitNormalWorld = rayResult.m_hitNormalLocal;
		}
		else {
			///need to transform normal into worldspace
			hitNormalWorld = m_collisionObject->getWorldTransform().getBasis()*rayResult.m_hitNormalLocal;
		}

		hitPointWorld.setInterpolate3(m_rayFromWorld,m_rayToWorld,rayResult.m_hitFraction);

		btRigidBody* body = btRigidBody::upcast(rayResult.m_collisionObject);

		if ((body->getBroadphaseProxy()->m_collisionFilterGroup & (short)(m_groupFilter)) == 0)
		{
			return m_lastFraction;
		}

		palRayHit hit;
		hit.Clear();
		hit.SetHitPosition(hitPointWorld.x(), hitPointWorld.y(), hitPointWorld.z());
		hit.SetHitNormal(hitNormalWorld.x(), hitNormalWorld.y(), hitNormalWorld.z());
		hit.m_bHit = true;
		hit.m_fDistance = m_range * rayResult.m_hitFraction;

		if (body != NULL) {
			hit.m_pBody = static_cast<palBodyBase *>(body->getUserPointer());
		}

		m_lastFraction = m_callback.AddHit(hit) / hit.m_fDistance;
		return m_lastFraction;
	}
};

void palBulletPhysics::RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range,
         palRayHitCallback& callback, palGroupFlags groupFilter) {
   btVector3 from(x,y,z);
   btVector3 dir(dx,dy,dz);
   btVector3 to = from + dir * range;

   palBulletCustomResultCallback rayCallback(from, to, range, callback, groupFilter);

   g_DynamicsWorld->rayTest(from, to, rayCallback);
}

void palBulletPhysics::AddRigidBody(palBulletBodyBase* body) {
	if (body->m_pbtBody != NULL)
	{
		g_DynamicsWorld->addRigidBody(body->m_pbtBody);
	   //reset the group to get rid of the default groups.
	   body->SetGroup(body->m_Group);
	   //set this to all ones, but the code should actually never use it.
	   body->m_pbtBody->getBroadphaseProxy()->m_collisionFilterMask = ~0;
	}
}

void palBulletPhysics::RemoveRigidBody(palBulletBodyBase* body) {
	if (body->m_pbtBody != NULL)
	{
		g_DynamicsWorld->removeRigidBody(body->m_pbtBody);
		delete body->m_pbtBody->getBroadphaseHandle();
		body->m_pbtBody->setBroadphaseHandle(NULL);
	}
}

PAL_MAP <btCollisionObject*, btCollisionObject*> pallisten;
PAL_VECTOR<palContactPoint> g_contacts;

bool listen_collision(btCollisionObject* b0, btCollisionObject* b1) {
	PAL_MAP <btCollisionObject*, btCollisionObject*>::iterator itr;
	itr = pallisten.find(b0);
	if (itr!=pallisten.end()) {
		//anything with b0
		if (itr->second ==  (btCollisionObject*)0)
			return true;
		//or specifically, b1
		if (itr->second ==  b1)
			return true;

	}
	itr = pallisten.find(b1);
	if (itr!=pallisten.end()) {
		if (itr->second ==  (btCollisionObject*)0)
			return true;
		if (itr->second == b0)
			return true;
	}
	return false;
}

void palBulletPhysics::NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled) {
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (a);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (b);
	btCollisionObject* b0 = body0->m_pbtBody;
	btCollisionObject* b1 = body1->m_pbtBody;
	if (enabled) {
		pallisten.insert(std::make_pair(b0,b1));
		pallisten.insert(std::make_pair(b1,b0));
	} else {
		PAL_MAP <btCollisionObject*, btCollisionObject*>::iterator itr;
		itr = pallisten.find(b0);
		if (itr!=pallisten.end()) {
			if (itr->second ==  b1)
				pallisten.erase(itr);
		}
		itr = pallisten.find(b1);
		if (itr!=pallisten.end()) {
			if (itr->second ==  b0)
				pallisten.erase(itr);
		}
	}
}
void palBulletPhysics::NotifyCollision(palBodyBase *pBody, bool enabled) {
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (pBody);
	btCollisionObject* b0 = body0->m_pbtBody;
	if (enabled) {
		pallisten.insert(std::make_pair(b0,(btCollisionObject*)0));
	} else {
		PAL_MAP <btCollisionObject*, btCollisionObject*>::iterator itr;
		itr = pallisten.find(b0);
		if (itr!=pallisten.end()) {
			if (itr->second ==  (btCollisionObject*)0)
				pallisten.erase(itr);
		}
	}
}

void palBulletPhysics::GetContacts(palBodyBase *pBody, palContact& contact) {
	contact.m_ContactPoints.clear();
	for (unsigned int i=0;i<g_contacts.size();i++) {
		if (g_contacts[i].m_pBody1 == pBody) {
			contact.m_ContactPoints.push_back(g_contacts[i]);
		}
		if (g_contacts[i].m_pBody2 == pBody) {
			contact.m_ContactPoints.push_back(g_contacts[i]);
		}
	}
}
void palBulletPhysics::GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact) {
	contact.m_ContactPoints.clear();
	for (unsigned int i=0;i<g_contacts.size();i++) {
		if ((g_contacts[i].m_pBody1 == a) && (g_contacts[i].m_pBody2 == b)) {
			contact.m_ContactPoints.push_back(g_contacts[i]);
		}
		if ((g_contacts[i].m_pBody2 == a) && (g_contacts[i].m_pBody1 == b)) {
			contact.m_ContactPoints.push_back(g_contacts[i]);
		}
	}
}

palBulletPhysics::palBulletPhysics() {
   m_fFixedTimeStep = 0.0;
	set_pe = 1;
	set_substeps = 1;
	m_dynamicsWorld = 0;
}

const char* palBulletPhysics::GetPALVersion() {
	static char verbuf[512];
	sprintf(verbuf,"PAL SDK V%d.%d.%d\nPAL Bullet V:%d.%d.%d\nFile: %s\nCompiled: %s %s\nModified:%s",
		PAL_SDK_VERSION_MAJOR,PAL_SDK_VERSION_MINOR,PAL_SDK_VERSION_BUGFIX,
		BULLET_PAL_SDK_VERSION_MAJOR,BULLET_PAL_SDK_VERSION_MINOR,BULLET_PAL_SDK_VERSION_BUGFIX,
		__FILE__,__TIME__,__DATE__,__TIMESTAMP__);
	return verbuf;
}

const char* palBulletPhysics::GetVersion() {
	static char verbuf[256];
	int v = btGetVersion();
	sprintf(verbuf,"Bullet V%d.%d",v/100,v%100);
	return verbuf;
}

void palBulletPhysics::SetFixedTimeStep(Float fixedStep) {
	m_fFixedTimeStep = fixedStep;
}

void palBulletPhysics::SetSolverAccuracy(Float fAccuracy) {
}

void palBulletPhysics::SetPE(int n) {
	set_pe = n;
}

void palBulletPhysics::SetSubsteps(int n) {
	set_substeps = n;
}

void palBulletPhysics::SetHardware(bool status) {
	//TODO: enable SPE's
}

bool palBulletPhysics::GetHardware(void) {
	//TODO: return if using SPE's
	return false;
}

void palBulletPhysics::Init(palPhysicsDesc& desc) {
   palPhysics::Init(desc);

	btBroadphaseInterface*	broadphase;
	btConstraintSolver*	solver;
#if 1
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	broadphase = new btAxisSweep3(worldMin,worldMax);
	//probably a memory leak...
#else
	broadphase = new btSimpleBroadphase;
#endif
	broadphase->getOverlappingPairCache()->setOverlapFilterCallback(new CustomOverlapFilterCallback);
	btDefaultCollisionConfiguration* collisionConfiguration = //new btDefaultCollisionConfiguration();
		new btSoftBodyRigidBodyCollisionConfiguration();

#ifndef USE_PARALLEL_DISPATCHER
	m_dispatcher = new btCollisionDispatcher(collisionConfiguration);
#else
	int maxNumOutstandingTasks = set_pe;
	btThreadSupportInterface*		m_threadSupportCollision = 0;
#ifdef OS_WINDOWS
m_threadSupportCollision = new Win32ThreadSupport(Win32ThreadSupport::Win32ThreadConstructionInfo(
								"collision",
								processCollisionTask,
								createCollisionLocalStoreMemory,
								maxNumOutstandingTasks));
#else
PosixThreadSupport::ThreadConstructionInfo tcInfo(
                        "collision",
                        processCollisionTask,
                        createCollisionLocalStoreMemory,
                        maxNumOutstandingTasks);
m_threadSupportCollision = new PosixThreadSupport(tcInfo);
#endif
	m_dispatcher = new	SpuGatheringCollisionDispatcher(m_threadSupportCollision,maxNumOutstandingTasks,collisionConfiguration);
#endif

	solver = new btSequentialImpulseConstraintSolver();


//	m_dynamicsWorld = new btSimpleDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_solver);

	btSoftRigidDynamicsWorld* dynamicsWorld = new btSoftRigidDynamicsWorld(m_dispatcher, broadphase, solver,collisionConfiguration);
	m_dynamicsWorld = dynamicsWorld;


	m_softBodyWorldInfo.m_dispatcher = m_dispatcher;
	m_softBodyWorldInfo.m_broadphase = broadphase;

	btVector3 gravity(m_fGravityX, m_fGravityY, m_fGravityZ);
	m_dynamicsWorld->setGravity(gravity);
	m_softBodyWorldInfo.m_gravity = gravity;

	m_softBodyWorldInfo.m_sparsesdf.Initialize();

	g_DynamicsWorld = m_dynamicsWorld;

	//
	m_CollisionMasks.resize(32U, ~0);
}

void palBulletPhysics::Cleanup() {
}

void palBulletPhysics::StartIterate(Float timestep) {
	g_contacts.clear(); //clear all contacts before the update TODO: CHECK THIS IS SAFE FOR MULTITHREADED!
	if (m_dynamicsWorld) {
		if (m_fFixedTimeStep > 0) {
			m_dynamicsWorld->stepSimulation(timestep,set_substeps,m_fFixedTimeStep);
		} else {
			m_dynamicsWorld->stepSimulation(timestep,set_substeps,timestep);
		}

		//collision iteration
		int i;
		int numManifolds = m_dispatcher->getNumManifolds();
		for (i=0;i<numManifolds;i++)
		{
			btPersistentManifold* contactManifold = m_dispatcher->getManifoldByIndexInternal(i);
			btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
			if (listen_collision(obA,obB)) {
				int numContacts = contactManifold->getNumContacts();
				for (int j=0;j<numContacts;j++)
				{
					btManifoldPoint& pt = contactManifold->getContactPoint(j);
					palContactPoint cp;
					cp.m_pBody1=static_cast<palBodyBase *>(obA->getUserPointer());
					cp.m_pBody2=static_cast<palBodyBase *>(obB->getUserPointer());
					btVector3 pos = pt.getPositionWorldOnB();
					cp.m_vContactPosition.x = pos.x();
					cp.m_vContactPosition.y = pos.y();
					cp.m_vContactPosition.z = pos.z();

					btVector3 norm = pt.m_normalWorldOnB;
					cp.m_vContactNormal.x = norm.x();
					cp.m_vContactNormal.y = norm.y();
					cp.m_vContactNormal.z = norm.z();

					cp.m_fDistance= pt.getDistance();

					g_contacts.push_back(cp);
				}
			}

		}
	}
}
bool palBulletPhysics::QueryIterationComplete() {
	return true;
}
void palBulletPhysics::WaitForIteration() {
	return;
}

void palBulletPhysics::Iterate(Float timestep) {
	StartIterate(timestep);
	WaitForIteration();
}


///////////////
palBulletBodyBase::palBulletBodyBase() {
	m_pbtBody = NULL;
	m_pbtMotionState = NULL;
}

palBulletBodyBase::~palBulletBodyBase() {
	dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics())->RemoveRigidBody(this);
	if (m_pbtBody) {
		Cleanup();
		delete m_pbtBody->getMotionState();
		delete m_pbtBody->getBroadphaseHandle();
		delete m_pbtBody->getCollisionShape();
		delete m_pbtBody;
		m_pbtBody = NULL;
	}
}

void palBulletBodyBase::SetMaterial(palMaterial *material) {
/*	for (unsigned int i=0;i<m_Geometries.size();i++) {
		palBulletGeometry *pbg = m_Geometries[i];
		pbg->m_pbtShape->s
	}*/
	palBodyBase::SetMaterial(material);
	if (m_pbtBody) {
		m_pbtBody->setFriction(material->m_fStatic);
		m_pbtBody->setRestitution(material->m_fRestitution);
	}
}

void palBulletBodyBase::BuildBody(const palVector3& pos, Float mass,
			palDynamicsType dynType,
			btCollisionShape *btShape,
			const palVector3& inertia) {


   btTransform trans;
   trans.setIdentity();
   trans.setOrigin(btVector3(pos[0], pos[1], pos[2]));

   m_pbtMotionState = new btDefaultMotionState(trans);

   btCollisionShape *pShape = btShape;
   btVector3 localInertia(0,0,0);

   //no given shape? get from geom
   if (!btShape) {
      palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (m_Geometries[0]);
      pbtg->m_pbtShape->calculateLocalInertia(mass,localInertia);
      pShape = pbtg->m_pbtShape;
   }

   if (dynType != PALBODY_DYNAMIC) {
      mass = 0;
   }

   m_pbtBody = new btRigidBody(mass,m_pbtMotionState,pShape,localInertia);
   m_pbtBody->setUserPointer(dynamic_cast<palBodyBase*>(this));
   // Disabling deactivition is really bad.  objects will never go to sleep. which is bad for
   // performance
   //m_pbtBody->setActivationState(DISABLE_DEACTIVATION);

   int currFlags = m_pbtBody->getCollisionFlags();

   switch (dynType)
   {
      case PALBODY_DYNAMIC:
      {
         currFlags &= (~btCollisionObject::CF_STATIC_OBJECT);
         currFlags &= (~btCollisionObject::CF_KINEMATIC_OBJECT);
         break;
      }
      case PALBODY_STATIC:
      {
         currFlags |= btCollisionObject::CF_STATIC_OBJECT;
         currFlags &= (~btCollisionObject::CF_KINEMATIC_OBJECT);
         break;
      }
      case PALBODY_KINEMATIC:
      {
         currFlags &= (~btCollisionObject::CF_STATIC_OBJECT);
         currFlags |= btCollisionObject::CF_KINEMATIC_OBJECT;
         break;
      }
   }

   m_pbtBody->setCollisionFlags(currFlags);

   dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics())->AddRigidBody(this);
}


void palBulletBodyBase::SetPosition(palMatrix4x4& location) {
	if (m_pbtBody) {
		btTransform newloc;
		newloc.setFromOpenGLMatrix(location._mat);
		if (m_pbtBody->getMotionState() != NULL)
		{
		   m_pbtBody->getMotionState()->setWorldTransform(newloc);
		}
		m_pbtBody->setCenterOfMassTransform(newloc);
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

palGroup palBulletBodyBase::GetGroup() const {
   if (!m_pbtBody)
      return palBodyBase::GetGroup();
   return convert_to_pal_group(m_pbtBody->getBroadphaseProxy()->m_collisionFilterGroup);
}

void palBulletBodyBase::SetGroup(palGroup group) {
	palBodyBase::SetGroup(group);
	if (!m_pbtBody)
		return;

	//int btgroup = convert_group(group);

	m_pbtBody->getBroadphaseProxy()->m_collisionFilterGroup = convert_group(group);
}
///////////////
palBulletBody::palBulletBody() {
}

palBulletBody::~palBulletBody() {
}

///////////////
palBulletGenericBody::palBulletGenericBody()
{

}

void palBulletGenericBody::Init(palMatrix4x4 &pos)
{
	// default to dynamic
	btCompoundShape* compound = new btCompoundShape();

	btVector3 localInertia(0, 0, 0);
	compound->calculateLocalInertia(m_fMass, localInertia);
	palGenericBody::SetInertia(localInertia.x(), localInertia.y(), localInertia.z());

	palVector3 pvInertia = palVector3::Create(localInertia.x(), localInertia.y(), localInertia.z());

	m_fInertiaXX = localInertia.x();
	m_fInertiaYY = localInertia.y();
	m_fInertiaZZ = localInertia.z();

	//Set the position to 0 since it will be moved in a sec.
	BuildBody(palVector3::Create(), m_fMass, GetDynamicsType(), compound, pvInertia);
	//Reset now that the body is created.
	SetGravityEnabled(IsGravityEnabled());
	SetPosition(pos);
}

bool palBulletGenericBody::IsDynamic() {
	if (m_pbtBody != NULL)
	{
		return !m_pbtBody->isStaticOrKinematicObject();
	}
	return palBulletGenericBody::IsDynamic();
}

bool palBulletGenericBody::IsKinematic() {
	if (m_pbtBody != NULL)
	{
		return m_pbtBody->isKinematicObject();
	}
	return palBulletGenericBody::IsKinematic();
}

bool palBulletGenericBody::IsStatic() {
	if (m_pbtBody != NULL)
	{
		return m_pbtBody->isStaticObject();
	}
	return palBulletGenericBody::IsStatic();
}

void palBulletGenericBody::SetDynamicsType(palDynamicsType dynType) {
	palGenericBody::SetDynamicsType(dynType);

	if (m_pbtBody == NULL)
	{
		return;
	}

	int currFlags = m_pbtBody->getCollisionFlags();

   btVector3 inertia(m_fInertiaXX, m_fInertiaYY, m_fInertiaZZ);
   switch (dynType)
	{
		case PALBODY_DYNAMIC:
		{
			currFlags &= (~btCollisionObject::CF_STATIC_OBJECT);
			currFlags &= (~btCollisionObject::CF_KINEMATIC_OBJECT);
			m_pbtBody->setMassProps(m_fMass, inertia);
		}
		case PALBODY_STATIC:
		{
			currFlags |= btCollisionObject::CF_STATIC_OBJECT;
			currFlags &= (~btCollisionObject::CF_KINEMATIC_OBJECT);

			m_pbtBody->setMassProps(btScalar(0.0), inertia);
		}
		case PALBODY_KINEMATIC:
		{
			currFlags &= (~btCollisionObject::CF_STATIC_OBJECT);
			currFlags |= btCollisionObject::CF_KINEMATIC_OBJECT;
			m_pbtBody->setMassProps(btScalar(0.0), inertia);
		}
	}

	m_pbtBody->setCollisionFlags(currFlags);

}


void palBulletGenericBody::SetMass(Float mass) {
	palGenericBody::SetMass(mass);
	if (m_pbtBody && GetDynamicsType() == PALBODY_DYNAMIC) {
		btVector3 inertia(m_fInertiaXX, m_fInertiaYY, m_fInertiaZZ);
		m_pbtBody->setMassProps(btScalar(mass), inertia);
	}
}

void palBulletGenericBody::SetGravityEnabled(bool enabled) {
	if (m_pbtBody != NULL) {
		if (enabled) {
			palVector3 pv;
			palFactory::GetInstance()->GetActivePhysics()->GetGravity(pv);
			m_pbtBody->setGravity(btVector3(pv.x, pv.y, pv.z));
		} else {
			m_pbtBody->setGravity(btVector3());
		}
	}
	m_bGravityEnabled = enabled;
}

bool palBulletGenericBody::IsGravityEnabled() {
	return m_bGravityEnabled;
}

void palBulletGenericBody::SetInertia(Float Ixx, Float Iyy, Float Izz) {
	palGenericBody::SetInertia(Ixx, Iyy, Izz);
	if (m_pbtBody) {
		btVector3 inertia(m_fInertiaXX, m_fInertiaYY, m_fInertiaZZ);
		m_pbtBody->setMassProps(btScalar(m_fMass), inertia);
	}
}

void palBulletGenericBody::ConnectGeometry(palGeometry* pGeom)
{
	palGenericBody::ConnectGeometry(pGeom);
	if (m_pbtBody != NULL)
	{

		btCompoundShape *compound = static_cast<btCompoundShape*>(m_pbtBody->getCollisionShape());
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (pGeom);
		palMatrix4x4 m = pbtg->GetOffsetMatrix();//GetLocationMatrix();
		btTransform localTrans;
		localTrans.setFromOpenGLMatrix(m._mat);
		compound->addChildShape(localTrans, pbtg->BulletGetCollisionShape());
		btVector3 inertia;
		dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics())->RemoveRigidBody(this);
		dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics())->AddRigidBody(this);
	}
}

void palBulletGenericBody::RemoveGeometry(palGeometry* pGeom)
{
	palGenericBody::RemoveGeometry(pGeom);
	if (m_pbtBody != NULL)
	{
		btCompoundShape *compound = static_cast<btCompoundShape*>(m_pbtBody->getCollisionShape());
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (pGeom);
		compound->removeChildShape(pbtg->BulletGetCollisionShape());
		btVector3 inertia;
		dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics())->RemoveRigidBody(this);
		dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics())->AddRigidBody(this);
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
		palBulletBodyBase::SetPosition(location);
	}
}

palMatrix4x4& palBulletStaticCompoundBody::GetLocationMatrix() {
	return palBulletCompoundBody::GetLocationMatrix();
}

palMatrix4x4& palBulletCompoundBody::GetLocationMatrix() {
	if (m_pbtBody) {
		btTransform t;
		m_pbtBody->getMotionState()->getWorldTransform(t);
		t.getOpenGLMatrix(m_mLoc._mat);
	}
	return m_mLoc;
}


bool palBulletBody::IsActive()
{
   return m_pbtBody->isActive();
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
		//m_pbtBody->setActivationState(DISABLE_DEACTIVATION);
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


palBulletStaticCompoundBody::palBulletStaticCompoundBody() {
}

void palBulletStaticCompoundBody::Finalize() {
	SumInertia();
	btCompoundShape* compound = new btCompoundShape();
	for (PAL_VECTOR<palGeometry *>::size_type i=0;i<m_Geometries.size();i++) {
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (m_Geometries[i]);

		palMatrix4x4 m = pbtg->GetOffsetMatrix();//GetLocationMatrix();

		btTransform localTrans;
		localTrans.setFromOpenGLMatrix(m._mat);

		compound->addChildShape(localTrans,pbtg->m_pbtShape);
	}

	btVector3 localInertia(0.0, 0.0, 0.0);
	compound->calculateLocalInertia(0, localInertia);
	m_fInertiaXX = localInertia.x();
	m_fInertiaYY = localInertia.y();
	m_fInertiaZZ = localInertia.z();

   palVector3 pvInertia = palVector3::Create(localInertia.x(), localInertia.y(), localInertia.z());
   //Set the position to 0 since it will be moved in a sec.
   BuildBody(palVector3::Create(m_fPosX,m_fPosY,m_fPosZ), m_fMass, PALBODY_STATIC, compound, pvInertia);
}


palBulletCompoundBody::palBulletCompoundBody() {
	;
}


void palBulletCompoundBody::Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ) {
	btCompoundShape* compound = new btCompoundShape();
	for (PAL_VECTOR<palGeometry *>::size_type i=0;i<m_Geometries.size();i++) {
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (m_Geometries[i]);

		palMatrix4x4 m = pbtg->GetOffsetMatrix();//GetLocationMatrix();

		btTransform localTrans;
		localTrans.setFromOpenGLMatrix(m._mat);

		compound->addChildShape(localTrans,pbtg->m_pbtShape);
	}

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(m_fPosX,m_fPosY,m_fPosZ));

	btVector3 localInertia(0.0, 0.0, 0.0);
	compound->calculateLocalInertia(finalMass,localInertia);
	palVector3 pvInertia = palVector3::Create(localInertia.x(), localInertia.y(), localInertia.z());

	m_fInertiaXX = localInertia.x();
	m_fInertiaYY = localInertia.y();
	m_fInertiaZZ = localInertia.z();

	//Set the position to 0 since it will be moved in a sec.
   BuildBody(palVector3::Create(m_fPosX,m_fPosY,m_fPosZ), finalMass, PALBODY_DYNAMIC, compound, pvInertia);
   m_fMass = finalMass;
}

palBulletGeometry::palBulletGeometry() {
	m_pbtShape = NULL;
}

palBulletGeometry::~palBulletGeometry() {
	delete m_pbtShape;
	m_pbtShape = NULL;
}

palBulletBoxGeometry::palBulletBoxGeometry() {
	m_pbtBoxShape = NULL;
}

void palBulletBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	palVector3 dim = GetXYZDimensions();

	m_pbtBoxShape = new btBoxShape(btVector3(dim.x*(Float)0.5,dim.y*(Float)0.5,dim.z*(Float)0.5));
	m_pbtShape = m_pbtBoxShape;
	m_pbtShape->setMargin(0.0f);
}

palBulletBox::palBulletBox() {
}

void palBulletBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	BuildBody(palVector3::Create(x,y,z) ,mass);
}

palBulletStaticBox::palBulletStaticBox() {
}

void palBulletStaticBox::Init(palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palStaticBox::Init(pos,width,height,depth);
	BuildBody(palVector3::Create(m_fPosX,m_fPosY,m_fPosZ), 0, PALBODY_STATIC);
	palBulletBodyBase::SetPosition(pos);
}

palBulletStaticSphere::palBulletStaticSphere() {
}

void palBulletStaticSphere::Init(palMatrix4x4 &pos, Float radius) {
	palStaticSphere::Init(pos,radius);
	BuildBody(palVector3::Create(m_fPosX,m_fPosY,m_fPosZ), 0, PALBODY_STATIC);
	palBulletBodyBase::SetPosition(pos);
}

palBulletStaticCapsule::palBulletStaticCapsule() {
}

void palBulletStaticCapsule::Init(Float x, Float y, Float z, Float radius, Float length) {
	palStaticCapsule::Init(x,y,z,radius,length);
	BuildBody(palVector3::Create(m_fPosX,m_fPosY,m_fPosZ), 0, PALBODY_STATIC);
}


palBulletSphereGeometry::palBulletSphereGeometry() {
	m_btSphereShape = NULL;
}

void palBulletSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	m_btSphereShape = new btSphereShape(radius); // this seems wrong!
	m_pbtShape = m_btSphereShape;
	m_pbtShape->setMargin(0.0f);
}

palBulletCapsuleGeometry::palBulletCapsuleGeometry() {
	m_btCylinderShape = NULL;
}

void palBulletCapsuleGeometry::Init(palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	palCapsuleGeometry::Init(pos,radius,length,mass);
	unsigned int upAxis = palFactory::GetInstance()->GetActivePhysics()->GetUpAxis();
	// for z up
	if (upAxis == 2)
	{
		m_btCylinderShape = new btCylinderShapeZ(btVector3(radius, radius, length/2.0)); //Half lengths
	}
	else if (upAxis == 0)
	{
		m_btCylinderShape = new btCylinderShapeX(btVector3(length/2.0, radius, radius)); //Half lengths
	}
	else
	{
		m_btCylinderShape = new btCylinderShape(btVector3(radius,length/2.0,radius)); //Half lengths
	}

	m_pbtShape = m_btCylinderShape;
	m_pbtShape->setMargin(0.0f);
}


palBulletSphere::palBulletSphere() {
}

void palBulletSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	BuildBody(palVector3::Create(x,y,z), mass);
}

palBulletCapsule::palBulletCapsule() {
}

void palBulletCapsule::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palCapsule::Init(x,y,z,radius,length,mass);
	BuildBody(palVector3::Create(x,y,z), mass);
}

palBulletOrientatedTerrainPlane::palBulletOrientatedTerrainPlane() {
}

void palBulletOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size) {
	palOrientatedTerrainPlane::Init(x,y,z,nx,ny,nz,min_size);

	btVector3 normal(nx,ny,nz);
	normal.normalize();
	m_pbtPlaneShape = new btStaticPlaneShape(normal,CalculateD());

	BuildBody(palVector3::Create(x,y,z), 0, PALBODY_STATIC, m_pbtPlaneShape);
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

	unsigned int upAxis = palFactory::GetInstance()->GetActivePhysics()->GetUpAxis();

	if (upAxis == 2)
	{
		m_pbtBoxShape = new btBoxShape(btVector3(min_size*(Float)0.5, min_size*(Float)0.5, (Float)1.0));
	}
	else if (upAxis == 0)
	{
		m_pbtBoxShape = new btBoxShape(btVector3(1.0, min_size*(Float)0.5, min_size*(Float)0.5));
	}
	else
	{
		m_pbtBoxShape = new btBoxShape(btVector3(min_size*(Float)0.5, (Float)1.0, min_size*(Float)0.5));
	}
	// This assumes y is up.  This is bad.  maybe use the gravity vector?
	BuildBody(palVector3::Create(x,y-1,z), 0, PALBODY_STATIC, m_pbtBoxShape);
}

palBulletTerrainMesh::palBulletTerrainMesh() {

}

static btTriangleIndexVertexArray* CreateTrimesh(const Float *pVertices, int nVertices, const int *pIndices, int nIndices)
{
	btTriangleIndexVertexArray* trimesh = new btTriangleIndexVertexArray();
	btIndexedMesh meshIndex;
	meshIndex.m_numTriangles = nIndices / 3;
	meshIndex.m_numVertices = nVertices;
	meshIndex.m_vertexStride = 3 * sizeof (Float);
	meshIndex.m_triangleIndexStride = 3 * sizeof (int);
	meshIndex.m_indexType = PHY_INTEGER;

	meshIndex.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(pIndices);
	meshIndex.m_vertexBase = reinterpret_cast<const unsigned char*>(pVertices);

	trimesh->addIndexedMesh(meshIndex);

	return trimesh;
}

void palBulletTerrainMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palTerrainMesh::Init(x,y,z,pVertices,nVertices,pIndices,nIndices);

   m_Indices.reserve(nIndices);
   for (int i = 0; i < nIndices; ++i)
   {
      m_Indices.push_back(pIndices[i]);
   }

   int nVertFloats = nVertices * 3;
   m_Vertices.reserve(nVertFloats);
   for (int i = 0; i < nVertFloats; ++i)
   {
      m_Vertices.push_back(pVertices[i]);
   }

   btTriangleIndexVertexArray* trimesh = CreateTrimesh(&m_Vertices.front(), nVertices, &m_Indices.front(), nIndices);
//	btTriangleMesh* trimesh = new btTriangleMesh(true, false);
//	int pi;
//	for (int i=0;i<nIndices/3;i++) {
//		pi = pIndices[i*3+0];
//		btVector3 v0(	pVertices[pi*3+0],
//						pVertices[pi*3+1],
//						pVertices[pi*3+2]);
//		pi = pIndices[i*3+1];
//		btVector3 v1(	pVertices[pi*3+0],
//						pVertices[pi*3+1],
//						pVertices[pi*3+2]);
//		pi = pIndices[i*3+2];
//		btVector3 v2(	pVertices[pi*3+0],
//						pVertices[pi*3+1],
//						pVertices[pi*3+2]);
//		trimesh->addTriangle(v0,v1,v2);
//	}

	m_pbtTriMeshShape = new btBvhTriangleMeshShape(trimesh,true);
	BuildBody(palVector3::Create(x,y,z), 0, PALBODY_STATIC, m_pbtTriMeshShape);
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

palBulletSphericalLink::~palBulletSphericalLink() {
	if (m_btp2p) {
		if (g_DynamicsWorld)
			g_DynamicsWorld->removeConstraint(m_btp2p);
		delete m_btp2p;
	}
}


void palBulletSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palSphericalLink::Init(parent,child,x,y,z);
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);

	palMatrix4x4 a = parent->GetLocationMatrix();
	palMatrix4x4 b = child->GetLocationMatrix();

	btVector3 pivotInA(x-a._41,y-a._42,z-a._43);
	btVector3 pivotInB = body1->m_pbtBody->getCenterOfMassTransform().inverse()(body0->m_pbtBody->getCenterOfMassTransform()(pivotInA)) ;
#if 0
	m_btp2p = new btPoint2PointConstraint(*(body0->m_pbtBody),*(body1->m_pbtBody),pivotInA,pivotInB);
	g_DynamicsWorld->addConstraint(m_btp2p,true);
#else
		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInB = btTransform::getIdentity();
		frameInA.setOrigin(pivotInA);
		frameInB.setOrigin(pivotInB);


	btGeneric6DofConstraint* p2p = new btGeneric6DofConstraint(*(body0->m_pbtBody),*(body1->m_pbtBody),
		frameInA,frameInB,true);
//	  btScalar	lo = btScalar(-1e30);
  //  btScalar	hi = btScalar(1e30);
//	p2p->setAngularLowerLimit(btVector3(lo,lo,lo));
//	p2p->setAngularLowerLimit(btVector3(hi,hi,hi));
	m_btp2p = p2p;
#endif
	g_DynamicsWorld->addConstraint(m_btp2p,true);
}

void palBulletSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {
	btGeneric6DofConstraint* g = m_btp2p;//dynamic_cast<btGeneric6DofConstraint *>(m_btp2p);
	btVector3 limit(cone_limit_rad,cone_limit_rad,twist_limit_rad);
	g->setAngularLowerLimit(-limit);
	g->setAngularUpperLimit(limit);
}

palBulletRevoluteLink::palBulletRevoluteLink() {
	m_btHinge = NULL;
}

palBulletRevoluteLink::~palBulletRevoluteLink() {
	if (m_btHinge) {
		if (g_DynamicsWorld)
			g_DynamicsWorld->removeConstraint(m_btHinge);
		delete m_btHinge;
	}
}

void palBulletRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {

//	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
//
//	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
//	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);
//	palMatrix4x4 a = parent->GetLocationMatrix();
//	palMatrix4x4 b = child->GetLocationMatrix();
//
//	btVector3 pivotInA(x-a._41,y-a._42,z-a._43);
//	btVector3 pivotInB = body1->m_pbtBody->getCenterOfMassTransform().inverse()(body0->m_pbtBody->getCenterOfMassTransform()(pivotInA)) ;
//
//	btVector3 axisInA(axis_x,axis_y,axis_z);
//	btVector3 axisInB = axisInA;
//	m_btHinge = new btHingeConstraint(*(body0->m_pbtBody),*(body1->m_pbtBody),
//		pivotInA,pivotInB,axisInA,axisInB);
//	g_DynamicsWorld->addConstraint(m_btHinge,true);

	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);

	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);

	btVector3 axis(axis_x,axis_y,axis_z);
	btVector3 pivotInA(m_pivotA.x,m_pivotA.y,m_pivotA.z);
	btVector3 pivotInB(m_pivotB.x,m_pivotB.y,m_pivotB.z);

	btTransform t;
	t = body0->m_pbtBody->getCenterOfMassTransform();
	btVector3 axisInA(axis.dot(t.getBasis().getColumn(0)),
						axis.dot(t.getBasis().getColumn(1)),
						axis.dot(t.getBasis().getColumn(2)));

	t = body1->m_pbtBody->getCenterOfMassTransform();
	btVector3 axisInB(axis.dot(t.getBasis().getColumn(0)),
							axis.dot(t.getBasis().getColumn(1)),
							axis.dot(t.getBasis().getColumn(2)));
	m_btHinge = new btHingeConstraint(*(body0->m_pbtBody),*(body1->m_pbtBody),
		pivotInA,pivotInB,axisInA,axisInB);
	g_DynamicsWorld->addConstraint(m_btHinge,true);

}

void palBulletRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	m_btHinge->setLimit(lower_limit_rad,upper_limit_rad);
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
	m_pbtShape->setMargin(0.0f);
}

void palBulletConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass){
	palConvexGeometry::Init(pos,pVertices,nVertices,pIndices,nIndices,mass);
	m_pbtConvexShape = new btConvexHullShape(pVertices,nVertices,sizeof(Float)*3);
	m_pbtShape = m_pbtConvexShape;
	m_pbtShape->setMargin(0.0f);
}

void palBulletConcaveGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
   palConcaveGeometry::Init(pos,pVertices,nVertices,pIndices,nIndices,mass);
   m_Indices.reserve(nIndices);
   for (int i = 0; i < nIndices; ++i)
   {
      m_Indices.push_back(pIndices[i]);
   }

   int nVertFloats = nVertices * 3;
   m_Vertices.reserve(nVertFloats);
   for (int i = 0; i < nVertFloats; ++i)
   {
      m_Vertices.push_back(pVertices[i]);
   }

   btTriangleIndexVertexArray* trimesh = CreateTrimesh(&m_Vertices.front(), nVertices, &m_Indices.front(), nIndices);
   m_pbtTriMeshShape = new btBvhTriangleMeshShape(trimesh,true);
   m_pbtShape = m_pbtTriMeshShape;
   m_pbtShape->setMargin(0.0f);
}

palBulletConvex::palBulletConvex() {
}

void palBulletConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	BuildBody(palVector3::Create(x,y,z), mass);
}

void palBulletConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,pIndices,nIndices,mass);
	BuildBody(palVector3::Create(x,y,z), mass);
}

palBulletStaticConvex::palBulletStaticConvex() {
}

void palBulletStaticConvex::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices) {
	palStaticConvex::Init(pos,pVertices,nVertices);
	BuildBody(palVector3::Create(m_fPosX, m_fPosY, m_fPosZ), 0, PALBODY_STATIC);
	palBulletBodyBase::SetPosition(pos);
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
	if (rayCallback.hasHit())
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
	genericConstraint = 0;
}

palBulletGenericLink::~palBulletGenericLink() {
	if (genericConstraint) {
		if (g_DynamicsWorld)
			g_DynamicsWorld->removeConstraint(genericConstraint);
		delete genericConstraint;
	}
}

void palBulletGenericLink::Init(palBodyBase *parent, palBodyBase *child,
								palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
								palVector3 linearLowerLimits, palVector3 linearUpperLimits,
								palVector3 angularLowerLimits, palVector3 angularUpperLimits)
{
	palGenericLink::Init(parent,child,parentFrame,childFrame,linearLowerLimits,linearUpperLimits,angularLowerLimits,angularUpperLimits);

	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);

	btTransform frameInA, frameInB;
	frameInA.setFromOpenGLMatrix(parentFrame._mat);
	frameInB.setFromOpenGLMatrix(childFrame._mat);

	genericConstraint = new btGeneric6DofConstraint(
		*(body0->m_pbtBody),*(body1->m_pbtBody),
		frameInA,frameInB,true);

	genericConstraint->setLinearLowerLimit(btVector3(linearLowerLimits.x,linearLowerLimits.y,linearLowerLimits.z));
	genericConstraint->setLinearUpperLimit(btVector3(linearUpperLimits.x,linearUpperLimits.y,linearUpperLimits.z));
	genericConstraint->setAngularLowerLimit(btVector3(angularLowerLimits.x,angularLowerLimits.y,angularLowerLimits.z));
	genericConstraint->setAngularUpperLimit(btVector3(angularUpperLimits.x,angularUpperLimits.y,angularUpperLimits.z));

	g_DynamicsWorld->addConstraint(genericConstraint);
}

palBulletAngularMotor::palBulletAngularMotor() {
	m_bhc = 0;
}

void palBulletAngularMotor::Init(palRevoluteLink *pLink, Float Max) {
	palAngularMotor::Init(pLink,Max);
	palBulletRevoluteLink *pbrl = dynamic_cast<palBulletRevoluteLink *> (m_link);
	if (pbrl)
		m_bhc = pbrl->m_btHinge;
}

void palBulletAngularMotor::Update(Float targetVelocity) {
	if (!m_bhc)
		return;
	m_bhc->enableAngularMotor(true,targetVelocity,m_fMax);
}

void palBulletAngularMotor::Apply() {

}

/*
if (m_pbtBody) {
		m_pbtBody->getWorldTransform().getOpenGLMatrix(m_mLoc._mat);
	}
	return m_mLoc;
	*/


void palBulletSoftBody::BulletInit(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices) {

	palBulletPhysics *pbf=dynamic_cast<palBulletPhysics *>(PF->GetActivePhysics());

	m_pbtSBody = btSoftBodyHelpers::CreateFromTriMesh(pbf->m_softBodyWorldInfo	,	pParticles,pIndices, nIndices/3);
	m_pbtSBody->generateBendingConstraints(2);
	m_pbtSBody->m_cfg.piterations=2;
			m_pbtSBody->m_cfg.collisions|=btSoftBody::fCollision::VF_SS;
			m_pbtSBody->randomizeConstraints();

			m_pbtSBody->setTotalMass(50,true);


		btSoftRigidDynamicsWorld* softWorld =	(btSoftRigidDynamicsWorld*)pbf->m_dynamicsWorld;
		softWorld->addSoftBody(m_pbtSBody);

		//m_pbtSBody->m_nodes[0].m_x
		//nNodes = nParticles;
}

int palBulletSoftBody::GetNumParticles() {
	return (int)m_pbtSBody->m_nodes.size();
}
palVector3* palBulletSoftBody::GetParticlePositions() {
	pos.resize(GetNumParticles());
	for (int i=0;i<GetNumParticles();i++) {
		pos[i].x = m_pbtSBody->m_nodes[i].m_x.x();
		pos[i].y = m_pbtSBody->m_nodes[i].m_x.y();
		pos[i].z = m_pbtSBody->m_nodes[i].m_x.z();
	}
	return &pos[0];
}

palBulletPatchSoftBody::palBulletPatchSoftBody() {
}

void palBulletPatchSoftBody::Init(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices) {
	palBulletSoftBody::BulletInit(pParticles,pMass,nParticles,pIndices,nIndices);
};

palBulletTetrahedralSoftBody::palBulletTetrahedralSoftBody() {
}

void palBulletTetrahedralSoftBody::Init(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices) {
	int *tris = ConvertTetrahedronToTriangles(pIndices,nIndices);
	palBulletSoftBody::BulletInit(pParticles,pMass,nParticles,tris,(nIndices/4)*12);
};


#ifdef STATIC_CALLHACK
void pal_bullet_call_me_hack() {
	printf("%s I have been called!!\n", __FILE__);
};
#endif
