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
#include "LinearMath/btIDebugDraw.h"

#include <BulletCollision/CollisionShapes/btShapeHull.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>

#ifndef OS_WINDOWS
#define USE_PTHREADS
#endif

#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"

#include <iostream>

#ifdef USE_PARALLEL_DISPATCHER
#include <BulletMultiThreaded/SpuGatheringCollisionDispatcher.h>
#include <BulletMultiThreaded/PlatformDefinitions.h>

#ifdef USE_LIBSPE2
#include "BulletMultiThreaded/SpuLibspe2Support.h"
#elif defined (_WIN32)
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
FACTORY_CLASS_IMPLEMENTATION(palBulletRevoluteSpringLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletPrismaticLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletGenericLink);
FACTORY_CLASS_IMPLEMENTATION(palBulletRigidLink);

FACTORY_CLASS_IMPLEMENTATION(palBulletVehicle);

FACTORY_CLASS_IMPLEMENTATION(palBulletCharacterController);

FACTORY_CLASS_IMPLEMENTATION(palBulletPSDSensor);

FACTORY_CLASS_IMPLEMENTATION(palBulletAngularMotor);
FACTORY_CLASS_IMPLEMENTATION(palBulletGenericLinkSpring);

FACTORY_CLASS_IMPLEMENTATION(palBulletPatchSoftBody);
FACTORY_CLASS_IMPLEMENTATION(palBulletTetrahedralSoftBody);

FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

class palBulletDebugDraw : public btIDebugDraw
{
public:
	palBulletDebugDraw() : m_pPalDebugDraw(0) {}

	bool inRange(const btVector3& point) {
		if (m_pPalDebugDraw == NULL)
			return false;

		if (m_pPalDebugDraw->GetRange() > 0.0f) {
			palVector3 difference(m_pPalDebugDraw->m_vRefPoint.x - point.x(), m_pPalDebugDraw->m_vRefPoint.y - point.y(),m_pPalDebugDraw-> m_vRefPoint.z - point.z());
			return m_pPalDebugDraw->GetRange2() >
				vec_mag2(&difference);
		}
		return true;
	}

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
		if (inRange(from)) {
			m_pPalDebugDraw->m_Lines.m_vVertices.push_back(palVector3(from.x(),
					from.y(), from.z()));
			m_pPalDebugDraw->m_Lines.m_vVertices.push_back(palVector3(to.x(),
					to.y(), to.z()));
			for (unsigned i = 0; i < 2; ++i) {
				m_pPalDebugDraw->m_Lines.m_vColors.push_back(palVector4(
						color.x(), color.y(), color.z(), 1.0f));
			}
		}
	}

	virtual void	drawBox (const btVector3& boxMin, const btVector3& boxMax, const btVector3& color, btScalar alpha)
	{
	}

	virtual void	drawSphere (const btVector3& p, btScalar radius, const btVector3& color)
	{
		drawSpherePatch(p, btVector3(0.0, 0.0, 1.0), btVector3(0.0, 1.0, 0.0), radius, 0.0, SIMD_2_PI, 0.0, SIMD_2_PI, color, 3.0f);
	}


	virtual void drawTriangle(const btVector3& v0,const btVector3& v1,const btVector3& v2,const btVector3& /*n0*/,const btVector3& /*n1*/,const btVector3& /*n2*/,const btVector3& color, btScalar alpha) {
		drawTriangle(v0,v1,v2,color,alpha);
	}

	virtual void drawTriangle(const btVector3& v0, const btVector3& v1,
			const btVector3& v2, const btVector3& color, btScalar alpha) {
		if (inRange(v1)) {
			m_pPalDebugDraw->m_Triangles.m_vVertices.push_back(palVector3(
					v0.x(), v0.y(), v0.z()));
			m_pPalDebugDraw->m_Triangles.m_vVertices.push_back(palVector3(
					v1.x(), v1.y(), v1.z()));
			m_pPalDebugDraw->m_Triangles.m_vVertices.push_back(palVector3(
					v2.x(), v2.y(), v2.z()));
			for (unsigned i = 0; i < 3; ++i) {
				m_pPalDebugDraw->m_Triangles.m_vColors.push_back(palVector4(
						color.x(), color.y(), color.z(), alpha));
			}
		}
	}

	virtual void drawContactPoint(const btVector3& PointOnB,
			const btVector3& normalOnB, btScalar distance, int lifeTime,
			const btVector3& color) {
		if (inRange(PointOnB)) {
			m_pPalDebugDraw->m_Points.m_vVertices.push_back(palVector3(
					PointOnB.x(), PointOnB.y(), PointOnB.z()));
			m_pPalDebugDraw->m_Points.m_vColors.push_back(palVector4(color.x(),
					color.y(), color.z(), 1.0f));
		}
	}

	virtual void reportErrorWarning(const char* warningString) {
	}

	virtual void draw3dText(const btVector3& location, const char* textString) {
		if (inRange(location)) {
			palDebugText debugText;
			for (unsigned i = 0; i < 3; ++i) {
				debugText.m_vPos._vec[i] = location[i];
			}
			debugText.text = textString;
			m_pPalDebugDraw->m_vTextItems.push_back(debugText);
		}
	}

	virtual void setDebugMode(int debugMode) {}

	virtual int getDebugMode() const { return DBG_DrawFeaturesText |
		DBG_DrawContactPoints |
		DBG_DrawText |
		DBG_DrawWireframe |
		//DBG_ProfileTimings |
		//DBG_EnableSatComparison |
		//DBG_DisableBulletLCP |
		//DBG_EnableCCD |
		DBG_DrawConstraints |
		DBG_FastWireframe |
		DBG_DrawConstraintLimits;
; }

	void SetPalDebugDraw(palDebugDraw *newDebugDraw) { m_pPalDebugDraw = newDebugDraw; }
	palDebugDraw *GetPalDebugDraw() { return m_pPalDebugDraw; }
private:
	palDebugDraw *m_pPalDebugDraw;
};

static btDiscreteDynamicsWorld* g_DynamicsWorld = NULL;
static bool g_bEnableCustomMaterials = false;

static bool CustomMaterialCombinerCallback(btManifoldPoint& cp, const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
{
	if (g_bEnableCustomMaterials)
	{
		btAdjustInternalEdgeContacts(cp,colObj1,colObj0, partId1,index1);
		//btAdjustInternalEdgeContacts(cp,colObj1,colObj0, partId1,index1, BT_TRIANGLE_CONVEX_BACKFACE_MODE);
		//btAdjustInternalEdgeContacts(cp,colObj1,colObj0, partId1,index1, BT_TRIANGLE_CONVEX_DOUBLE_SIDED+BT_TRIANGLE_CONCAVE_DOUBLE_SIDED);
	}
	return true;
}

struct CustomOverlapFilterCallback: public btOverlapFilterCallback
{
	virtual ~CustomOverlapFilterCallback()
	{}
	// return true when pairs need collision
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0,btBroadphaseProxy* proxy1) const {
		palBulletPhysics* physics = static_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics());

		short proxy0GroupBits = proxy0->m_collisionFilterGroup;
		short proxy1GroupBits = proxy1->m_collisionFilterGroup;

		palGroup p0Group = convert_to_pal_group(proxy0GroupBits);
		palGroup p1Group = convert_to_pal_group(proxy1GroupBits);

		// if it hasn't been set, then default to colliding.
		size_t maskVectorSize = physics->m_CollisionMasks.size();
		if (maskVectorSize <= size_t(p0Group)
					|| maskVectorSize <= size_t(p1Group)) {
			return true;
		}

		short proxy0Mask = physics->m_CollisionMasks[p0Group];
		short proxy1Mask = physics->m_CollisionMasks[p1Group];

		proxy0->m_collisionFilterMask = proxy0Mask;
		proxy1->m_collisionFilterMask = proxy1Mask;

		bool collides = ((proxy0GroupBits & proxy1Mask) != 0) && ((proxy1GroupBits & proxy0Mask) != 0);
		return collides;
	}
};

////////////////////////////////////////////////////
class palBulletAction : public btActionInterface {
public:
	palBulletAction(palAction& action)
	: mAction(action)
	{
	}

	~palBulletAction() {}

	virtual void updateAction(btCollisionWorld *collisionWorld, btScalar deltaTimeStep)
	{
		mAction(deltaTimeStep);
	}

	// Need to call and pass the pal debug drawer to the action.
	virtual void debugDraw(btIDebugDraw *debugDrawer) {}
private:
	palAction& mAction;
};

