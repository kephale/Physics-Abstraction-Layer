#include "palActuators.h"
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. 
		Actuators (Motors)
		Implementation.
*/

FACTORY_CLASS_IMPLEMENTATION(palForceActuator);
FACTORY_CLASS_IMPLEMENTATION(palImpulseActuator);
FACTORY_CLASS_IMPLEMENTATION(palDCMotor);
FACTORY_CLASS_IMPLEMENTATION(palPropeller);
FACTORY_CLASS_IMPLEMENTATION(palLiquidDrag);
FACTORY_CLASS_IMPLEMENTATION(palHydrofoil);
FACTORY_CLASS_IMPLEMENTATION(palSpring);
FACTORY_CLASS_IMPLEMENTATION(palFakeBuoyancy);

/*
class palForceActuator : public palActuator {
public:
	virtual void Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetForce(Float force);

	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;

	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;

	Float m_fForce;

	palMatrix4x4 m_BodyOriginal;

	palBody *m_pBody;
};
*/

palForceActuator::palForceActuator(){
	m_Type=PAL_ACTUATOR_FORCE;
	m_pBody = NULL;
}

void palForceActuator::Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z) {
	m_pBody = pbody;
	m_BodyOriginal=m_pBody->GetLocationMatrix();
	m_fRelativePosX = px - m_BodyOriginal._41;
	m_fRelativePosY = py - m_BodyOriginal._42;
	m_fRelativePosZ = pz - m_BodyOriginal._43;

	m_fAxisX=axis_x;
	m_fAxisY=axis_y;
	m_fAxisZ=axis_z;
	m_fForce=0;
}

void palForceActuator::SetForce(Float force) {
	m_fForce=force;
}

void palForceActuator::Apply() {
	palMatrix4x4 m,resp;
	mat_identity(&m);
	mat_translate(&m,m_fRelativePosX,m_fRelativePosY,m_fRelativePosZ);
	palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();
	mat_multiply(&resp,&m,&bodypos); //the acting force points resulting position.
	palVector3 tpos,respos;
	tpos.x=resp._41;	tpos.y=resp._42;	tpos.z=resp._43;
	vec_mat_mul(&respos,&bodypos,&tpos);
	//printf("body is at      : %f %f %f\n",bodypos._41,bodypos._42,bodypos._43);
	//printf("forcepoint is at: %f %f %f\n",respos.x,respos.y,respos.z);
	palVector3 axis,resaxis;
	axis.x=m_fAxisX;	axis.y=m_fAxisY;	axis.z=m_fAxisZ;
	//printf("axis was: %f %f %f\n",axis.x,axis.y,axis.z);
	vec_mat_mul(&resaxis,&bodypos,&axis);
	//printf("axis is:  %f %f %f\n",resaxis.x,resaxis.y,resaxis.z);
	//printf("forcepoint is at: %f %f %f\n",resp._41,resp._42,resp._43);
	//m_pBody->AddForceAtPosition(respos.x,respos.y,respos.z,resaxis.x*m_fForce,resaxis.y*m_fForce,resaxis.z*m_fForce);
	//m_pBody->AddForceAtPosition(respos.x,respos.y,respos.z,axis.x*m_fForce,axis.y*m_fForce,axis.z*m_fForce);
	m_pBody->ApplyForceAtPosition(tpos.x,tpos.y,tpos.z,axis.x*m_fForce,axis.y*m_fForce,axis.z*m_fForce);
	//m_pBody->AddForceAtPosition(1,1,0,axis.x*m_fForce,axis.y*m_fForce,axis.z*m_fForce);
	
}


palImpulseActuator::palImpulseActuator() {
	m_pBody = NULL;
}

void palImpulseActuator::Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z)
 {
		m_pBody = pbody;

		m_BodyOriginal=m_pBody->GetLocationMatrix();
		m_fRelativePosX = px - m_BodyOriginal._41;
		m_fRelativePosY = py - m_BodyOriginal._42;
		m_fRelativePosZ = pz - m_BodyOriginal._43;
				
		m_fAxisX=axis_x;
		m_fAxisY=axis_y;
		m_fAxisZ=axis_z;
		
		m_fImpulse = 0;
	}


