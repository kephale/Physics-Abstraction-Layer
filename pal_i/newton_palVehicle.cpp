#include "newton_palVehicle.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CDECL PhysicsCarWheelUpdate(const NewtonJoint *vehicle)
{
    void * wheelid = 0;
    for(wheelid = NewtonVehicleGetFirstTireID(vehicle); wheelid; wheelid = NewtonVehicleGetNextTireID(vehicle, wheelid))
    {
        palNewtonWheel *wheel = (palNewtonWheel *)NewtonVehicleGetTireUserData(vehicle, wheelid);
        wheel->setTirePhysics(vehicle, wheelid);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CDECL PhysicsCarSetTransform(const NewtonBody *body, const float *matrix) {
    palMatrix4x4 mat;
    memcpy(mat._mat, matrix, sizeof(float)*16);
	

	// dedicated car callback - this is a safe cast
    palNewtonCar *car = (palNewtonCar *)NewtonBodyGetUserData(body);

	memcpy(car->car_position._mat, matrix, sizeof(float)*16);

	// read the velocity into a buffer
	Float bodyVelocityBuffer[3];
		
	NewtonBodyGetVelocity(body, bodyVelocityBuffer);

//	printf("car is at pos: %f %f %f\n",mat._41,mat._42,mat._43);
//	printf("car has vel: %f %f %f\n",bodyVelocityBuffer[0],bodyVelocityBuffer[1],bodyVelocityBuffer[2]);
//	fflush(stdout);
	void *wheelid = 0;
	for(wheelid = NewtonVehicleGetFirstTireID(car->getVehicleJoint()); wheelid; wheelid = NewtonVehicleGetNextTireID(car->getVehicleJoint(), wheelid)) {
		palNewtonWheel *wheel = (palNewtonWheel *)NewtonVehicleGetTireUserData(car->getVehicleJoint(), wheelid);
		NewtonVehicleGetTireMatrix(car->getVehicleJoint(), wheelid, wheel->m_mLoc._mat);
		//printf("wheel %d is at: %f %f %f\n",wheelid,mat._41,mat._42,mat._43);
	}
}

palNewtonCar::palNewtonCar() {
	m_maxSteerAngle=0.6f;
}

palNewtonCar::~palNewtonCar() {
}

void palNewtonCar::setSteeringPercent(Float steeringPercent)
{
	// cap at + or - 100
	cap(100.0f, &steeringPercent, true);

	steeringPercent /= 100.0f;

	setSteering(steeringPercent * m_maxSteerAngle);
}

void palNewtonCar::setThrottlePercent(Float throttlePercent)
{
	// cap at + or - 100
	cap(100.0f, &throttlePercent, true);

	throttlePercent /= 100.0f;

	setTorque(throttlePercent * m_maxTorque);
}

void palNewtonCar::setBrakesPercent(Float brakesPercent)
{
	cap(100.0f, &brakesPercent, true);

	brakesPercent /= 100.0f;

	setBrakes(brakesPercent * m_maxBrakes);
}

// only used by callback - shouldn't be used externally
void palNewtonCar::setVehicleSpeed(Float speed)
{
	m_vehicleSpeed = speed;
}

NewtonJoint* palNewtonCar::getVehicleJoint()
{
	return m_vehicleJoint;
}


// callback for applying gravity - formally PhysicsApplyForceAndTorque
void CDECL PhysicsApplyGravity(const NewtonBody* body)
{
	Float mass;
	Float Ixx;
	Float Iyy;
	Float Izz;

    // get the body's mass, apply gravity
	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);

	Float force[3];
	force[0] = 0.0f;
	force[1] = mass * -9.8f;
	force[2] = 0.0f;
	#pragma message("todo: rewrite for grav")

	NewtonBodySetForce (body, &force[0]);
}

void palNewtonCar::Init(		palBody *chassis,
								Float maxTorque,
								Float maxBrakes
								) {
	m_maxTorque = maxTorque;
	m_maxBrakes = maxBrakes;
		palNewtonPhysics * pnp = dynamic_cast<palNewtonPhysics *>(PF->GetActivePhysics());
	NewtonWorld* g_nWorld = pnp->GetNewtonWorld();
	palNewtonBody *pnb = dynamic_cast<palNewtonBody *>(chassis);
	m_carBody = pnb->m_pntnBody;

	NewtonBodySetUserData(m_carBody, this);
	// set callbacks
    NewtonBodySetTransformCallback(m_carBody, PhysicsCarSetTransform);

    NewtonBodySetForceAndTorqueCallback(m_carBody, PhysicsApplyGravity);

	// y is up
	Float upDirection[3];
	upDirection[0] = 0.0f;
	upDirection[1] = 1.0f;
	upDirection[2] = 0.0f;
	m_vehicleJoint = NewtonConstraintCreateVehicle (g_nWorld, &upDirection[0], m_carBody);

    NewtonVehicleSetTireCallback(m_vehicleJoint, PhysicsCarWheelUpdate);
	
	// disable auto freeze - we always want this to be updated
    NewtonBodySetAutoFreeze(m_carBody, 0);

	// set minimum linear damping
	NewtonBodySetLinearDamping(m_carBody, 0);
}

NewtonBody* palNewtonCar::getBody()
{
	return m_carBody;
}


// cap the target variable to the maxMagnitude
// if positiveAndNegative is true, target will be capped at -maxMagnitude <= target <= maxMagnitude
void palNewtonCar::cap(Float maxMagnitude, Float* target, bool positiveAndNegative)
{
	if(*target > maxMagnitude)
	{
		*target = maxMagnitude;
	}
	else if(*target < -maxMagnitude)
	{
		*target = -maxMagnitude;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palMatrix4x4& palNewtonWheel::GetLocationMatrix() {
	return m_mLoc;
}


palNewtonWheel::palNewtonWheel() {
    m_steerAngle = 0.0f;
    m_torque = 0.0f;
    m_brakes = 0.0f;

    m_radius = 0.0f;
}

palNewtonWheel::~palNewtonWheel() {
}

void palNewtonCar::setBrakes(Float brakes)
{
	for (VECTOR<palWheel *>::size_type i=0;i<m_vWheels.size();i++) {
		if (m_vWheels[i]->m_bBrake) {
			palNewtonWheel *pnw = dynamic_cast<palNewtonWheel *>(m_vWheels[i]);
			pnw->setBrakes(brakes);
		}
	}
}


// interface uses this to set values for Newton - externally use setSteeringPercent()
void palNewtonCar::setSteering(Float steerangle)
{

	for (VECTOR<palWheel *>::size_type i=0;i<m_vWheels.size();i++) {
		if (m_vWheels[i]->m_bSteer){
			palNewtonWheel *pnw = dynamic_cast<palNewtonWheel *>(m_vWheels[i]);
			pnw->setSteer(steerangle);
		}
	}
}

// interface uses this to set values for Newton - externally use setThrottlePercent()
void palNewtonCar::setTorque(Float torque)
{

	for (VECTOR<palWheel *>::size_type i=0;i<m_vWheels.size();i++) {
		if (m_vWheels[i]->m_bDrive) {
			palNewtonWheel *pnw = dynamic_cast<palNewtonWheel *>(m_vWheels[i]);
			pnw->setTorque(torque);
		}
	}
}


void palNewtonWheel::setSteer(Float newsteerangle)
{
    m_steerAngle = newsteerangle;
}

void palNewtonWheel::setTorque(Float torque)
{
	m_torque = torque;
}

void palNewtonWheel::setBrakes(Float brakes)
{
    m_brakes = brakes;
}



void palNewtonWheel::setTirePhysics(const NewtonJoint *vehicle, void* id)
{
	Float omega;
	Float speed;
	Float brakeAcceleration;


	Float currentSteerAngle;

	currentSteerAngle = NewtonVehicleGetTireSteerAngle (vehicle, id);
	NewtonVehicleSetTireSteerAngle (vehicle, id, currentSteerAngle +  (m_steerAngle - currentSteerAngle) * 0.25f);

	// get the tire angular velocity
	omega = NewtonVehicleGetTireOmega(vehicle, id);

	// add some viscous damp to the tire torque (this prevents out of control spin)
	NewtonVehicleSetTireTorque (vehicle, id, m_torque - (40.f * omega));

	// calculate the tire speed at the contact
	// set the max side slip speed as a fraction of the tire speed
	speed = m_radius * omega;
	NewtonVehicleSetTireMaxSideSleepSpeed (vehicle, id, speed * 0.8f);

	// The side slip is usually proportional to the tire longitudinal speed, and tire load
	NewtonVehicleSetTireSideSleepCoeficient (vehicle, id, speed * 0.5f);

	// if the brakes are applied...
	if (m_brakes > 0.0f) 
	{
		// ask Newton for the precise acceleration needed to stop the tire
		brakeAcceleration = NewtonVehicleTireCalculateMaxBrakeAcceleration (vehicle, id);

		// tell Newton you want this tire stoped but only if the torque needed is less than 
		// the brake pad can withstand (assume max brake pad torque is 500 newton * meter)
		NewtonVehicleTireSetBrakeAcceleration (vehicle, id, brakeAcceleration, 500.0f * m_brakes);

		// set some side slipe as funtion of the linear speed 
		speed = NewtonVehicleGetTireLongitudinalSpeed (vehicle, id);
		NewtonVehicleSetTireMaxSideSleepSpeed (vehicle, id, speed * 0.1f);
	}
	// reset the brakes
	m_brakes = 0.0f;
}

void palNewtonWheel::rigupPhysics(NewtonJoint *vehicle, 	
		Float wheelX, 
		Float wheelY,
		Float wheelZ,  
		Float tireRadius,
		Float tireMass,
		Float tireWidth,
		Float tireSuspensionShock,
		Float tireSuspensionSpring,
		Float tireSuspensionLength,
		//palNewtonWheel *wheelpointer, 
		int tyreid)
{ 
	m_radius = tireRadius;

	// create the tyre's position
    //matrix4 tirePosition;
    //tirePosition.setTranslation(wheelPosition);
	palMatrix4x4 tirePosition;
	mat_identity(&tirePosition);
	#pragma message("todo: tire orientation settings.")
	mat_rotate(&tirePosition,90,0,1,0);
	mat_set_translation(&tirePosition,wheelX,wheelY,wheelZ);


    // the tire will spin around the lateral axis of the same tire space
	Float tirePin[3];

	
#if 1
	tirePin[0] = 0.0f;
	tirePin[1] = 0.0f;
	tirePin[2] = 1.0f;
#else
	tirePin[0] = 1.0f;
	tirePin[1] = 0.0f;
	tirePin[2] = 0.0f;
#endif
    
	// add the tire and set this as the user data
    // parent should be the vehicle joint

	NewtonVehicleAddTire (vehicle, tirePosition._mat, &tirePin[0], tireMass, tireWidth, tireRadius, 
						  tireSuspensionShock, tireSuspensionSpring, tireSuspensionLength, 
						  this, tyreid);
}