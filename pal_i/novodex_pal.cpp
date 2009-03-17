#if defined(_MSC_VER)
#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#endif
#include "novodex_pal.h"
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Novodex implementation.
		This enables the use of NovodeX via PAL.

		Implementation
	Author:
		Adrian Boeing
	Revision History:
		Version 0.0.3 : 17/01/08 - Bugfix "-" sign for plane dot product
		Version 0.0.2 : 11/11/06 - updated for AGEIA
		Version 0.0.1 : 12/08/04 - Physics
	TODO:
		-get to 1.0 (ie: same as pal.h)
*/

FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
//FACTORY_CLASS_IMPLEMENTATION(palNovodexCollisionDetection);
FACTORY_CLASS_IMPLEMENTATION(palNovodexPhysics);

FACTORY_CLASS_IMPLEMENTATION(palNovodexMaterialUnique);

FACTORY_CLASS_IMPLEMENTATION(palNovodexOrientatedTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palNovodexTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palNovodexTerrainMesh);
FACTORY_CLASS_IMPLEMENTATION(palNovodexTerrainHeightmap);


FACTORY_CLASS_IMPLEMENTATION(palNovodexBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palNovodexSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palNovodexCapsuleGeometry);
FACTORY_CLASS_IMPLEMENTATION(palNovodexConvexGeometry);

FACTORY_CLASS_IMPLEMENTATION(palNovodexConvex);
FACTORY_CLASS_IMPLEMENTATION(palNovodexBox);
FACTORY_CLASS_IMPLEMENTATION(palNovodexSphere);
FACTORY_CLASS_IMPLEMENTATION(palNovodexCapsule);
FACTORY_CLASS_IMPLEMENTATION(palNovodexCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palNovodexStaticConvex);
FACTORY_CLASS_IMPLEMENTATION(palNovodexStaticBox);
FACTORY_CLASS_IMPLEMENTATION(palNovodexStaticSphere);
FACTORY_CLASS_IMPLEMENTATION(palNovodexStaticCapsule);
FACTORY_CLASS_IMPLEMENTATION(palNovodexStaticCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palNovodexGenericBody);

FACTORY_CLASS_IMPLEMENTATION(palNovodexRevoluteLink);
FACTORY_CLASS_IMPLEMENTATION(palNovodexSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palNovodexPrismaticLink);
FACTORY_CLASS_IMPLEMENTATION(palNovodexGenericLink);


FACTORY_CLASS_IMPLEMENTATION(palNovodexPSDSensor);
FACTORY_CLASS_IMPLEMENTATION(palNovodexContactSensor);

FACTORY_CLASS_IMPLEMENTATION(palNovodexAngularMotor);

#ifdef NOVODEX_ENABLE_FLUID
FACTORY_CLASS_IMPLEMENTATION(palNovodexFluid);
#endif

FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

#include "Stream.h"
#include <NxCooking.h>

static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;

#include <deque>
//std::deque<NxVec3> g_contacts;
PAL_VECTOR<palContactPoint> g_contacts;
#if 0
PAL_MAP<NxActor*, palBodyBase* > g_Bodies;

void palNovodexPhysics::NotifyBodyAdded(palBodyBase* pBody) {
	palNovodexBodyBase *pnb = dynamic_cast<palNovodexBodyBase *>(pBody);
	if (!pnb) return;
	g_Bodies.insert(std::make_pair(pnb->m_Actor,pBody));
}
palBodyBase* LookupActor(NxActor *a) {
	if (!a) return 0;
	PAL_MAP<NxActor** , palBodyBase* >::iterator itr;
	itr = g_Bodies.find(a);
	if (itr!=g_Bodies.end()) {
		return itr->second;
	}
	return 0;
}
#else
palBodyBase* LookupActor(NxActor *a) {
	return (palBodyBase*) a->userData;
}
#endif

class ContactReport : public NxUserContactReport
{
public:
    virtual void onContactNotify(NxContactPair& pair, NxU32 events)
	{


/*		if (pair.actors[0])
		{
			ActorUserData* ud = (ActorUserData*)pair.actors[0]->userData;
			if (ud)  ud->contactEvents = events;
		}

		if (pair.actors[1])
		{
			ActorUserData* ud = (ActorUserData*)pair.actors[1]->userData;
			if (ud)  ud->contactEvents = events;
		}*/

//		if(events & NX_NOTIFY_ON_START_TOUCH)	printf("Start touch\n");
//		if(events & NX_NOTIFY_ON_END_TOUCH)		printf("End touch\n");

		// Iterate through contact points
		NxContactStreamIterator i(pair.stream);
		//user can call getNumPairs() here
		while(i.goNextPair())
		{
			//user can also call getShape() and getNumPatches() here
			while(i.goNextPatch())
			{
				//user can also call getPatchNormal() and getNumPoints() here
				const NxVec3& nn = i.getPatchNormal();
				while(i.goNextPoint())
				{
					palContactPoint cp;

					//user can also call getShape() and getNumPatches() here
					const NxVec3& np = i.getPoint();
					//g_contacts.push_back(contactPoint);

					NxShape *s0 = i.getShape(0);
					NxShape *s1 = i.getShape(1);

					cp.m_pBody1 = LookupActor(&(s0->getActor()));
					cp.m_pBody2 = LookupActor(&(s1->getActor()));
					cp.m_vContactPosition.x = np.x;
					cp.m_vContactPosition.y = np.y;
					cp.m_vContactPosition.z = np.z;

					cp.m_vContactNormal.x = nn.x;
					cp.m_vContactNormal.y = nn.y;
					cp.m_vContactNormal.z = nn.z;

					cp.m_fDistance = i.getSeparation();
					g_contacts.push_back(cp);
				}
			}
		}
	}

} gContactReport;

NxScene* palNovodexPhysics::NxGetScene() {
	return gScene;
}

NxPhysicsSDK* palNovodexPhysics::NxGetPhysicsSDK() {
	return gPhysicsSDK;
}

palNovodexPhysics::palNovodexPhysics() {
	m_bListen = true;
	set_use_hardware = false;
	set_substeps = 1;
	set_pe = 1;
}

const char* palNovodexPhysics::GetPALVersion() {
	static char verbuf[512];
	sprintf(verbuf,"PAL SDK V%d.%d.%d\nPAL Novodex V:%d.%d.%d\nFile: %s\nCompiled: %s %s\nModified:%s",
		PAL_SDK_VERSION_MAJOR,PAL_SDK_VERSION_MINOR,PAL_SDK_VERSION_BUGFIX,
		NOVODEX_PAL_SDK_VERSION_MAJOR,NOVODEX_PAL_SDK_VERSION_MINOR,NOVODEX_PAL_SDK_VERSION_BUGFIX,
		__FILE__,__TIME__,__DATE__,__TIMESTAMP__);
	return verbuf;
}

const char* palNovodexPhysics::GetVersion() {
	static char verbuf[256];
	sprintf(verbuf,"PhysX V%d.%d.%d",NX_SDK_VERSION_MAJOR,NX_SDK_VERSION_MINOR,NX_SDK_VERSION_BUGFIX);
	return verbuf;
}

