#ifndef BULLET_PALVEHICLE_H
#define BULLET_PALVEHICLE_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Bullet vehicle implementation.
		This enables the use of bullet vehicles via PAL.
	Author:
		Adrian Boeing
	Revision History:
	Version 0.0.1 : 17/08/07 - Vehicle and wheel
	TODO:
		- motor,gears,etc.
	notes:
*/



#include "bullet_pal.h"
#include "../pal/palVehicle.h"

class palBulletWheel : public palWheel {
public:
	palBulletWheel();
	void Init(Float x, Float y, Float z, Float radius, Float width, Float suspension_rest_length, Float suspension_Ks, Float suspension_Kd, bool powered, bool steering, bool brakes,
				Float suspension_Travel, Float friction_Slip);
	palMatrix4x4& GetLocationMatrix();
	btRaycastVehicle*	m_vehicle;
	int m_WheelIndex;
};

class palBulletVehicle : public palVehicle {
public:
	palBulletVehicle();
	void Init(palBody *chassis, Float MotorForce, Float BrakeForce);

	virtual palWheel* AddWheel();
	virtual void Finalize();

	virtual void Control(Float steering, Float acceleration, bool brakes);
	virtual void Update() {
		//do nothing...
	}

	//bullet code:
	virtual void ForceControl(Float steering, Float accelerationforce, Float brakeforce);

	Float	m_cEngineForce;
	Float	m_cBreakingForce;
	Float	m_cVehicleSteering;

	btRigidBody* m_carChassis;
	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster*	m_vehicleRayCaster;
	btRaycastVehicle*	m_vehicle;
	btDynamicsWorld* m_dynamicsWorld;

	FACTORY_CLASS(palBulletVehicle,palVehicle,Bullet,1)
};


#endif
