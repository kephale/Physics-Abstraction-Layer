#ifndef PALACTUATORS_H
#define PALACTUATORS_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/** \file palActuators.h
	\brief
		PAL - Physics Abstraction Layer. 
		Actuators (Motors)
		
	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.261 : 28/09/06 - Cleanup
		Version 0.26  : 04/12/04 - New propeller model, new hydrofoil model, liquid drag class, documentation
		Version 0.25  : 14/09/04 - Impulse actuator
		Version 0.24  : 05/09/04 - Added PID, documentation, cleaned a bit, basic spring class.
		Version 0.23  : 01/09/04 - Added generic DC motor - removed dynamechs motor, documentation
		Version 0.22  : 11/08/04 - Added DC motor from dynamechs
		Version 0.21  : 08/08/04 - Bugfix
		Version 0.2   : 25/07/04 - Propellor integrated, need to make palforceactuaor private, but maintain public pfo
		Version 0.1   : 06/07/04 - Creation
	</pre>
	\todo
		- BUG: palForceActuator does NOT work
		- abstract some how : body actuators VS link actuators?
		- confirm force actuator : do i need to translate the realtive position variable?
		- correct actuator type setting
		- clean laplace - possibly remove
		- fix propelor model to accept parameters
*/

#include <vector>
#include <deque>
#include "pal.h"

typedef enum {
	PAL_ACTUATOR_NONE = 0,
	PAL_ACTUATOR_FORCE = 1,
	PAL_ACTUATOR_DCMOTOR = 2,
	PAL_ACTUATOR_SERVO = 3 
} palActuatorType;

/** The base actuator class.
*/
class palActuator : public palFactoryObject {
public:
	/** Ensures the actuator operates for the current time step
	*/
	virtual void Apply() = 0;
	palActuatorType m_Type;
//protected:
};

/** A generic angular actuator.
	Uses the engine-specific capabilities to achieve a target velocity.
*/
class palAngularMotor :  public palActuator {
public:
	virtual void Init(palRevoluteLink *pLink, Float Max) {
		m_link = pLink;
		m_fMax = Max;
	};
	virtual void Update(Float targetVelocity) = 0;
	palRevoluteLink *GetLink() {
		return m_link;
	}
protected:
	Float m_fMax;
	palRevoluteLink *m_link;
};

/** A "force" actuator.
	Applys a force to a given body at a given position in a given direction.
*/
class palForceActuator : public palActuator {
public:
	palForceActuator();
	virtual void Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetForce(Float force);
	virtual void Apply();

	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;

	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;

	Float m_fForce;

	palMatrix4x4 m_BodyOriginal;
	palBody *m_pBody;

	FACTORY_CLASS(palForceActuator,palForceActuator,*,1);
};

/** A "impulse" actuator.
	Applys a impulse to a given body at a given position in a given direction.
	In classical physics, momentum is related to velocity by \f$p=m.v\f$ where p is the Momentum, m is the Mass, and v is the Velocity.
	An impulse is simply a change in momentum.
*/
class palImpulseActuator : public palActuator {
public:
	palImpulseActuator();
	/** Initializes the actuator
	\param pbody The body to connect the actuator to
	\param px The x position of the actuator's center
	\param py The y position of the actuator's center
	\param pz The z position of the actuator's center
	\param axis_x The axis vector which supplies the direction of the actuator's impulse. (x)
	\param axis_y The axis vector which supplies the direction of the actuator's impulse. (y)
	\param axis_z The axis vector which supplies the direction of the actuator's impulse. (z)
	*/
	virtual void Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z);
	/** Sets the impulse.
	*/
	virtual void SetImpulse(Float impulse) {
		m_fImpulse = impulse;
	}
	virtual void Apply();
protected:
	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;

	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;

	Float m_fImpulse;

	palMatrix4x4 m_BodyOriginal;
	palBody *m_pBody;

	FACTORY_CLASS(palImpulseActuator,palImpulseActuator,*,1);
};


