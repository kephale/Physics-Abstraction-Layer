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


#include "newton_pal.h"
#include "../pal/palVehicle.h"

class palNewtonCar;
/////////////////////// h4x0r:
class palNewtonWheel : public palWheel {
public:
	palNewtonWheel();
	~palNewtonWheel();

	virtual palMatrix4x4& GetLocationMatrix();

	void Init(Float x, Float y, Float z, Float radius, Float width, Float suspension_rest_length, Float suspension_Ks, Float suspension_Kd, bool powered, bool steering, bool brakes) {
		palWheel::Init(x,y,z,radius,width,suspension_rest_length,suspension_Ks,suspension_Kd,powered,steering,brakes);
		rigupPhysics(m_vehicleJoint,x,y,z,
			radius,
			20.0f, //tire mass
			width,
			suspension_Kd,
			suspension_Ks,
			suspension_rest_length,
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