////////////////////////////////////////////////////
void palBulletPhysics::SetGroupCollision(palGroup a, palGroup b, bool enabled) {
	short bits = convert_group(a);
	short other_bits = convert_group(b);

	if (m_CollisionMasks.size() <= size_t(std::max(a, b)))
	{
		m_CollisionMasks.resize(std::max(a,b)+1, ~0);
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

	rayCallback.m_collisionFilterGroup = ~0;
	rayCallback.m_collisionFilterMask = ~0;

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
		m_collisionFilterGroup = ~0;
		m_collisionFilterMask = (short) groupFilter;
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

//		if ((body->getBroadphaseProxy()->m_collisionFilterGroup & (short)(m_groupFilter)) == 0)
//		{
//			return m_lastFraction;
//		}

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
	if (body && body->m_pbtBody != NULL) {
		//reset the group to get rid of the default groups.
		palGroup group = body->GetGroup();
		g_DynamicsWorld->addRigidBody(body->m_pbtBody, convert_group(group), m_CollisionMasks[group]);
	}
}

void palBulletPhysics::RemoveRigidBody(palBulletBodyBase* body) {
	if (body && body->m_pbtBody) {
		g_DynamicsWorld->removeRigidBody(body->m_pbtBody);
		delete body->m_pbtBody->getBroadphaseHandle();
		body->m_pbtBody->setBroadphaseHandle(NULL);
	}
}

void palBulletPhysics::ClearBroadPhaseCachePairs(palBulletBodyBase *body) {
	btBroadphaseProxy *proxy = body->BulletGetRigidBody()->getBroadphaseProxy();
	if (proxy != NULL) {
		proxy->m_collisionFilterMask = m_CollisionMasks[body->GetGroup()];
		g_DynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(proxy,
					g_DynamicsWorld->getDispatcher());
		// Reset the collision mask
	}
}

void palBulletPhysics::AddAction(palAction *action) {
	if (action != NULL) {
		palBulletAction* bulletAction = new palBulletAction(*action);
		m_BulletActions[action] = bulletAction;
		g_DynamicsWorld->addAction(bulletAction);
	}
}

void palBulletPhysics::RemoveAction(palAction *action) {
	if (action != NULL) {
		PAL_MAP<palAction*, btActionInterface*>::iterator item = m_BulletActions.find(action);
		if (item != m_BulletActions.end()) {
			btActionInterface* bulletAction = item->second;
			if (bulletAction != NULL) {
				g_DynamicsWorld->removeAction(bulletAction);
				delete bulletAction;
				bulletAction = NULL;
			}
			m_BulletActions.erase(item);
		}
	}
}

void palBulletPhysics::CallActions(Float timestep) {
// Do nothing here.  The dynamics world does this stuff.
}

static PAL_MAP <btCollisionObject*, btCollisionObject*> pallisten;
static PAL_VECTOR<palContactPoint> g_contacts;

#ifdef USE_LISTEN_COLLISION
static bool listen_collision(btCollisionObject* b0, btCollisionObject* b1) {
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
#endif

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

void palBulletPhysics::GetContacts(palBodyBase *pBody, palContact& contact) const {
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
void palBulletPhysics::GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact) const {
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

palBulletPhysics::palBulletPhysics()
: m_fFixedTimeStep(0.0f)
, set_substeps(1)
, set_pe(1)
, m_dynamicsWorld(NULL)
, m_dispatcher(NULL)
, m_pbtDebugDraw(NULL)
{}

const char* palBulletPhysics::GetPALVersion() const {
	static char verbuf[512];
	sprintf(verbuf,"PAL SDK V%d.%d.%d\nPAL Bullet V:%d.%d.%d\nFile: %s\nCompiled: %s %s\nModified:%s",
		PAL_SDK_VERSION_MAJOR,PAL_SDK_VERSION_MINOR,PAL_SDK_VERSION_BUGFIX,
		BULLET_PAL_SDK_VERSION_MAJOR,BULLET_PAL_SDK_VERSION_MINOR,BULLET_PAL_SDK_VERSION_BUGFIX,
		__FILE__,__TIME__,__DATE__,__TIMESTAMP__);
	return verbuf;
}

const char* palBulletPhysics::GetVersion() const {
	static char verbuf[256];
	int v = btGetVersion();
	sprintf(verbuf,"Bullet V%d.%d",v/100,v%100);
	return verbuf;
}

void palBulletPhysics::SetFixedTimeStep(Float fixedStep) {
	m_fFixedTimeStep = fixedStep;
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

bool palBulletPhysics::GetHardware(void) const {
	//TODO: return if using SPE's
	return false;
}

void palBulletPhysics::Init(const palPhysicsDesc& desc) {
   palPhysics::Init(desc);

	btBroadphaseInterface*	broadphase;
	btConstraintSolver*	solver;
#if 0
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	broadphase = new btAxisSweep3(worldMin,worldMax);
	//probably a memory leak...
#else
	broadphase = new btDbvtBroadphase();
#endif
	broadphase->getOverlappingPairCache()->setOverlapFilterCallback(new CustomOverlapFilterCallback);
	// so ghosts and the character controller will work.
	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
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

	if (m_Properties["Bullet_UseInternalEdgeUtility"] == "true") {
		g_bEnableCustomMaterials = true;
		gContactAddedCallback = &CustomMaterialCombinerCallback;
	} else {
		g_bEnableCustomMaterials = false;
		gContactAddedCallback = NULL;
	}

//	m_dynamicsWorld = new btSimpleDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_solver);

	btSoftRigidDynamicsWorld* dynamicsWorld = new btSoftRigidDynamicsWorld(m_dispatcher, broadphase, solver,collisionConfiguration);
	m_dynamicsWorld = dynamicsWorld;

	m_softBodyWorldInfo.m_dispatcher = m_dispatcher;
	m_softBodyWorldInfo.m_broadphase = broadphase;

	btVector3 gravity(m_fGravityX, m_fGravityY, m_fGravityZ);
	m_dynamicsWorld->setGravity(gravity);
	m_softBodyWorldInfo.m_gravity = gravity;

	m_softBodyWorldInfo.m_sparsesdf.Initialize();

	m_pbtDebugDraw = new palBulletDebugDraw;
	g_DynamicsWorld = m_dynamicsWorld;

	m_CollisionMasks.resize(32U, ~0);

	m_dynamicsWorld->getSolverInfo().m_solverMode =
			SOLVER_USE_FRICTION_WARMSTARTING | SOLVER_USE_2_FRICTION_DIRECTIONS
			| SOLVER_RANDMIZE_ORDER | SOLVER_USE_WARMSTARTING | SOLVER_SIMD;
	// Reset so it assigns it to the world properly
	SetSolverAccuracy(palSolver::GetSolverAccuracy());
}

void palBulletPhysics::Cleanup() {
	delete m_dynamicsWorld;
	delete m_dispatcher;
	delete m_pbtDebugDraw;

	m_dynamicsWorld = NULL;
	m_dispatcher = NULL;
	m_pbtDebugDraw = NULL;
	g_DynamicsWorld = NULL;

	PAL_MAP<palAction*, btActionInterface*>::iterator i, iend;
	i = m_BulletActions.begin();
	iend = m_BulletActions.end();
	for (; i != iend; ++i) {
		btActionInterface* bulletAction = i->second;
		delete bulletAction;
	}
	// This isn't really necessary, I just don't like bad pointers hanging around.
	m_BulletActions.clear();
}

void palBulletPhysics::StartIterate(Float timestep) {
	g_contacts.clear(); //clear all contacts before the update TODO: CHECK THIS IS SAFE FOR MULTITHREADED!
	if (m_dynamicsWorld) {

		palDebugDraw* debugDraw = GetDebugDraw();
		m_pbtDebugDraw->SetPalDebugDraw(debugDraw);
		if (debugDraw != NULL) {
			m_dynamicsWorld->setDebugDrawer(m_pbtDebugDraw);
		} else {
			m_dynamicsWorld->setDebugDrawer(NULL);

		}

		if (m_fFixedTimeStep > 0) {
			m_dynamicsWorld->stepSimulation(timestep,set_substeps,m_fFixedTimeStep);
		} else {
			m_dynamicsWorld->stepSimulation(timestep,0);
		}

		if (debugDraw != NULL) {
;			m_dynamicsWorld->debugDrawWorld();
		}

		//collision iteration
		int i;
		int numManifolds = m_dispatcher->getNumManifolds();
		for (i=0;i<numManifolds;i++)
		{
			btPersistentManifold* contactManifold = m_dispatcher->getManifoldByIndexInternal(i);
			btCollisionObject* obA = static_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = static_cast<btCollisionObject*>(contactManifold->getBody1());
#ifdef USE_LISTEN_COLLISION
			if (listen_collision(obA,obB)) {
#endif
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
					cp.m_fImpulse= pt.getAppliedImpulse();

					if (pt.m_lateralFrictionInitialized)
					{
						for (unsigned i = 0; i < 3; ++i)
						{
							cp.m_vImpulseLateral1[i] = pt.m_lateralFrictionDir1[i] * pt.m_appliedImpulseLateral1;
							cp.m_vImpulseLateral2[i] = pt.m_lateralFrictionDir2[i] * pt.m_appliedImpulseLateral2;
						}
					}

					g_contacts.push_back(cp);
				}
#ifdef USE_LISTEN_COLLISION
			}
#endif
		}
	}
}

void palBulletPhysics::SetSolverAccuracy(Float fAccuracy) {
	palSolver::SetSolverAccuracy(fAccuracy);
	if (m_dynamicsWorld != NULL) {
		if (fAccuracy < 1.0f)
			fAccuracy = 1.0f;
		m_dynamicsWorld->getSolverInfo().m_numIterations = int(fAccuracy);
	}
}

float palBulletPhysics::GetSolverAccuracy() const {
	// if they set 0-1, we want to return that, otherwise, return int value.
	float result = palSolver::GetSolverAccuracy();
	if (result >= 1.0f && m_dynamicsWorld != NULL) {
		// Add one because the accuracy is 0 based.
		result = float(m_dynamicsWorld->getSolverInfo().m_numIterations);
	}
	return result;
}

bool palBulletPhysics::QueryIterationComplete() const {
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
palBulletBodyBase::palBulletBodyBase()
  : m_pbtBody(0)
  , m_fSkinWidth() {}

palBulletBodyBase::~palBulletBodyBase() {
	palBulletPhysics* bulletPhysics = dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics());
	if (bulletPhysics) {
		bulletPhysics->RemoveRigidBody(this);
	}
	if (m_pbtBody) {
		Cleanup();

		while (m_pbtBody->getNumConstraintRefs() > 0) {
			g_DynamicsWorld->removeConstraint(m_pbtBody->getConstraintRef(0));
		}

		delete m_pbtBody->getMotionState();
		delete m_pbtBody->getBroadphaseHandle();
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
		//m_pbtBody->
		m_pbtBody->setFriction(material->m_fStatic);
		m_pbtBody->setRestitution(material->m_fRestitution);
		if (material->m_bEnableAnisotropicFriction)
		{
			btVector3 btVec;
			for (unsigned i = 0; i < 3; ++i)
			{
				btVec[i] = material->m_vStaticAnisotropic._vec[i];
			}
			m_pbtBody->setAnisotropicFriction(btVec);
		}
	}
}

void palBulletBodyBase::BuildBody(const palMatrix4x4& pos, Float mass,
			palDynamicsType dynType,
			btCollisionShape *btShape,
			const palVector3& palInertia) {

	btTransform trans;

	convertPalMatToBtTransform(trans, pos);

	btVector3 localInertia(palInertia.x, palInertia.y, palInertia.z);

	btDefaultMotionState* motionState = new btDefaultMotionState(trans);
	btCollisionShape *pShape = btShape;

	//no given shape? get from geom
	if (!btShape) {
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (m_Geometries[0]);
		localInertia.setValue(pbtg->m_fInertiaXX, pbtg->m_fInertiaYY, pbtg->m_fInertiaZZ);
		pShape = pbtg->m_pbtShape;
	}

	if (dynType != PALBODY_DYNAMIC) {
		mass = 0;
		localInertia.setValue(0.0f, 0.0f, 0.0f);
	}

	m_pbtBody = new btRigidBody(mass,motionState,pShape,localInertia);
	m_pbtBody->setUserPointer(dynamic_cast<palBodyBase*>(this));
	// Disabling deactivition is really bad.  objects will never go to sleep. which is bad for
	// performance
	//m_pbtBody->setActivationState(DISABLE_DEACTIVATION);

	AssignDynamicsType(dynType, mass, localInertia);

	m_pbtBody->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK | m_pbtBody->getCollisionFlags());
	dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics())->AddRigidBody(this);
}

void palBulletBodyBase::AssignDynamicsType(palDynamicsType dynType, Float mass, const btVector3& inertia) {
	int currFlags = m_pbtBody->getCollisionFlags();

	switch (dynType) {
		case PALBODY_DYNAMIC:
		{
			currFlags &= (~btCollisionObject::CF_STATIC_OBJECT);
			currFlags &= (~btCollisionObject::CF_KINEMATIC_OBJECT);
			m_pbtBody->setMassProps(mass, inertia);
			break;
		}
		case PALBODY_STATIC:
		{
			currFlags |= btCollisionObject::CF_STATIC_OBJECT;
			currFlags &= (~btCollisionObject::CF_KINEMATIC_OBJECT);
			m_pbtBody->setMassProps(btScalar(0.0), btVector3(0.0f, 0.0f, 0.0f));
			break;
		}
		case PALBODY_KINEMATIC:
		{
			currFlags &= (~btCollisionObject::CF_STATIC_OBJECT);
			currFlags |= btCollisionObject::CF_KINEMATIC_OBJECT;
			m_pbtBody->setMassProps(btScalar(0.0), btVector3(0.0f, 0.0f, 0.0f));
			break;
		}
	}

	m_pbtBody->setCollisionFlags(currFlags);
	m_pbtBody->updateInertiaTensor();

}

void palBulletBodyBase::SetPosition(const palMatrix4x4& location) {
	if (m_pbtBody) {
		btTransform newloc;
	   convertPalMatToBtTransform(newloc, location);
		if (m_pbtBody->getMotionState() != NULL)
		{
			m_pbtBody->getMotionState()->setWorldTransform(newloc);
		}
		m_pbtBody->setCenterOfMassTransform(newloc);
		m_pbtBody->activate();
	} else {
		palBodyBase::SetPosition(location);
	}
}

const btTransform palBulletBodyBase::GetWorldTransform() const {
	btTransform xform;
	if (m_pbtBody) {
		if (m_pbtBody->getMotionState() != NULL) {
			m_pbtBody->getMotionState()->getWorldTransform(xform);
		}
		else {
			xform = m_pbtBody->getWorldTransform();
		}
	}
	else {
		convertPalMatToBtTransform(xform, m_mLoc);
	}

	return xform;
}

const palMatrix4x4& palBulletBodyBase::GetLocationMatrix() const {
	if (m_pbtBody) {
		if (m_pbtBody->getMotionState() != NULL)
		{
			btTransform xform;
			m_pbtBody->getMotionState()->getWorldTransform(xform);
			convertBtTransformToPalMat(m_mLoc, xform);
		}
		else
		{
			convertBtTransformToPalMat(m_mLoc, m_pbtBody->getWorldTransform());
		}
	}

	return m_mLoc;
}

palGroup palBulletBodyBase::GetGroup() const {
	if (!m_pbtBody || m_pbtBody->getBroadphaseProxy() == NULL)
		return palBodyBase::GetGroup();
	return convert_to_pal_group(m_pbtBody->getBroadphaseProxy()->m_collisionFilterGroup);
}

void palBulletBodyBase::SetGroup(palGroup group) {
	bool changing = group != GetGroup();
	palBodyBase::SetGroup(group);

	if (!changing || !m_pbtBody || m_pbtBody->getBroadphaseProxy() == NULL)
		return;
	palBulletPhysics* bulletPhysics = dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics());

	m_pbtBody->getBroadphaseProxy()->m_collisionFilterGroup = convert_group(group);

	bulletPhysics->ClearBroadPhaseCachePairs(this);

	//bulletPhysics->RemoveRigidBody(this);
	//bulletPhysics->AddRigidBody(this);
}