void palNovodexPhysics::Init(Float gravity_x, Float gravity_y, Float gravity_z) {
	palPhysics::Init(gravity_x,gravity_y,gravity_z);
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if(!gPhysicsSDK) {
		SET_ERROR("Could not create NovodeX physics SDK");
		return;
	}
#ifndef NDEBUG
	gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect ("localhost", 5425);
#endif


	//removed for AGEIA v. 2.7.0
//	gPhysicsSDK->setParameter(NX_MIN_SEPARATION_FOR_PENALTY, -0.05f);

	NxVec3	gravity(gravity_x,gravity_y,gravity_z);

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.0001f);
	//continuous CD
//	gPhysicsSDK->setParameter(NX_CONTINUOUS_CD, true);
//	gPhysicsSDK->setParameter(NX_CCD_EPSILON, 0.0001f);

	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= gravity;
//	sceneDesc.broadPhase			= NX_BROADPHASE_COHERENT;
	//sceneDesc.broadPhase			= NX_BROADPHASE_QUADRATIC;
//	sceneDesc.collisionDetection	= true;
	sceneDesc.userContactReport     = &gContactReport;
	sceneDesc.flags |= NX_SF_SIMULATE_SEPARATE_THREAD|NX_SF_ENABLE_MULTITHREAD;
	sceneDesc.threadMask=0xfffffffe;
	sceneDesc.internalThreadCount   = set_pe;
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if (!gScene) {
		SET_ERROR("Could not create scene");
		return;
	}
	gPhysicsSDK->setParameter(NX_CONTINUOUS_CD,1);
}

void palNovodexPhysics::Cleanup() {
	if (gPhysicsSDK)
		gPhysicsSDK->release();
}

///////////////////////////////////////////////////////
void palNovodexPhysics::SetSolverAccuracy(Float fAccuracy) {
}

void palNovodexPhysics::StartIterate(Float timestep) {
	g_contacts.clear(); //clear all contacts before the update TODO: CHECK THIS IS SAFE FOR MULTITHREADED!
	gScene->simulate(timestep);
	gScene->flushStream();
}
bool palNovodexPhysics::QueryIterationComplete() {
	return gScene->checkResults(NX_RIGID_BODY_FINISHED);
}

void palNovodexPhysics::WaitForIteration() {
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

void palNovodexPhysics::SetPE(int n) {
	set_pe = n;
}
void palNovodexPhysics::SetSubsteps(int n) {
	set_substeps = n;
}
void palNovodexPhysics::SetHardware(bool status) {
	set_use_hardware = status;
}

bool palNovodexPhysics::GetHardware(void) {
	//TODO: check if running on hardware
	return gPhysicsSDK->getHWVersion() ? true:false;
}
//static int g_materialcount = 1;
///////////////////////////////////////////////////////
void palNovodexPhysics::Iterate(Float timestep) {
	if (!gScene) {
		SET_ERROR("Physics not initialized");
		return;
	}
#if 0
	int i;
	for (i=0;i<g_forces.size();i++) {
	g_forces[i].Apply();
	}
#endif
	//gScene->startRun(timestep);
	//gScene->setTiming(timestep / 4.0f, 4, NX_TIMESTEP_FIXED);
	gScene->setTiming(timestep, set_substeps, NX_TIMESTEP_FIXED);
	StartIterate(timestep);

	WaitForIteration();
}
/*
void palNovodexPhysics::NotifyGeometryAdded(palGeometry* pGeom) {
	palNovodexGeometry *png = dynamic_cast<palNovodexGeometry *>(pGeom);
	png->m_pShape
	m_Shapes.insert(std::make_pair(png ,pGeom));
}
	*/

///////////////////////////////////////////////////////

void palNovodexPhysics::SetCollisionAccuracy(Float fAccuracy) {
	;//todo
}



void palNovodexPhysics::RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit) {
	hit.Clear();
	NxVec3 orig(x,y,z);
	NxVec3 dir(dx,dy,dz);
	NxRay ray(orig, dir);
	NxRaycastHit nhit;
	//NxReal dist;
	NxShape* closestShape = gScene->raycastClosestShape(ray, NX_ALL_SHAPES, nhit, -1, range);
	if (closestShape) {
		hit.m_bHit=true;
		hit.m_fDistance = nhit.distance;

		const NxVec3& wi = nhit.worldImpact;
		hit.SetHitPosition(wi.x,wi.y,wi.z);

		const NxVec3& wn = nhit.worldNormal;
		hit.SetHitNormal(wn.x,wn.y,wn.z);

		NxShape *ns = nhit.shape;
		if (ns) {
			NxActor& a = ns->getActor();
			hit.m_pBody = LookupActor(&a);
		}
		hit.m_pGeom = 0;
	}
}

