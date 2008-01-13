#ifndef PALVEHICLE_H
#define PALVEHICLE_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*! \file palVehicle.h
	\brief
		PAL - Physics Abstraction Layer. 
		Vehicle
	\author
		Adrian Boeing
	\version
	<pre>
		Version 0.1   : 17/08/07 - Split from test case
	</pre>
	\todo
		- Motor, Gears, etc.
*/


class palVehicle;

class palWheel {
public:
	/** Initializes a wheel.
	\param x The x-coordinate of the wheel (relative to car center)
	\param y The y-coordinate of the wheel (relative to car center)
	\param z The z-coordinate of the wheel (relative to car center)
	\param radius The radius of the wheel
	\param width The width of the wheel
	\param suspension_rest_length The resting length of the suspension spring
	\param suspension_Ks The spring constant for the suspension
	\param suspension_Kd The dampening constant for the suspension
	\param powered Flag indicating whether this wheel is powered (driven) by the motor
	\param steering Flag indicating whether this wheel is affected by steering
	\param breaks Flag indicating whether this wheel is affected by breaking
	*/
	virtual void Init(Float x, Float y, Float z, Float radius, Float width, Float suspension_rest_length, Float suspension_Ks, Float suspension_Kd, bool powered, bool steering, bool brakes) {
		m_fPosX = x;
		m_fPosY = y;
		m_fPosZ = z;
		mat_identity(&m_mLoc);
		mat_translate(&m_mLoc,x,y,z);
		m_fRadius=radius;
		m_fWidth=width;
		m_fSuspension_Length=suspension_rest_length;
		m_fSuspension_Ks=suspension_Ks;
		m_fSuspension_Kd=suspension_Kd;
		m_bDrive=powered;
		m_bSteer=steering;
		m_bBrake=brakes;
	}
	/* Returns wheel location and orientation in world coordinates
	*/
	virtual palMatrix4x4& GetLocationMatrix() = 0;

	Float m_fPosX;
	Float m_fPosY;
	Float m_fPosZ;
	palMatrix4x4 m_mLoc;
	Float m_fRadius;
	Float m_fWidth;
	Float m_fSuspension_Length;
	Float m_fSuspension_Ks;
	Float m_fSuspension_Kd;
	bool m_bDrive;
	bool m_bSteer;
	bool m_bBrake;
	palVehicle *m_pVehicle;
};

class palVehicle : public palFactoryObject {
public:
	/* Initialise the vehicle
	\param chassis A palBody that represents the chassis shape
	\param MotorForce The maximum force the motor can exert
	\param BrakeForce The maximum force the brakes can exert
	*/
	virtual void Init(palBody *chassis, Float MotorForce, Float BrakeForce) {
		m_pbChassis=chassis;
		m_fMotorForce=MotorForce;
		m_fBrakeForce=BrakeForce;
	}
	/* Returns the vehicles (chassis) location and orientation in world coordinates
	*/
	virtual palMatrix4x4& GetLocationMatrix() {
		return m_pbChassis->GetLocationMatrix();
	}
	/*  Adds a wheel to this vehicle
	*/
	virtual palWheel* AddWheel() = 0;
	/* Finalizes the initialisation of the vehicle. After this point no modifications to the vehicle can be made.
	*/
	virtual void Finalize() = 0;
	/*   Sets the vehicles steering direction, acceleration, and whether the brakes are active.
	\param steering a percentage indicating the steering angle (1.0 -> -1.0)
	\param acceleration a percentage indicating the acceleration (1.0 -> -1.0)
	\param brakes sets the brakes as presently inactive or active
	*/
	virtual void Control(Float steering, Float acceleration, bool brakes) = 0;
	/* Updates the vehicles state.  This function should be called for each iteration of the physics system
	*/
	virtual void Update() = 0;

	unsigned int GetNumWheels() {
		return (unsigned int) m_vWheels.size();
	}
	palWheel *GetWheel(unsigned int i) {
		if (i>m_vWheels.size())
			return 0;
		return m_vWheels[i];
	}
	palBody *m_pbChassis;
	Float m_fMotorForce;
	Float m_fBrakeForce;
	VECTOR<palWheel *> m_vWheels;
};
#endif
