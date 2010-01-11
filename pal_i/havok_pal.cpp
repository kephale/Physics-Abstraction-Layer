#include "havok_pal.h"

FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palHavokPhysics);

//FACTORY_CLASS_IMPLEMENTATION(palHavokCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palHavokBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palHavokSphereGeometry);
//FACTORY_CLASS_IMPLEMENTATION(palHavokCapsuleGeometry);
//FACTORY_CLASS_IMPLEMENTATION(palHavokConvexGeometry);

FACTORY_CLASS_IMPLEMENTATION(palHavokBox);
FACTORY_CLASS_IMPLEMENTATION(palHavokSphere);
//FACTORY_CLASS_IMPLEMENTATION(palHavokCapsule);
//FACTORY_CLASS_IMPLEMENTATION(palHavokConvex);

FACTORY_CLASS_IMPLEMENTATION(palHavokStaticBox);
FACTORY_CLASS_IMPLEMENTATION(palHavokStaticSphere);

FACTORY_CLASS_IMPLEMENTATION(palHavokTerrainPlane);
//FACTORY_CLASS_IMPLEMENTATION(palHavokTerrainMesh);

//FACTORY_CLASS_IMPLEMENTATION(palHavokPSDSensor);
FACTORY_CLASS_IMPLEMENTATION(palHavokSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palHavokRevoluteLink);


FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

bool g_write = false;
hkpWorld* physicsWorld = 0;
hkpMultithreadingUtil* multithreadingUtil = 0;

inline void hw() {
	if (!physicsWorld) return;
	if (g_write == true)
		return;
	g_write=true;
	physicsWorld->markForWrite();
}

inline void hu() {
	if (!physicsWorld) return;
	if (g_write == false)
		return;
	g_write=false;
	physicsWorld->unmarkForWrite();
}

static void HK_CALL errorReport(const char* msg, void*)
{
	printf("%s", msg);
}

palHavokPhysics::palHavokPhysics()
: m_fFixedTimeStep(0.0f)
{
	set_use_hardware = false;
	set_substeps = 1;
	set_pe = 1;
}

const char* palHavokPhysics::GetPALVersion() {
	static char verbuf[512];
	sprintf(verbuf,"PAL SDK V%d.%d.%d\nPAL Havok V:%d.%d.%d\nFile: %s\nCompiled: %s %s\nModified:%s",
		PAL_SDK_VERSION_MAJOR,PAL_SDK_VERSION_MINOR,PAL_SDK_VERSION_BUGFIX,
		HAVOK_PAL_SDK_VERSION_MAJOR,HAVOK_PAL_SDK_VERSION_MINOR,HAVOK_PAL_SDK_VERSION_BUGFIX,
		__FILE__,__TIME__,__DATE__,__TIMESTAMP__);
	return verbuf;
}

void palHavokPhysics::Init(palPhysicsDesc& desc) {
   palPhysics::Init(desc);
		// Initialize the base system including our memory system
	hkPoolMemory* memoryManager = new hkPoolMemory();
	hkThreadMemory* threadMemory = new hkThreadMemory(memoryManager, 16);
	hkBaseSystem::init( memoryManager, threadMemory, errorReport );
	memoryManager->removeReference();


	// We now initialize the stack area to 100k (fast temporary memory to be used by the engine).
	char* stackBuffer;
	{
		int stackSize = 0x100000;
		stackBuffer = hkAllocate<char>( stackSize, HK_MEMORY_CLASS_BASE);
		hkThreadMemory::getInstance().setStackArea( stackBuffer, stackSize);
	}


		// Create the physics world
		{
			// The world cinfo contains global simulation parameters, including gravity, solver settings etc.
			hkpWorldCinfo worldInfo;
			worldInfo.m_gravity = hkVector4(m_fGravityX, m_fGravityY, m_fGravityZ);

			// Set the simulation type of the world to multi-threaded.
			worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;


			worldInfo.m_collisionTolerance=0.0001f;
			physicsWorld = new hkpWorld(worldInfo);
		}


		//
		// When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
		// to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
		// must call markForRead / markForWrite before it modifies the world to enable these checks.
		//

		hw();
//		physicsWorld->markForWrite();


		//
		// Register all collision agents, even though only box - box will be used in this particular example.
		// It's important to register collision agents before adding any entities to the world.
		//
		{
			hkpAgentRegisterUtil::registerAllAgents( physicsWorld->getCollisionDispatcher() );
		}

				//
		// Create a multi-threading utility.  This utility will create a number of threads that will run the
		// physics step in parallel. The main thread calls functions in the utility which signal these threads
		// to start a new step, or wait for step completion.
		//
		hkpMultithreadingUtilCinfo info;
		info.m_world = physicsWorld;
		info.m_numThreads = set_pe;

		// In this example we enable timers. The multi-threading util will allocate a buffer per thread for capturing timer data.
		// If you leave this flag to false, no timers will be enabled.
		info.m_enableTimers = true;
		multithreadingUtil = new hkpMultithreadingUtil(info);

}