Float palBulletBodyBase::GetSkinWidth() const {
	if (m_pbtBody != NULL) {
		return m_pbtBody->getContactProcessingThreshold();
	}
	return m_fSkinWidth;
}

bool palBulletBodyBase::SetSkinWidth(Float skinWidth) {
	m_fSkinWidth = skinWidth;
	if (m_pbtBody != NULL) {
		m_pbtBody->setContactProcessingThreshold(btScalar(skinWidth));
	}
	return true;
}

///////////////
palBulletBody::palBulletBody() {
}

palBulletBody::~palBulletBody() {
}

/*
* palActivation implementation
*/
// Bullet supports them all
const std::bitset<palBulletBody::DUMMY_ACTIVATION_SETTING_TYPE> 
	palBulletBody::SUPPORTED_SETTINGS = std::bitset<palBulletBody::DUMMY_ACTIVATION_SETTING_TYPE>(~(0xFFFFFFFF << palBulletBody::DUMMY_ACTIVATION_SETTING_TYPE));

Float palBulletBody::GetActivationLinearVelocityThreshold() const {
	Float velocity;
	if (m_pbtBody) {
		velocity = m_pbtBody->getLinearSleepingThreshold();
	}
	else {
		velocity = Float(-1.0);
	}
	return velocity;
}

void palBulletBody::SetActivationLinearVelocityThreshold(Float velocity) {
	if (m_pbtBody) {
		m_pbtBody->setSleepingThresholds(btScalar(velocity), btScalar(m_pbtBody->getAngularSleepingThreshold()));
	}
}

Float palBulletBody::GetActivationAngularVelocityThreshold() const {
	Float velocity;
	if (m_pbtBody) {
		velocity = m_pbtBody->getAngularSleepingThreshold();
	}
	else {
		velocity = Float(-1.0);
	}
	return velocity;
}

void palBulletBody::SetActivationAngularVelocityThreshold(Float omega) {
	if (m_pbtBody) {
		m_pbtBody->setSleepingThresholds(btScalar(m_pbtBody->getLinearSleepingThreshold()), btScalar(omega));
	}
}

Float palBulletBody::GetActivationTimeThreshold() const {
	Float timeThreshold;
	//if (m_pbtBody) {
		timeThreshold = gDeactivationTime;
	//}
	//else {
	//	timeThreshold = Float(-1.0);
	//}
	return timeThreshold;
}

void palBulletBody::SetActivationTimeThreshold(Float timeThreshold) {
	//if (m_pbtBody) {
		// Yes, it's global, and yes if you set it, it sets the global one.
		gDeactivationTime = timeThreshold;
	//}
}

const std::bitset<palBulletBody::DUMMY_ACTIVATION_SETTING_TYPE>& palBulletBody::GetSupportedActivationSettings() const {
	return SUPPORTED_SETTINGS;
}


///////////////
palBulletGenericBody::palBulletGenericBody()
: m_bGravityEnabled(true)
, m_pCompound(NULL)
, m_pConcave(NULL)
{

}

