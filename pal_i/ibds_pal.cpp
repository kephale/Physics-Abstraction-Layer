#include "ibds_pal.h"


FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palIBDSPhysics);

FACTORY_CLASS_IMPLEMENTATION(palIBDSCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palIBDSBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palIBDSBox);

FACTORY_CLASS_IMPLEMENTATION(palIBDSSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palIBDSSphere);

FACTORY_CLASS_IMPLEMENTATION(palIBDSCylinderGeometry);
FACTORY_CLASS_IMPLEMENTATION(palIBDSCylinder);


FACTORY_CLASS_IMPLEMENTATION(palIBDSConvexGeometry);
FACTORY_CLASS_IMPLEMENTATION(palIBDSConvex);

FACTORY_CLASS_IMPLEMENTATION(palIBDSTerrainPlane);

FACTORY_CLASS_IMPLEMENTATION(palIBDSSphericalLink);

FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

using namespace IBDS;


int controllerIndex = Simulation::TIMESTEP_ITERATIVE;

palIBDSPhysics::palIBDSPhysics() {
}


void palIBDSPhysics::Init(palPhysicsDesc& desc) {
	palPhysics::Init(desc);
	Simulation *sim = Simulation::getCurrent ();
	sim->setCollisionDetectionMethod (CollisionDetection::CD_BULLET);
	sim->setMaxDistance (controllerIndex, 0.000001);
	sim->setMaxVelDiff (controllerIndex, 0.001);
	sim->setMaxCorrectionSteps (controllerIndex, 100);
	sim->setCRMethod (Simulation::CR_JBNEWTON);
	sim->setTimeOfImpactComputation (Simulation::TOI_BINARYSEARCH);
	(*sim->getCollisionResponseController ())[sim->getCRMethod ()]->setMaxCorrectionSteps (100);
	(*sim->getCollisionResponseController ())[sim->getCRMethod ()]->setMaxVelDiff (0.001);
	(*sim->getCollisionResponseController ())[sim->getCRMethod ()]->setShockPropagation (true);
	sim->setMaxDistanceContacts (controllerIndex, 0.0001);
	sim->setMaxCorrectionStepsContacts (controllerIndex, 5000);
	(*sim->getTimeStepController ())[controllerIndex]->setVCorrectionContacts (false);
	sim->getCollisionDetection ()->setTolerance (0.04);
	sim->setVCorrection (controllerIndex, true);
	sim->setMaxCorrImpulse (controllerIndex, 10);
	sim->setMaxJointImpulse (controllerIndex, -1);
	sim->setIntegrationMethod (controllerIndex, TimeStepController::RUNGE_KUTTA);

	sim->setGravitation(Vector3D(m_fGravityX, m_fGravityY, m_fGravityZ));
}

void palIBDSPhysics::Cleanup() {
	delete Simulation::getCurrent ();
}

const char* palIBDSPhysics::GetPALVersion() {
	static char verbuf[512];
	sprintf(verbuf,"PAL SDK V%d.%d.%d\nPAL IBDS V:%d.%d.%d\nFile: %s\nCompiled: %s %s\nModified:%s",
		PAL_SDK_VERSION_MAJOR,PAL_SDK_VERSION_MINOR,PAL_SDK_VERSION_BUGFIX,
		IBDS_PAL_SDK_VERSION_MAJOR,IBDS_PAL_SDK_VERSION_MINOR,IBDS_PAL_SDK_VERSION_BUGFIX,
		__FILE__,__TIME__,__DATE__,__TIMESTAMP__);
	return verbuf;
}

const char* palIBDSPhysics::GetVersion() {
	char buf[256];
	sprintf(buf,"IBDS %s",IBDS_VERSION_NUMBER);
	return buf;
}

/*
	static const int TIMESTEP_ITERATIVE = 0;
	static const int TIMESTEP_LINEARTIME = 1;

	toiMethod = TOI_BINARYSEARCH;
	crMethod = CR_JBNEWTON;
	collisionResponseController.push_back (new CollisionResponseJBNewton ());
	collisionResponseController.push_back (new CollisionResponseJBPoisson ());

	timeStepController.push_back (new TimeStepClassic ());
	timeStepController.push_back (new TimeStepLinear ());

	static const int TAYLOR = 0;
	static const int ADAPTIVE_TAYLOR = 1;
	static const int RUNGE_KUTTA = 2;
	static const int EMBEDDED_RUNGE_KUTTA = 3;
*/
//TOI_NONE
//TOI_BINARYSEARCH
//TOI_INTERPOLATIONSEARCH

void palIBDSPhysics::Iterate(Float timestep) {
	TimeManager::getCurrent ()->setTimeStepSize (timestep);
	Simulation::getCurrent ()->timeStep ();
}

IBDS::Simulation* palIBDSPhysics::IBDSGetSimulation() {
	return Simulation::getCurrent ();
}

IBDS::TimeManager* palIBDSPhysics::IBDSGetTimeManager() {
	return TimeManager::getCurrent ();
}

palIBDSBodyBase::palIBDSBodyBase() {
	m_prb = 0;
}