void palImpulseActuator::Apply()
{
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,m_fRelativePosX,m_fRelativePosY,m_fRelativePosZ);
	//printf("rel:%f %f %f  ",m._41,m._42,m._43);
	palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();
	

	palMatrix4x4 out;

	mat_multiply(&out,&bodypos,&m);
	palVector3 newpos;
	newpos.x=out._41;
	newpos.y=out._42;
	newpos.z=out._43;
//	printf("output : %f %f %f ",out._41,out._42,out._43);

//	imp_pos=out;
	
	mat_identity(&m);
	mat_translate(&m,m_fAxisX,m_fAxisY,m_fAxisZ);
	mat_multiply(&out,&bodypos,&m);

//	printf("output : %f %f %f\n",out._41,out._42,out._43);

//	imp_axis=out;

	palVector3 newaxis;
	newaxis.x=out._41-bodypos._41;
	newaxis.y=out._42-bodypos._42;
	newaxis.z=out._43-bodypos._43;
	vec_norm(&newaxis);

	m_pBody->ApplyImpulseAtPosition(newpos.x,newpos.y,newpos.z,newaxis.x*m_fImpulse,newaxis.y*m_fImpulse,newaxis.z*m_fImpulse);


	}


void palHydrofoil::Init(palBody *pbody, Float px, Float py, Float pz, Float o_axis_x, Float o_axis_y, Float o_axis_z, Float lift_axis_x, Float lift_axis_y, Float lift_axis_z, Float Af, Float a, Float b, Float c, Float density) {
	m_fAxisX=o_axis_x;
	m_fAxisY=o_axis_y;
	m_fAxisZ=o_axis_z;
	m_row=density;
	m_alpha=0;
	m_Af=Af;
	m_CL_a=a;
	m_CL_b=b;
	m_CL_c=c;
	palImpulseActuator::Init(pbody,px,py,pz,lift_axis_x,lift_axis_y,lift_axis_z);
}

void palHydrofoil::Apply() {
			palMatrix4x4 m;
			palMatrix4x4 out;
			palVector3 V;
			m_pBody->GetLinearVelocity(V);
			palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();

			mat_identity(&m);
			mat_translate(&m,m_fAxisX,m_fAxisY,m_fAxisZ);
			mat_multiply(&out,&bodypos,&m); //out is now the facing of the sub

			palVector3 uvw;
			uvw.x=out._41-bodypos._41;
			uvw.y=out._42-bodypos._42;
			uvw.z=out._43-bodypos._43;
			vec_norm(&uvw);
			vec_vec_mul(&uvw,&uvw,&V); //now have uvw for velocity fowards

			Float alpha;
			
			/*alpha = atan2(uvw.y,uvw.x);//arctan(w/u);
			alpha=Clamp(alpha);			
			printf("ca:%5.3f ",alpha);*/
			alpha=m_alpha;


			Float lift = Float(0.5) * m_row * (alpha*alpha*m_CL_a+alpha*m_CL_b+m_CL_c)* m_Af * uvw.x*uvw.x;
			//		printf("{%f,%f (%f %f %f)}",(alpha*alpha*m_CL_a+alpha*m_CL_b+m_CL_c),Float(0.5) * m_row * m_Af,m_CL_a,m_CL_b,m_CL_c);

//			if (alpha<0) lift = -lift;

//			printf("lift%5.3f :%f, %f\n",lift,uvw.x,alpha);
			SetImpulse(lift);
			palImpulseActuator::Apply();
	}
				

#if 0
palPropellor::palPropellor() {
	m_last_voltage = 0;
	l = new Laplace(0.1f);
}

void palPropellor::Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z, Float Ct, Float J, Float b, Float K, Float R, Float L) {
	palPropellor::Ct=Ct;
	l->InitQuadratic(K, J*L, (b*L)+(J*R), (b*R)+(K*K)); //the motors transfer function
	palImpulseActuator::Init(pbody,px,py,pz,axis_x,axis_y,axis_z);
}


void palPropellor::Update(Float voltage) {
	Float temp;
	palPhysics *p=dynamic_cast<palPhysics *>(m_pParent);
	if (p) {
		Float current_time = p->GetCurrentTime();
		temp=l->GetOutput(current_time,voltage); //find the output from the laplace transform of our motor transfer function
		Float thrust;
		thrust =  Ct * temp * fabs(temp); //use yoergers model to calculate propellor thrust
		printf("thrust:%f (%f)\n",thrust,temp);
		SetImpulse(thrust); 
		palImpulseActuator::Apply();//apply the force
	} else {
		printf("no physics!\n");
	}
}
#endif

