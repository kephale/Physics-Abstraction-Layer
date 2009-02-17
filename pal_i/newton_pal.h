#ifndef NEWTON_PAL_H
#define NEWTON_PAL_H

#define NEWTON_PAL_SDK_VERSION_MAJOR 0
#define NEWTON_PAL_SDK_VERSION_MINOR 1
#define NEWTON_PAL_SDK_VERSION_BUGFIX 70

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Newton implementation.
		This enables the use of newton via PAL.
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.1.71: 21/01/09 - Added contact information for collision system
		Version 0.1.70: 30/10/08 - Impulse newton workaround
		Version 0.1.69: 07/10/08 - Raycast body bugfix
		Version 0.1.68: 30/09/08 - PAL Versioning
		Version 0.1.67: 22/09/08 - Raycasting for collision subsystem
		Version 0.1.66: 13/07/08 - Compound body finalize mass & inertia method
		Version 0.1.65: 04/05/08 - Static box, compound body [todo: cleanup bugfix, other static geom support]
		Version 0.1.64: 10/04/07 - Angular Motor
		Version 0.1.63: 12/12/07 - Generalised terrain
		Version 0.1.62: 20/11/07 - PSD fix.
		Version 0.1.61: 19/10/07 - Version Detection
		Version 0.1.6 : 18/08/07 - Convex geom & body
		Version 0.1.54: 25/07/07 - Orientated plane
		Version 0.1.53: 15/07/07 - Body sleep
		Version 0.1.52: 22/06/07 - Body set linear & angular velocity 
		Version 0.1.51: 23/02/05 - Cylinder fix
		Version 0.1.5 : 18/02/05 - Destructor callbacks
		Version 0.1.42: 11/02/05 - Bugfix PSD rotation
		Version 0.1.41: 20/12/04 - Bugfix hinge angle
		Version 0.1.4 : 04/12/04 - API 1.3 integration, bugfixes, cylinder and cylinder geometry 
		Version 0.1.31: 13/09/04 - Angular impulse
		Version 0.1.3 : 05/09/04 - Impulse
		Version 0.1.22: 01/09/04 - Revolute link angles from newton
		Version 0.1.21: 19/08/04 - Geometry fix
		Version 0.1.2 : 29/07/04 - Materials update, geometry update
		Version 0.1.13: 08/07/04 - Bugfix compound object, unfreeze body, force actuator
		Version 0.1.12: 06/07/04 - Add force and torque
		Version 0.1.1 : 05/07/04 - Geometry support, compound object
		Version 0.1.0 : 24/06/04 - Body-velocities, redid materials, contact sensor
		Version 0.0.95: 23/06/04 - Experimental branch - set get force/torque, psd sensor
		Version 0.0.94: 22/06/04 - Basic spherical joint limit support, revolut link limit
		Version 0.0.93: 15/06/04 - Prismatic link
		Version 0.0.92: 13/06/04 - terrain mesh, terrain heightmap
		Version 0.0.9 : 12/06/04 - terrain plane
		Version 0.0.81: 09/06/04 - Floor fix
		Version 0.0.8 : 07/06/04 - sphere, link, spherical link, revolute link, started cylinder
		Version 0.0.5 : 06/06/04 - Physics, body, box, started sphere, started material 
	TODO:
		-ALSO COMPLETE SPHERICAL LIMITS
		-Collision subsystem contact points
		-get to 1.0 (ie: same as pal.h)
		-1.+ make set position work correctly (ie: allowing reset)
	notes:
*/

#include "../pal/pal.h"
#include "../pal/palFactory.h"
#include "../pal/palCollision.h"

#include <Newton.h>
#if defined(_MSC_VER)
#pragma comment(lib,"Newton.lib")
#pragma warning(disable : 4250)
#endif

/*
class palNewtonMaterial : public palMaterial {
public:
	palNewtonMaterial();
	void Init(Float static_friction, Float kinetic_friction, Float restitution);
protected:
	FACTORY_CLASS(palNewtonMaterial,palMaterial,Newton,1);
	int m_groupID;
};
*/
//	void AddBuoyancyForce(float fluidDensity, float fluidLinearViscosity, float fluidAngularViscosity, float *plane = NULL);