void palHavokPhysics::Iterate(Float timestep) {
	StartIterate(timestep);
	WaitForIteration();
}
//////////////////////////////////////////////////////////////////////
void palHavokPhysics::StartIterate(Float timestep) {
	hu();
	// This will signal all threads which are in the wait state, to start processing stepDeltaTime() concurrently.
	multithreadingUtil->startStepWorld( timestep );
}

bool palHavokPhysics::QueryIterationComplete() {
	WaitForIteration();
	return true;
}

void palHavokPhysics::WaitForIteration() {
	// We can now do other things in this thread if we wish, while the physics runs in separate threads.
	// The call below will block until all physics processing the timestep is finished in all threads.
	multithreadingUtil->waitForStepWorldFinished();
}
void palHavokPhysics::SetPE(int n) {
	set_pe = n;
	if (multithreadingUtil) {
		multithreadingUtil->setNumThreads(n);
	}
}

void palHavokPhysics::SetFixedTimeStep(float fixedTimeStep) {
   m_fFixedTimeStep = fixedTimeStep; //Not sure how to implement this. TODO
}

void palHavokPhysics::SetSubsteps(int n) {
	set_substeps = n;// not relevant?
}

void palHavokPhysics::SetHardware(bool status) {
	set_use_hardware = status;//todo
}

bool palHavokPhysics::GetHardware(void) {
	return false; //todo
}

//////////////////////////////////////////////////////////////////////
palHavokBodyBase::palHavokBodyBase(){
	pBody = 0;
}

palHavokBodyBase::~palHavokBodyBase(){
}

palMatrix4x4& palHavokBodyBase::GetLocationMatrix() {
	if (!pBody) return m_mLoc;
	hu();
	hkTransform t;
	t = pBody->getTransform();
	//hkReal hkmat[4*4];
	//t.get4x4ColumnMajor(hkmat);
	t.get4x4ColumnMajor(m_mLoc._mat);
	return m_mLoc;
}

void palHavokBodyBase::SetPosition(palMatrix4x4& location) {
	if (!pBody) return;
	hw();
	hkTransform t;
	t.set4x4ColumnMajor(location._mat);
	pBody->setTransform(t);
}

void palHavokBodyBase::BuildBody(Float x, Float y, Float z, Float mass, bool dynamic) {
	hw();
	palHavokGeometry *phg=dynamic_cast<palHavokGeometry *> (m_Geometries[0]);
	if (!phg) {
		SET_ERROR("No geometry");
	}
	info.m_shape= phg->pShape;
	info.m_position.set(x,y,z);
	if (!dynamic){
		info.m_motionType = hkpMotion::MOTION_FIXED;
	} else {
		info.m_motionType = hkpMotion::MOTION_DYNAMIC;
		info.m_inertiaTensor = phg->pMassProp->m_inertiaTensor;
		info.m_centerOfMass = phg->pMassProp->m_centerOfMass;
		info.m_mass = mass;
	}

	pBody = new hkpRigidBody(info);
	physicsWorld->addEntity(pBody);
	phg->pShape->removeReference();
	pBody->removeReference();
}

//////////////////////////////////////////////////////////////////////
palHavokBody::palHavokBody() {
}

void palHavokBody::ApplyForce(Float fx, Float fy, Float fz) {
	palBody::ApplyForce(fx,fy,fz);
}
void palHavokBody::ApplyTorque(Float tx, Float ty, Float tz){
	palBody::ApplyTorque(tx,ty,tz);
}

void palHavokBody::ApplyImpulse(Float fx, Float fy, Float fz){
	palBody::ApplyImpulse(fx,fy,fz);
}
void palHavokBody::ApplyAngularImpulse(Float fx, Float fy, Float fz){
	palBody::ApplyAngularImpulse(fx,fy,fz);
}

void palHavokBody::GetLinearVelocity(palVector3& velocity){
	hu();
	hkVector4 v = pBody->getLinearVelocity();
	velocity.x = v(0);
	velocity.y = v(1);
	velocity.z = v(2);
}

void palHavokBody::GetAngularVelocity(palVector3& velocity_rad){
	hu();
	hkVector4 v = pBody->getAngularVelocity();
	velocity_rad.x = v(0);
	velocity_rad.y = v(1);
	velocity_rad.z = v(2);
}

void palHavokBody::SetLinearVelocity(palVector3 velocity){
	hw();
	hkVector4 v(velocity.x,velocity.y,velocity.z);
	pBody->setLinearVelocity(v);
}