/*
toruqe = K_T * i(armature) //equation
and 
V=IR
ie:
Vbattery=Imotor.Rmotor+VbackEMF
ie:
i(armature)=(v(applied)-v(back))/r(armature)
ie
\f[ T_A(t) = K_T\frac{V_a(t)-K_b.\omega_n(t)}{R_a} \f]
*/

Float CalcSphereForce(Float radius,Float hpos,Float density, Float volume = -1) {
	hpos += radius;
	if (hpos < radius*2) {
		Float hunder;
		hunder = radius*2 - hpos;
		if (hunder > radius * 2)
			hunder = radius*2;

		if (volume < 0)
			volume =  (Float)M_PI * hunder * (3 * radius * hunder - hunder*hunder) / 3.0f;
		Float gravity = 9.8f;
		Float fb = volume * gravity * density;
		return fb;
	}
	return 0;
}

void palFakeBuoyancy::Apply() {
		palMatrix4x4 m;
		m=m_pBody->GetLocationMatrix();
		palBoxGeometry *pbg;
		pbg = dynamic_cast<palBoxGeometry *>(m_pBody->m_Geometries[0]);
		if (pbg ){
			/*
			float box_height = pbg->m_fHeight;
			if (m._42 < box_height) {
				float hunder = box_height - m._42; //how much of the box's height is under water?
				if (hunder > box_height) hunder = box_height; //maximum is box height

				float volume = pbg->m_fWidth*pbg->m_fDepth * hunder;

				float gravity = 9.8f;

				float fb = volume * gravity * m_density;

				m_pBody->AddForce(0,fb,0);
			}
			*/
			Float volume = pbg->m_fWidth*pbg->m_fDepth * pbg->m_fHeight;
			Float quatervolume = volume/4;
			Float quatersphereradius = static_cast<Float>( pow((3/4.0)*quatervolume / M_PI,1/3.0) );

			Float r = quatersphereradius;
			//r = pbg->m_fHeight

			palMatrix4x4 m;
			palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();
			palMatrix4x4 out;

			mat_identity(&m);
			mat_translate(&m,pbg->m_fWidth*0.5f,0,0);
			mat_multiply(&out,&bodypos,&m);
			m_pBody->ApplyForceAtPosition(out._41,out._42,out._43,0,
				CalcSphereForce(r,out._42,m_density),0);
//					CalcSphereForce(pbg->m_fHeight,out._42,m_density,volume*0.5f),0);

			mat_identity(&m);
			mat_translate(&m,-pbg->m_fWidth*0.5f,0,0);
			mat_multiply(&out,&bodypos,&m);
			m_pBody->ApplyForceAtPosition(out._41,out._42,out._43,0,
					CalcSphereForce(r,out._42,m_density),0);
//					CalcSphereForce(pbg->m_fHeight,out._42,m_density,volume*0.5f),0);

			//the following are the z-axis floats:
			mat_identity(&m);
			mat_translate(&m,0,0,pbg->m_fDepth*0.5f);
			mat_multiply(&out,&bodypos,&m);
			m_pBody->ApplyForceAtPosition(out._41,out._42,out._43,0,
				CalcSphereForce(r,out._42,m_density),0);

			mat_identity(&m);
			mat_translate(&m,0,0,-pbg->m_fDepth*0.5f);
			mat_multiply(&out,&bodypos,&m);
			m_pBody->ApplyForceAtPosition(out._41,out._42,out._43,0,
				CalcSphereForce(r,out._42,m_density),0);

		} 
		palSphereGeometry *psg;
		psg = dynamic_cast<palSphereGeometry *>(m_pBody->m_Geometries[0]);
		if (psg) {
			m_pBody->ApplyForce(0,
				CalcSphereForce(psg->m_fRadius,m._42,m_density),0);
			/*
			float sphere_radius = psg->m_fRadius;
			if (m._42 < sphere_radius*2) {
				float hunder = sphere_radius*2 - m._42; //how much of the box's height is under water?
				if (hunder > sphere_radius*2) hunder = sphere_radius*2; //maximum is box height
				//float volume = sphere_radius*sphere_radius * hunder; //wrong wrong wrong.

				float volume =  M_PI * hunder * (3 * sphere_radius * hunder - hunder*hunder) / 3.0f;

				float gravity = 9.8f;
				float fb = volume * gravity * m_density;
				m_pBody->AddForce(0,fb,0);
			}
			*/
		}
	}

