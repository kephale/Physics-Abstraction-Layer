#ifndef NOVODEX_PAL_H
#define NOVODEX_PAL_H

#define NOVODEX_PAL_SDK_VERSION_MAJOR 0
#define NOVODEX_PAL_SDK_VERSION_MINOR 1
#define NOVODEX_PAL_SDK_VERSION_BUGFIX 1
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. NovodeX/PhysX implementation.
		This enables the use of NovodeX via PAL.
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.1.01: 14/10/08 - Generic link init bugfix
		Version 0.1.0 : 30/09/08 - PAL Versioning
		Version 0.0.76: 24/09/08 - Static convex body
		Version 0.0.75: 13/07/08 - Compound body finalize mass & inertia method
		Version 0.0.74: 08/07/08 - Fixed fluid twoway definition bug
		Version 0.0.73: 05/07/08 - Collision detection system support, Solver system support, nVidia support (PhysX 2.8.1)
		Version 0.0.72: 26/05/08 - Collision group support
		Version 0.0.71: 04/05/08 - Joint static bugfix, static compound body
		Version 0.0.7 : 18/01/07 - PSD Sensor
		Version 0.0.61: 17/01/08 - Plane bugfix, fluid #ifdef
		Version 0.0.6 : 28/12/07 - Static box, sphere, and capsule
		Version 0.0.51: 19/10/07 - Version number request
		Version 0.0.5 : 18/10/07 - Generic 6DOF constraint
		Version 0.0.4 : 18/08/07 - Convex geom and body
		Version 0.0.34: 25/07/07 - Orientated plane
		Version 0.0.33: 15/07/07 - Body sleep
		Version 0.0.32: 22/06/07 - Set linear & angular velocity
		Version 0.0.31: 16/11/06 - Fixed terrain mesh generation
		Version 0.0.3 : 11/11/06 - AGEIA support (PhysX 2.6.2)
		Version 0.0.2 : 18/02/05 - Cleanup
		Version 0.0.14: 16/09/04 - Impulse - testcode
		Version 0.0.13: 19/08/04 - Geometry update, compound body
		Version 0.0.12: 16/08/04 - Link limits, prismatic link, terrain mesh & heightmap
		Version 0.0.11: 15/08/04 - Cylinder geom&body, body - set&get forces, torques, velocities, link - revolute, spherical
		Version 0.0.1 : 12/08/04 - Physics, TerrainPlane, basic:Geom, Box Geom, Sphere Geom, Box, Sphere
	TODO:
		- collision/solver accuracy levels
		- FIX joints with static bodies
		- FIX convex geom and body bug
		- FIX the force & torque code!
		- do i need joint description info?
		- errors for plane
		- correct set position for terrains
		- Setup abstraction!
*/
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "../pal/pal.h"
#include "../pal/palFactory.h"
#include "../pal/palFluid.h"
#include "../pal/palCollision.h"
#include "../pal/palSolver.h"

#if !defined(PAL_DISABLE_FLUID)
#define NOVODEX_ENABLE_FLUID
#endif

#include <NxPhysics.h>
#if defined(_MSC_VER)
//#pragma comment(lib, "NxFoundation.lib")
//#pragma comment(lib, "NxPhysics.lib")
#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "NxCooking.lib")
#pragma warning(disable : 4250) //dominance
#endif

class palNovodexMaterialUnique : public palMaterialUnique {
public:
	palNovodexMaterialUnique();
	void Init(const PAL_STRING & name,Float static_friction, Float kinetic_friction, Float restitution); 


	NxMaterialDesc	m_MaterialDesc;
	NxMaterial*	m_pMaterial;
	NxMaterialIndex m_Index;
protected:
	FACTORY_CLASS(palNovodexMaterialUnique,palMaterialUnique,Novodex,2);
};

class palNovodexPhysics: public palPhysics, public palCollisionDetection, public palSolver {
public:
	palNovodexPhysics();
	void Init(Float gravity_x, Float gravity_y, Float gravity_z);
	void Cleanup();

	const char* GetPALVersion();
	const char* GetVersion();
	//Novodex specific:
	NxScene* GetScene();
	NxPhysicsSDK* GetPhysicsSDK();