/* Laplace transform class.
	Finds the laplace transform of a quadratic.
*/
class Laplace {
public:
	Laplace(Float delta_t) {
		m_delta_t=delta_t;
		m_LastTime = 0.0f;
	}
	bool InitQuadratic(Float num, Float de_a, Float de_b, Float de_c);
	Float GetOutput(Float time, Float input);
protected:
	Float m_delta_t;
	Float m_LastTime;
	std::vector<Float> m_Impulse;
	std::deque<Float> m_Inputs;
	void QuadraticRoots(Float a, Float b, Float c, Float &r1, Float &r1i,Float &r2, Float &r2i);
};
class PID {
public:
	/** Initializes the PID controller.
	\param Kp The Proportional gain constant.
	\param Ki The Integral gain constant.
	\param Kd The Derivative gain constant.
	\param MinOut The minimum output value for the PID controller.
	\param MaxOut The maximum output value for the PID controller.
	\param MinInt The minimum integrated value for the PID controller.
	\param MaxInt The maximum integrated value for the PID controller.
	*/
	void Init(Float Kp, Float Ki, Float Kd, Float MinOut = -100, Float MaxOut = 100, Float MinInt = -100, Float MaxInt = 100) {
		m_Kp_gain = Kp;
		m_Ki_gain = Ki;
		m_Kd_gain = Kd;
		m_min=MinOut;
		m_max=MaxOut;
		m_i_min = MinInt;
		m_i_max = MaxInt;
		m_last_error=0;
		m_integral=0;
		
	}
	/** Updates the PID controller.
	\param desired The desired signal value.
	\param actual The current actual signal value.
	\param dt The change in time since the last execution
	*/
	Float Update(Float desired, Float actual, Float dt);

	
	/** Updates the PID controller.
	\param error The difference between the desired signal value and the actual signal value.
	\param dt The change in time since the last execution
	*/
	Float Update(Float error, Float dt);
private:
	Float m_last_error;
	Float m_integral;

	Float m_min,m_max;
	Float m_i_min,m_i_max;

	Float m_Kp_gain;
	Float m_Ki_gain;
	Float m_Kd_gain;
};


/** A propeller.
	Simulates a propeller and a corresponding DC motor for an underwater body.

	Based on a 1st order feed foward model:
	\f[i_m = \frac{1}{k_t}.\alpha_1.T_r\f]

  Where:
	\f$i_m\f$ is the motor drive current
	\f$k_t\f$ is the motor torque constant
	\f$\alpha_1\f$ is an experimentally determined constant (N.m/N)
	\f$T_r\f$ is the thrust

	The actual implementation is:
	\f[T = \alpha_{lumped}.V(t)\f]
	Where
	\f$\alpha_{lumped}\f$ is a lumped parameter
*/

class palPropeller : public palImpulseActuator {
public:
	palPropeller() {
		m_Voltage=0;
		m_a_l=0;
	}
	/** Initializes the propeller
	\param pbody The body to connect the actuator to
	\param px The x position of the propeller's center
	\param py The y position of the propeller's center
	\param pz The z position of the propeller's center
	\param axis_x The axis vector which supplies the direction of the propeller's thrust. (x)
	\param axis_y The axis vector which supplies the direction of the propeller's thrust. (y)
	\param axis_z The axis vector which supplies the direction of the propeller's thrust. (z)
	\param a_l The lumped parameter (alpha)
	*/
	virtual void Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z, Float a_l);
	/** Set the input voltage
	*/
	virtual void SetVoltage(Float voltage) {
		m_Voltage=voltage;
	}
	virtual void Apply();
protected:
	Float m_Voltage;
	Float m_a_l;

	FACTORY_CLASS(palPropeller,palPropeller,*,1);
};
//Preliminary experiments in Model-Based Thruster Control for Underwater Vehicle Positioning, Whitcomb, Yoerger

