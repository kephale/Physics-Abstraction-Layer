#include "bullet_palVehicle.h"

//#define SWAPXZ

palBulletVehicle::palBulletVehicle() {
}

palBulletVehicle::~palBulletVehicle() {
	delete m_vehicleRayCaster;
	m_vehicleRayCaster = NULL;
	delete m_vehicle;
	m_vehicle = NULL;
}

void palBulletVehicle::Init(palBody *chassis, Float MotorForce, Float BrakeForce) {
	palVehicle::Init(chassis,MotorForce,BrakeForce);
	m_cEngineForce = 0;
	m_cBreakingForce = 0;
	m_cVehicleSteering = 0;

	palBulletPhysics *pbp = dynamic_cast<palBulletPhysics *>(PF->GetActivePhysics());
	m_dynamicsWorld = pbp->BulletGetDynamicsWorld();
	palBulletBody *pbb = dynamic_cast<palBulletBody *>(chassis);
	m_carChassis = pbb->BulletGetRigidBody();
#if 1
	btTransform tr;
	btQuaternion q;
	q.setRotation(btVector3(0,0,1),btScalar(M_PI*0.5f));
	tr.setIdentity();
	tr.setRotation(q);
	//		m_carChassis->setCenterOfMassTransform(tr);
	//		m_carChassis->setWorldTransform(tr);
#endif

	m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_dynamicsWorld);
	m_vehicle = new btRaycastVehicle(m_tuning,m_carChassis,m_vehicleRayCaster);

	///never deactivate the vehicle
	m_carChassis->setActivationState(DISABLE_DEACTIVATION);

	m_dynamicsWorld->addVehicle(m_vehicle);

	unsigned int upAxis = palFactory::GetInstance()->GetActivePhysics()->GetUpAxis();

	int rightIndex = 0;
	int upIndex = 1;
	int forwardIndex = 2;

   if (upAxis == 2) {
		rightIndex = 0;
		upIndex = 2;
		forwardIndex = 1;
	} else if (upAxis == 0) {
		rightIndex = 1;
		upIndex = 0;
		forwardIndex = 2;
	}

	//ie: x is z, y is y, z is x
	//choose coordinate system
	m_vehicle->setCoordinateSystem(rightIndex,upIndex,forwardIndex);


}


void palBulletVehicle::Finalize() {
	float	rollInfluence = 2.0f;//1.0f;
	//float	wheelFriction = 1000;//1e30f;
	for (int i=0; i<m_vehicle->getNumWheels(); i++) {
		btWheelInfo& wheel = m_vehicle->getWheelInfo(i);

		wheel.m_suspensionStiffness = m_vWheels[i]->m_fSuspension_Ks;
		wheel.m_wheelsDampingRelaxation = m_vWheels[i]->m_fSuspension_Kd;
		wheel.m_wheelsDampingCompression = m_vWheels[i]->m_fSuspension_Kd;
		//wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;

		((palBulletWheel*)m_vWheels[i])->m_WheelIndex = i;
	}
}

void palBulletVehicle::ForceControl(Float steering, Float acceleration, Float brakes) {
	m_cEngineForce = acceleration;
	m_cBreakingForce = brakes;
	m_cVehicleSteering = steering;
	//for (int i=0;i<m_vehicle->getNumWheels();i++) {
	for (PAL_VECTOR<palWheel *>::size_type i=0;i<m_vWheels.size();i++) {
		if (m_vWheels[i]->m_bDrive )
			m_vehicle->applyEngineForce(m_cEngineForce,(int)i);
		if (m_vWheels[i]->m_bBrake )
			m_vehicle->setBrake(m_cBreakingForce,(int)i);
		if (m_vWheels[i]->m_bSteer )
			m_vehicle->setSteeringValue(m_cVehicleSteering,(int)i);
	}
}

void palBulletVehicle::Control(Float steering, Float acceleration, bool brakes) {
	if (brakes)
		m_cBreakingForce = m_fBrakeForce;
	m_cEngineForce = acceleration * m_fMotorForce;
	m_cVehicleSteering = steering*0.3f;
	ForceControl(m_cVehicleSteering,m_cEngineForce,m_cBreakingForce);
}


palWheel* palBulletVehicle::AddWheel() {
	palBulletWheel *pbw = new palBulletWheel;
	pbw->m_pVehicle =  this;
	pbw->m_vehicle = m_vehicle;
	m_vWheels.push_back(pbw);
	return pbw;
}

palBulletWheel::palBulletWheel() {
	m_WheelIndex = -1;
};

void palBulletWheel::Init(Float x, Float y, Float z, Float radius, Float width, Float suspension_rest_length, Float suspension_Ks, Float suspension_Kd, bool powered, bool steering, bool brakes,
			Float suspension_Travel, Float friction_Slip) {
	palWheel::Init(x,y,z,radius,width,suspension_rest_length,suspension_Ks,suspension_Kd,powered,steering,brakes,
				suspension_Travel, friction_Slip);


	unsigned int upAxis = palFactory::GetInstance()->GetActivePhysics()->GetUpAxis();

	btVector3 wheelDirectionCS0(0.0f, 0.0f, 0.0f);
	wheelDirectionCS0[upAxis] = -1.0f;

   btVector3 wheelAxleCS(0.0f, 0.0f, 1.0f);

	if (upAxis == 2) {
	   wheelAxleCS.setValue(1.0f, 0.0f, 0.0f);
	} else if (upAxis == 0) {
		wheelAxleCS.setValue(0.0f, 1.0f, 0.0f);
	}

	btVector3 connectionPointCS0(x,y,z);

	palBulletVehicle *pbv = dynamic_cast<palBulletVehicle *>(m_pVehicle);
	btRaycastVehicle::btVehicleTuning tuning = pbv->m_tuning;
	tuning.m_frictionSlip = friction_Slip;
	tuning.m_maxSuspensionTravelCm = suspension_Travel;
	m_vehicle->addWheel(connectionPointCS0,wheelDirectionCS0,wheelAxleCS,suspension_rest_length,radius,tuning,steering);
}
palMatrix4x4& palBulletWheel::GetLocationMatrix() {
	if (m_WheelIndex<0)
		return m_mLoc;
	m_vehicle->updateWheelTransform(m_WheelIndex,true);
	m_vehicle->getWheelInfo(m_WheelIndex).m_worldTransform.getOpenGLMatrix(m_mLoc._mat);
	return m_mLoc;
}


