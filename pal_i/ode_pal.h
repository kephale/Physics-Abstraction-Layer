#ifndef ODE_PAL_H
#define ODE_PAL_H

#define ODE_PAL_SDK_VERSION_MAJOR 0
#define ODE_PAL_SDK_VERSION_MINOR 1
#define ODE_PAL_SDK_VERSION_BUGFIX 8

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. ODE implementation.
		This enables the use of ODE via PAL.
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.1.08: 30/09/08 - PAL Version
		Version 0.1.07: 23/07/08 - Collision detection subsytem
		Version 0.1.06: 15/07/08 - Update for ODE 0.10.0 & dInitODE2 bugfix, staticbox deconstructor
		Version 0.1.05: 13/07/08 - Compound body finalize mass & inertia method
		Version 0.1.04: 26/05/08 - Collision group support
		Version 0.1.03: 04/05/08 - Static box, compound body, joints attach fix
		Version 0.1.02: 10/04/08 - ODE Joint, Angular Motor
		Version 0.1.01: 17/01/08 - Plane fix, TriMesh compatibility
		Version 0.1.0 : 13/01/08 - Capsule fix.
		Version 0.0.99: 19/11/07 - Include compound body.
		Version 0.0.98: 18/11/07 - Fix of static terrain, include convex bodys
		Version 0.0.97: 19/10/07 - Version number request
		Version 0.0.96: 25/07/07 - Orientated plane
		Versoin 0.0.95: 15/07/07 - Body sleep
		Versoin 0.0.94: 22/06/07 - Set angular & linear velocities
		Version 0.0.93: 22/12/04 - SetPosition update
		Version 0.0.92: 05/09/04 - Impulse
		Version 0.0.91: 12/08/04 - Revolute link add torque & get angle
		Version 0.0.9 : 04/08/04 - Materials
		Version 0.0.8 : 12/07/04 - Geometries (sphere,cylinder,box) & body updates, bug fixes
		Version 0.0.72: 21/06/04 - Revolute joint limits & spherical joint limits
		Version 0.0.71: 15/06/04 - started prismatic link (anchor fix?)
		Version 0.0.7 : 12/06/04 - added 'cleanup' code, no more memory leaks, terrain plane, started terrain heightmap
		Version 0.0.6 : 09/06/04 - sphere, cylinder, link, spherical link, revolute link
		Version 0.0.5 : 04/06/04 - physics, box
	TODO:
		-collision ray body and distance
		-collision notify between two specific bodies
		-fix cylinder inertia matrix calc => build from pal inertias?
	notes:
*/

#include "../pal/pal.h"
#include "../pal/palFactory.h"
#include "../pal/palCollision.h"

#include <ode/ode.h>

#if defined(_MSC_VER)
#pragma warning(disable : 4250)

#ifndef NDEBUG
#if defined(dSINGLE)
#pragma comment( lib, "ode_singled.lib" )
#else
#pragma comment( lib, "ode_doubled.lib" )
#endif
#else
#if defined(dSINGLE)
#pragma comment( lib, "ode_single.lib" )
#else
#pragma comment( lib, "ode_double.lib" )
#endif
#endif

#endif //_MSC_VER

#define ODE_MATINDEXLOOKUP int

class palODEMaterials : public palMaterials {
public:
	palODEMaterials();
// todo: fill this in! =P
// have a opdebodyid-> material name ? map (or index => better!)
	virtual void NewMaterial(PAL_STRING name, Float static_friction, Float kinetic_friction, Float restitution);
	static void InsertIndex(dGeomID odeGeom, palMaterial *mat);

	static palMaterial *GetODEMaterial(dGeomID odeGeomA,dGeomID odeGeomB);
	
protected:
	static ODE_MATINDEXLOOKUP* GetMaterialIndex(dGeomID odeGeom);
	virtual void SetIndex(int posx, int posy, palMaterial *pm);
	virtual void SetNameIndex(PAL_STRING name);

	static PAL_VECTOR<PAL_STRING> g_MaterialNames;
	static std_matrix<palMaterial *> g_Materials; 
	static PAL_MAP <dGeomID, ODE_MATINDEXLOOKUP> g_IndexMap; //or make this part of the global?  this is evil.
	FACTORY_CLASS(palODEMaterials,palMaterials,ODE,2);
};


class palODEPhysics: public palPhysics, public palCollisionDetection {
public:
	palODEPhysics();
	void Init(Float gravity_x, Float gravity_y, Float gravity_z);
	void SetGravity(Float gravity_x, Float gravity_y, Float gravity_z);