/** A DC Motor model for a Revolute Joint.
	Standard armature controlled DC motor model.
	\f[ T_A(t) = K_T\frac{V_a(t)-K_b.\omega_n(t)}{R_a} \f]
	Where,
		\f$T_A(t)\f$ is the torque.
		\f$K_T\f$ is the motor torque constant (Nm/A)
		\f$V_a(t)\f$ is the applied armature voltage (V)
		\f$K_b\f$ is the motor back emf constant (Vs/rad)
		\f$\omega_n(t)\f$ is the angular velocity of the motor (rad/s)		
		\f$R_a\f$ is the armature resistance. (ohms)
*/
class palDCMotor : public palActuator {
public:
	palDCMotor() {
		m_pRLink=NULL;
	}
	/** Initializes the motor.
	None of the parameters should be set to zero.
	\param revolute_link The revolute link the motor is attached to.
	\param torque_constant \f$K_T\f$, the motor torque constant (Nm/A)
	\param back_EMF_constant \f$K_b\f$, the motor back emf constant (Vs/rad)
	\param armature_resistance \f$R_a\f$, the armature resistance. (ohms)
	*/
	void Init(palRevoluteLink *revolute_link, 
		Float torque_constant,
        Float back_EMF_constant,
		Float armature_resistance) {

		m_torque_constant = torque_constant;
		m_back_EMF_constant = back_EMF_constant;
		m_armature_resistance = armature_resistance;		
	
		m_pRLink=revolute_link;
		m_Voltage=0;
		m_last_torque=0;
	}
	/** Set the input voltage
	\param voltage Applied armature voltage
	*/
	void SetVoltage(Float voltage) {
		m_Voltage=voltage;
	}

	void Apply() {
		Float torque =  m_torque_constant*(m_Voltage -
                        m_pRLink->GetAngularVelocity()*m_back_EMF_constant)/m_armature_resistance;	
		m_pRLink->ApplyTorque(torque);
		//printf("%f %f\n",torque,m_last_torque);
		m_last_torque=torque;
	}

	Float m_last_torque;

	Float m_Voltage;

	Float m_torque_constant;
	Float m_back_EMF_constant;
	Float m_armature_resistance;

	palRevoluteLink *m_pRLink;

private:
	FACTORY_CLASS(palDCMotor,palDCMotor,*,1);
};


/** A spring.
	A spring exerts a force on two connected bodies, according to Hook's law.
	\f[ f_a = -k_s (|L| - R) + k_d \frac{\dot L \bullet L}{|L|} \frac{L}{|L|} \f]
	\f[ f_b = -f_a \f]
	The above equations represent the force that is applied to two bodies (a and b). 
	Where \f$k_s\f$ is the spring constant. ("stiffness")
		\f$k_d\f$ is the damping constant.
		\f$R\f$ is the resting length of the spring.
		\f$L\f$ is the distance between the two bodies.
		and \f$\dot L\f$ is the difference between the two bodies velocities.
*/
class palSpring : public palActuator {
public:
	palSpring() {
		m_pBody1=NULL;
		m_pBody2=NULL;
		mRestLen=0;
		mKs=0;
		mKd=0;
//		memset(&last_force,0,sizeof(palVector3));
	}

	/** Initializes the spring.
	\param pb1 The body to connect the spring to (1)
	\param pb2 The body to connect the spring to (2)
	\param rest_length The resting length of the spring.
	\param Ks The spring constant.
	\param Kd The damping constant.
	*/
	void Init(palBody *pb1,palBody *pb2, float rest_length, float Ks, float Kd) {
		m_pBody1=pb1;
		m_pBody2=pb2;
		mRestLen=rest_length;
		mKs=Ks;
		mKd=Kd;
	}

	//f=-kx (hookes law)
	void Apply();
protected:
	palBody *m_pBody1;
	palBody *m_pBody2;