palMatrix4x4& palIBDSBodyBase::GetLocationMatrix() {
	if (m_prb) {
	Vector3D *translation = m_prb->getCenterOfMass ();
	Matrix3x3 *rotation = m_prb->getRotationMatrix ();


	Float *val = m_mLoc._mat;
	val[0] = (*rotation)[0][0]; val[1] = (*rotation)[0][1]; val[2] = (*rotation)[0][2]; val[3] = 0;
	val[4] = (*rotation)[1][0]; val[5] = (*rotation)[1][1]; val[6] = (*rotation)[1][2]; val[7] = 0;
	val[8] = (*rotation)[2][0]; val[9] = (*rotation)[2][1]; val[10] = (*rotation)[2][2]; val[11] = 0;
	val[12] = (*translation)[0]; val[13] = (*translation)[1]; val[14] = (*translation)[2]; val[15] = 1;
	}
	return m_mLoc;
}


void palIBDSBodyBase::BuildBody(Float fx, Float fy, Float fz, Float mass, bool dynamic) {
	m_prb = new RigidBody ();
	mat_identity(&m_mLoc);
	palBodyBase::SetPosition(fx,fy,fz);
	m_prb->setDynamic (dynamic);
	m_prb->setMass (mass);
	if (!dynamic) {
		m_prb->setCenterOfMassV (Vector3D (0,0.0,0.0));
	}
/*
	for (int i=0;i<m_Geometries.size();i++) {
		palIBDSGeometry *pig=dynamic_cast<palIBDSGeometry *> (m_Geometries[i]);
		m_prb->addGeometry(pig->m_pGeom);
	}
	*/
	Simulation::getCurrent ()->addBody (controllerIndex,m_prb);
		for (int i=0;i<m_Geometries.size();i++) {
			palIBDSGeometry *pig=dynamic_cast<palIBDSGeometry *> (m_Geometries[i]);
			pig->Attach();

		}

	Simulation::getCurrent ()->buildModel();
}

void palIBDSBodyBase::SetPosition(palMatrix4x4& loc) {
	palBodyBase::SetPosition(loc);
	if (m_prb) {
		m_prb->setCenterOfMass (Vector3D (loc._41,loc._42,loc._43));
		//todo rot
	}
}

void palIBDSBodyBase::SetMaterial(palMaterial *material) {
	palBodyBase::SetMaterial(material);
	m_prb->setStaticFriction(material->m_fStatic);
	m_prb->setDynamicFriction(material->m_fKinetic);
	m_prb->setRestitution(material->m_fRestitution);
}

////////////////////////////////////////////////////////////////

palIBDSGeometry::palIBDSGeometry() {
	m_pGeom = 0;
}
#if 0
Geometry* createCubeGeometry (Real sx, Real sy, Real sz)
{
	// Cube data
	 Real vertices[] =
	{
		-0.5,  0.5,  0.5,
		-0.5,  -0.5,  0.5,
		-0.5,  -0.5,  -0.5,
		-0.5,  0.5,  -0.5,
		0.5,  -0.5,  0.5,
		0.5,  -0.5,  -0.5,
		0.5,  0.5,  0.5,
		0.5,  0.5,  -0.5
	};


	MeshGeometry *geo = new MeshGeometry ();
	Vector3D scale = Vector3D (sx, sy, sz);
	geo->setTriangles (8, vertices, 12, faces, NULL, NULL, &scale);
	return geo;
}
#endif

palIBDSBoxGeometry::palIBDSBoxGeometry() {
}
/*
void palIBDSBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	//m_pGeom = createCubeGeometry(width,height,depth);
}*/

void palIBDSBoxGeometry::Attach() {
	GenericAttach();
}

void palIBDSGeometry::GenericAttach() {
	palIBDSBodyBase *pibb = dynamic_cast<palIBDSBodyBase *>(m_pBody);
	if (pibb) {
		Real *vertices = new Real[GetNumberOfVertices()*3];
		Float *v = GenerateMesh_Vertices();
		int i;
		for (i=0;i<GetNumberOfVertices()*3;i++) {
			vertices[i]=v[i];
		}
		Vector3D scale = Vector3D (1, 1, 1);
		Simulation::getCurrent ()->addCollisionObject (pibb->IBDSGetRigidBody(),
		GetNumberOfVertices(), vertices, GetNumberOfIndices()/3, GenerateMesh_Indices(), &scale);
		delete vertices;
	}
}

palIBDSCylinderGeometry::palIBDSCylinderGeometry() {

}

//virtual void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
void palIBDSCylinderGeometry::Attach() {
	GenericAttach();
}

palIBDSConvexGeometry::palIBDSConvexGeometry() {
}

void palIBDSConvexGeometry::Attach() {
	GenericAttach();
}

palIBDSConvex::palIBDSConvex() {
}

void palIBDSConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	BuildBody(x,y,z,mass,true);
}