void palLiquidDrag::Init(palBody *pbody, Float area, Float CD, Float density) {
		m_pBody=pbody;
		m_density=density;
		m_CD=CD;
		m_area=area;
	}

void palLiquidDrag::Apply() {
		palVector3 vout,V;
		//linear drag
		m_pBody->GetLinearVelocity(V);
		Float drag = Float(0.5) * m_density * m_CD * m_area * vec_mag(&V) * vec_mag(&V);
		vec_const_mul(&vout,&V,drag);
		m_pBody->ApplyImpulse(-vout.x,-vout.y,-vout.z);
		//angular drag
		m_pBody->GetAngularVelocity(V);
		drag = Float(0.5) * m_density * m_CD * m_area * vec_mag(&V) * vec_mag(&V);
		vec_const_mul(&vout,&V,drag);
		m_pBody->ApplyAngularImpulse(-vout.x,-vout.y,-vout.z);
	}
/*
void palLiquidDrag::Apply(){
		palVector3 vout,V;
		m_pBody->GetLinearVelocity(V);
		Float drag = Float(0.5) * m_density * m_CD * m_area * vec_mag(&V) * vec_mag(&V);
		vec_const_mul(&vout,&V,drag);
		m_pBody->ApplyImpulse(-vout.x,-vout.y,-vout.z);
	}
*/


const Float laplace_epsilon = 0.000001f;

/* Laplace transform class.
	Finds the laplace transform of a quadratic.
*/
bool Laplace::InitQuadratic(Float num, Float de_a, Float de_b, Float de_c) {
		Float r1,r2,r1i,r2i;
		//first, lets find the roots of the denomenator
		QuadraticRoots(de_a,de_b,de_c,r1,r1i,r2,r2i);
		if ((r1i>0) || (r2i>0)) {
			printf("cant handle complex\n");
			return false;
		}
		if (r1==r2) {
			//TODO: handle squares
			printf("cant handle square\n");
			return false;
		}
		Float alpha= -r1;
		Float beta = -r2;
		Float alpha_i = -r1i;
		Float beta_i = -r2i;

		//calculate impulse response (perform the laplace transform)
		int done_flag=0;
		Float t=0;
		m_Impulse.clear();
		while (done_flag!=2) {
			t+=m_delta_t;
		//	Float y = ( 1/(beta - alpha) ) * ( exp(-alpha*t) -exp(-beta*t) ) * (num/de_a); //impulse
//			if ((done_flag == 0) && (y>0)) done_flag = 1;
//			if ((done_flag == 1) && (y< laplace_epsilon)) done_flag = 2;
			Float y =(( 1/(beta * alpha) ) + ( exp(-alpha*t) / (alpha*(alpha-beta)) ) + ( exp(-beta*t) / (beta *(beta- alpha)))) * (num/de_a);  //step
			if (m_Impulse.size()>0)
			if (fabs(m_Impulse[m_Impulse.size()-1] - y)< laplace_epsilon)
				done_flag = 2;
//			printf("y(%f):%f\n",t,y);
			m_Impulse.push_back(y);
		}
		m_Inputs.clear();
		m_Inputs.resize(m_Impulse.size()); //create space for the inputs
	return true;
	}

Float Laplace::GetOutput(Float time, Float input) {
		if (time<m_LastTime) {
		//	ARG_ERROR("input time is less than a previous time step");
		}
		int stepold = (int)(m_LastTime/m_delta_t);
		int stepnew = (int)(time/m_delta_t);
		int steps = stepnew-stepold;//(Float)((time-m_LastTime)/m_delta_t);
//		printf("steps taken: %d [%f %f %f] [%f]\n",steps,time,m_LastTime,m_delta_t,(time-m_LastTime)/m_delta_t);
		if (steps>0) {
			for (int i=0;i<steps;i++) {
				m_Inputs.push_front(input);
				m_Inputs.pop_back();
			}
			m_LastTime = time;
		}
		unsigned int max;
		max = (unsigned int)(time/m_delta_t);
		//max++;
		if (max>m_Impulse.size()) max = (unsigned int)m_Impulse.size();
		Float out;
		out = 0;
		for (unsigned int i=0;i<max;i++) {
		//	printf("(%d)adding %f at power %f\n",i,m_Impulse[i],m_Inputs[i]);
			out+=m_Impulse[i] * m_Inputs[i];
			if (i>0) {
				out-=m_Impulse[i-1] * m_Inputs[i-1];
			}
		}
		return out;
	}