class palNewtonMaterialUnique : public palMaterialUnique {
public:
	palNewtonMaterialUnique();
	void Init(const PAL_STRING & name,Float static_friction, Float kinetic_friction, Float restitution); 
	
	int m_GroupID;
protected:
	FACTORY_CLASS(palNewtonMaterialUnique,palMaterialUnique,Newton,2);
};

class palNewtonMaterialInteraction : public palMaterialInteraction  {
public:
	palNewtonMaterialInteraction();
	void Init(palMaterialUnique *pM1, palMaterialUnique *pM2, Float static_friction, Float kinetic_friction, Float restitution); 
protected:
	FACTORY_CLASS(palNewtonMaterialInteraction,palMaterialInteraction,Newton,2);
};

class palNewtonPhysics: public palPhysics, public palCollisionDetection {
public:
	palNewtonPhysics();
	void Init(Float gravity_x, Float gravity_y, Float gravity_z);
//	void SetDefaultMaterial(palMaterial *pmat);
//	void SetGroundPlane(bool enabled, Float size);
	void Cleanup();
	const char* GetPALVersion();
	const char* GetVersion();

	//colision detection functionality
	virtual void SetCollisionAccuracy(Float fAccuracy);
	virtual void SetGroupCollision(palGroup a, palGroup b, bool enabled);
	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit);
	virtual void NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled);
	virtual void NotifyCollision(palBodyBase *pBody, bool enabled);
	virtual void GetContacts(palBodyBase *pBody, palContact& contact);
	virtual void GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact);

	//extra methods provided by newton abilities:
	void InitWater(Float fluidDensity, Float fluidLinearViscosity, Float fluidAngularViscosity, Float plane_a = 0, Float plane_b = 1, Float plane_c = 0, Float plane_d = 0);
	NewtonWorld* GetNewtonWorld();
protected:
	void Iterate(Float timestep);
	FACTORY_CLASS(palNewtonPhysics,palPhysics,Newton,1)
};

class palNewtonBody;

typedef struct {
	bool set_force;
	bool set_torque;
	float force[3];
	float torque[3];
	bool add_force;
	bool add_torque;
	float aforce[3];
	float atorque[3];
	palGroup groupID;
	palNewtonBody *pb;
} palNewtonBodyData; 

class palNewtonBody : virtual public palBody {
public:
	palNewtonBody();
	~palNewtonBody();

	virtual void SetPosition(palMatrix4x4& location);
	virtual palMatrix4x4& GetLocationMatrix();
#if 0
	virtual void SetForce(Float fx, Float fy, Float fz);
	virtual void GetForce(palVector3& force);
	virtual void AddForce(Float fx, Float fy, Float fz);
	virtual void AddTorque(Float tx, Float ty, Float tz);
	virtual void SetTorque(Float tx, Float ty, Float tz);
	virtual void GetTorque(palVector3& torque);
#endif
	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);
	
	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

	virtual void SetActive(bool active);

	virtual void SetGroup(palGroup group);
	virtual void SetMaterial(palMaterial *material);
//protected:
	NewtonBody *m_pntnBody;
	bool static_body;
protected:

	
	void BuildBody(Float fx, Float fy, Float fz);

	palNewtonBodyData m_callbackdata;

};

class palNewtonGeometry : virtual public palGeometry {
public:
	//virtual palMatrix4x4& GetLocationMatrix(); //unfinished!
	~palNewtonGeometry();
	NewtonCollision *m_pntnCollision;
};

class palNewtonBoxGeometry : public palNewtonGeometry, public palBoxGeometry  {
public:
	palNewtonBoxGeometry();
	void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
protected:
	FACTORY_CLASS(palNewtonBoxGeometry,palBoxGeometry,Newton,1)
};