	//colision detection functionality
	virtual void SetCollisionAccuracy(Float fAccuracy);
	virtual void SetGroupCollision(palGroup a, palGroup b, bool enabled);
	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit);
	virtual void NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled);
	virtual void NotifyCollision(palBodyBase *pBody, bool enabled);
	virtual void GetContacts(palBodyBase *pBody, palContact& contact);
	virtual void GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact);

	//solver functionality
	virtual void SetSolverAccuracy(Float fAccuracy);
	virtual void StartIterate(Float timestep);
	virtual bool QueryIterationComplete();
	virtual void WaitForIteration();
	virtual void SetPE(int n);
	virtual void SetSubsteps(int n);
	virtual void SetHardware(bool status);
	virtual bool GetHardware(void);
protected:
	void Iterate(Float timestep);

	//notification callbacks:
	//virtual void NotifyGeometryAdded(palGeometry* pGeom);
	//virtual void NotifyBodyAdded(palBodyBase* pBody);
//	PAL_MAP<NxShape* , palGeometry* > m_Shapes;
	FACTORY_CLASS(palNovodexPhysics,palPhysics,Novodex,1)
	
	bool set_use_hardware;
	int set_substeps;
	int set_pe;
};

class palNovodexGeometry : virtual public palGeometry {
public:
	palNovodexGeometry();
//	virtual palMatrix4x4& GetLocationMatrix(); //unfinished!
	
	NxShapeDesc *m_pShape;
protected:

};

class palNovodexBoxGeometry : public palNovodexGeometry, public palBoxGeometry  {
public:
	palNovodexBoxGeometry();
	~palNovodexBoxGeometry();
	void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass); //unfinished!

	NxBoxShapeDesc *m_pBoxShape;
protected:
	FACTORY_CLASS(palNovodexBoxGeometry,palBoxGeometry,Novodex,1)
};

class palNovodexBodyBase :virtual public palBodyBase {
public:
	palNovodexBodyBase();
	~palNovodexBodyBase();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetPosition(palMatrix4x4& location);
	virtual void SetMaterial(palMaterial *material);
	virtual void SetGroup(palGroup group);
	NxActor *m_Actor;
protected:
	NxBodyDesc m_BodyDesc;
	NxActorDesc m_ActorDesc;

protected:
	void BuildBody(Float mass, bool dynamic = true);
};

class palNovodexBody : virtual public palBody, virtual public palNovodexBodyBase {
public:
	palNovodexBody();
	~palNovodexBody();

	virtual void SetPosition(palMatrix4x4& location);
//	palMatrix4x4& GetLocationMatrix();
#if 0
	virtual void ApplyForce(Float fx, Float fy, Float fz);

	void SetForce(Float fx, Float fy, Float fz);
	void GetForce(palVector3& force);

	void SetTorque(Float tx, Float ty, Float tz);
	void GetTorque(palVector3& torque);

	void AddForce(Float fx, Float fy, Float fz);
	void AddTorque(Float tx, Float ty, Float tz);

#endif
	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetActive(bool active);

//	virtual void SetMaterial(palMaterial *material);


	//Additional Novodex abilities?
	//void SetMass(Float mass); //how to support compound bodies?
	
	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

};

class palNovodexBox : public palBox, public palNovodexBody {
public:
	palNovodexBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
protected:
	FACTORY_CLASS(palNovodexBox,palBox,Novodex,1)
};

class palNovodexStaticBox : virtual public palStaticBox, virtual public palNovodexBodyBase {
public:
	palNovodexStaticBox();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth);
protected:
	FACTORY_CLASS(palNovodexStaticBox,palStaticBox,Novodex,1)
};

class palNovodexSphereGeometry : public palNovodexGeometry, public palSphereGeometry  {
public:
	palNovodexSphereGeometry();
	~palNovodexSphereGeometry();
	void Init(palMatrix4x4 &pos, Float radius, Float mass);

	NxSphereShapeDesc *m_pSphereShape;
protected:
	FACTORY_CLASS(palNovodexSphereGeometry,palSphereGeometry,Novodex,1)
};

class palNovodexSphere : public palSphere, public palNovodexBody {
public:
	palNovodexSphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);

protected:
	FACTORY_CLASS(palNovodexSphere,palSphere,Novodex,1)
};

class palNovodexStaticSphere : virtual public palStaticSphere, virtual public palNovodexBodyBase {
public:
	palNovodexStaticSphere();
	virtual void Init(palMatrix4x4 &pos, Float radius);
protected:
	FACTORY_CLASS(palNovodexStaticSphere,palStaticSphere,Novodex,1)
};

