#ifndef IBDS_PAL_H
#define IBDS_PAL_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. IBDS implementation.
		This enables the use of IBDS via PAL.
	Author: 
		Adrian Boeing
	Revision History:
	Version 0.0.3 : 12/01/08 - version support, ibds 1.0.8
	Version 0.0.2 : 13/12/07 - collision support, sphere, spheregeom, materials
	Version 0.0.1 : 12/12/07 - prelim: physics, basebody, body, boxgeom, box, terrainplane
	TODO:
		-set object orientation!
		-set vel/get vel & assoc functions
		-convex shapes
	notes:
*/

#include "../pal/palFactory.h"
#include <DynamicSimulation/TimeManager.h>
#include <DynamicSimulation/Simulation.h>
#include <DynamicSimulation/RigidBody.h>
#include <DynamicSimulation/BallJoint.h>
#include <DynamicSimulation/SliderJoint.h>
#include <DynamicSimulation/HingeJoint.h>
#include <Math/SimMath.h>
#include <DynamicSimulation/MeshGeometry.h>

#if defined(_MSC_VER)
#ifndef NDEBUG
#pragma comment( lib, "libbulletcollision_d.lib")
#pragma comment( lib, "libbulletmath_d.lib")
#pragma comment( lib, "Dynamicsimulationd.lib")
#pragma comment( lib, "CollisionDetectiond.lib")
#pragma comment( lib, "Mathd.lib")
#pragma comment( lib, "qhulld.lib")
#else
#pragma comment( lib, "libbulletcollision.lib")
#pragma comment( lib, "libbulletmath.lib")
#pragma comment( lib, "Dynamicsimulation.lib")
#pragma comment( lib, "CollisionDetection.lib")
#pragma comment( lib, "Math.lib")
#pragma comment( lib, "qhull.lib")
#endif
#endif

class palIBDSPhysics: public palPhysics {
public:
	palIBDSPhysics();
	virtual void Init(Float gravity_x, Float gravity_y, Float gravity_z);
	virtual void Cleanup();
	const char* GetVersion();
	//extra methods provided by IBDS abilities:
protected:
	virtual void Iterate(Float timestep);
	FACTORY_CLASS(palIBDSPhysics,palPhysics,IBDS,1)
};

class palIBDSBodyBase : virtual public palBodyBase {
public:
	palIBDSBodyBase();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetPosition(palMatrix4x4& location);
	virtual void SetMaterial(palMaterial *material);
	IBDS::RigidBody *m_prb;
protected:
	void BuildBody(Float fx, Float fy, Float fz, Float mass, bool dynamic);
};


class palIBDSGeometry : virtual public palGeometry {
public:
	palIBDSGeometry();
	IBDS::Geometry* m_pGeom;
	virtual void Attach() = 0;
protected:
	void GenericAttach();
};

class palIBDSBoxGeometry : public palIBDSGeometry, public palBoxGeometry  {
public:
	palIBDSBoxGeometry();
	//virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
	virtual void Attach();
protected:
	FACTORY_CLASS(palIBDSBoxGeometry,palBoxGeometry,IBDS,1)
};


class palIBDSSphereGeometry : public palSphereGeometry , public palIBDSGeometry {
public:
	palIBDSSphereGeometry();
	//virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
	virtual void Attach();
protected:
	FACTORY_CLASS(palIBDSSphereGeometry,palSphereGeometry,IBDS,1)
};

class palIBDSCylinderGeometry : public palCapsuleGeometry , public palIBDSGeometry {
public:
	palIBDSCylinderGeometry();
	//virtual void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
	virtual void Attach();
protected:
	FACTORY_CLASS(palIBDSCylinderGeometry,palCapsuleGeometry,IBDS,1)
};

class palIBDSConvexGeometry : public palIBDSGeometry, public palConvexGeometry  {
public:
	palIBDSConvexGeometry();
	//virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
	virtual void Attach();
protected:
	FACTORY_CLASS(palIBDSConvexGeometry,palConvexGeometry,IBDS,1)
};



class palIBDSBody : virtual public palBody, virtual public palIBDSBodyBase {
public:
	palIBDSBody();

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

	virtual void SetActive(bool active);

	virtual void SetPosition(palMatrix4x4& location) {
		palIBDSBodyBase::SetPosition(location);
	}
protected:

};


class palIBDSCompoundBody : public palCompoundBody, public palIBDSBody {
public:
	palIBDSCompoundBody();
	virtual void Finalize();
protected:
	FACTORY_CLASS(palIBDSCompoundBody,palCompoundBody,IBDS,1)
};


class palIBDSCylinder : public palCapsule, public palIBDSBody {
public:
	palIBDSCylinder();
	virtual void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
	
protected:
	FACTORY_CLASS(palIBDSCylinder,palCapsule,IBDS,1)
};

class palIBDSConvex : public palIBDSBody, public palConvex {
public:
	palIBDSConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palIBDSConvex,palConvex,IBDS,1)
};

class palIBDSBox : public palBox, public palIBDSBody {
public:
	palIBDSBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by IBDS abilities:
protected:
	FACTORY_CLASS(palIBDSBox,palBox,IBDS,1)
};

class palIBDSSphere : public palSphere, public palIBDSBody {
public:
	palIBDSSphere();
	virtual void Init(Float x, Float y, Float z, Float radius, Float mass);
protected:
	FACTORY_CLASS(palIBDSSphere,palSphere,IBDS,1)
};

class palIBDSTerrainPlane : public palTerrainPlane, public  palIBDSBodyBase {
public:
	palIBDSTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
public:
	FACTORY_CLASS(palIBDSTerrainPlane,palTerrainPlane,IBDS,1)
};

class palIBDSSphericalLink : public palSphericalLink {
public:
	palIBDSSphericalLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);
	virtual void SetLimits(Float cone_limit_rad, Float twist_limit_rad) {};
protected:
	FACTORY_CLASS(palIBDSSphericalLink,palSphericalLink,IBDS,1)
};

class palIBDSRevoluteLink: public palRevoluteLink {
public:
	palIBDSRevoluteLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad); 
protected:
	FACTORY_CLASS(palIBDSRevoluteLink,palRevoluteLink,IBDS,1)
};

class palIBDSPrismaticLink:  public palPrismaticLink {
public:
	palIBDSPrismaticLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z); 
protected:
	FACTORY_CLASS(palIBDSPrismaticLink,palPrismaticLink,IBDS,1)
};



#endif