void palNovodexPhysics::NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled) {
	palNovodexBodyBase *b0 = dynamic_cast<palNovodexBodyBase *> (a);
	palNovodexBodyBase *b1 = dynamic_cast<palNovodexBodyBase *> (b);
	if (!b0) return;
	if (!b1) return;
	if (enabled)
		gScene->setActorPairFlags(*(b0->m_Actor),*(b1->m_Actor),NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	else
		gScene->setActorPairFlags(*(b0->m_Actor),*(b1->m_Actor),0);
}
void palNovodexPhysics::NotifyCollision(palBodyBase *a, bool enabled) {
	palNovodexBodyBase *b0 = dynamic_cast<palNovodexBodyBase *> (a);
	if (!b0) return;

	NxActor **ppact = gScene->getActors();
	for (unsigned int i=0;i<gScene->getNbActors();i++) {
		if (ppact[i]!=b0->m_Actor)
			if (enabled)
				gScene->setActorPairFlags(*(b0->m_Actor),*(ppact[i]),NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
			else
				gScene->setActorPairFlags(*(b0->m_Actor),*(ppact[i]),0);
	}
}

void palNovodexPhysics::GetContacts(palBodyBase *pBody, palContact& contact) {
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
void palNovodexPhysics::GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact) {
	contact.m_ContactPoints.clear();
	for (unsigned int i=0;i<g_contacts.size();i++) {
		if ((g_contacts[i].m_pBody1 == a) && (g_contacts[i].m_pBody2 == b)) {
			contact.m_ContactPoints.push_back(g_contacts[i]);
		}
	}
}

void palNovodexPhysics::SetGroupCollision(palGroup a, palGroup b, bool enabled) {
	gScene->setGroupCollisionFlag(a,b,enabled);
}


///////////////////////////////////////////////////////
palNovodexMaterialUnique::palNovodexMaterialUnique() {

}

void palNovodexMaterialUnique::Init(PAL_STRING name,Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterialUnique::Init(name,static_friction,kinetic_friction,restitution);
	//m_Index=g_materialcount;
	if (gPhysicsSDK) {
	//default material
	m_MaterialDesc.restitution		= restitution;
	m_MaterialDesc.staticFriction	= static_friction;
	m_MaterialDesc.dynamicFriction	= kinetic_friction;

	m_pMaterial =  gScene->createMaterial(m_MaterialDesc);
	m_Index = m_pMaterial->getMaterialIndex();

	//gPhysicsSDK->setMaterialAtIndex(m_Index, &m_Material);
	//m_Index= gPhysicsSDK->addMaterial(m_Material);
	//gScene->
	}
	//g_materialcount++;
}
///////////////////////////////////////////////////////

palNovodexTerrain::palNovodexTerrain() {
	m_Actor=NULL;
}

palMatrix4x4& palNovodexTerrain::GetLocationMatrix() {
	if (m_Actor)
		m_Actor->getGlobalPose().getColumnMajor44(m_mLoc._mat);
//		m_Actor->getGlobalPoseReference().getColumnMajor44(m_mLoc._mat);
	return m_mLoc;
}

void palNovodexTerrain::SetMaterial(palMaterial *material) {
	if (!m_Actor) return;
	palNovodexMaterialUnique *pm = dynamic_cast<palNovodexMaterialUnique *>(material);
	if (pm) {
		 NxShape *const *ps = m_Actor->getShapes();
		 for (unsigned int i=0;i<m_Actor->getNbShapes();i++)
			ps[i]->setMaterial(pm->m_Index);
	}
}

palNovodexOrientatedTerrainPlane::palNovodexOrientatedTerrainPlane() {
}


void palNovodexOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size) {
	palOrientatedTerrainPlane::Init(x,y,z,nx,ny,nz,min_size);
	NxPlaneShapeDesc PlaneDesc;
	NxActorDesc ActorDesc;
	PlaneDesc.normal.x = nx;
	PlaneDesc.normal.y = ny;
	PlaneDesc.normal.z = nz;
	PlaneDesc.d = CalculateD();
	ActorDesc.shapes.pushBack(&PlaneDesc);
	m_Actor=gScene->createActor(ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
}

palNovodexTerrainPlane::palNovodexTerrainPlane() {

}

void palNovodexTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
	NxPlaneShapeDesc PlaneDesc;
	NxActorDesc ActorDesc;
	PlaneDesc.normal.x = 0;
	PlaneDesc.normal.y = 1;
	PlaneDesc.normal.z = 0;

	palVector3 pos;
	palVector3 norm;
	vec_set(&norm,0,1,0);
	vec_set(&pos,0,y,0);
	Float d = vec_dot(&norm,&pos);

	PlaneDesc.d = d;

	ActorDesc.shapes.pushBack(&PlaneDesc);
	m_Actor=gScene->createActor(ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
}

void palNovodexTerrainPlane::InitND(Float nx,Float ny, Float nz, Float d) {
	NxPlaneShapeDesc PlaneDesc;
	NxActorDesc ActorDesc;
	PlaneDesc.normal.x = nx;
	PlaneDesc.normal.y = ny;
	PlaneDesc.normal.z = nz;
	PlaneDesc.d = d;
	ActorDesc.shapes.pushBack(&PlaneDesc);
	m_Actor=gScene->createActor(ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
}
/*
palMatrix4x4& palNovodexTerrainPlane::GetLocationMatrix() {
	mat_identity(&m_mLoc);
	return m_mLoc;
}

void palNovodexTerrainPlane::SetMaterial(palMaterial *material) {
	printf("TODO!\n");
}*/
///////////////////////////////////////////////////////
palNovodexGeometry::palNovodexGeometry() {
	m_pShape = NULL;
	m_pCreatedShape = NULL;
}
/*
palMatrix4x4& palNovodexGeometry::GetLocationMatrix() {
	printf("TODO!\n");
	mat_identity(&m_mLoc);
	return m_mLoc;
}*/

void palNovodexGeometry::ReCalculateOffset() {
	palGeometry::ReCalculateOffset();
	NxMat34 m;
	m.setColumnMajor44(m_mOffset._mat);
	if (m_pShape)
		m_pShape->localPose = m;
}

///////////////////////////////////////////////////////
palNovodexBoxGeometry::palNovodexBoxGeometry() {
	m_pBoxShape=NULL;
}

palNovodexBoxGeometry::~palNovodexBoxGeometry() {
	delete m_pBoxShape;
}

void palNovodexBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	m_pBoxShape = new NxBoxShapeDesc;
	m_pBoxShape->dimensions = NxVec3(width*0.5f,height*0.5f,depth*0.5f);
	NxMat34 m;
	m.setColumnMajor44(m_mOffset._mat);
	m_pBoxShape->localPose = m;
	m_pShape = m_pBoxShape;
}

///////////////////////////////////////////////////////

palNovodexBodyBase::palNovodexBodyBase() {
	m_Actor = NULL;
	m_ActorDesc.density = 0; //we want to specify properties via mass
	m_ActorDesc.body = &m_BodyDesc;
}

palNovodexBodyBase::~palNovodexBodyBase() {
	if (m_Actor) {
		Cleanup();
		gScene->releaseActor(*m_Actor);
		m_Actor = NULL;
	}
}

void palNovodexBodyBase::SetGroup(palGroup group) {
	palBodyBase::SetGroup(group);
	if (!m_Actor) return;
	NxShape *const *ps = m_Actor->getShapes();
	 for (unsigned int i=0;i<m_Actor->getNbShapes();i++)
		ps[i]->setGroup(group);
}

void palNovodexBodyBase::SetMaterial(palMaterial *material) {
	if (!m_Actor) return;
	palNovodexMaterialUnique *pm = dynamic_cast<palNovodexMaterialUnique *>(material);
	if (pm) {
		 NxShape *const *ps = m_Actor->getShapes();
		 for (unsigned int i=0;i<m_Actor->getNbShapes();i++)
			ps[i]->setMaterial(pm->m_Index);
	}
	palBodyBase::SetMaterial(material);
}

void palNovodexBodyBase::SetPosition(palMatrix4x4& location) {
	NxMat34 m;
	m.setColumnMajor44(location._mat);
	if (m_Actor) {
		m_Actor->setGlobalPose(m);
	} else {
		m_ActorDesc.globalPose = m;
		palBodyBase::SetPosition(location);
	}
}

palMatrix4x4& palNovodexBodyBase::GetLocationMatrix() {
	if (m_Actor) {
		m_Actor->getGlobalPose().getColumnMajor44(m_mLoc._mat);
	}
	return m_mLoc;
}

void palNovodexBodyBase::BuildBody(Float mass, bool dynamic) {
	palNovodexGeometry *png=dynamic_cast<palNovodexGeometry *> (m_Geometries[0]);
	if (!png)
		return;
#ifdef NOVODEX_ENABLE_FLUID
	png->m_pShape->shapeFlags |= NX_SF_FLUID_TWOWAY;
#endif

	if (false) {
	NxSimpleTriangleMesh stm;
	stm.numVertices = png->GetNumberOfVertices();
	stm.numTriangles = png->GetNumberOfIndices()/3;
	stm.pointStrideBytes = sizeof(NxVec3);
	stm.triangleStrideBytes = sizeof(int)*3;

	stm.points = png->GenerateMesh_Vertices();
	stm.triangles = png->GenerateMesh_Indices();
	stm.flags |= NX_MF_FLIPNORMALS;

	NxCCDSkeleton *skel = gPhysicsSDK->createCCDSkeleton(stm);
	png->m_pShape->ccdSkeleton = skel;
	png->m_pShape->shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body
	}

	m_ActorDesc.shapes.pushBack(png->m_pShape);
	if (dynamic) {
		png->CalculateInertia();
		m_BodyDesc.mass = mass;
		m_BodyDesc.massSpaceInertia = NxVec3(png->m_fInertiaXX,png->m_fInertiaYY,png->m_fInertiaZZ);
	} else {
		m_BodyDesc.mass = 0;
		m_ActorDesc.body = 0;

	}
	m_Actor = gScene->createActor(m_ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
	if (!dynamic)
		m_Actor->raiseBodyFlag(NX_BF_KINEMATIC);
}
////////////////////////////
palNovodexGenericBody::palNovodexGenericBody() {
}

void palNovodexGenericBody::Init(palMatrix4x4& pos) {
	m_BodyDesc.mass = 1; //default to 1
	m_BodyDesc.massSpaceInertia = NxVec3(1,1,1); //set to defaults.

	m_Actor = gScene->createActor(m_ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
	SetPosition(pos);
}

void palNovodexGenericBody::SetPosition(palMatrix4x4& location) {
	NxMat34 m;
	m.setColumnMajor44(location._mat);
	if (!m_bKinematic) {
		palNovodexBodyBase::SetPosition(location);
	} else {
		if (m_Actor)
			m_Actor->moveGlobalPose(m);
		palBodyBase::SetPosition(location);
	}
}

bool palNovodexGenericBody::IsDynamic() {
	return !m_Actor->readBodyFlag(NX_BF_KINEMATIC);
}

bool palNovodexGenericBody::IsKinematic() {
	return m_Actor->readBodyFlag(NX_BF_KINEMATIC) && GetMass() != 0.0;
}

bool palNovodexGenericBody::IsStatic() {
	return m_Actor->readBodyFlag(NX_BF_KINEMATIC) && GetMass() == 0.0;
}

void palNovodexGenericBody::SetDynamicsType(palDynamicsType dynType) {

	palGenericBody::SetDynamicsType(dynType);

	switch (dynType)
	{
		case PALBODY_DYNAMIC:
		{
			m_Actor->clearBodyFlag(NX_BF_KINEMATIC);
			m_Actor->setMass(m_fMass);
		}
		case PALBODY_STATIC:
		{
			m_Actor->raiseBodyFlag(NX_BF_KINEMATIC);
			m_Actor->setMass(0);
		}
		case PALBODY_KINEMATIC:
		{
			m_Actor->raiseBodyFlag(NX_BF_KINEMATIC);
			m_Actor->setMass(m_fMass);
		}
	}

}

void palNovodexGenericBody::SetMass(Float mass)  {
	palGenericBody::SetMass(mass);
	m_Actor->setMass(mass);
}

void palNovodexGenericBody::SetInertia(Float Ixx, Float Iyy, Float Izz) {
	palGenericBody::SetInertia(Ixx,Iyy,Izz);
	NxVec3 inertia(Ixx,Iyy,Izz);
	m_Actor->setMassSpaceInertiaTensor(inertia);
}
void palNovodexGenericBody::SetCenterOfMass_LocalTransform(palMatrix4x4 loc) {
	NxMat34 m;
	m.setColumnMajor44(loc._mat);
	m_Actor->setCMassOffsetLocalPose(m);
}

void palNovodexGenericBody::SetCenterOfMass(palMatrix4x4& loc) {
	palGenericBody::SetCenterOfMass(loc);
	NxMat34 m;
	m.setColumnMajor44(loc._mat);
	m_Actor->setCMassOffsetGlobalPose(m);
}


void palNovodexGenericBody::ConnectGeometry(palGeometry* pGeom) {
	palNovodexGeometry *png=dynamic_cast<palNovodexGeometry *> (pGeom);
	if (!png)
		return;
	palGenericBody::ConnectGeometry(pGeom);
#ifdef NOVODEX_ENABLE_FLUID
	png->m_pShape->shapeFlags |= NX_SF_FLUID_TWOWAY;
#endif
	NxShapeDesc* pdesc = png->NxGetShapeDesc();
	png->m_pCreatedShape = m_Actor->createShape(*pdesc);
	//setLocalPose
	m_Actor->updateMassFromShapes(0,m_fMass);
	m_Actor->wakeUp();
}
void palNovodexGenericBody::RemoveGeometry(palGeometry* pGeom) {
	palNovodexGeometry *png=dynamic_cast<palNovodexGeometry *> (pGeom);
	if (!png)
		return;
	if (!png->NxGetShape())
		return;
	PAL_VECTOR<palGeometry*>::iterator it;
	for (it=m_Geometries.begin();it!=m_Geometries.end();it++) {
		if (*it == pGeom) {
			m_Actor->releaseShape(*png->NxGetShape());
			m_Actor->updateMassFromShapes(0,m_fMass);
			m_Geometries.erase(it);
			return;
		}
	}
}
////////////////////////////
palNovodexBody::~palNovodexBody() {
}

palNovodexBody::palNovodexBody() {
}


void palNovodexBody::SetPosition(palMatrix4x4& location) {
	palNovodexBodyBase::SetPosition(location);
}

bool palNovodexBody::IsActive()
{
	return !m_Actor->isSleeping();
}

void palNovodexBody::SetActive(bool active) {
	if (active)
		m_Actor->wakeUp();
	else
		m_Actor->putToSleep();
}

#if 0
void palNovodexBody::SetForce(Float fx, Float fy, Float fz) {
	NxVec3 v;
	v.x=fx; v.y=fy; v.z=fz;
	//m_Actor->setForce(v);
	m_Actor->addForce(v);

#pragma message("todo: set & get force & torque impl")
/* novodex people write:
- Reworked applyForce code:
- removed these methods of NxActor because they were causing user confusion
(they were hoping that it did more than just read back what they have previously set...)
setForce (), setTorque(), getForce(), getTorque()

The replacement for setForce/setTorque is calling addForce/addTorque just once (per frame).
The replacement of getForce ()/getTorque() is to keep track of the forces you add.
*/
}

void palNovodexBody::GetForce(palVector3& force) {
	NxVec3 v;
//	m_Actor->getForce(v);
	force.x=v.x;
	force.y=v.y;
	force.z=v.z;
}

void palNovodexBody::SetTorque(Float tx, Float ty, Float tz) {
	NxVec3 v;
	v.x=tx; v.y=ty; v.z=tz;
//	m_Actor->setTorque(v);
	m_Actor->addTorque(v);
}

void palNovodexBody::GetTorque(palVector3& force) {
	NxVec3 v;
//	m_Actor->getTorque(v);
	force.x=v.x;
	force.y=v.y;
	force.z=v.z;
}

void palNovodexBody::ApplyForce(Float fx, Float fy, Float fz) {
//	NxVec3 v;
//	v.x=fx; v.y=fy; v.z=fz;
//	m_Actor->addForce(v);
	Float ts=PF->GetActivePhysics()->GetLastTimestep();
	ApplyImpulse(fx*ts,fy*ts,fz*ts);
}
#endif

void palNovodexBody::ApplyImpulse(Float fx, Float fy, Float fz) {
	NxVec3 v;
	v = m_Actor->getLinearMomentum();
	v.x+=fx;	v.y+=fy;	v.z+=fz;
	m_Actor->setLinearMomentum(v);
}

void palNovodexBody::ApplyAngularImpulse(Float fx, Float fy, Float fz) {
	NxVec3 v;
	v = m_Actor->getAngularMomentum ();
	v.x+=fx;	v.y+=fy;	v.z+=fz;
	m_Actor->setAngularMomentum (v);
}


void palNovodexBody::ApplyTorque(Float tx, Float ty, Float tz) {
	NxVec3 v;
	v.x=tx; v.y=ty; v.z=tz;
	m_Actor->addTorque(v);
}

void palNovodexBody::GetLinearVelocity(palVector3& force) {
	NxVec3 v;
	v = m_Actor->getLinearVelocity();
	force.x=v.x;
	force.y=v.y;
	force.z=v.z;
}

void palNovodexBody::GetAngularVelocity(palVector3& force) {
	NxVec3 v;
	v = m_Actor->getAngularVelocity();
	force.x=v.x;
	force.y=v.y;
	force.z=v.z;
}

void palNovodexBody::SetLinearVelocity(palVector3 velocity_rad) {
	NxVec3 v;
	v.x = velocity_rad.x;
	v.y = velocity_rad.y;
	v.z = velocity_rad.z;
	m_Actor->setLinearVelocity(v);
}
void palNovodexBody::SetAngularVelocity(palVector3 velocity_rad) {
	NxVec3 v;
	v.x = velocity_rad.x;
	v.y = velocity_rad.y;
	v.z = velocity_rad.z;
	m_Actor->setAngularVelocity(v);
}

///////////////////////////////////////////////////////

palNovodexBox::palNovodexBox() {

}

void palNovodexBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	BuildBody(mass,true);
}

palNovodexStaticBox::palNovodexStaticBox() {
}

void palNovodexStaticBox::Init(palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palStaticBox::Init(pos,width,height,depth);
	BuildBody(0,false);
}

///////////////////////////////////////////////////////
palNovodexSphereGeometry::palNovodexSphereGeometry() {
	m_pSphereShape=NULL;
}

palNovodexSphereGeometry::~palNovodexSphereGeometry() {
	if (m_pSphereShape)
		delete m_pSphereShape;
}

void palNovodexSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	m_pSphereShape = new NxSphereShapeDesc;
	m_pSphereShape->radius = radius;
	NxMat34 m;
	m.setColumnMajor44(m_mOffset._mat);
	m_pSphereShape->localPose = m;
	m_pShape = m_pSphereShape;
}

palNovodexSphere::palNovodexSphere(){
}

void palNovodexSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	BuildBody(mass,true);
}

palNovodexStaticSphere::palNovodexStaticSphere() {
}

void palNovodexStaticSphere::Init(palMatrix4x4 &pos, Float radius) {
	palStaticSphere::Init(pos,radius);
	BuildBody(0,false);
}

///////////////////////////////////////////////////////

palNovodexCapsuleGeometry::~palNovodexCapsuleGeometry() {
	if (m_pCapShape) {
		delete m_pCapShape;
	}
}

palNovodexCapsuleGeometry::palNovodexCapsuleGeometry() {
	m_pCapShape = NULL;
}

void palNovodexCapsuleGeometry::Init(palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	palCapsuleGeometry::Init(pos,radius,length,mass);
	m_pCapShape = new NxCapsuleShapeDesc;
	m_pCapShape->radius=radius;
	m_pCapShape->height=length;
	NxMat34 m;
	m.setColumnMajor44(m_mOffset._mat);
	m_pCapShape->localPose = m;
	m_pShape = m_pCapShape;
}

palNovodexCapsule::palNovodexCapsule(){
}

void palNovodexCapsule::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palCapsule::Init(x,y,z,radius,length,mass);
	BuildBody(mass,true);
}

