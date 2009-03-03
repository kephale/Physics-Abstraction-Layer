#ifndef HAVOK_PAL_H
#define HAVOK_PAL_H
//(c) Adrian Boeing 2008, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Havok implementation.
		This enables the use of Havok via PAL.
	Author:
		Adrian Boeing
	Revision History:
	Version 0.0.1: 02/07/08 - Initial: physics, bodybase, body, geom, spheregeom, sphere, staticsphere, boxgeom, box, staticbox, terrainplane
	Todo:
		-capsules,convex,compounds
		-materials
		-body impulse/torque,etc.
		-sphere link limits (?)
		-solver accuracy
		-solver query
		-spe testing/hardware accel
*/

#if 1
#include <Common/hkAnimationPhysicsPublicInclude.h>
#else

// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/hkThreadMemory.h>
#include <Common/Base/Memory/Memory/Pool/hkPoolMemory.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Monitor/hkMonitorStream.h>

// Dynamics includes
#include <Physics/Collide/hkpCollide.h>
#include <Physics/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>


#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

#include <Physics/Utilities/Thread/Multithreading/hkpMultithreadingUtil.h>

// Keycode
#include <Common/Base/keycode.cxx>

#define HK_CLASSES_FILE <Common/Serialize/ClassList/hkPhysicsClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

// Generate a custom list to trim memory requirements
#define HK_COMPAT_FILE <Common/Compat/hkCompatVersions.h>
#include <Common/Compat/hkCompat_None.cxx>
#endif

#include "../pal/palFactory.h"
#include "../pal/palSolver.h"

#if defined (OS_WINDOWS)
#include <windows.h>
#endif

#if defined(_MSC_VER)
#pragma comment( lib, "hkBase.lib")
#pragma comment( lib, "hkSerialize.lib")
#pragma comment( lib, "hkSceneData.lib")
#pragma comment( lib, "hkVisualize.lib")
#pragma comment( lib, "hkCompat.lib")
#pragma comment( lib, "hkpCollide.lib")
#pragma comment( lib, "hkpConstraintSolver.lib")
#pragma comment( lib, "hkpDynamics.lib")
#pragma comment( lib, "hkpInternal.lib")
#pragma comment( lib, "hkpUtilities.lib")
#pragma comment( lib, "hkpVehicle.lib")
#pragma warning(disable : 4250)
#endif

class palHavokPhysics: public palPhysics, public palSolver {
public:
	palHavokPhysics();
	virtual void Init(Float gravity_x, Float gravity_y, Float gravity_z);
	virtual void Cleanup() {};
	const char* GetVersion() {return 0;};
	virtual void Iterate(Float timestep);

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

	bool set_use_hardware;
	int set_substeps;
	int set_pe;

	FACTORY_CLASS(palHavokPhysics,palPhysics,Havok,1)
};

class palHavokGeometry : virtual public palGeometry {
public:
	palHavokGeometry();
	~palHavokGeometry();

	//void GenericCreate();
	hkpShape* pShape;
	hkpMassProperties* pMassProp;
};

class palHavokBodyBase :virtual public palBodyBase {
public:
	palHavokBodyBase();
	~palHavokBodyBase();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetPosition(palMatrix4x4& location);
	virtual void SetMaterial(palMaterial *material) {};

	hkpRigidBody* pBody;
protected:
	hkpRigidBodyCinfo info;
	void BuildBody(Float x, Float y, Float z, Float mass, bool dynamic = true);
};

class palHavokBody :  virtual public palBody, virtual public palHavokBodyBase {
public:
	palHavokBody();
//	~palHavokBody();

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

	//@return if the body is active or sleeping
	virtual bool IsActive();
	virtual void SetActive(bool active);

	virtual void SetPosition(palMatrix4x4& location) {
		palHavokBodyBase::SetPosition(location);
	}
protected:
};



class palHavokBoxGeometry : public palHavokGeometry, public palBoxGeometry  {
public:
	palHavokBoxGeometry();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
protected:
	FACTORY_CLASS(palHavokBoxGeometry,palBoxGeometry,Havok,1)
};

class palHavokSphereGeometry : public palSphereGeometry , public palHavokGeometry {
public:
	palHavokSphereGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
protected:
	FACTORY_CLASS(palHavokSphereGeometry,palSphereGeometry,Havok,1)
};
/*
class palHavokCapsuleGeometry : public palCapsuleGeometry , public palHavokGeometry {
public:
	palHavokCapsuleGeometry() {};
protected:
	FACTORY_CLASS(palHavokCapsuleGeometry,palCapsuleGeometry,Havok,1)
};

class palHavokConvexGeometry : public palHavokGeometry, public palConvexGeometry  {
public:
	palHavokConvexGeometry();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palHavokConvexGeometry,palConvexGeometry,Havok,1)
};
*/
class palHavokBox : public palBox, public palHavokBody {
public:
	palHavokBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by Havok abilities:
protected:
	FACTORY_CLASS(palHavokBox,palBox,Havok,1)
};


class palHavokStaticBox : virtual public palStaticBox, virtual public palHavokBodyBase {
public:
	palHavokStaticBox();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth);
protected:
	FACTORY_CLASS(palHavokStaticBox,palStaticBox,Havok,1)
};

class palHavokSphere : public palSphere, public palHavokBody {
public:
	palHavokSphere();
	virtual void Init(Float x, Float y, Float z, Float radius, Float mass);
protected:
	FACTORY_CLASS(palHavokSphere,palSphere,Havok,1)
};

class palHavokStaticSphere : virtual public palStaticSphere, virtual public palHavokBodyBase {
public:
	palHavokStaticSphere();
	virtual void Init(palMatrix4x4 &pos, Float radius);
protected:
	FACTORY_CLASS(palHavokStaticSphere,palStaticSphere,Havok,1)
};



class palHavokSphericalLink : public palSphericalLink {
public:
	palHavokSphericalLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);
	virtual void SetLimits(Float cone_limit_rad, Float twist_limit_rad);
protected:
	FACTORY_CLASS(palHavokSphericalLink,palSphericalLink,Havok,1)
};

class palHavokRevoluteLink: public palRevoluteLink {
public:
	palHavokRevoluteLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad);
protected:
	FACTORY_CLASS(palHavokRevoluteLink,palRevoluteLink,Havok,1)
};

class palHavokPrismaticLink:  public palPrismaticLink {
public:
	palHavokPrismaticLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);

protected:
	FACTORY_CLASS(palHavokPrismaticLink,palPrismaticLink,Havok,1)
};




class palHavokTerrainPlane : public palTerrainPlane, public palHavokBodyBase {
public:
	palHavokTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
public:
	FACTORY_CLASS(palHavokTerrainPlane,palTerrainPlane,Havok,1)
};


#endif