	//colision detection functionality
	virtual void SetCollisionAccuracy(Float fAccuracy);
	virtual void SetGroupCollision(palGroup a, palGroup b, bool enabled);
	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit);
	virtual void NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled);
	virtual void NotifyCollision(palBodyBase *pBody, bool enabled);
	virtual void GetContacts(palBodyBase *pBody, palContact& contact);
	virtual void GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact);


//	void SetDefaultMaterial(palMaterial *pmat);
//	void SetGroundPlane(bool enabled, Float size);
	const char* GetPALVersion();
	const char* GetVersion();
	dWorldID GetWorld();
	dSpaceID GetSpace();
	void Cleanup();

	
	
protected:
	void Iterate(Float timestep);
	FACTORY_CLASS(palODEPhysics,palPhysics,ODE,1)
};

class palODEBody : virtual public palBody {
public:
	palODEBody();
	~palODEBody();
	virtual void SetPosition(Float x, Float y, Float z);
	virtual void SetPosition(palMatrix4x4& location); 
#if 0
	virtual void SetForce(Float fx, Float fy, Float fz);
	virtual void GetForce(palVector3& force);

	virtual void AddForce(Float fx, Float fy, Float fz);
	virtual void AddTorque(Float tx, Float ty, Float tz);

	virtual void SetTorque(Float tx, Float ty, Float tz);
	virtual void GetTorque(palVector3& torque);

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float ix, Float iy, Float iz);
#endif

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

	virtual void SetActive(bool active);

	virtual void SetGroup(palGroup group);

	virtual void SetMaterial(palMaterial *material);
	//virtual void a() {};
	virtual palMatrix4x4& GetLocationMatrix();
//protected:
	dBodyID odeBody; // the ODE body
};

class palODEGeometry : virtual public palGeometry {
public:
	palODEGeometry();
	~palODEGeometry();
	virtual palMatrix4x4& GetLocationMatrix(); //unfinished!
	//ode abilites:
	void SetPosition(palMatrix4x4 &pos);
	virtual void SetMaterial(palMaterial *material);
	dGeomID odeGeom; // the ODE geometries representing this body
};

class palODEBoxGeometry : public palBoxGeometry , public palODEGeometry {
public:
	palODEBoxGeometry();
	void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
protected:
	FACTORY_CLASS(palODEBoxGeometry,palBoxGeometry,ODE,1)
};

class palODESphereGeometry : public palSphereGeometry , public palODEGeometry {
public:
	palODESphereGeometry();
	void Init(palMatrix4x4 &pos, Float radius, Float mass);
protected:
	FACTORY_CLASS(palODESphereGeometry,palSphereGeometry,ODE,1)
};

class palODECapsuleGeometry : public palCapsuleGeometry , public palODEGeometry {
public:
	palODECapsuleGeometry();
	void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
	virtual palMatrix4x4& GetLocationMatrix(); 
protected:
	FACTORY_CLASS(palODECapsuleGeometry,palCapsuleGeometry,ODE,1)
};


class palODEBox : public palBox, public palODEBody {
public:
	palODEBox();
	//void SetPosition(Float x, Float y, Float z); //duplicate to ensure dominance
	void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by ODE abilities:
	void SetMass(Float mass);
protected:
	FACTORY_CLASS(palODEBox,palBox,ODE,1)
};

class palODEStaticBox:public palStaticBox {
public:
	palODEStaticBox();
	~palODEStaticBox();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth);
	virtual palMatrix4x4& GetLocationMatrix() {
		return m_mLoc;
	}
protected:
	FACTORY_CLASS(palODEStaticBox,palStaticBox,ODE,1)
};


class palODESphere : public palSphere, public palODEBody {
public:
	palODESphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);
	//extra methods provided by ODE abilities:
	void SetMass(Float mass);
	//void SetRadius(Float radius);
protected:
	FACTORY_CLASS(palODESphere,palSphere,ODE,1)
};



class palODECylinder : public palCapsule, public palODEBody {
public:
	palODECylinder();
	void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
	//extra methods provided by ODE abilities:
	//void SetRadiusLength(Float radius, Float length);
	void SetMass(Float mass);
protected:
	FACTORY_CLASS(palODECylinder,palCapsule,ODE,1)
};

class palODELink : virtual public palLink {
public:
	palODELink();
	dJointID GetJointID() {
		return odeJoint;
	}
protected:
	dJointID odeJoint; //the ODE joint
	dJointID odeMotorJoint; //the ODE motorised joint
};