palNovodexStaticCapsule::palNovodexStaticCapsule(){
}

void palNovodexStaticCapsule::Init(palMatrix4x4 &pos, Float radius, Float length) {
	palStaticCapsule::Init(pos, radius,length);
	BuildBody(0,false);
}

///////////////////////////////////////////////////////

palNovodexStaticCompoundBody::palNovodexStaticCompoundBody() {
}

void palNovodexStaticCompoundBody::Finalize() {
	for (unsigned int i=0;i<m_Geometries.size();i++) {
		palNovodexGeometry *png=dynamic_cast<palNovodexGeometry *> (m_Geometries[i]);
		m_ActorDesc.shapes.pushBack(png->m_pShape);
	}
	m_BodyDesc.mass = 0;
	m_ActorDesc.body = 0;
	m_Actor = gScene->createActor(m_ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
	m_Actor->raiseBodyFlag(NX_BF_KINEMATIC);
}

palNovodexCompoundBody::palNovodexCompoundBody() {

}

void palNovodexCompoundBody::Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ) {
	for (unsigned int i=0;i<m_Geometries.size();i++) {
		palNovodexGeometry *png=dynamic_cast<palNovodexGeometry *> (m_Geometries[i]);
		m_ActorDesc.shapes.pushBack(png->m_pShape);
	}
	m_BodyDesc.mass = finalMass;
//	m_BodyDesc.massSpaceInertia = NxVec3(m_fInertiaXX,m_fInertiaYY,m_fInertiaZZ);

	m_Actor = gScene->createActor(m_ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
}

///////////////////////////////////////////////////////

palNovodexLink::palNovodexLink(){
	m_Jdesc = NULL;
	m_Joint = NULL;
}


///////////////////////////////////////////////////////

palNovodexRevoluteLink::palNovodexRevoluteLink() {
	m_RJdesc = NULL;
	m_RJoint = NULL;
}

palNovodexRevoluteLink::~palNovodexRevoluteLink() {
	if (m_RJdesc)
		delete m_RJdesc;
}

void palNovodexRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	m_RJdesc = new NxRevoluteJointDesc;
	m_Jdesc = m_RJdesc;

	palNovodexBodyBase *body0 = dynamic_cast<palNovodexBodyBase *> (parent);
	palNovodexBodyBase *body1 = dynamic_cast<palNovodexBodyBase *> (child);

	NxVec3 pivot(x,y,z);
	NxVec3 c(axis_x,axis_y,axis_z);


	m_RJdesc->setToDefault();

//	m_RJdesc->motor.maxForce=0;
//	m_RJdesc->flags |= NX_RJF_MOTOR_ENABLED

	if (dynamic_cast<palStatic *>(body0))
		m_RJdesc->actor[0] = 0;
	else
		m_RJdesc->actor[0] = body0->m_Actor;

	if (dynamic_cast<palStatic *>(body1))
		m_RJdesc->actor[1] = 0;
	else
		m_RJdesc->actor[1] = body1->m_Actor;

    m_RJdesc->setGlobalAnchor(pivot);
    m_RJdesc->setGlobalAxis(c);
    m_Joint = gScene->createJoint(*m_RJdesc);
	if (m_Joint)
		m_RJoint = m_Joint->isRevoluteJoint();
}
void palNovodexRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	if (!m_Joint)
		return;

	m_RJoint->setFlags( m_RJoint->getFlags() | NX_RJF_LIMIT_ENABLED);

	NxJointLimitPairDesc limit;
	limit.setToDefault();
	limit.low.value = lower_limit_rad;
	limit.high.value= upper_limit_rad;
	m_RJoint->setLimits(limit);
}
/*
Float palNovodexRevoluteLink::GetAngle() {
	return m_RJoint->getAngle();
}

Float palNovodexRevoluteLink::GetAngularVelocity() {
	return m_RJoint->getVelocity();
}
*/
///////////////////////////////////////////////////////