palBulletGenericBody::~palBulletGenericBody()
{
	delete m_pCompound;
	m_pCompound = NULL;
	delete m_pConcave;
	m_pConcave = NULL;
}

void palBulletGenericBody::Init(const palMatrix4x4 &pos) {

	palGenericBody::Init(pos);
	palVector3 pvInertia;
	GetInertia(pvInertia.x, pvInertia.y, pvInertia.z);

	for (unsigned i = 0; i < 3; ++i) {
		if (pvInertia._vec[i] < SIMD_EPSILON) {
			pvInertia._vec[i] = 1.0f;
		}
	}

	if (IsUsingOneCenteredGeometry()) {
		BuildBody(pos, m_fMass, GetDynamicsType(), NULL, pvInertia);
	} else if (IsUsingConcaveShape()) {
		RebuildConcaveShapeFromGeometry();
		BuildBody(pos, m_fMass, GetDynamicsType(), m_pConcave, pvInertia);
	} else {
		InitCompoundIfNull();
		BuildBody(pos, m_fMass, GetDynamicsType(), m_pCompound, pvInertia);
	}

	//Reset now that the body is created.
	SetGravityEnabled(IsGravityEnabled());
}

bool palBulletGenericBody::IsDynamic() const {
	if (m_pbtBody != NULL) {
		return !m_pbtBody->isStaticOrKinematicObject();
	}
	return palBulletGenericBody::IsDynamic();
}

bool palBulletGenericBody::IsKinematic() const {
	if (m_pbtBody != NULL) {
		return m_pbtBody->isKinematicObject();
	}
	return palBulletGenericBody::IsKinematic();
}

bool palBulletGenericBody::IsStatic() const {
	if (m_pbtBody != NULL) {
		return m_pbtBody->isStaticObject();
	}
	return palBulletGenericBody::IsStatic();
}

void palBulletGenericBody::SetDynamicsType(palDynamicsType dynType) {
	palGenericBody::SetDynamicsType(dynType);

	if (m_pbtBody == NULL) {
		return;
	}

	btVector3 inertia(m_fInertiaXX, m_fInertiaYY, m_fInertiaZZ);
	AssignDynamicsType(dynType, m_fMass, inertia);
	//Have to reset gravity after setting the dynamics type because statics and kinematics have 0 gravity.
	SetGravityEnabled(IsGravityEnabled());
	palBulletPhysics* physics = dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics());
	physics->RemoveRigidBody(this);
	physics->AddRigidBody(this);
}


void palBulletGenericBody::SetMass(Float mass) {
	palGenericBody::SetMass(mass);
	if (m_pbtBody && m_eDynType == PALBODY_DYNAMIC) {
		btVector3 inertia(m_fInertiaXX, m_fInertiaYY, m_fInertiaZZ);
		m_pbtBody->setMassProps(btScalar(mass), inertia);
		m_pbtBody->updateInertiaTensor();
	}
}

void palBulletGenericBody::SetGravityEnabled(bool enabled) {
	if (m_pbtBody != NULL) {
		if (enabled && m_eDynType == PALBODY_DYNAMIC) {
			palVector3 pv;
			palFactory::GetInstance()->GetActivePhysics()->GetGravity(pv);
			m_pbtBody->setGravity(btVector3(pv.x, pv.y, pv.z));
		} else {
			m_pbtBody->setGravity(btVector3(btScalar(0.0), btScalar(0.0), btScalar(0.0)));
		}
	}
	m_bGravityEnabled = enabled;
}

bool palBulletGenericBody::IsGravityEnabled() const {
	return m_bGravityEnabled;
}