palIBDSSphereGeometry::palIBDSSphereGeometry() {
}
/*
void palIBDSSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
}*/
void palIBDSSphereGeometry::Attach() {
	palIBDSBodyBase *pibb = dynamic_cast<palIBDSBodyBase *>(m_pBody);
	if (pibb) {
	Simulation::getCurrent ()->addCollisionSphere (pibb->IBDSGetRigidBody(), Vector3D(m_mOffset._41,m_mOffset._42,m_mOffset._43), m_fRadius);
	}
}


palIBDSBody::palIBDSBody() {
}



void palIBDSBody::ApplyForce(Float fx, Float fy, Float fz){}
	 void palIBDSBody::ApplyTorque(Float tx, Float ty, Float tz){}

	 void palIBDSBody::ApplyImpulse(Float fx, Float fy, Float fz){}
	 void palIBDSBody::ApplyAngularImpulse(Float fx, Float fy, Float fz){}

	 void palIBDSBody::GetLinearVelocity(palVector3& velocity){
		Vector3D *pv3d	= m_prb->getCenterOfMassV();
		velocity.x = pv3d->v[0];
		velocity.y = pv3d->v[1];
		velocity.z = pv3d->v[2];
	 }

	 void palIBDSBody::GetAngularVelocity(palVector3& velocity_rad){
		Vector3D *pv3d	= m_prb->getAngularVelocity();
		velocity_rad.x = pv3d->v[0];
		velocity_rad.y = pv3d->v[1];
		velocity_rad.z = pv3d->v[2];
	 }

	 void palIBDSBody::SetLinearVelocity(palVector3 velocity){
		 m_prb->setCenterOfMassV (Vector3D (velocity.x,velocity.y,velocity.z));
	 }

	 void palIBDSBody::SetAngularVelocity(palVector3 velocity_rad){
		m_prb->setAngularVelocity (Vector3D (velocity_rad.x,velocity_rad.y,velocity_rad.z));
	 }

	 bool palIBDSBody::IsActive() { return true; }
	 void palIBDSBody::SetActive(bool active) {}


palIBDSCompoundBody::palIBDSCompoundBody() {
}

void palIBDSCompoundBody::Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ) {
	BuildBody(m_fPosX,m_fPosY,m_fPosZ,finalMass,true);
	m_prb->setInertiaTensor (Vector3D(iXX, iYY, iZZ));
}

palIBDSBox::palIBDSBox() {
}

void palIBDSBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	BuildBody(x,y,z,mass,true);
	m_prb->setInertiaTensor (SimMath::computeBoxIntertiaTensor (mass, width, height, depth));
}

palIBDSCylinder::palIBDSCylinder() {
}

void palIBDSCylinder::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palCapsule::Init(x,y,z,radius,length,mass);
	BuildBody(x,y,z,mass,true);
	m_prb->setInertiaTensor (Vector3D(m_Geometries[0]->m_fInertiaXX, m_Geometries[0]->m_fInertiaYY, m_Geometries[0]->m_fInertiaZZ));
}

palIBDSSphere::palIBDSSphere() {
}

void palIBDSSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	BuildBody(x,y,z,mass,true);
	m_prb->setInertiaTensor (SimMath::computeSphereIntertiaTensor (mass, radius));
}

palIBDSTerrainPlane::palIBDSTerrainPlane() {
}

void palIBDSTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
	GenerateDefaultBoxGeom(0.5f);
	BuildBody(x,y,z,0,false);
}


palIBDSSphericalLink::palIBDSSphericalLink() {
}
void palIBDSSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palIBDSBody *body0 = dynamic_cast<palIBDSBody *> (parent);
	palIBDSBody *body1 = dynamic_cast<palIBDSBody *> (child);

	BallJoint *bj = new BallJoint (body0->IBDSGetRigidBody() , body1->IBDSGetRigidBody(), new Vector3D (x,y,z));
	Simulation::getCurrent ()->addJoint (controllerIndex, bj);

    Simulation::getCurrent ()->buildModel();
}


palIBDSRevoluteLink::palIBDSRevoluteLink() {
}

void palIBDSRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palIBDSBody *body0 = dynamic_cast<palIBDSBody *> (parent);
	palIBDSBody *body1 = dynamic_cast<palIBDSBody *> (child);

	HingeJoint *hj = new HingeJoint (body0->IBDSGetRigidBody(), body1->IBDSGetRigidBody(),
		new Vector3D (x,y,z),
		new Vector3D (x+axis_x,y+axis_y,z+axis_z));

	Simulation::getCurrent ()->addJoint (controllerIndex, hj);

    Simulation::getCurrent ()->buildModel();

}
void palIBDSRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
}

palIBDSPrismaticLink::palIBDSPrismaticLink() {
}

void palIBDSPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palIBDSBody *body0 = dynamic_cast<palIBDSBody *> (parent);
	palIBDSBody *body1 = dynamic_cast<palIBDSBody *> (child);

	SliderJoint *sj = new SliderJoint (body0->IBDSGetRigidBody() , body1->IBDSGetRigidBody(),
		new Vector3D (x,y,z),
		new Vector3D (x+axis_x,y+axis_y,z+axis_z),
		new Vector3D (x,y,z));
	Simulation::getCurrent ()->addJoint (controllerIndex,sj);

    Simulation::getCurrent ()->buildModel();
}