palNovodexSphericalLink::palNovodexSphericalLink() {
	m_SJoint=NULL;
	m_SJdesc=NULL;
}
palNovodexSphericalLink::~palNovodexSphericalLink() {
	if (m_SJdesc)
		delete m_SJdesc;
}
void palNovodexSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {

	palSphericalLink::Init(parent,child,x,y,z);
	m_SJdesc = new NxSphericalJointDesc;
	m_Jdesc = m_SJdesc;

	palNovodexBodyBase *body0 = dynamic_cast<palNovodexBodyBase *> (parent);
	palNovodexBodyBase *body1 = dynamic_cast<palNovodexBodyBase *> (child);

	NxVec3 pivot(x,y,z);

	m_SJdesc->setToDefault();
    m_SJdesc->actor[0] = body0->m_Actor;
    m_SJdesc->actor[1] = body1->m_Actor;
    m_SJdesc->setGlobalAnchor(pivot);

    m_Joint = gScene->createJoint(*m_SJdesc);
	if (!m_Joint){
		SET_ERROR("Could not create joint");
		return;
	}
	m_SJoint = m_Joint->isSphericalJoint();
}

void palNovodexSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {

	m_SJdesc->twistLimit.low.value = -twist_limit_rad;
	m_SJdesc->twistLimit.high.value = twist_limit_rad;
	m_SJdesc->twistSpring.setToDefault();

	m_SJdesc->swingLimit.value = cone_limit_rad;
	m_SJdesc->swingSpring.setToDefault();

	m_SJoint->loadFromDesc(*m_SJdesc);
	m_SJoint->setFlags(m_SJoint->getFlags() | NX_SJF_TWIST_LIMIT_ENABLED | NX_SJF_SWING_LIMIT_ENABLED);
}