class palNovodexCapsuleGeometry: public palCapsuleGeometry, public palNovodexGeometry {
public:
	palNovodexCapsuleGeometry();
	~palNovodexCapsuleGeometry();
	void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);

	NxCapsuleShapeDesc *m_pCapShape;
protected:
	FACTORY_CLASS(palNovodexCapsuleGeometry,palCapsuleGeometry,Novodex,1)
};

class palNovodexCapsule : public palCapsule, public palNovodexBody {
public:
	palNovodexCapsule();
	void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
protected:
	FACTORY_CLASS(palNovodexCapsule,palCapsule,Novodex,1)
};

class palNovodexStaticCapsule : public palStaticCapsule, public palNovodexBodyBase {
public:
	palNovodexStaticCapsule();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length);
	
protected:
	FACTORY_CLASS(palNovodexStaticCapsule,palStaticCapsule,Novodex,1)
};


class palNovodexConvexGeometry : public palNovodexGeometry, public palConvexGeometry  {
public:
	palNovodexConvexGeometry();
	~palNovodexConvexGeometry();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
	
	NxConvexMeshDesc  *m_pConvexMesh;
	NxConvexShapeDesc *m_pConvexShape;
protected:
	FACTORY_CLASS(palNovodexConvexGeometry,palConvexGeometry,Novodex,1)
};


class palNovodexConvex : public palNovodexBody, public palConvex {
public:
	palNovodexConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
protected:
	FACTORY_CLASS(palNovodexConvex,palConvex,Novodex,1)
};

class palNovodexStaticConvex : virtual public palStaticConvex, virtual public palNovodexBodyBase {
public:
	palNovodexStaticConvex();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices);
protected:
	FACTORY_CLASS(palNovodexStaticConvex,palStaticConvex,Novodex,1)
};

class palNovodexCompoundBody : public palCompoundBody, public palNovodexBody {
public:
	palNovodexCompoundBody();
	virtual void Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ);
protected:
	FACTORY_CLASS(palNovodexCompoundBody,palCompoundBody,Novodex,1)
};


class palNovodexStaticCompoundBody : public palStaticCompoundBody, public palNovodexBodyBase {
public:
	palNovodexStaticCompoundBody();
	void Finalize();
	virtual palMatrix4x4& GetLocationMatrix() {
		return palNovodexBodyBase::GetLocationMatrix();
	}
protected:
	FACTORY_CLASS(palNovodexStaticCompoundBody,palStaticCompoundBody,Novodex,1)
};

class palNovodexLink : virtual public palLink {
public:
	palNovodexLink();
	NxJointDesc *m_Jdesc;
	NxJoint *m_Joint;
protected:
	
};

class palNovodexRevoluteLink: public palRevoluteLink, public palNovodexLink {
public:
	palNovodexRevoluteLink();
	~palNovodexRevoluteLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	void SetLimits(Float lower_limit_rad, Float upper_limit_rad); 

//	Float GetAngle(); 
//	Float GetAngularVelocity(); 

	NxRevoluteJoint *m_RJoint;
	NxRevoluteJointDesc *m_RJdesc;
protected:
	FACTORY_CLASS(palNovodexRevoluteLink,palRevoluteLink,Novodex,1)
};

class palNovodexSphericalLink : public palSphericalLink, public palNovodexLink {
public:
	palNovodexSphericalLink();
	~palNovodexSphericalLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);

	void SetLimits(Float cone_limit_rad, Float twist_limit_rad);

	NxSphericalJoint  *m_SJoint;
	NxSphericalJointDesc *m_SJdesc;
protected:
	FACTORY_CLASS(palNovodexSphericalLink,palSphericalLink,Novodex,1)
};

class palNovodexPrismaticLink:  public palPrismaticLink, public palNovodexLink {
public:
	palNovodexPrismaticLink();
	~palNovodexPrismaticLink();

	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z); 

	NxPrismaticJoint *m_PJoint;
	NxPrismaticJointDesc *m_PJdesc;
protected:
	FACTORY_CLASS(palNovodexPrismaticLink,palPrismaticLink,Novodex,1)
};