void palBulletGenericBody::SetCollisionResponseEnabled(bool enabled) {
	if (m_pbtBody != NULL) {
		int collisionFlags = m_pbtBody->getCollisionFlags();
		if (enabled) {
			m_pbtBody->setCollisionFlags(collisionFlags & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		} else {
			m_pbtBody->setCollisionFlags(collisionFlags | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
	}
}

bool palBulletGenericBody::IsCollisionResponseEnabled() const {
	bool result = true;
	if (m_pbtBody != NULL) {
		int collisionFlags = m_pbtBody->getCollisionFlags();
		result = !(collisionFlags & btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	return result;
}

void palBulletGenericBody::SetInertia(Float Ixx, Float Iyy, Float Izz) {
	palGenericBody::SetInertia(Ixx, Iyy, Izz);
	if (m_pbtBody && m_eDynType == PALBODY_DYNAMIC) {
		btVector3 inertia(m_fInertiaXX, m_fInertiaYY, m_fInertiaZZ);
		m_pbtBody->setMassProps(btScalar(m_fMass), inertia);
		m_pbtBody->updateInertiaTensor();
	}
}

void palBulletGenericBody::SetLinearDamping(Float damping)
{
	palGenericBody::SetLinearDamping(damping);
	if (m_pbtBody)
	{
		m_pbtBody->setDamping(btScalar(damping), m_pbtBody->getAngularDamping());
	}
}

Float palBulletGenericBody::GetLinearDamping() const
{
	if (m_pbtBody)
	{
		return Float(m_pbtBody->getLinearDamping());
	}
	return palGenericBody::GetLinearDamping();
}

void palBulletGenericBody::SetAngularDamping(Float damping)
{
	palGenericBody::SetAngularDamping(damping);
	if (m_pbtBody)
	{
		m_pbtBody->setDamping(m_pbtBody->getLinearDamping(), damping);
	}
}

Float palBulletGenericBody::GetAngularDamping() const
{
	if (m_pbtBody)
	{
		return Float(m_pbtBody->getAngularDamping());
	}
	return palGenericBody::GetAngularDamping();
}

void palBulletGenericBody::SetMaxAngularVelocity(Float maxAngVel)
{
	palGenericBody::SetMaxAngularVelocity(maxAngVel);
	// TODO this will have to be done at tick time.
}

Float palBulletGenericBody::GetMaxAngularVelocity() const
{
	return palGenericBody::GetMaxAngularVelocity();
}


void palBulletGenericBody::InitCompoundIfNull() {
	if (m_pCompound == NULL) {
		m_pCompound = new btCompoundShape(true);
		for (unsigned i = 0; i < m_Geometries.size(); ++i)
			AddShapeToCompound(m_Geometries[i]);
	}
}

void palBulletGenericBody::AddShapeToCompound(palGeometry* pGeom) {
	if (m_pCompound == NULL)
		return;

	palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (pGeom);
	palMatrix4x4 m = pbtg->GetOffsetMatrix();//GetLocationMatrix();
	btTransform localTrans;
	convertPalMatToBtTransform(localTrans, m);
	if (pbtg->BulletGetCollisionShape()->isCompound() || pbtg->BulletGetCollisionShape()->isConvex()) {
		// Ugh, Can't add a concave shape to a compound shape.
		m_pCompound->addChildShape(localTrans, pbtg->BulletGetCollisionShape());
	}
}

void palBulletGenericBody::RemoveShapeFromCompound(palGeometry* pGeom) {
	if (m_pCompound == NULL) {
		return;
	}

	palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (pGeom);
	m_pCompound->removeChildShape(pbtg->BulletGetCollisionShape());
}

bool palBulletGenericBody::IsUsingOneCenteredGeometry() const {
	bool result = false;
	if (m_Geometries.size() == 1) {
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (m_Geometries[0]);
		palMatrix4x4 m = pbtg->GetOffsetMatrix();
		if (mat_is_identity(&m))
			result = true;
	}
	return result;
}

bool palBulletGenericBody::IsUsingConcaveShape() const {
	bool concaveFound = false;
	if (!m_Geometries.empty()) {
		for (size_t i = 0; i < m_Geometries.size(); ++i) {
			palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *>(m_Geometries[i]);
			concaveFound = pbtg->BulletGetCollisionShape()->isConcave();
			if (concaveFound)
				break;
		}
	}
	return concaveFound;
}

void palBulletGenericBody::RebuildConcaveShapeFromGeometry() {
	delete m_pConcave;
	m_pConcave = NULL;

	btTriangleMesh* tmesh = new btTriangleMesh(true, false);

	for (size_t i = 0; i < m_Geometries.size(); ++i) {
		palGeometry* pGeom = m_Geometries[i];
		int* pIndices = pGeom->GenerateMesh_Indices();
		Float* pVertices = pGeom->GenerateMesh_Vertices();
		int nIndices = pGeom->GetNumberOfIndices();

		int pi = 0;
		for (int i=0;i<nIndices/3;i++) {
			pi = pIndices[i*3+0];
			btVector3 v0(  pVertices[pi*3+0],
						pVertices[pi*3+1],
						pVertices[pi*3+2]);
			pi = pIndices[i*3+1];
			btVector3 v1(  pVertices[pi*3+0],
						pVertices[pi*3+1],
						pVertices[pi*3+2]);
			pi = pIndices[i*3+2];
			btVector3 v2(  pVertices[pi*3+0],
						pVertices[pi*3+1],
						pVertices[pi*3+2]);
			tmesh->addTriangle(v0,v1,v2);
		}
	}
	m_pConcave = new btBvhTriangleMeshShape(tmesh, true, true);
}

void palBulletGenericBody::ConnectGeometry(palGeometry* pGeom) {
	palGenericBody::ConnectGeometry(pGeom);

 	if (m_pbtBody != NULL)
	{
		if (IsUsingOneCenteredGeometry()) {
			palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (pGeom);
			btCollisionShape* shape = pbtg->BulletGetCollisionShape();
			m_pbtBody->setCollisionShape(shape);
		} else if (IsUsingConcaveShape()) {
			RebuildConcaveShapeFromGeometry();
			m_pbtBody->setCollisionShape(m_pConcave);
		} else {
			AddShapeToCompound(pGeom);
			// This is done after the above on purpose
			InitCompoundIfNull();
			m_pbtBody->setCollisionShape(m_pCompound);
		}
		palBulletPhysics* physics = dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics());
		physics->RemoveRigidBody(this);
		physics->AddRigidBody(this);
	}
}

void palBulletGenericBody::RemoveGeometry(palGeometry* pGeom)
{
	palGenericBody::RemoveGeometry(pGeom);
	if (m_pbtBody != NULL)
	{
		if (IsUsingOneCenteredGeometry()) {
			palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (pGeom);
			btCollisionShape* shape = pbtg->BulletGetCollisionShape();
			m_pbtBody->setCollisionShape(shape);
		} else if (IsUsingConcaveShape()) {
			RebuildConcaveShapeFromGeometry();
			m_pbtBody->setCollisionShape(m_pConcave);
		} else {
			RemoveShapeFromCompound(pGeom);
			// This is done after the above on purpose
			InitCompoundIfNull();
			m_pbtBody->setCollisionShape(m_pCompound);
		}
		palBulletPhysics* physics = dynamic_cast<palBulletPhysics*>(palFactory::GetInstance()->GetActivePhysics());
		physics->RemoveRigidBody(this);
		physics->AddRigidBody(this);
	}
}


void palBulletCompoundBody::SetPosition(const palMatrix4x4& location) {
	palBulletBodyBase::SetPosition(location);
}

const palMatrix4x4& palBulletStaticCompoundBody::GetLocationMatrix() const {
	return palBulletCompoundBody::GetLocationMatrix();
}

const palMatrix4x4& palBulletCompoundBody::GetLocationMatrix() const {
	if (m_pbtBody) {
		btTransform t;
		m_pbtBody->getMotionState()->getWorldTransform(t);
		convertBtTransformToPalMat(m_mLoc, t);
	}
	return m_mLoc;
}


bool palBulletBody::IsActive() const
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
		m_pbtBody->activate();
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

void palBulletBody::GetLinearVelocity(palVector3& velocity) const {
	btVector3 vel = m_pbtBody->getLinearVelocity();
	velocity.x = vel.x();
	velocity.y = vel.y();
	velocity.z = vel.z();
}
void palBulletBody::GetAngularVelocity(palVector3& velocity) const {
	btVector3 vel = m_pbtBody->getAngularVelocity();
	velocity.x = vel.x();
	velocity.y = vel.y();
	velocity.z = vel.z();
}

void palBulletBody::SetLinearVelocity(const palVector3& velocity) {
	m_pbtBody->setLinearVelocity(btVector3(velocity.x,velocity.y,velocity.z));
}

void palBulletBody::SetAngularVelocity(const palVector3& velocity) {
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
		convertPalMatToBtTransform(localTrans, m);

		compound->addChildShape(localTrans,pbtg->m_pbtShape);
	}

   palVector3 pvInertia(m_fInertiaXX, m_fInertiaYY, m_fInertiaZZ);
   //Set the position to 0 since it will be moved in a sec.
   BuildBody(m_mLoc, m_fMass, PALBODY_STATIC, compound, pvInertia);
}


palBulletCompoundBody::palBulletCompoundBody() {
}

void palBulletCompoundBody::Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ) {
	btCompoundShape* compound = new btCompoundShape();
	for (PAL_VECTOR<palGeometry *>::size_type i=0;i<m_Geometries.size();i++) {
		palBulletGeometry *pbtg=dynamic_cast<palBulletGeometry *> (m_Geometries[i]);

		palMatrix4x4 m = pbtg->GetOffsetMatrix();//GetLocationMatrix();

		btTransform localTrans;
		convertPalMatToBtTransform(localTrans, m);

		compound->addChildShape(localTrans,pbtg->m_pbtShape);
	}

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(m_mLoc._41, m_mLoc._42, m_mLoc._43));

	palVector3 pvInertia(iXX, iYY, iZZ);

	//Set the position to 0 since it will be moved in a sec.
   BuildBody(m_mLoc, finalMass, PALBODY_DYNAMIC, compound, pvInertia);
   m_fMass = finalMass;
}

palBulletGeometry::palBulletGeometry()
  : m_pbtShape(NULL) {}

palBulletGeometry::~palBulletGeometry() {
	delete m_pbtShape;
	m_pbtShape = NULL;
}

Float palBulletGeometry::GetMargin() const {
	return m_pbtShape == NULL ? Float(0.0) : Float(m_pbtShape->getMargin());
}

bool palBulletGeometry::SetMargin(Float margin) {
	if (m_pbtShape)
	{
		if (margin < 0.0) {
			m_pbtShape->setMargin(btScalar(0.04));
		} else {
			m_pbtShape->setMargin(btScalar(margin));
		}
	}
	return true;
}

palBulletBoxGeometry::palBulletBoxGeometry()
: m_pbtBoxShape(NULL) {}

void palBulletBoxGeometry::Init(const palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	palVector3 dim = GetXYZDimensions();

	m_pbtBoxShape = new btBoxShape(btVector3(dim.x*(Float)0.5,dim.y*(Float)0.5,dim.z*(Float)0.5));
	m_pbtShape = m_pbtBoxShape;
	//m_pbtShape->setMargin(0.0f);
}

palBulletBox::palBulletBox() {
}

void palBulletBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	palMatrix4x4 mat;
	mat_identity(&mat);
	mat_set_translation(&mat, x, y, z);
	BuildBody(mat, mass);
}

palBulletStaticBox::palBulletStaticBox() {
}

void palBulletStaticBox::Init(const palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palStaticBox::Init(pos,width,height,depth);
	BuildBody(m_mLoc, 0, PALBODY_STATIC);
	palBulletBodyBase::SetPosition(pos);
}

palBulletStaticSphere::palBulletStaticSphere() {
}

void palBulletStaticSphere::Init(const palMatrix4x4 &pos, Float radius) {
	palStaticSphere::Init(pos,radius);
	BuildBody(m_mLoc, 0, PALBODY_STATIC);
	palBulletBodyBase::SetPosition(pos);
}

palBulletStaticCapsule::palBulletStaticCapsule() {
}

void palBulletStaticCapsule::Init(Float x, Float y, Float z, Float radius, Float length) {
	palStaticCapsule::Init(x,y,z,radius,length);
	BuildBody(m_mLoc, 0, PALBODY_STATIC);
}


palBulletSphereGeometry::palBulletSphereGeometry()
  : m_btSphereShape(0) {}

void palBulletSphereGeometry::Init(const palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	m_btSphereShape = new btSphereShape(radius); // this seems wrong!
	m_pbtShape = m_btSphereShape;
	//m_pbtShape->setMargin(0.0f);
}

palBulletCapsuleGeometry::palBulletCapsuleGeometry()
  : m_btCylinderShape(0) {}

void palBulletCapsuleGeometry::Init(const palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	palCapsuleGeometry::Init(pos,radius,length,mass);
	unsigned int upAxis = palFactory::GetInstance()->GetActivePhysics()->GetUpAxis();
	// for z up
	if (upAxis == 2)
	{
		m_btCylinderShape = new btCylinderShapeZ(btVector3(radius, radius, length/2.0f)); //Half lengths
	}
	else if (upAxis == 0)
	{
		m_btCylinderShape = new btCylinderShapeX(btVector3(length/2.0f, radius, radius)); //Half lengths
	}
	else
	{
		m_btCylinderShape = new btCylinderShape(btVector3(radius,length/2.0f,radius)); //Half lengths
	}

	m_pbtShape = m_btCylinderShape;
	//m_pbtShape->setMargin(0.0f);
}


palBulletSphere::palBulletSphere() {
}

void palBulletSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	palMatrix4x4 mat;
	mat_identity(&mat);
	mat_set_translation(&mat,x,y,z);
	BuildBody(mat, mass);
}

palBulletCapsule::palBulletCapsule() {
}

void palBulletCapsule::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palCapsule::Init(x,y,z,radius,length,mass);
	palMatrix4x4 mat;
	mat_identity(&mat);
	mat_set_translation(&mat,x,y,z);
	BuildBody(mat, mass);
}

palBulletOrientatedTerrainPlane::palBulletOrientatedTerrainPlane()
  : m_pbtPlaneShape(0) {}

palBulletOrientatedTerrainPlane::~palBulletOrientatedTerrainPlane() {
        delete m_pbtPlaneShape;
}

void palBulletOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size) {
	palOrientatedTerrainPlane::Init(x,y,z,nx,ny,nz,min_size);

	btVector3 normal(nx,ny,nz);
	normal.normalize();
	m_pbtPlaneShape = new btStaticPlaneShape(normal,CalculateD());

	palMatrix4x4 mat;
	mat_identity(&mat);
	mat_set_translation(&mat,x,y,z);
	BuildBody(mat, 0, PALBODY_STATIC, m_pbtPlaneShape);
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


palBulletTerrainPlane::palBulletTerrainPlane()
  : m_pbtBoxShape(0) {}

void palBulletTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);

	unsigned int upAxis = palFactory::GetInstance()->GetActivePhysics()->GetUpAxis();
	palMatrix4x4 mat;
	mat_identity(&mat);

	if (upAxis == 2)
	{
		m_pbtBoxShape = new btBoxShape(btVector3(min_size*(Float)0.5, min_size*(Float)0.5, (Float)1.0));
	   mat_set_translation(&mat,x,y,z-1.0f);
	}
	else if (upAxis == 0)
	{
		m_pbtBoxShape = new btBoxShape(btVector3(1.0, min_size*(Float)0.5, min_size*(Float)0.5));
		mat_set_translation(&mat,x-1.0f,y,z);
	}
	else
	{
		m_pbtBoxShape = new btBoxShape(btVector3(min_size*(Float)0.5, (Float)1.0, min_size*(Float)0.5));
		mat_set_translation(&mat,x,y-1.0f,z);
	}
	BuildBody(mat, 0, PALBODY_STATIC, m_pbtBoxShape);
}

palBulletTerrainMesh::palBulletTerrainMesh()
: m_pbtTriMeshShape(0) {}

palBulletTerrainMesh::~palBulletTerrainMesh() {
	delete m_pbtTriMeshShape;
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
	if (sizeof(Float) == sizeof(float))
	{
		meshIndex.m_vertexType = PHY_FLOAT;
	}
	else
	{
		meshIndex.m_vertexType = PHY_DOUBLE;
	}

	trimesh->addIndexedMesh(meshIndex);

	return trimesh;
}

void palBulletTerrainMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
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

	palTerrainMesh::Init(x,y,z,&m_Vertices.front(),nVertices,&m_Indices.front(),nIndices);

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
	palMatrix4x4 mat;
	mat_identity(&mat);
	mat_set_translation(&mat,x,y,z);
	BuildBody(mat, 0, PALBODY_STATIC, m_pbtTriMeshShape);
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


palBulletSphericalLink::palBulletSphericalLink()
  : m_btp2p(0) {}

palBulletSphericalLink::~palBulletSphericalLink() {
	if (m_btp2p) {
		if (g_DynamicsWorld)
			g_DynamicsWorld->removeConstraint(m_btp2p);
		delete m_btp2p;
	}
}


void palBulletSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palSphericalLink::Init(parent,child,x,y,z);
	const palBulletBodyBase *parentBodyBase = dynamic_cast<palBulletBodyBase *> (parent);
	const palBulletBodyBase *childBodyBase = dynamic_cast<palBulletBodyBase *> (child);
	btRigidBody* parentBulletBody = parentBodyBase->m_pbtBody;
	btRigidBody* childBulletBody = childBodyBase->m_pbtBody;

	const btVector3 pivotLocation(x, y, z);

	/* Even though we'll only use the location of the pivot and not
	 * its orientation, we need to account for rotation of the parent
	 * and child bodies because we need the location of the pivot in
	 * their frames of reference, which might be rotated. (For
	 * example, if the parent is translated by (-5,0,0) and rotated 90
	 * degrees clockwise about z, the global origin isn't just
	 * translated for the parent to (5,0,0), it's rotated to be at
	 * (0,5,0) in the parent's coordinate system.) */
	const btTransform& worldToParent(parentBodyBase->GetWorldTransform().inverse());
	const btVector3 pivotInParent(worldToParent * pivotLocation);

	const btTransform& worldToChild(childBodyBase->GetWorldTransform().inverse());
	const btVector3 pivotInChild(worldToChild * pivotLocation);

	btTransform frameInParent(btTransform::getIdentity());
	frameInParent.setOrigin(pivotInParent);
	btTransform frameInChild(btTransform::getIdentity());
	frameInChild.setOrigin(pivotInChild);

	btGeneric6DofConstraint* p2p = new btGeneric6DofConstraint(*parentBulletBody, *childBulletBody,
			frameInParent, frameInChild, true);

	// 0 might work fine, but it might be more stable to provide a small degree of freedom here
	const float epsilon = 0.000001f;
	p2p->setLinearLowerLimit(btVector3(-epsilon, -epsilon, -epsilon));
	p2p->setLinearUpperLimit(btVector3(epsilon, epsilon, epsilon));

	m_btp2p = p2p;
	g_DynamicsWorld->addConstraint(m_btp2p,true);
}

void palBulletSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {
	btGeneric6DofConstraint* g = m_btp2p;//dynamic_cast<btGeneric6DofConstraint *>(m_btp2p);
	btVector3 limit(cone_limit_rad,cone_limit_rad,twist_limit_rad);
	g->setAngularLowerLimit(-limit);
	g->setAngularUpperLimit(limit);
	palSphericalLink::SetLimits(cone_limit_rad, twist_limit_rad);
}


/**
 * The Bullet class btHingeConstraint uses the btAdjustAngleToLimits function
 * (from btTypedConstraint.h), which has a bug
 * (http://code.google.com/p/bullet/issues/detail?id=377). We can't
 * replace btAdjustAngleToLimits, so we'll subclass btHingeConstraint,
 * instead.
 *
 * The following method and the palHingeConstraint class are based on
 * code from Bullet. These may be removed an btHingeConstraint used
 * again if/when this bug is fixed.  See the end of this file for the
 * Bullet license.
 *
 */

#include "bullet_palHingeConstraint.h"

btScalar adjustAngleToLimits(btScalar angleInRadians, btScalar angleLowerLimitInRadians, btScalar angleUpperLimitInRadians) {
    if(angleLowerLimitInRadians >= angleUpperLimitInRadians) {
		return angleInRadians;
    }
    else if(angleInRadians < angleLowerLimitInRadians) {
		btScalar diffLo = btFabs(btNormalizeAngle(angleLowerLimitInRadians - angleInRadians));
		btScalar diffHi = btFabs(btNormalizeAngle(angleUpperLimitInRadians - angleInRadians));
		return (diffLo < diffHi) ? angleInRadians : (angleInRadians + SIMD_2_PI);
    }
    else if (angleInRadians > angleUpperLimitInRadians) {
		btScalar diffHi = btFabs(btNormalizeAngle(angleInRadians - angleUpperLimitInRadians));
		btScalar diffLo = btFabs(btNormalizeAngle(angleInRadians - angleLowerLimitInRadians));
		return (diffLo < diffHi) ? (angleInRadians - SIMD_2_PI) : angleInRadians;
    }
    else {
		return angleInRadians;
    }
}

palBulletRevoluteLink::palBulletRevoluteLink()
	: palLink(PAL_LINK_REVOLUTE), m_btHinge(0) {}

palBulletRevoluteLink::~palBulletRevoluteLink() {
	if (m_btHinge) {
		if (g_DynamicsWorld)
			g_DynamicsWorld->removeConstraint(m_btHinge);
		delete m_btHinge;
	}
}

void palBulletRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);

	btTransform frameA, frameB;

	convertPalMatToBtTransform(frameA, m_frameA);
	convertPalMatToBtTransform(frameB, m_frameB);
	m_btHinge = new palHingeConstraint(*(body0->BulletGetRigidBody()),*(body1->BulletGetRigidBody()), frameA, frameB, false);
	g_DynamicsWorld->addConstraint(m_btHinge,true);

}

void palBulletRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	palRevoluteLink::SetLimits(lower_limit_rad, upper_limit_rad);
	m_btHinge->setLimit(lower_limit_rad,upper_limit_rad);
}

void palBulletRevoluteLink::GetPosition(palVector3& pos) const {
	//Get the pivot in the frame A and transform it to global coordinates
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (m_pParent);
	btTransform pivotInGlobal = (body0->BulletGetRigidBody())->getCenterOfMassTransform() * m_btHinge->getAFrame();

	pos.x = pivotInGlobal.getOrigin().x();
	pos.y = pivotInGlobal.getOrigin().y();
	pos.z = pivotInGlobal.getOrigin().z();
}

Float palBulletRevoluteLink::GetAngle() const {
	return m_btHinge->getHingeAngle();
}

palLink::linkFeedback* palBulletRevoluteLink::GetFeedback() const
	throw(palIllegalStateException) {
	if (!m_btHinge) {
		throw palIllegalStateException("Init must be called first");
	}
	if (!feedback) {
		const_cast<palBulletRevoluteLink*>(this)->feedback = new bulletRevoluteLinkFeedback(m_btHinge);
	}
	return feedback;
}

palBulletRevoluteLink::bulletRevoluteLinkFeedback::bulletRevoluteLinkFeedback(palHingeConstraint *hinge)
	: m_btHinge(hinge)
{
}

bool palBulletRevoluteLink::bulletRevoluteLinkFeedback::IsEnabled() const {
	return m_btHinge->needsFeedback();
}

Float palBulletRevoluteLink::bulletRevoluteLinkFeedback::GetValue() const {
	return m_btHinge->getAppliedImpulse();
}

bool palBulletRevoluteLink::bulletRevoluteLinkFeedback::SetEnabled(bool enable) {
	m_btHinge->enableFeedback(enable);
	return IsEnabled();
}