palNovodexPrismaticLink::palNovodexPrismaticLink() {
	m_PJoint=NULL;
	m_PJdesc=NULL;
}

palNovodexPrismaticLink::~palNovodexPrismaticLink() {
	if (m_PJdesc)
		delete m_PJdesc;
}

void palNovodexPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palPrismaticLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	m_PJdesc = new NxPrismaticJointDesc;
	m_Jdesc=m_PJdesc;

	palNovodexBodyBase *body0 = dynamic_cast<palNovodexBodyBase *> (parent);
	palNovodexBodyBase *body1 = dynamic_cast<palNovodexBodyBase *> (child);

	NxVec3 pivot(x,y,z);
	NxVec3 c(axis_x,axis_y,axis_z);

	m_PJdesc->setToDefault();
	m_PJdesc->actor[0] = body0->m_Actor;
    m_PJdesc->actor[1] = body1->m_Actor;
	m_PJdesc->setGlobalAnchor(pivot);
	m_PJdesc->setGlobalAxis(c);

    m_Joint = gScene->createJoint(*m_PJdesc);
	if (!m_Joint){
		SET_ERROR("Could not create joint");
		return;
	}
	m_PJoint = m_Joint->isPrismaticJoint();
}

///////////////////////////////////////////////////////
palNovodexGenericLink::palNovodexGenericLink() {
	m_DJoint=0;
	m_DJdesc=0;
}
void palNovodexGenericLink::Init(palBodyBase *parent, palBodyBase *child, palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
		palVector3 linearLowerLimits,
		palVector3 linearUpperLimits,
		palVector3 angularLowerLimits,
		palVector3 angularUpperLimits) {

	palGenericLink::Init(parent,child,parentFrame,childFrame,linearLowerLimits,linearUpperLimits,angularLowerLimits,angularUpperLimits);
	palNovodexBody *body0 = dynamic_cast<palNovodexBody *> (parent);
	palNovodexBody *body1 = dynamic_cast<palNovodexBody *> (child);

	m_DJdesc = new NxD6JointDesc;

	m_DJdesc->setToDefault();

	m_DJdesc->actor[0] = body0->m_Actor;
    m_DJdesc->actor[1] = body1->m_Actor;

/*
	printf("n0:%f,%f,%f\n",parentFrame._11,parentFrame._12,parentFrame._13);
	printf("x0:%f,%f,%f\n",parentFrame._31,parentFrame._32,parentFrame._33);
	printf("a0:%f,%f,%f\n",parentFrame._41,parentFrame._42,parentFrame._43);

	printf("n1:%f,%f,%f\n",childFrame._11,childFrame._12,childFrame._13);
	printf("x1:%f,%f,%f\n",childFrame._31,childFrame._32,childFrame._33);
	printf("a1:%f,%f,%f\n",childFrame._41,childFrame._42,childFrame._43);
*/

	m_DJdesc->localNormal[0] = NxVec3(parentFrame._11,parentFrame._12,parentFrame._13);
	m_DJdesc->localAxis[0]	 = NxVec3(parentFrame._31,parentFrame._32,parentFrame._33);
	m_DJdesc->localAnchor[0] = NxVec3(parentFrame._41,parentFrame._42,parentFrame._43);

	m_DJdesc->localNormal[1] = NxVec3(childFrame._11,childFrame._12,childFrame._13);
	m_DJdesc->localAxis[1]	 = NxVec3(childFrame._31,childFrame._32,childFrame._33);
	m_DJdesc->localAnchor[1] = NxVec3(childFrame._41,childFrame._42,childFrame._43);


//	d6Desc.setGlobalAnchor(globalAnchor);
//	d6Desc.setGlobalAxis(globalAxis);

	m_DJdesc->swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	m_DJdesc->swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	m_DJdesc->twistMotion = NX_D6JOINT_MOTION_LOCKED;


	m_DJdesc->xMotion = NX_D6JOINT_MOTION_LOCKED;
	m_DJdesc->yMotion = NX_D6JOINT_MOTION_LOCKED;
	m_DJdesc->zMotion = NX_D6JOINT_MOTION_LOCKED;

#define EP 0.0001f

	if ((linearLowerLimits.x<-EP) || (linearUpperLimits.x>EP)) {
		m_DJdesc->xMotion = NX_D6JOINT_MOTION_LIMITED;
		m_DJdesc->linearLimit.value = linearUpperLimits.x;
	}
//	d6Desc.linearLimit.value = gLinearLimit;
//	d6Desc.swing1Limit.value = gSwing1Limit;
//	d6Desc.swing2Limit.value = gSwing2Limit;


	if ((fabs(angularLowerLimits.z)<EP) && (fabs(angularUpperLimits.z)<EP)) {
		m_DJdesc->twistMotion = NX_D6JOINT_MOTION_LIMITED;
		m_DJdesc->twistLimit.low.value = (NxReal) DEG2RAD*angularLowerLimits.z;
		m_DJdesc->twistLimit.high.value = (NxReal) DEG2RAD*angularUpperLimits.z;
	}


    m_Joint = gScene->createJoint(*m_DJdesc);
}

///////////////////////////////////////////////////////
palNovodexTerrainMesh::palNovodexTerrainMesh() {

}
void palNovodexTerrainMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palTerrainMesh::Init(x,y,z,pVertices,nVertices,pIndices,nIndices);

	// Build physical model
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= m_nVertices;
	terrainDesc.numTriangles				= m_nIndices / 3;
	terrainDesc.pointStrideBytes			= sizeof(Float)*3;
	terrainDesc.triangleStrideBytes			= 3*sizeof(int);
	terrainDesc.points						= pVertices;
	terrainDesc.triangles					= pIndices;
	terrainDesc.flags						= 0;

 	MemoryWriteBuffer buf;
	NxInitCooking();
    bool status = NxCookTriangleMesh(terrainDesc, buf);


	NxTriangleMeshShapeDesc terrainShapeDesc;
	//terrainShapeDesc.meshData				= gPhysicsSDK->createTriangleMesh(terrainDesc);
	terrainShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));

	NxCloseCooking();

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&terrainShapeDesc);
	m_Actor=gScene->createActor(ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);

}
/*
palMatrix4x4& palNovodexTerrainMesh::GetLocationMatrix() {
	mat_identity(&m_mLoc);
	return m_mLoc;
}*/
/////////////
palNovodexTerrainHeightmap::palNovodexTerrainHeightmap() {
}