class palODESphericalLink : public palSphericalLink, public palODELink {
public:
	palODESphericalLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);
//	void SetLimits(Float lower_limit_rad, Float upper_limit_rad); 
//	void SetTwistLimits(Float lower_limit_rad, Float upper_limit_rad);
	//extra methods provided by ODE abilities:
	void SetAnchor(Float x, Float y, Float z);
protected:
	void InitMotor();
	FACTORY_CLASS(palODESphericalLink,palSphericalLink,ODE,1)
};

class palODERevoluteLink: public palRevoluteLink, public palODELink {
public:
	palODERevoluteLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad); 
//	virtual Float GetAngle();
	virtual void AddTorque(Float torque);
	//extra methods provided by ODE abilities:
	virtual void SetAnchorAxis(Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
protected:
	FACTORY_CLASS(palODERevoluteLink,palRevoluteLink,ODE,1)
};


class palODEPrismaticLink: public palPrismaticLink, public palODELink {
public:
	palODEPrismaticLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	//extra methods provided by ODE abilities:
	void SetAnchorAxis(Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
protected:
	FACTORY_CLASS(palODEPrismaticLink,palPrismaticLink,ODE,1)
};

class palODETerrain : virtual public palTerrain {
public:
	palODETerrain();
	virtual void SetMaterial(palMaterial *material);
	virtual palMatrix4x4& GetLocationMatrix();
//protected:
	dGeomID odeGeom; // the ODE geometries representing this body
};

class palODETerrainPlane : public palTerrainPlane, public palODETerrain {
public:
	palODETerrainPlane();
	void Init(Float x, Float y, Float z, Float min_size);
	virtual palMatrix4x4& GetLocationMatrix();
protected:
	FACTORY_CLASS(palODETerrainPlane,palTerrainPlane,ODE,1)
};

class palODEOrientatedTerrainPlane :  public palOrientatedTerrainPlane, public palODETerrain  {
public:
	palODEOrientatedTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);
	virtual palMatrix4x4& GetLocationMatrix() {return palOrientatedTerrainPlane::GetLocationMatrix();}
protected:
	FACTORY_CLASS(palODEOrientatedTerrainPlane,palOrientatedTerrainPlane,ODE,1)
};

class palODETerrainMesh : virtual public palTerrainMesh, public palODETerrain {
public:
	palODETerrainMesh();
	void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
//	palMatrix4x4& GetLocationMatrix();
protected:
	FACTORY_CLASS(palODETerrainMesh,palTerrainMesh,ODE,1)
};

class palODETerrainHeightmap : virtual public palTerrainHeightmap, private palODETerrainMesh {
public:
	palODETerrainHeightmap();
	void Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap);
//	palMatrix4x4& GetLocationMatrix();
protected:
	FACTORY_CLASS(palODETerrainHeightmap,palTerrainHeightmap,ODE,1)
};

class palODEConvexGeometry : public palODEGeometry, public palConvexGeometry  {
public:
	palODEConvexGeometry();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palODEConvexGeometry,palConvexGeometry,ODE,1)
};


class palODEConvex : public palODEBody, public palConvex {
public:
	palODEConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palODEConvex,palConvex,ODE,1)
};

class palODECompoundBody : public palCompoundBody, public palODEBody {
public:
	palODECompoundBody();
//	virtual void SetPosition(palMatrix4x4& location);
//	virtual palMatrix4x4& GetLocationMatrix();
	virtual void Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ);
protected:
	FACTORY_CLASS(palODECompoundBody,palCompoundBody,ODE,1)
};

class palODEStaticCompoundBody:public palStaticCompoundBody {
public:
	palODEStaticCompoundBody();
	virtual palMatrix4x4& GetLocationMatrix() {
		return m_mLoc;
	}
	virtual void Finalize();
protected:
	FACTORY_CLASS(palODEStaticCompoundBody,palStaticCompoundBody,ODE,1)
};

class palODEPSDSensor : public palPSDSensor {
public:
	palODEPSDSensor();
	void Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range); //position, direction
	Float GetDistance();
protected:
	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;
	dGeomID odeRayId;
	FACTORY_CLASS(palODEPSDSensor,palPSDSensor,ODE,1)
};

class palODEAngularMotor : public palAngularMotor {
public:
	palODEAngularMotor();
	virtual void Init(palRevoluteLink *pLink, Float Max);
	virtual void Update(Float targetVelocity);
	virtual void Apply();
protected:
	dJointID odeJoint; //the ODE joint
	FACTORY_CLASS(palODEAngularMotor,palAngularMotor,ODE,1)
};
#endif
