#ifndef PALEXTRAACTUATORS_H
#define PALEXTRAACTUATORS_H

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/** \file palExtraActuators.h
	\brief
		PAL - Physics Abstraction Layer. 
		Actuators (Motors)
		
	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.2   : 11/11/07 - This documentation
	</pre>
	\todo
*/


class palAngularMotorPID {
public:
	void Init(palRevoluteLink *pLink, Float Kp, Float Ki, Float Kd, Float MaxTorque, Float PID_IntegratorLimit=100) {
		m_link=pLink;
		m_pid.Init(Kp,Ki,Kd,-MaxTorque,MaxTorque,-PID_IntegratorLimit,PID_IntegratorLimit);
	}

	void Update(Float desiredAngle, Float dt) {
		Float out = m_pid.Update(diff_angle(desiredAngle,m_link->GetAngle()),dt);
		m_link->ApplyAngularImpulse(out);
	}
	palRevoluteLink *m_link;
	PID m_pid;
};

//the DC motor.
class palDMDCMotor : public palActuator {
public:
	palDMDCMotor() {
		m_pRLink=NULL;
	}
	void Init(palRevoluteLink *prl, Float torque_constant,
                                 Float back_EMF_constant,
                                 Float armature_resistance,
                                 Float rotor_inertia,
                                 Float coulomb_friction_constant,
                                 Float viscous_friction_constant,
                                 Float max_brush_drop,
                                 Float half_drop_value)
{
    m_torque_constant = torque_constant;
    m_back_EMF_constant = back_EMF_constant;
    m_armature_resistance = armature_resistance;
    m_rotor_inertia = rotor_inertia;

    m_coulomb_friction_constant = coulomb_friction_constant;
    m_viscous_friction_constant = viscous_friction_constant;

    m_max_brush_drop = max_brush_drop;
    m_half_drop_value = half_drop_value;

	m_pRLink=prl;
	m_Voltage=0;
}

	void SetVoltage(Float voltage) {
		m_Voltage=voltage;
	}

	void Apply() {
		//
			//we are just going to add torque, not reset it -> so we dont need external torque
		Float torque = computeTau(m_Voltage,0,m_pRLink->GetAngularVelocity());
		m_pRLink->ApplyAngularImpulse(torque);
	}

	Float computeTau(Float source_voltage,
                               Float external_torque,
                               Float joint_vel)
{
   Float speed_sign = sgn(joint_vel);

   if (source_voltage != 0.0)
   {
      // subtract brush drop.
      // May be made more efficient as "source_voltage" is the only
      // variable. Left as is for clarity.
      source_voltage -= sgn(source_voltage)*m_max_brush_drop *
         (1.0 - pow((Float)0.5, (fabs(source_voltage)/m_half_drop_value)));
   }

   // add torque developed internally by motor (Kt*i).
   Float torque = external_torque + m_torque_constant*(source_voltage -
                        joint_vel*m_back_EMF_constant)/m_armature_resistance;

   // subtract internal friction losses from torque.
   if (joint_vel == 0.0)
   {
      // coulomb friction only. (opposes torque)
      if (torque != 0.0)
      {
         if (m_coulomb_friction_constant > fabs(torque))
         {
            torque = 0.0;       // insufficient to overcome stiction.

         }
         else
         {
            torque -= sgn(torque)*m_coulomb_friction_constant;
            m_stiction_flag = false;
         }
      }
   }
   else
   {
      // friction opposes motorSpeed.
      torque -= speed_sign*m_coulomb_friction_constant +
                joint_vel*m_viscous_friction_constant;
      m_stiction_flag = false;
   }

   m_prev_vel = joint_vel;
   return torque;
}


private:
	palRevoluteLink *m_pRLink;
	Float sgn(Float x) {
		if (x < 0.0)
			return -1.0;
		else if (x > 0.0)
			return 1.0;
		else
			return 0.0;
	}
	Float m_Voltage;

	Float m_torque_constant;
	Float m_back_EMF_constant;
	Float m_armature_resistance;
	Float m_rotor_inertia;
	Float m_coulomb_friction_constant;
	Float m_viscous_friction_constant;
	Float m_max_brush_drop;
	Float m_half_drop_value;
protected:
	bool  m_stiction_flag; // Used to indicate stiction (zero vel. crossover).
	Float m_prev_vel;
private:
	FACTORY_CLASS(palDMDCMotor,palDMDCMotor,*,1);
};
#endif