	float mRestLen;
	float mKs;
	float mKd;

private:
	FACTORY_CLASS(palSpring,palSpring,*,1);
};

/** Simulates drag through a liquid on a body.
	Drag is calculated from the drag equation:
	\f[D=\frac{1}{2}.\rho.C_D.A_f.V^2\f]
	Where,
	\f$C_D\f$ is the drag coefficient
	\f$\rho \f$ is the fluid density.
	\f$A_f \f$ is the frontal area.
	L is the lift force.
	V is the realtive velocity.
*/
class palLiquidDrag : public palActuator {
public:
	palLiquidDrag() {};
	/** Initializes the liquid drag
	\param pbody The body to which the drag is applied.
	\param area The frontal area of the body to which the drag is applied
	\param CD The drag coefficient
	\param density The fluid density
	*/
	void Init(palBody *pbody, Float area, Float CD, Float density=0.99829f);
	void Apply();
protected:
	palBody *m_pBody;
	Float m_density;
	Float m_CD;
	Float m_area;

	FACTORY_CLASS(palLiquidDrag,palLiquidDrag,*,1);
};


/** A Hydrofoil.
	Simulates the lift from a fin for an underwater body.

	Based on the lift equation:
	\f[L=\frac{1}{2}.\rho.C_L.A_f.V^2\f]
	Where,
	\f$C_L=a\alpha^2+b\alpha+c\f$ is the lift coefficient
	\f$\alpha \f$ is the angle of attack.
	\f$\rho \f$ is the fluid density. (water is approx. 1)
	\f$A_f \f$ is the frontal area.
	L is the lift force.
	V is the realtive velocity.
	a,b,c are experimentally determined.
	*/
class palHydrofoil: public palImpulseActuator {
public:
	palHydrofoil() {
	}
	/** Initializes the hydrofoil.
	\param pbody The body to connect the actuator to
	\param px The x position of the actuator's center
	\param py The y position of the actuator's center
	\param pz The z position of the actuator's center
	\param o_axis_x The orientation axis of the hydrofoil (x)
	\param o_axis_y The orientation axis of the hydrofoil (y)
	\param o_axis_z The orientation axis of the hydrofoil (z)
	\param lift_axis_x The direction in which the lift force is applied (x)
	\param lift_axis_y The direction in which the lift force is applied (y)
	\param lift_axis_z The direction in which the lift force is applied (z)
	\param Af The frontal area of the hydrofoil.
	\param a The quadratic term of the lift coefficient
	\param b The linear term of the lift coefficient
	\param c The constant term of the lift coefficient
	\param density The density of the liquid.
	*/
	virtual void Init(palBody *pbody, Float px, Float py, Float pz, Float o_axis_x, Float o_axis_y, Float o_axis_z, Float lift_axis_x, Float lift_axis_y, Float lift_axis_z, Float Af, Float a, Float b, Float c, Float density = 0.99829);
	
	/** Sets the angle of attack for the hydrofoil
	\param alpha The angle of attack, must be in the range of -pi/2 to +pi/2
	*/
	void SetAngle(Float alpha)  //alpha -pi/2 to pi/2
	{
		m_alpha=alpha;
	}
	
	virtual void Apply(); 

protected:
	Float m_row;
	Float m_alpha;
	Float m_Af;
	Float m_CL_a;
	Float m_CL_b;
	Float m_CL_c;
	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;	

	FACTORY_CLASS(palHydrofoil,palHydrofoil,*,1);
};


//temporary solution to allow faked buoyancy
class palFakeBuoyancy : public palActuator {
public:
	palFakeBuoyancy() {};

	void Init(palBody *pbody, Float density=998.29f) {
		m_pBody=pbody;
		m_density=density;
	}
	void Apply();
protected:
	palBody *m_pBody;
	Float m_density;

	FACTORY_CLASS(palFakeBuoyancy,palFakeBuoyancy,*,1);
};

#endif