void palNovodexTerrainHeightmap::Init(Float px, Float py, Float pz, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap) {
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
	palNovodexTerrainMesh::Init(px,py,pz,v,nv,ind,ni);

	delete [] v;
	delete [] ind;
}
/*
palMatrix4x4& palNovodexTerrainHeightmap::GetLocationMatrix() {
	return palNovodexTerrainMesh::GetLocationMatrix();
}


/*
void palNovodexTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
	NxPlaneShapeDesc PlaneDesc;
	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&PlaneDesc);
	gScene->createActor(ActorDesc);
}

palMatrix4x4& palNovodexTerrainPlane::GetLocationMatrix() {
	mat_identity(&m_mLoc);
	return m_mLoc;
}
*/

void palNovodexSpring::Init(palBody *pb1,palBody *pb2,
		Float x1, Float y1, Float z1,
		Float x2, Float y2, Float z2,
		Float rest_length, Float Ks, Float Kd) {
			palNovodexBody *body0 = dynamic_cast<palNovodexBody *> (pb1);
			palNovodexBody *body1 = dynamic_cast<palNovodexBody *> (pb2);

			m_pSpring  = gScene->createSpringAndDamperEffector(NxSpringAndDamperEffectorDesc());
			NxVec3 pos1(x1,y1,z1);
			NxVec3 pos2(x2,y2,z2);
			m_pSpring->setBodies(body0->m_Actor,pos1,body1->m_Actor,pos2);
			m_pSpring->setLinearSpring(rest_length*0.25f,rest_length,rest_length*2,2000,2000);

		}

////////////////////////////////////////////


palNovodexConvexGeometry::palNovodexConvexGeometry() {
	m_pConvexMesh=NULL;
	m_pConvexShape=NULL;
}

palNovodexConvexGeometry::~palNovodexConvexGeometry() {
	delete m_pConvexMesh;
	delete m_pConvexShape;
}

void palNovodexConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	palConvexGeometry::Init(pos,pVertices,nVertices,mass);
//	palGeometry::SetPosition(pos);//m_Loc = pos;
//	palGeometry::SetMass(mass);
	// Create descriptor for convex mesh
	m_pConvexMesh = new NxConvexMeshDesc;
	m_pConvexMesh->numVertices			= nVertices;
	m_pConvexMesh->pointStrideBytes		= sizeof(Float)*3;
	m_pConvexMesh->points				= pVertices;
	m_pConvexMesh->flags				= NX_CF_COMPUTE_CONVEX;

	m_pConvexShape = new NxConvexShapeDesc;
	m_pShape = m_pConvexShape;

	NxInitCooking();
	MemoryWriteBuffer buf;
    bool status = NxCookConvexMesh(*m_pConvexMesh, buf);
    m_pConvexShape->meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));
}

void palNovodexConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
	palGeometry::SetPosition(pos);//m_Loc = pos;
	palGeometry::SetMass(mass);

	//palBoxGeometry::Init(pos,width,height,depth,mass);
	m_pConvexMesh = new NxConvexMeshDesc;
	m_pConvexMesh->numVertices			= nVertices;
	m_pConvexMesh->pointStrideBytes		= sizeof(Float)*3;
	m_pConvexMesh->points				= pVertices;
	m_pConvexMesh->numTriangles			= nIndices;
	m_pConvexMesh->triangles			= pIndices;
	m_pConvexMesh->triangleStrideBytes	= 3 * sizeof(int);
	m_pConvexMesh->flags				= 0;

	m_pConvexShape = new NxConvexShapeDesc;
/*
    NxMat34 m;
	m.setColumnMajor44(m_mOffset._mat);
	m_pConvexShape->localPose = m;*/
	m_pShape = m_pConvexShape;

	NxInitCooking();
	MemoryWriteBuffer buf;
    bool status = NxCookConvexMesh(*m_pConvexMesh, buf);
    m_pConvexShape->meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));

}


//////////////////////////////
palNovodexStaticConvex::palNovodexStaticConvex() {
}

void palNovodexStaticConvex::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices) {
	palStaticConvex::Init(pos,pVertices,nVertices);
	////
	palNovodexConvexGeometry *png=dynamic_cast<palNovodexConvexGeometry *> (m_Geometries[0]);
	m_ActorDesc.shapes.pushBack(png->m_pConvexShape);

	m_BodyDesc.mass = 0;
	m_ActorDesc.body = 0;

	m_Actor = gScene->createActor(m_ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
	m_Actor->raiseBodyFlag(NX_BF_KINEMATIC);
}

palNovodexConvex::palNovodexConvex() {
}

void palNovodexConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	////
	palNovodexConvexGeometry *png=dynamic_cast<palNovodexConvexGeometry *> (m_Geometries[0]);
	m_ActorDesc.shapes.pushBack(png->m_pConvexShape);
	m_fMass = mass;

	m_BodyDesc.mass = mass;
	m_BodyDesc.massSpaceInertia = NxVec3(png->m_fInertiaXX,png->m_fInertiaYY,png->m_fInertiaZZ);

	m_Actor = gScene->createActor(m_ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);
//	m_Actor->raiseBodyFlag(NX_BF_KINEMATIC);
}

void palNovodexConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
	palBody::SetPosition(x,y,z);

	palNovodexConvexGeometry *m_pGeom = new palNovodexConvexGeometry;
	m_Geometries.push_back(m_pGeom);
	SetGeometryBody(m_pGeom);
	m_pGeom->Init(m_mLoc,pVertices,nVertices,pIndices,nIndices,mass);

	palNovodexConvexGeometry *png=dynamic_cast<palNovodexConvexGeometry *> (m_Geometries[0]);

	m_ActorDesc.shapes.pushBack(png->m_pConvexShape);
	//set mass:
	//png->CalculateInertia();
	m_BodyDesc.mass = mass;
	m_BodyDesc.massSpaceInertia = NxVec3(png->m_fInertiaXX,png->m_fInertiaYY,png->m_fInertiaZZ);
	//*/
	m_fMass = mass;
	//end set mass

//	NxBodyDesc bodyDesc;
//	m_ActorDesc.body = &bodyDesc;
//	m_ActorDesc.density = 1;

	 //m_ActorDesc.body = NULL;
	//m_ActorDesc.globalPose.t = NxVec3(0,2,0);

	m_Actor = gScene->createActor(m_ActorDesc);
	m_Actor->userData=dynamic_cast<palBodyBase*>(this);

}


///////////////////////////////////////////////////////////////////////////////
palNovodexPSDSensor::palNovodexPSDSensor() {
}
void palNovodexPSDSensor::Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range) {
	palPSDSensor::Init(body,x,y,z,dx,dy,dz,range);
	palVector3 pos;
	body->GetPosition(pos);
	m_fRelativePosX = m_fPosX - pos.x;
	m_fRelativePosY = m_fPosY - pos.y;
	m_fRelativePosZ = m_fPosZ - pos.z;
}

