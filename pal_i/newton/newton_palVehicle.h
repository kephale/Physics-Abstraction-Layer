#ifndef NEWTON_PALVEHICLE_H
#define NEWTON_PALVEHICLE_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Bullet vehicle implementation.
		This enables the use of bullet vehicles via PAL.
	Author:
		Adrian Boeing
	Revision History:
	Version 0.2.1 : 12/12/07 - OSX compatibility and warning removal
	Version 0.2.0 : 13/11/07 - Wheel orientation fix.
	Version 0.0.1 : 30/10/07 - Vehicle and wheel
	TODO:
		- motor,gears,etc.
	notes:
*/

#if NEWTON_MAJOR_VERSION == 2 && NEWTON_MINOR_VERSION == 0
#warning "Vehicles not supported in Newton 2.0"
#else

#include "newton_pal.h"
#include "../pal/palVehicle.h"


class palNewtonCar;
/////////////////////// h4x0r:
class palNewtonWheel : public palWheel {
public:
	palNewtonWheel();
	~palNewtonWheel();

	virtual palMatrix4x4& GetLocationMatrix();

	void Init(const palWheelInfo& wheelInfo) {
	   palWheel::Init(wheelInfo);
		rigupPhysics(m_vehicleJoint,wheelInfo.m_fPosX, wheelInfo.m_fPosY, wheelInfo.m_fPosZ,
			wheelInfo.m_fRadius,
			20.0f, //tire mass
			wheelInfo.m_fWidth,
			wheelInfo.m_fSuspension_Kd,
			wheelInfo.m_fSuspension_Ks,
			wheelInfo.m_fSuspension_Rest_Length,
			m_vwID);
	};

    void rigupPhysics(NewtonJoint *vehicle,
		Float wheelX,
		Float wheelY,
		Float wheelZ,
		Float tireRadius,
		Float tireMass,
		Float tireWidth,
		Float tireSuspensionShock,
		Float tireSuspensionSpring,
		Float tireSuspensionLength,
		//SPhysicsCar* car,
		//palNewtonWheel *wheelpointer,
		int tyreid);
    void setTirePhysics(const NewtonJoint *vehicle, void* id);

	// set
    void setSteer(Float steerAngle);
    void setTorque(Float torque);
    void setBrakes(Float brakes);

	palNewtonCar *m_pVehicle;
	NewtonJoint *m_vehicleJoint;
	int m_vwID;
protected:
    Float m_steerAngle;
    Float m_radius;
    Float m_torque;
    Float m_brakes;

};


class palNewtonCar : public palVehicle {
public:
	palNewtonCar();
	~palNewtonCar();

	palMatrix4x4 car_position;

	virtual void Init(
		palBody *chassis,
		Float MotorForce, Float BrakeForce);

	//PAL_VECTOR<palNewtonWheel *> m_vWheels;
	virtual palWheel* AddWheel() {
		palNewtonWheel *pnw = new palNewtonWheel;
		pnw->m_pVehicle =  this;
		pnw->m_vehicleJoint = m_vehicleJoint;
		pnw->m_vwID = (int)m_vWheels.size();
		m_vWheels.push_back(pnw);
		return pnw;
	}

	virtual void Finalize() {};
	virtual void Control(Float steering, Float acceleration, bool brakes) {
			setSteeringPercent(25*steering);
			setThrottlePercent(25*acceleration);
			setBrakesPercent(brakes ? (Float)100 : 0);
	};
	virtual void ForceControl(Float steering, Float acceleration, Float brakes) {
			setSteeringPercent(25*steering);
			setThrottlePercent(25*acceleration);
			setBrakesPercent(100.0 * brakes);
	};
	virtual void Update() {};
	// driving interface
	void setSteeringPercent(Float steeringPercent);
	void setThrottlePercent(Float throttlePercent);
	void setBrakesPercent(Float brakesPercent);

	// set
    void setSteering(Float steerangle);
    void setTorque(Float torque);
    void setBrakes(Float brakes);
	void setVehicleSpeed(Float speed);


	NewtonJoint* getVehicleJoint();
	NewtonBody* getBody();


protected:
	void cap(Float limit, Float* target, bool positiveAndNegative);

	Float m_vehicleSpeed;
    NewtonBody *m_carBody;
    NewtonJoint *m_vehicleJoint;
	Float m_maxTorque;
	Float m_maxSteerAngle;
	Float m_maxBrakes;
private:
	FACTORY_CLASS(palNewtonCar,palVehicle,Newton,1)
};

#endif

#endif