class palNewtonSphereGeometry : public palSphereGeometry , public palNewtonGeometry {
public:
	palNewtonSphereGeometry();
	void Init(palMatrix4x4 &pos, Float radius, Float mass);
protected:
	FACTORY_CLASS(palNewtonSphereGeometry,palSphereGeometry,Newton,1)
};

class palNewtonCylinderGeometry : public palCapsuleGeometry , public palNewtonGeometry {
public:
	palNewtonCylinderGeometry();
	void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
protected:
	FACTORY_CLASS(palNewtonCylinderGeometry,palCapsuleGeometry,Newton,1)
};

class palNewtonBox : public palBox, public palNewtonBody {
public:
	palNewtonBox();

	//void SetPosition(Float x, Float y, Float z) {palNewtonBody::SetPosition(x,y,z);}; //duplicate to ensure dominance
	//palMatrix4x4& GetLocationMatrix() {return palNewtonBody::GetLocationMatrix();}

	void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
//	using void palNewtonBody::SetPosition;
//	using void palBox::GenericInit(void *param, ...); //mmm internal compiler errors
//	void impGenericInit(void *param, va_list arg_ptr) {
//		palBox::GenericInit(param,arg_ptr);
//	}
	//extra methods provided by newton abilities:
	void SetMass(Float mass);
protected:
	FACTORY_CLASS(palNewtonBox,palBox,Newton,1)
};

class palNewtonStaticBox : virtual public palStaticBox, virtual public palNewtonBody {
public:
	palNewtonStaticBox();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth);
	virtual palMatrix4x4& GetLocationMatrix() {
		return m_mLoc;
	}
protected:
	FACTORY_CLASS(palNewtonStaticBox,palStaticBox,Newton,1)
};

class palNewtonSphere : public palSphere, public palNewtonBody {
public:
	palNewtonSphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);
//	Float m_fRadius;
	//extra methods provided by newton abilities:
	void SetMass(Float mass);
	//void SetRadius(Float radius);
protected:
	FACTORY_CLASS(palNewtonSphere,palSphere,Newton,1)
};

class palNewtonCompoundBody : public palCompoundBody, public palNewtonBody {
public:
	palNewtonCompoundBody();
	virtual void Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ);
protected:
	FACTORY_CLASS(palNewtonCompoundBody,palCompoundBody,Newton,1)
};

class palNewtonStaticCompoundBody : public palStaticCompoundBody, public palNewtonBody {
public:
	palNewtonStaticCompoundBody();
	void Finalize();
	virtual palMatrix4x4& GetLocationMatrix() {
		return m_mLoc;
	}
protected:
	FACTORY_CLASS(palNewtonStaticCompoundBody,palStaticCompoundBody,Newton,1)
};

/*
class palCompoundBody : virtual public palBody {
public:
	virtual void Init(Float x, Float y, Float z);

	virtual palSphereGeometry *AddSphere();
	virtual palBoxGeometry *AddBox();
	virtual palCapsuleGeometry *AddCylinder();
	virtual void Finalize() = 0;*/

class palNewtonCylinder : public palCapsule, public palNewtonBody {
public:
	palNewtonCylinder();
	void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
//	virtual void SetPosition(palMatrix4x4& location);
//	virtual palMatrix4x4& GetLocationMatrix();
	//extra methods provided by newton abilities:
	void SetMass(Float mass);
protected:
	FACTORY_CLASS(palNewtonCylinder,palCapsule,Newton,1)
};

typedef struct {
	bool limit_enabled;
	Float limit_lower;
	Float limit_upper;

	Float motor_velocity;
	Float motor_force;

	Float data1,data2; //joint data (case: hinge, angle, omega)
} palNewtonLinkData;

class palNewtonLink : virtual public palLink {
public:
	palNewtonLink();
protected:
	palNewtonLinkData m_callbackdata;
	NewtonJoint* m_pntnJoint;
};

class palNewtonSphericalLink : public palSphericalLink, public palNewtonLink {
public:
	palNewtonSphericalLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);

	void SetLimits(Float cone_limit_rad, Float twist_limit_rad);
	//void SetLimits(Float lower_limit_rad, Float upper_limit_rad); 
	//void SetTwistLimits(Float lower_limit_rad, Float upper_limit_rad);