Float palNovodexPSDSensor::GetDistance() {
	palMatrix4x4 m;
	palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();
	palMatrix4x4 out;

	mat_identity(&m);
	mat_translate(&m,m_fRelativePosX,m_fRelativePosY,m_fRelativePosZ);
	mat_multiply(&out,&bodypos,&m);

	NxVec3 orig(out._41,out._42,out._43);

	mat_identity(&m);
	mat_translate(&m,m_fAxisX,m_fAxisY,m_fAxisZ);
	mat_multiply(&out,&bodypos,&m);

	palVector3 newaxis;
	newaxis.x=out._41-bodypos._41;
	newaxis.y=out._42-bodypos._42;
	newaxis.z=out._43-bodypos._43;
	vec_norm(&newaxis);

	NxVec3 dir(newaxis.x,newaxis.y,newaxis.z);

#if 0
printf("o:%f %f %f\n",orig.x, orig.y, orig.z);
#endif

	NxRay ray(orig, dir);
	NxRaycastHit hit;
	NxReal dist;
	NxShape* closestShape = gScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit);
	if (closestShape) {
		const NxVec3& worldImpact = hit.worldImpact;
		dist = hit.distance;
		if (dist<m_fRange)
			return dist;
	}
	return m_fRange;
}

//////////////////////////////////////////////////
palNovodexContactSensor::palNovodexContactSensor() {}

void palNovodexContactSensor::Init(palBody *body) {
	palNovodexPhysics *pnp = dynamic_cast<palNovodexPhysics *>(PF->GetActivePhysics());
	if (!pnp) return;
	pnp->NotifyCollision(body,true);
	/*
	palNovodexBodyBase *b0 = dynamic_cast<palNovodexBodyBase *> (body);

	NxActor **ppact = gScene->getActors();
	for (int i=0;i<gScene->getNbActors();i++) {
		if (ppact[i]!=b0->m_Actor)
			gScene->setActorPairFlags(*(b0->m_Actor),*(ppact[i]),NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_ON_END_TOUCH);
	}
	*/
}

void palNovodexContactSensor::GetContactPosition(palVector3& contact) {
	palNovodexPhysics *pnp = dynamic_cast<palNovodexPhysics *>(PF->GetActivePhysics());
	if (!pnp) return;
	palContact c;
		pnp->GetContacts(m_pBody,c);
	if (c.m_ContactPoints.size()>0){
		contact = c.m_ContactPoints[0].m_vContactPosition;
		return;
	}
	contact.x = -999;
	contact.y = -999;
	contact.z = -999;
	//
	printf("No contact");
}

palNovodexAngularMotor::palNovodexAngularMotor() {
	m_j = 0;
}
void palNovodexAngularMotor::Init(palRevoluteLink *pLink, Float Max) {
	palAngularMotor::Init(pLink,Max);
	palNovodexRevoluteLink *pnrl = dynamic_cast<palNovodexRevoluteLink *> (m_link);
	if (pnrl)
		m_j = pnrl->m_RJoint;
	if (!m_j)
		return;

	m_j->setFlags( m_j->getFlags() | NX_RJF_MOTOR_ENABLED);

	NxMotorDesc motorDesc;//(0,m_fMax);
	//m_j->velTarget = 0;

	m_j->getMotor(motorDesc);
	motorDesc.velTarget = 0;
	motorDesc.maxForce = m_fMax;
	m_j->setMotor(motorDesc);

}
void palNovodexAngularMotor::Update(Float targetVelocity) {
	if (!m_j) return;
	NxMotorDesc motorDesc;
	m_j->getMotor(motorDesc);

	motorDesc.velTarget = targetVelocity;
	m_j->setMotor(motorDesc);
}
void palNovodexAngularMotor::Apply() {
}



///////////////////////////////////////////////////////////////////////////////
#ifdef NOVODEX_ENABLE_FLUID
#define REST_DENSITY 1000
#define REST_PARTICLES_PER_METER 15
#define KERNEL_RADIUS_MULTIPLIER 1.8
#define MOTION_LIMIT_MULTIPLIER 3
#define PACKET_SIZE_MULTIPLIER 8
bool bHardwareScene = false;

palNovodexFluid::palNovodexFluid() {
	fluid = 0;
	ParticleBufferNum = 0;
	ParticleBufferCap = 0;
}
void palNovodexFluid::Init() {
	//
};
void palNovodexFluid::AddParticle(Float x, Float y, Float z, Float vx, Float vy, Float vz) {
	vParticles.push_back(NxVec3(x,y,z));
};
int palNovodexFluid::GetNumParticles() {
	return (int)vParticles.size();
}
palVector3* palNovodexFluid::GetParticlePositions() {
	pos.resize(GetNumParticles());
	for (int i=0;i<GetNumParticles();i++) {
		pos[i].x = vParticles[i].x;
		pos[i].y = vParticles[i].y;
		pos[i].z = vParticles[i].z;
	}
	return &pos[0];
}
/*palVector3& GetParticlePosition(int i) {
m_ppos.x = vParticles[i].x;
m_ppos.y = vParticles[i].y;
m_ppos.z = vParticles[i].z;
return m_ppos;
}*/
void palNovodexFluid::Finalize() {
	ParticleBufferNum = (NxU32)vParticles.size();
	ParticleBufferCap = (NxU32)vParticles.size();

	// Set structure to pass particles, and receive them after every simulation step
	NxParticleData particles;
	//particles.maxParticles			= &ParticleBufferCap;
	particles.numParticlesPtr		= &ParticleBufferNum;
	particles.bufferPos				= &vParticles[0].x;
	particles.bufferPosByteStride	= sizeof(NxVec3);

	NxFluidDesc fluidDesc;
	fluidDesc.maxParticles                  = ParticleBufferCap;
	fluidDesc.kernelRadiusMultiplier		= (NxReal)KERNEL_RADIUS_MULTIPLIER;
	fluidDesc.restParticlesPerMeter			= (NxReal)REST_PARTICLES_PER_METER;
	fluidDesc.motionLimitMultiplier			= (NxReal)MOTION_LIMIT_MULTIPLIER;
	fluidDesc.packetSizeMultiplier			= PACKET_SIZE_MULTIPLIER;
	fluidDesc.stiffness						= 50;
	fluidDesc.viscosity						= 22;
	fluidDesc.restDensity					= 1000;
	fluidDesc.damping						= 0;
	fluidDesc.restitutionForStaticShapes 	= (NxReal)0.4;
	fluidDesc.dynamicFrictionForStaticShapes= (NxReal)0.03;
	//fluidDesc.staticCollisionRestitution	= 0.4;
	//fluidDesc.staticCollisionAdhesion		= 0.03;
	fluidDesc.simulationMethod				= NX_F_SPH; //NX_F_NO_PARTICLE_INTERACTION;

	fluidDesc.flags |= NX_FF_COLLISION_TWOWAY;

	fluidDesc.collisionMethod =  NX_F_STATIC | NX_F_DYNAMIC;
	fluidDesc.collisionResponseCoefficient = 1.0f;

	fluidDesc.initialParticleData			= particles;
	fluidDesc.particlesWriteData			= particles;

	if(!bHardwareScene)
		fluidDesc.flags &= ~NX_FF_HARDWARE;

	fluid = gScene->createFluid(fluidDesc);
}

#ifdef STATIC_CALLHACK
void pal_novodex_call_me_hack() {
	printf("%s I have been called!!\n", __FILE__);
};
#endif

#endif