class palNovodexGenericLink : public palGenericLink, public palNovodexLink {
public:
	palNovodexGenericLink();
	void Init(palBodyBase *parent, palBodyBase *child, palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
		palVector3 linearLowerLimits,
		palVector3 linearUpperLimits,
		palVector3 angularLowerLimits,
		palVector3 angularUpperLimits);
	NxD6Joint* m_DJoint;
	NxD6JointDesc *m_DJdesc;
protected:
	FACTORY_CLASS(palNovodexGenericLink,palGenericLink,Novodex,1)
};

class palNovodexTerrain : virtual public palTerrain {
public:
	palNovodexTerrain();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material);
	NxActor *m_Actor;
};

class palNovodexTerrainPlane : public palTerrainPlane, public palNovodexTerrain  {
public:
	palNovodexTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
	virtual void InitND(Float nx,Float ny, Float nz, Float d);
//	virtual palMatrix4x4& GetLocationMatrix();
//	virtual void SetMaterial(palMaterial *material);
protected:
	FACTORY_CLASS(palNovodexTerrainPlane,palTerrainPlane,Novodex,1)
};

class palNovodexOrientatedTerrainPlane :  public palOrientatedTerrainPlane, public palNovodexTerrain  {
public:
	palNovodexOrientatedTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);
	virtual palMatrix4x4& GetLocationMatrix() {return palOrientatedTerrainPlane::GetLocationMatrix();}
protected:
	FACTORY_CLASS(palNovodexOrientatedTerrainPlane,palOrientatedTerrainPlane,Novodex,1)
};

class palNovodexTerrainMesh :  virtual public palTerrainMesh, public palNovodexTerrain {
public:
	palNovodexTerrainMesh();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
//	virtual palMatrix4x4& GetLocationMatrix();
//	virtual void SetMaterial(palMaterial *material) {};
protected:
	FACTORY_CLASS(palNovodexTerrainMesh,palTerrainMesh,Novodex,1)
};

class palNovodexTerrainHeightmap : virtual public palTerrainHeightmap, private palNovodexTerrainMesh {
public:
	palNovodexTerrainHeightmap();
	virtual void Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap);
//	virtual palMatrix4x4& GetLocationMatrix();
//	virtual void SetMaterial(palMaterial *material) {};
protected:
	FACTORY_CLASS(palNovodexTerrainHeightmap,palTerrainHeightmap,Novodex,1)
};


class palNovodexPSDSensor : public palPSDSensor {
public:
	palNovodexPSDSensor();
	void Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range); //position, direction
	Float GetDistance();
protected:
	
	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;
	FACTORY_CLASS(palNovodexPSDSensor,palPSDSensor,Novodex,1)
};

class palNovodexContactSensor: public palContactSensor {
public:
	palNovodexContactSensor();
	void Init(palBody *body);
	void GetContactPosition(palVector3& contact);
	palVector3 m_Contact;
protected:
	FACTORY_CLASS(palNovodexContactSensor,palContactSensor,Novodex,1);
};


class palNovodexAngularMotor : public palAngularMotor {
public:
	palNovodexAngularMotor();
	virtual void Init(palRevoluteLink *pLink, Float Max);
	virtual void Update(Float targetVelocity);
	virtual void Apply();
protected:
	NxRevoluteJoint *m_j;
	FACTORY_CLASS(palNovodexAngularMotor,palAngularMotor,Novodex,1)
};


//extrastuff:

class palNovodexSpring  {
public:
	void Init(palBody *pb1,palBody *pb2, 
		Float x1, Float y1, Float z1,
		Float x2, Float y2, Float z2,
		Float rest_length, Float Ks, Float Kd);
protected:
	NxSpringAndDamperEffector *m_pSpring;
};



//////////////////

#ifdef NOVODEX_ENABLE_FLUID
class palNovodexFluid : public palSPHFluid {
public:
	palNovodexFluid();
	void Init();
	void AddParticle(Float x, Float y, Float z, Float vx, Float vy, Float vz);
	int GetNumParticles();
	palVector3* GetParticlePositions();
	void Finalize();

protected:
	palVector3 m_ppos;

	NxU32 ParticleBufferCap;
	NxU32 ParticleBufferNum;

	PAL_VECTOR<palVector3> pos;
	PAL_VECTOR<NxVec3> vParticles;
	NxFluid* fluid;

	FACTORY_CLASS(palNovodexFluid,palSPHFluid,Novodex,2)
};
#endif

#ifdef STATIC_CALLHACK
extern void pal_novodex_call_me_hack();
#endif

#endif