protected:
	FACTORY_CLASS(palNewtonSphericalLink,palSphericalLink,Newton,1)
};

class palNewtonRevoluteLink: public palRevoluteLink, public palNewtonLink {
public:
	friend class palNewtonAngularMotor;
	palNewtonRevoluteLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	void SetLimits(Float lower_limit_rad, Float upper_limit_rad); 

	Float GetAngle(); 
	Float GetAngularVelocity(); 

protected:
	FACTORY_CLASS(palNewtonRevoluteLink,palRevoluteLink,Newton,1)
};


class palNewtonPrismaticLink:  public palPrismaticLink, public palNewtonLink {
public:
	palNewtonPrismaticLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z); 
protected:
	FACTORY_CLASS(palNewtonPrismaticLink,palPrismaticLink,Newton,1)
};

class palNewtonPSDSensor : public palPSDSensor {
public:
	palNewtonPSDSensor();
	void Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range); //position, direction
	Float GetDistance();
protected:
	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;
	FACTORY_CLASS(palNewtonPSDSensor,palPSDSensor,Newton,1)
};

class palNewtonContactSensor: public palContactSensor {
public:
	palNewtonContactSensor();
	void Init(palBody *body); //location and size?
	void GetContactPosition(palVector3& contact);
	palVector3 m_Contact;
protected:
	FACTORY_CLASS(palNewtonContactSensor,palContactSensor,Newton,1);
};

class palNewtonTerrain : virtual public palTerrain {
public:
	palNewtonTerrain();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material);
protected:
	NewtonBody *m_pntnBody;
};

class palNewtonTerrainPlane : public palTerrainPlane, public palNewtonTerrain {
public:
	palNewtonTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
protected:
	FACTORY_CLASS(palNewtonTerrainPlane,palTerrainPlane,Newton,1)
};

class palNewtonOrientatedTerrainPlane :  public palOrientatedTerrainPlane, public palNewtonTerrain {
public:
	palNewtonOrientatedTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);
	virtual palMatrix4x4& GetLocationMatrix();
protected:
	FACTORY_CLASS(palNewtonOrientatedTerrainPlane,palOrientatedTerrainPlane,Newton,1)
};

class palNewtonTerrainMesh :  virtual public palTerrainMesh, public palNewtonTerrain {
public:
	palNewtonTerrainMesh();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
protected:
	FACTORY_CLASS(palNewtonTerrainMesh,palTerrainMesh,Newton,1)
};

class palNewtonTerrainHeightmap : virtual public palTerrainHeightmap, private palNewtonTerrainMesh {
public:
	palNewtonTerrainHeightmap();
	virtual void Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap);
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material);
protected:
	FACTORY_CLASS(palNewtonTerrainHeightmap,palTerrainHeightmap,Newton,1)
};

class palNewtonForceActuator : public palForceActuator {
public:
	palNewtonForceActuator();
	virtual void Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z);
	FACTORY_CLASS(palNewtonForceActuator,palForceActuator,Newton,2);
};

////////////

class palNewtonConvexGeometry : public palNewtonGeometry, public palConvexGeometry  {
public:
	palNewtonConvexGeometry();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palNewtonConvexGeometry,palConvexGeometry,Newton,1)
};


class palNewtonConvex : public palNewtonBody, public palConvex {
public:
	palNewtonConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palNewtonConvex,palConvex,Newton,1)
};

class palNewtonAngularMotor : public palAngularMotor {
public:
	palNewtonAngularMotor();
	virtual void Init(palRevoluteLink *pLink, Float Max);
	virtual void Update(Float targetVelocity);
	virtual void Apply();
protected:
	palNewtonRevoluteLink *m_pnrl;
	FACTORY_CLASS(palNewtonAngularMotor,palAngularMotor,Newton,1)
};

#ifdef STATIC_CALLHACK
extern void pal_newton_call_me_hack();
#endif

#endif