void Laplace::QuadraticRoots(Float a, Float b, Float c, Float &r1, Float &r1i,Float &r2, Float &r2i) {
		r1=r2=r1i=r2i=0;
		//-b+-sqrt(b2 - 4ac) / 2a
		Float d = b*b - 4*a*c;
		if (d<0) {
			//results are imaginary
			r1 = -b / (2*a);
			r2 = -b / (2*a);
			r1i=  sqrt(-d) / (2*a);
			r2i= -sqrt(-d) / (2*a)	;
		} else {
			//results are real
			r1 = (-b + sqrt(d))/ (2*a);
			r2 = (-b - sqrt(d))/ (2*a);
		}
}
Float PID::Update(Float error, Float dt) {
			//integrate
			m_integral += error;
			//clamp integrator
			if (m_integral>m_i_max)
				m_integral = m_i_max;
			if (m_integral< m_i_min)
				m_integral = m_i_min;
			//derivative
			Float derivative = (error-m_last_error);
			Float out = m_Kp_gain * error + m_Kd_gain * derivative/dt + m_Ki_gain * m_integral*dt;
			if (out>m_max)
				out = m_max;
			if (out< m_min)
				out= m_min;
			m_last_error = error;
			return out;
}
Float PID::Update(Float desired, Float actual, Float dt) {
			Float error = desired-actual;
			return Update(error,dt);
}

void palPropeller::Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z, Float a_l) {
		m_a_l=a_l;	
		palImpulseActuator::Init(pbody,px,py,pz,axis_x,axis_y,axis_z);
 }


void palPropeller::Apply() {

palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,m_fRelativePosX,m_fRelativePosY,m_fRelativePosZ);
	//printf("rel:%f %f %f  ",m._41,m._42,m._43);
	palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();
	

	palMatrix4x4 out;

	mat_multiply(&out,&bodypos,&m);
	palVector3 newpos;
	newpos.x=out._41;
	newpos.y=out._42;
	newpos.z=out._43;
if (newpos.y > 0.1f ) return;

		Float thrust = m_Voltage*m_a_l;
		SetImpulse(thrust);
		palImpulseActuator::Apply();
	}

void palSpring::Apply() {
		palVector3 p1,p2;
		m_pBody1->GetPosition(p1);
		m_pBody2->GetPosition(p2);
		palVector3 deltaP,deltaV;

		Float dist;
		vec_sub(&deltaP,&p1,&p2);
		dist = vec_mag(&deltaP); //get the distance between the positions
				
		palVector3 p1v,p2v;
		Float HTerm,DTerm;
		HTerm = (dist - mRestLen) * mKs; //calc: Ks * (dist - R )

		m_pBody1->GetLinearVelocity(p1v);
		m_pBody2->GetLinearVelocity(p2v);
		//deltaV = p1->mVelocity - p2->mVelocity;
		vec_sub(&deltaV,&p1v,&p2v); //get the velocitiy delta
		
		DTerm = vec_dot(&deltaP,&deltaV) * mKd / dist; 

		//hey! check this code! is it right? -- kd term not correct?
		palVector3 springForce;	
		vec_const_mul(&springForce,&deltaP,1.0f/dist); 
		//springForce = deltaP * 1.0f/dist;
		//springForce = springForce * -(HTerm + DTerm);
		vec_const_mul(&springForce,&springForce,-(HTerm + DTerm));

		m_pBody1->ApplyImpulse(springForce.x,springForce.y,springForce.z);
		m_pBody2->ApplyImpulse(-springForce.x,-springForce.y,-springForce.z);
/*
		vec_sub(&springForce,&springForce,&last_force);
		m_pBody1->AddForce(springForce.x,springForce.y,springForce.z);
		m_pBody2->AddForce(-springForce.x,-springForce.y,-springForce.z);
		//p1->mForce = p1->mForce + springForce;
		//p2->mForce = p2->mForce - springForce;

		last_force = springForce;
		*/
	}