#ifdef TODO
virtual std::string palBulletRevoluteLink::toString() const {
    std::string result("palAngularMotor[link=");
    result.append(m_link->toString());
    result.append("]");
    return result;
}
#endif

////////////////////////////////////////////////////////


palBulletRevoluteSpringLink::palBulletRevoluteSpringLink()
  : m_bt6Dof(0)
{
}

palBulletRevoluteSpringLink::~palBulletRevoluteSpringLink() {
	if (m_bt6Dof) {
		if (g_DynamicsWorld)
			g_DynamicsWorld->removeConstraint(m_bt6Dof);
		delete m_bt6Dof;
		m_bt6Dof = NULL;
	}
}

void palBulletRevoluteSpringLink::Init(palBodyBase *parent, palBodyBase *child,
									   Float x, Float y, Float z,
									   Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteSpringLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);

	btTransform frameA, frameB;

	convertPalMatToBtTransform(frameA, m_frameA);
	convertPalMatToBtTransform(frameB, m_frameB);

	m_bt6Dof = new SubbtGeneric6DofSpringConstraint(*(body0->BulletGetRigidBody()),
													*(body1->BulletGetRigidBody()),
													frameA,
													frameB,
													false);

	// Set the lower limit higher that the upper limit by default.  This means free movement.
	m_bt6Dof->setAngularLowerLimit(btVector3(0.0f, 0.0f, SIMD_PI + 0.1f));
	m_bt6Dof->setAngularUpperLimit(btVector3(0.0f, 0.0f, SIMD_PI));

	g_DynamicsWorld->addConstraint(m_bt6Dof,true);
}

void palBulletRevoluteSpringLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	palRevoluteSpringLink::SetLimits(lower_limit_rad, upper_limit_rad);
	m_bt6Dof->setLimit(5, lower_limit_rad, upper_limit_rad);
}

void palBulletRevoluteSpringLink::SetSpring(const palSpringDesc& springDesc) {
	bool enable = springDesc.m_fSpringCoef > SIMD_EPSILON || springDesc.m_fDamper > SIMD_EPSILON;
	m_bt6Dof->enableSpring(5, enable);
	m_bt6Dof->setStiffness(5, springDesc.m_fSpringCoef);
	m_bt6Dof->setDamping(5, springDesc.m_fDamper);
	m_bt6Dof->setEquilibriumPoint(5, springDesc.m_fTarget);
	//m_bt6Dof->getRotationalLimitMotor(2)->m_bounce = btScalar(0.3);
	//m_bt6Dof->setParam(BT_CONSTRAINT_STOP_CFM, btScalar(1.0e-5f), 5);
}

void palBulletRevoluteSpringLink::GetSpring(palSpringDesc& springDescOut) const {
	m_bt6Dof->getSpringDesc(5, springDescOut);
}


////////////////////////////////////////////////////////

palBulletPrismaticLink::palBulletPrismaticLink()
	: palLink(PAL_LINK_PRISMATIC), m_btSlider(0) {}

palBulletPrismaticLink::~palBulletPrismaticLink()
{
	delete m_btSlider;
}

void palBulletPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palPrismaticLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);

	//New method calculating the frames
	btVector3 constraintDefaultAxis(1, 0, 0); //X is the Slider default axis
	btVector3 constraintSpecifiedAxis(axis_x,axis_y,axis_z);  //Direction of axis of rotation

	//Rotation to align x with axis
	btScalar angle = acos(constraintDefaultAxis.dot(constraintSpecifiedAxis));
	btVector3 direction = constraintDefaultAxis.cross(constraintSpecifiedAxis);

	btVector3 pivot(x,y,z); // constraint position in world space
	btQuaternion rot(direction, angle); // constraint rotation in world space

	//printf("constrainDefaultAxis: %f %f %f \n",constraintDefaultAxis.x(),constraintDefaultAxis.y(),constraintDefaultAxis.z() );
	//printf("constrainSpecifiedAxis: %f %f %f \n",constraintSpecifiedAxis.x(),constraintSpecifiedAxis.y(),constraintSpecifiedAxis.z() );
	//printf("angle: %f \n",angle);
	//printf("direction: %f %f %f \n",direction.x(),direction.y(),direction.z() );

	btTransform ctWorldTransform;
	ctWorldTransform.setIdentity();
	ctWorldTransform.setOrigin(pivot);
	if (direction.length()>0.0)
		ctWorldTransform.setRotation(rot);

	btTransform t_A = (body0->m_pbtBody)->getCenterOfMassTransform().inverse() * ctWorldTransform;
	btTransform t_B = (body1->m_pbtBody)->getCenterOfMassTransform().inverse() * ctWorldTransform;

	m_btSlider = new btSliderConstraint(*(body0->m_pbtBody),*(body1->m_pbtBody), t_A, t_B, true);

	//Constraint the angular movement
	m_btSlider->setLowerAngLimit(0.0f);
	m_btSlider->setUpperAngLimit(0.0f);

	g_DynamicsWorld->addConstraint(m_btSlider);
}

void palBulletPrismaticLink::SetLimits(Float lower_limit, Float upper_limit) {
	m_btSlider->setLowerLinLimit(lower_limit);
	m_btSlider->setUpperLinLimit(upper_limit);
}
//////////////////////////////


palBulletConvexGeometry::palBulletConvexGeometry()
	: m_pbtConvexShape(0) {}

void palBulletConvexGeometry::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	palConvexGeometry::Init(pos,pVertices,nVertices,mass);
	InternalInit(pVertices, nVertices);
}

void palBulletConvexGeometry::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass){
	palConvexGeometry::Init(pos,pVertices,nVertices,pIndices,nIndices,mass);
	InternalInit(pVertices, nVertices);
}

void palBulletConvexGeometry::InternalInit(const Float *pVertices, int nVertices)
{
//	btTriangleMesh* trimesh = new btTriangleMesh();
//
//	for (int i = 0; i < nVertices; i++)
//	{
//		btVector3 vertex0(pVertices[i * 3], pVertices[i*3+1], pVertices[i*3+2]);
//		btVector3 vertex1(pVertices[(i + 1) * 3], pVertices[(i + 1) * 3 + 1], pVertices[(i + 1) * 3 + 2]);
//		btVector3 vertex2(pVertices[(i + 2) * 3], pVertices[(i + 2) * 3 + 1], pVertices[(i + 2) * 3 + 2]);
//
//		trimesh->addTriangle(vertex0,vertex1,vertex2);
//	}
//
//
//	btConvexShape* tmpConvexShape = new btConvexTriangleMeshShape(trimesh);
//
//	//create a hull approximation
//	btShapeHull* hull = new btShapeHull(tmpConvexShape);
//	btScalar margin = tmpConvexShape->getMargin();
//	hull->buildHull(margin);
//	tmpConvexShape->setUserPointer(hull);
//
//	btConvexHullShape* convexShape = new btConvexHullShape();
//	for (int i=0;i<hull->numVertices();i++)
//	{
//		convexShape->addPoint(hull->getVertexPointer()[i]);
//	}
//
//	delete tmpConvexShape;
//	delete hull;
#ifndef BT_USE_DOUBLE_PRECISION
//	if (sizeof(btScalar) == sizeof(Float))
//	{
	m_pbtConvexShape = new btConvexHullShape(pVertices,nVertices,sizeof(btScalar)*3);
#else
//	}
//	else
//	{
	m_pbtConvexShape = new btConvexHullShape();
	for (unsigned i = 0; i < nVertices; ++i)
		{
			m_pbtConvexShape->addPoint(btVector3(pVertices[3*i + 0], pVertices[3*i + 1], pVertices[3*i + 2]));
		}
#endif
//	}
    // default margin is 0.04
//	m_pbtConvexShape = convexShape;
	m_pbtShape = m_pbtConvexShape;
	//m_pbtShape->setMargin(0.0f);
}

palBulletConcaveGeometry::palBulletConcaveGeometry()
	: m_pbtTriMeshShape(0) {}

palBulletConcaveGeometry::~palBulletConcaveGeometry() {
    if (m_pbtTriMeshShape) {
        /* You might think Bullet would clean this up when
           m_pbtTriMeshShape gets deleted by ~palBulletGeometry, but
           Bullet doesn't clean up the mesh interface. */
        delete m_pbtTriMeshShape->getMeshInterface();
    }
}

void palBulletConcaveGeometry::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
	palConcaveGeometry::Init(pos,pVertices,nVertices,pIndices,nIndices,mass);

	btTriangleIndexVertexArray* trimesh = CreateTrimesh(m_pUntransformedVertices, nVertices, m_pIndices, nIndices);
	m_pbtTriMeshShape = new btBvhTriangleMeshShape(trimesh,true);

	btTriangleInfoMap* triangleInfoMap = new btTriangleInfoMap();
	btGenerateInternalEdgeInfo(m_pbtTriMeshShape, triangleInfoMap);

	m_pbtShape = m_pbtTriMeshShape;
	//m_pbtShape->setMargin(0.0f);
}

palBulletConvex::palBulletConvex() {
}

void palBulletConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	palMatrix4x4 mat;
	mat_identity(&mat);
	mat_set_translation(&mat,x,y,z);
	BuildBody(mat, mass);
}

void palBulletConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,pIndices,nIndices,mass);
	palMatrix4x4 mat;
	mat_identity(&mat);
	mat_set_translation(&mat,x,y,z);
	BuildBody(mat, mass);
}

palBulletStaticConvex::palBulletStaticConvex() {
}

void palBulletStaticConvex::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices) {
	palStaticConvex::Init(pos,pVertices,nVertices);
	BuildBody(m_mLoc, 0, PALBODY_STATIC);
	palBulletBodyBase::SetPosition(pos);
}

void palBulletStaticConvex::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices){
	palStaticConvex::Init(pos,pVertices,nVertices, pIndices, nIndices);
	BuildBody(m_mLoc, 0, PALBODY_STATIC);
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
Float palBulletPSDSensor::GetDistance() const {
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

	g_DynamicsWorld->rayTest(from, to, rayCallback);
	if (rayCallback.hasHit())
		{
			btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
			if (body)
				{
					return m_fRange*rayCallback.m_closestHitFraction;
				}
		}
	return m_fRange;
}


palBulletGenericLink::palBulletGenericLink()
	: genericConstraint(0) {}

palBulletGenericLink::~palBulletGenericLink() {
	if (genericConstraint) {
		if (g_DynamicsWorld)
			g_DynamicsWorld->removeConstraint(genericConstraint);
		delete genericConstraint;
	}
}

void palBulletGenericLink::Init(palBodyBase *parent, palBodyBase *child,
								const palMatrix4x4& parentFrame,
								const palMatrix4x4& childFrame,
								const palVector3& linearLowerLimits,
								const palVector3& linearUpperLimits,
								const palVector3& angularLowerLimits,
								const palVector3& angularUpperLimits)
{
	palGenericLink::Init(parent,child,parentFrame,childFrame,linearLowerLimits,linearUpperLimits,angularLowerLimits,angularUpperLimits);

	palBulletBodyBase *body0 = dynamic_cast<palBulletBodyBase *> (parent);
	palBulletBodyBase *body1 = dynamic_cast<palBulletBodyBase *> (child);

	btTransform frameInA, frameInB;

	convertPalMatToBtTransform(frameInA, parentFrame);
	convertPalMatToBtTransform(frameInB, childFrame);

	genericConstraint = new SubbtGeneric6DofSpringConstraint(
		*(body0->m_pbtBody),*(body1->m_pbtBody),
		frameInA,frameInB,true);

	genericConstraint->setLinearLowerLimit(btVector3(linearLowerLimits.x,linearLowerLimits.y,linearLowerLimits.z));
	genericConstraint->setLinearUpperLimit(btVector3(linearUpperLimits.x,linearUpperLimits.y,linearUpperLimits.z));
	genericConstraint->setAngularLowerLimit(btVector3(angularLowerLimits.x,angularLowerLimits.y,angularLowerLimits.z));
	genericConstraint->setAngularUpperLimit(btVector3(angularUpperLimits.x,angularUpperLimits.y,angularUpperLimits.z));

	g_DynamicsWorld->addConstraint(genericConstraint);
}

palBulletRigidLink::palBulletRigidLink()
	:  palLink(PAL_LINK_RIGID), palBulletRevoluteLink(), palRigidLink()
{
}

palBulletRigidLink::~palBulletRigidLink()
{
}

void palBulletRigidLink::Init(palBodyBase *parent, palBodyBase *child)
{
	palRigidLink::Init(parent, child);
	const float TOLERANCE = 0.01f;
	
	palBulletRevoluteLink::Init(parent, child, m_fPosX, m_fPosY, m_fPosZ, 1, 0, 0);
	/* Bullet can get into weird states with angles exactly at its boundaries (PI and -PI)
	 * if the limits are exactly equal, so perturb them slightly. */
	btScalar angle = m_btHinge->getHingeAngle();
	// tried SIMD_EPSILON, but that's too small
	btScalar lowerLimit = angle - TOLERANCE;
	// clamp it to make sure it's in the valid range for Bullet
	if (lowerLimit < -SIMD_PI) {
		lowerLimit = -SIMD_PI;
	}
	btScalar upperLimit = angle + TOLERANCE;
	// clamp it to make sure it's in the valid range for Bullet
	if (upperLimit > SIMD_PI) {
		upperLimit = SIMD_PI;
	}
	SetLimits(lowerLimit, upperLimit);
}


std::ostream& operator<<(std::ostream &os, const palBulletRigidLink& link)
{
    const palLink& superLink = *(static_cast<const palLink*>(&link));
    os << superLink;
    const palBulletRevoluteLink* revoluteLink = dynamic_cast<const palBulletRevoluteLink*>(&superLink);
    if (revoluteLink) {
		os << "[angle=" << revoluteLink->m_btHinge->getHingeAngle() << "]";
    }
    return os;
}

palBulletAngularMotor::palBulletAngularMotor()
	: m_bhc(0) {}

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
	m_bhc->getRigidBodyA().activate();
	m_bhc->getRigidBodyB().activate();
}

void palBulletAngularMotor::Apply() {

}

//////////////////////////////////////////////////////////

palBulletGenericLinkSpring::palBulletGenericLinkSpring()
  : m_pBulletLink(NULL)
{

}

void palBulletGenericLinkSpring::Init(palGenericLink* link) {
	BaseClass::Init(link);
	m_pBulletLink = dynamic_cast<palBulletGenericLink*>(link);
}

void palBulletGenericLinkSpring::SetLinearSpring(unsigned axis, const palSpringDesc& spring) {
	BaseClass::SetLinearSpring(axis, spring);
	if (axis > 2) return;
	m_pBulletLink->BulletGetGenericConstraint()->setStiffness(axis, spring.m_fSpringCoef);
	m_pBulletLink->BulletGetGenericConstraint()->setDamping(axis, spring.m_fDamper);
	m_pBulletLink->BulletGetGenericConstraint()->setEquilibriumPoint(axis, spring.m_fTarget);
	m_pBulletLink->BulletGetGenericConstraint()->enableSpring(axis, spring.m_fSpringCoef > FLT_EPSILON);
}

void palBulletGenericLinkSpring::GetLinearSpring(unsigned axis, palSpringDesc& out) const {
	BaseClass::GetLinearSpring(axis, out);
}

void palBulletGenericLinkSpring::SetAngularSpring(unsigned axis, const palSpringDesc& spring) {
	BaseClass::SetAngularSpring(axis, spring);
	if (axis > 2) return;
	axis += 3;
	m_pBulletLink->BulletGetGenericConstraint()->setStiffness(axis, spring.m_fSpringCoef);
	m_pBulletLink->BulletGetGenericConstraint()->setDamping(axis, spring.m_fDamper);
	m_pBulletLink->BulletGetGenericConstraint()->setEquilibriumPoint(axis, spring.m_fTarget);
	m_pBulletLink->BulletGetGenericConstraint()->enableSpring(axis, spring.m_fSpringCoef > FLT_EPSILON);
}

void palBulletGenericLinkSpring::GetAngularSpring(unsigned axis, palSpringDesc& out) const {
	BaseClass::GetAngularSpring(axis, out);
}

void palBulletGenericLinkSpring::Apply() {

}

/*
  if (m_pbtBody) {
  m_pbtBody->getWorldTransform().getOpenGLMatrix(m_mLoc._mat);
  }
  return m_mLoc;
*/

palBulletSoftBody::palBulletSoftBody()
	: m_pbtSBody(0) {}

void palBulletSoftBody::BulletInit(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices) {

	palBulletPhysics *pbf=dynamic_cast<palBulletPhysics *>(PF->GetActivePhysics());

// TODO this will crash with double precision, so the particle list needs to be copied to a list of doubles.
#ifndef BT_USE_DOUBLE_PRECISION
	m_pbtSBody = btSoftBodyHelpers::CreateFromTriMesh(pbf->m_softBodyWorldInfo	,	pParticles,pIndices, nIndices/3);
#endif
	m_pbtSBody->generateBendingConstraints(2);
	m_pbtSBody->m_cfg.piterations=2;
	m_pbtSBody->m_cfg.collisions|=btSoftBody::fCollision::VF_SS;
	m_pbtSBody->randomizeConstraints();

	m_pbtSBody->setTotalMass(50,true);

	btSoftRigidDynamicsWorld* softWorld =	(btSoftRigidDynamicsWorld*)pbf->m_dynamicsWorld;
	softWorld->addSoftBody(m_pbtSBody);
}

int palBulletSoftBody::GetNumParticles() const {
	return (int)m_pbtSBody->m_nodes.size();
}
palVector3* palBulletSoftBody::GetParticlePositions() {
	pos.resize(GetNumParticles());
	for (int i=0;i<GetNumParticles();i++) {
		pos[i].x = Float(m_pbtSBody->m_nodes[i].m_x.x());
		pos[i].y = Float(m_pbtSBody->m_nodes[i].m_x.y());
		pos[i].z = Float(m_pbtSBody->m_nodes[i].m_x.z());
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

std::ostream& operator<<(std::ostream& out, const btVector3& v) {
	out << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
	return out;
}

#ifdef STATIC_CALLHACK
void pal_bullet_call_me_hack() {
	printf("%s I have been called!!\n", __FILE__);
};
#endif

// Bullet license below is for function adjustAngleToLimits and class
// palHingeConstraint (which are based on on Bullet code).

/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2010 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
