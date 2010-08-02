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
		Version 0.1.01: 05/09/08 - Doxygen documentation
		Version 0.1   : 17/08/07 - Split from test case
	</pre>
	\todo
		- Improve documentation.
		- Motor, Gears, etc.
*/

#include <pal/palMath.h>
#include <pal/palBase.h>
#include <pal/palBodies.h>

class palVehicle;

/** Initializes a wheel.
\param x
\param y
\param z
\param radius
\param width The width of the wheel
\param suspension_rest_length
\param suspension_Ks
\param suspension_Kd
\param suspension_Travel
\param Fr
\param powered
\param steering
\param breaks
*/
struct palWheelInfo {
	palWheelInfo()
	: m_fPosX(0.0)
	, m_fPosY(0.0)
	, m_fPosZ(0.0)
	, m_fRadius(0.5)
	, m_fWidth(0.2)
	, m_fSuspension_Rest_Length(0.21)
	, m_fSuspension_Ks(25000.0)
	, m_fSuspension_Kd(25000.0)
	, m_fSuspension_Travel(0.2)
	, m_fFriction_Slip(0.1)
	, m_fRoll_Influence(1.0)
	, m_bDrive(false)
	, m_bSteer(false)
	, m_bBrake(true)
	{
	}

	Float m_fPosX; //< The x-coordinate of the wheel (relative to car center)
	Float m_fPosY; //< The y-coordinate of the wheel (relative to car center)
	Float m_fPosZ; //< The z-coordinate of the wheel (relative to car center)
	Float m_fRadius; //< The radius of the wheel
	Float m_fWidth; //< The width of the wheel
	Float m_fSuspension_Rest_Length; //< The resting length of the suspension spring
	Float m_fSuspension_Ks; //< The spring constant for the suspension in Newtons/Meter
	Float m_fSuspension_Kd; //< The dampening constant for the suspension in Newtons/Meters/second
	Float m_fSuspension_Travel; //< The max distance the suspension may travel in meters
	Float m_fFriction_Slip; //< The conversion of wheel load to lateral force the wheel can apply.
	Float m_fRoll_Influence; //< Scalar from the height of the center of mass to the ground where 1.0 is on the ground, for where the roll center is.
	bool m_bDrive; //< Flag indicating whether this wheel is powered (driven) by the motor
	bool m_bSteer; //< Flag indicating whether this wheel is affected by steering
	bool m_bBrake; //< Flag indicating whether this wheel is affected by breaking
};

/** The wheel class.
This represents a wheel.
*/
class palWheel {
public:
	virtual void Init(const palWheelInfo& wheelInfo) {
		m_WheelInfo = wheelInfo;
	}
	/* Returns wheel location and orientation in world coordinates
	*/
	virtual palMatrix4x4& GetLocationMatrix() = 0;

	palVehicle *m_pVehicle;
	palWheelInfo m_WheelInfo;
	palMatrix4x4 m_mLoc;
};

/** The vehicle class.
This represents a vehicle. The vehicle contains a number of wheels.
*/
class palVehicle : public palFactoryObject {
public:

   virtual ~palVehicle()
   {
      for (unsigned i = 0; i < m_vWheels.size(); ++i)
      {
         delete m_vWheels[i];
      }
      m_vWheels.clear();
   }
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
	virtual const palMatrix4x4& GetLocationMatrix() {
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

	/*   Sets the vehicles steering direction, acceleration, and whether the brakes are active.
	\param steering a percentage indicating the steering angle (1.0 -> -1.0)
	\param acceleration The acceleration force.
	\param brakes The new breaking force
	 */
	virtual void ForceControl(Float steering, Float acceleration, Float brakes) = 0;

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
	PAL_VECTOR<palWheel *> m_vWheels;
};
#endif