void palHavokBody::SetAngularVelocity(palVector3 velocity_rad){
	hw();
	hkVector4 v(velocity_rad.x,velocity_rad.y,velocity_rad.z);
	pBody->setAngularVelocity(v);
}

bool palHavokBody::IsActive()
{
   //TODO what is the method for this?
   return true;
}

void palHavokBody::SetActive(bool active) {
	hw();
	if (active)
		pBody->activate();
	else
		pBody->deactivate();
}

//////////////////////////////////////////////////////////////////////

palHavokGeometry::palHavokGeometry() {
	pMassProp = 0;
	pShape = 0;
}

palHavokGeometry::~palHavokGeometry() {
	delete pMassProp;
	delete pShape;
}

palHavokSphereGeometry::palHavokSphereGeometry() {
}
void palHavokSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	hw();
	pMassProp = new hkpMassProperties;
	palSphereGeometry::Init(pos,radius,mass);
	pShape = new hkpSphereShape(radius);
	hkpInertiaTensorComputer::computeSphereVolumeMassProperties(radius, mass, *pMassProp);
}

palHavokBoxGeometry::palHavokBoxGeometry() {
}
void palHavokBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	hw();
	pMassProp = new hkpMassProperties;
	palBoxGeometry::Init(pos,width,height,depth,mass);
	hkVector4 boxSize(width*0.5,height*0.5,depth*0.5);
	pShape = new hkpBoxShape(boxSize);
	if (mass>0)
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties(boxSize,mass,*pMassProp);
}

//////////////////////////////////////////////////////////////////////
palHavokBox::palHavokBox() {
}
void palHavokBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	BuildBody(x,y,z,mass,true);
}

palHavokStaticBox::palHavokStaticBox() {
}
void palHavokStaticBox::Init(palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palStaticBox::Init(pos,width,height,depth);
	BuildBody(pos._41, pos._42, pos._43,0,false);
	SetPosition(pos);
}
//////////////////////////////////////////////////////////////////////
palHavokSphere::palHavokSphere() {
}

void palHavokSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	BuildBody(x,y,z,mass,true);
}

palHavokStaticSphere::palHavokStaticSphere() {
}

void palHavokStaticSphere::Init(palMatrix4x4 &pos, Float radius) {
	palStaticSphere::Init(pos,radius);
	BuildBody(pos._41, pos._42, pos._43,0,false);
	SetPosition(pos);
}

//////////////////////////////////////////////////////////////////////

palHavokSphericalLink::palHavokSphericalLink(){
}

void palHavokSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	hw();
	palSphericalLink::Init(parent,child,x,y,z);
	palHavokBodyBase *body0 = dynamic_cast<palHavokBodyBase *> (parent);
	palHavokBodyBase *body1 = dynamic_cast<palHavokBodyBase *> (child);

	hkpBallAndSocketConstraintData* bsData = new hkpBallAndSocketConstraintData();
	hkVector4 pivot(x,y,z);
	hkTransform t0 = body0->pBody->getTransform();
	hkTransform t1 = body1->pBody->getTransform();
	bsData->setInWorldSpace(t0,t1, pivot);
	hkpConstraintInstance* bsInstance = new hkpConstraintInstance( body0->pBody, body1->pBody, bsData );
	physicsWorld->addConstraint( bsInstance );
	bsData->removeReference();
	bsInstance->removeReference();
}

void palHavokSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {

}

palHavokRevoluteLink::palHavokRevoluteLink() {
}
void palHavokRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	hw();
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palHavokBodyBase *body0 = dynamic_cast<palHavokBodyBase *> (parent);
	palHavokBodyBase *body1 = dynamic_cast<palHavokBodyBase *> (child);

	hkpLimitedHingeConstraintData* lhc;
	hkVector4 pivot(x,y,z);
	hkVector4 axis(axis_x, axis_y, axis_z);

	// Create constraint
	lhc = new hkpLimitedHingeConstraintData();
	lhc->setInWorldSpace(body0->pBody->getTransform(), body1->pBody->getTransform(), pivot, axis);
	//lhc->setMinAngularLimit(-HK_REAL_PI/4.0f);
	//lhc->setMaxAngularLimit(HK_REAL_PI/2.0f);
	physicsWorld->createAndAddConstraintInstance( body0->pBody, body1->pBody, lhc )->removeReference();
	lhc->removeReference();
}

void palHavokRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {

}

#if 0
class palHavokPrismaticLink:  public palPrismaticLink {
public:
	palHavokPrismaticLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);

protected:
	FACTORY_CLASS(palHavokPrismaticLink,palPrismaticLink,Havok,1)
};
#endif
//////////////////////////////////////////////////////////////////////
palHavokTerrainPlane::palHavokTerrainPlane() {
}

void palHavokTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
	GenerateDefaultBoxGeom(0.5f);
	BuildBody(x,y,z,0,false);
}
