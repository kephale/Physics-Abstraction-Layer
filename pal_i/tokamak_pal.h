#ifndef TOKAMAK_PAL_H
#define TOKAMAK_PAL_H

#define TOKAMAK_PAL_SDK_VERSION_MAJOR 0
#define TOKAMAK_PAL_SDK_VERSION_MINOR 1
#define TOKAMAK_PAL_SDK_VERSION_BUGFIX 24

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Tokamak implementation.
		This enables the use of tokamak via PAL.
	Author:
		Adrian Boeing
	Revision History:
		Version 0.1.25: 20/03/09 - 64bit compatibility
		Version 0.1.24: 22/02/09 - Added solver support for substeps
		Version 0.1.23: 18/02/09 - Public set/get for Tokamak functionality & documentation
		Version 0.1.22: 30/09/08 - PAL Versioning
		Version 0.1.21: 15/07/08 - Compound body finalize mass & inertia method
		Version 0.1.20: 15/12/07 - Body deletion
		Version 0.1.19: 19/10/07 - Version number request
		Version 0.1.18: 25/07/07 - Orientated plane
		Version 0.1.17: 15/07/07 - Body sleep
		Version 0.1.16: 22/06/07 - Set linear velocity (angular unsupported!)
		Version 0.1.15: 23/10/06 - neRigidBodyControllerCallback API change, static call hack
		Version 0.1.14: 23/02/05 - Geometry postion
		Version 0.1.13: 13/09/04 - Angular Impulse
		Version 0.1.12: 05/09/04 - Impulse
		Version 0.1.11: 19/08/04 - Geometry fix
		Version 0.1.1 : 29/07/04 - Material&geometrie update
		Version 0.1.0 : 08/07/04 - Geometries : cylinders, sphere, boxes, compound body
		Version 0.0.9 : 25/06/04 - renamed, PSD sensor, new material system, fixed joint limits!, contact sensor
		Version 0.0.78: 21/06/04 - Fixed revolute joints, Joint limits - need fixing
		Version 0.0.77: 15/06/04 - Started prismatic
		Version 0.0.76: 12/06/04 - Terrain heightmap, terrain mesh
		Version 0.0.7 : 11/06/04 - Reimplemented terrain plane,
		Version 0.0.61: 09/06/04 - Fixed floor
		Version 0.0.6 : 06/06/04 - Allow joints, added sphere
		Version 0.0.5 : 04/06/04 - Allow materials & bodies
	TODO:
		-Add body base class
		-Set angular velocity
		-Verify correct operation of compound body
		-Correct location setting in Geometry: set location (take into account body rotation)
		-optimize contact sensor via contact ID's
		-get to 1.0 (ie: same as pal.h)
*/

#include "../pal/pal.h"
#include "../pal/palFactory.h"
#include "../pal/palSolver.h"

//#define USE_QHULL

#include <tokamak.h>
#if defined(_MSC_VER)
#pragma message("Remember to set compiler definition to : TOKAMAK_USE_DLL")
//#pragma comment(lib, "tokamak.lib")
#pragma warning(disable : 4250)
#endif
/*
class palTokamakMaterial: public palMaterial {
public:
	palTokamakMaterial();
	void Init(Float static_friction, Float kinetic_friction, Float restitution);
	int m_Index;
protected:
	static int g_materialcount;
	FACTORY_CLASS(palTokamakMaterial,palMaterial,Tokamak,1);
};
*/


class palTokamakMaterialUnique : public palMaterialUnique {
public:
	palTokamakMaterialUnique();
<<<<<<< .working
	void Init(const PAL_STRING & name,Float static_friction, Float kinetic_friction, Float restitution); 
=======
	void Init(PAL_STRING name,Float static_friction, Float kinetic_friction, Float restitution);
>>>>>>> .merge-right.r138

	int m_Index;
protected:
	FACTORY_CLASS(palTokamakMaterialUnique,palMaterialUnique,Tokamak,2);
};

class palTokamakMaterialInteraction : public palMaterialInteraction  {
public:
	palTokamakMaterialInteraction();
	void Init(palMaterialUnique *pM1, palMaterialUnique *pM2, Float static_friction, Float kinetic_friction, Float restitution);
protected:
	FACTORY_CLASS(palTokamakMaterialInteraction,palMaterialInteraction,Tokamak,2);
};

/** Tokamak Physics Class
	Additionally Supports:
		- Solver
*/
class palTokamakPhysics: public palPhysics, public palSolver  {
public:
	palTokamakPhysics();
	void Init(Float gravity_x, Float gravity_y, Float gravity_z);
	void Cleanup();
	const char* GetVersion();
	const char* GetPALVersion();

	//solver functionality
	virtual void SetSolverAccuracy(Float fAccuracy);
	virtual void StartIterate(Float timestep);
	virtual bool QueryIterationComplete();
	virtual void WaitForIteration();
	virtual void SetPE(int n);
	virtual void SetSubsteps(int n);
	virtual void SetHardware(bool status);
	virtual bool GetHardware(void);

	//Tokamak specific:
	/** Returns the current Tokamak Simulator in use by PAL
		\return A pointer to the current neSimulator
	*/
	neSimulator* TokamakGetSimulator();
protected:
	int set_substeps;
	void Iterate(Float timestep);
	FACTORY_CLASS(palTokamakPhysics,palPhysics,Tokamak,1)
};

/** Tokamak Body Class
*/
class palTokamakBody : virtual public palBody {
	friend class palTokamakRevoluteLink;
	friend class palTokamakSphericalLink;
	friend class palTokamakPrismaticLink;
          friend class palTokamakConvexGeometry;
public:
	palTokamakBody();
	~palTokamakBody();
//	void SetPosition(Float x, Float y, Float z);
	void SetPosition(palMatrix4x4& location);

#if 0
	void SetForce(Float fx, Float fy, Float fz);
	void GetForce(palVector3& force);

	void SetTorque(Float tx, Float ty, Float tz);
	void GetTorque(palVector3& torque);

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);
#endif

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);


	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

	virtual void SetActive(bool active);
	virtual bool IsActive();

	virtual void SetMaterial(palMaterial *material);

	//virtual void a() {};
	palMatrix4x4& GetLocationMatrix();

	/** Returns the Tokamak Rigid Body associated with the PAL body
		\return Returns a pointer to the neRigidBody
	*/
	neRigidBody* TokamakGetRigidBody() {return m_ptokBody;}
protected:
	neRigidBody *m_ptokBody;
};

/** Tokamak Geometry Class
*/
class palTokamakGeometry : virtual public palGeometry {
public:
	palTokamakGeometry();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetPosition(palMatrix4x4& location);
	virtual void SetMaterial(palMaterial *material);

	//Tokamak specific:
	/** Returns the Tokamak geometry used by PAL geometry
		\return A pointer to the neGeometry
	*/
	neGeometry* TokamakGetGeometry() {return m_ptokGeom;}
protected:
	neGeometry *m_ptokGeom;
};

class palTokamakBoxGeometry : public palBoxGeometry , public palTokamakGeometry {
public:
	palTokamakBoxGeometry();
	void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
//extra methods provided by tokamak abilities:
	void SetDimensions(Float width, Float height, Float depth);
protected:
	FACTORY_CLASS(palTokamakBoxGeometry,palBoxGeometry,Tokamak,1)
};

class palTokamakSphereGeometry: public palSphereGeometry, public palTokamakGeometry {
public:
	palTokamakSphereGeometry();
	void Init(palMatrix4x4 &pos, Float radius, Float mass);
//extra methods provided by tokamak abilities:
	void SetRadius(Float radius);
protected:
	FACTORY_CLASS(palTokamakSphereGeometry,palSphereGeometry,Tokamak,1)
};

class palTokamakCylinderGeometry: public palCapsuleGeometry, public palTokamakGeometry {
public:
	palTokamakCylinderGeometry();
	void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
//extra methods provided by tokamak abilities:
	void SetRadiusLength(Float radius, Float length);
protected:
	FACTORY_CLASS(palTokamakCylinderGeometry,palCapsuleGeometry,Tokamak,1)
};

#ifdef USE_QHULL
class palTokamakConvexGeometry : public palTokamakGeometry, public palConvexGeometry  {
public:
	palTokamakConvexGeometry() {};
	~palTokamakConvexGeometry() {};
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);

	BYTE *GenerateConvexData(const Float *pVertices, int nVertices);
//	bool ReadConvexData(char * filename, neByte *& adjacency);
	void PreProcess(neByte *& d);
	void TokamakInitQHull(palMatrix4x4 &pos, neByte *data);
protected:
	FACTORY_CLASS(palTokamakConvexGeometry,palConvexGeometry,Tokamak,1)
};


class palTokamakConvex : public palTokamakBody, public palConvex {
public:
	palTokamakConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
	void SetMass(Float mass);
protected:
	FACTORY_CLASS(palTokamakConvex,palConvex,Tokamak,1)
};
#endif

class palTokamakBox : public palBox, public palTokamakBody {
public:
	//virtual void b() {};
	palTokamakBox();
//	void SetPosition(Float x, Float y, Float z); //duplicate to ensure dominance
	void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by tokamak abilities:
	void SetMass(Float mass);
	void SetDimensions(Float width, Float height, Float depth);
protected:
	FACTORY_CLASS(palTokamakBox,palBox,Tokamak,1)
};

class palTokamakSphere : public palSphere, public palTokamakBody {
public:
	palTokamakSphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);
	//extra methods provided by tokamak abilities:
	void SetMass(Float mass);
	void SetRadius(Float radius);
protected:
	FACTORY_CLASS(palTokamakSphere,palSphere,Tokamak,1)
};

class palTokamakCylinder : public palCapsule, public palTokamakBody {
public:
	palTokamakCylinder();
	void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
	//extra methods provided by tokamak abilities:
	void SetRadiusLength(Float radius, Float length);
	void SetMass(Float mass);
protected:
	FACTORY_CLASS(palTokamakCylinder,palCapsule,Tokamak,1)
};

class palTokamakCompoundBody : public palCompoundBody, public palTokamakBody {
public:
	palTokamakCompoundBody ();
	virtual void Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ);
protected:
	FACTORY_CLASS(palTokamakCompoundBody,palCompoundBody,Tokamak,1)
};


/** Tokamak Link Class
*/
class palTokamakLink : virtual public palLink {
public:
	palTokamakLink();
	//Tokamak specific:
	/** Returns the Tokamak Joint associated with the PAL link
		\return A pointer to the neJoint
	*/
	neJoint* TokamakGetJoint() {return m_ptokJoint;}
protected:
	neJoint * m_ptokJoint;
};

class palTokamakSphericalLink : public palSphericalLink, public palTokamakLink {
public:
	palTokamakSphericalLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);
	//void SetLimits(Float lower_limit_rad1, Float upper_limit_rad1,Float lower_limit_rad2, Float upper_limit_rad2 ); //radians

	void SetLimits(Float cone_limit_rad, Float twist_limit_rad);
/*
	void SetLimits(Float lower_limit_rad, Float upper_limit_rad);
	void SetTwistLimits(Float lower_limit_rad, Float upper_limit_rad);
*/
	//extra methods provided by tokamak abilities:
	void SetAnchor(Float x, Float y, Float z);
protected:
	FACTORY_CLASS(palTokamakSphericalLink,palSphericalLink,Tokamak,1)
};

class palTokamakRevoluteLink: public palRevoluteLink, public palTokamakLink {
public:
	palTokamakRevoluteLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	void SetLimits(Float lower_limit_rad, Float upper_limit_rad);
protected:
	FACTORY_CLASS(palTokamakRevoluteLink,palRevoluteLink,Tokamak,1)
};

class palTokamakPrismaticLink:  public palPrismaticLink, public palTokamakLink {
public:
	palTokamakPrismaticLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
protected:
	FACTORY_CLASS(palTokamakPrismaticLink,palPrismaticLink,Tokamak,1)
};

/*
class TokamakTerrain : virtual public palTerrain {
};
*/

class palTokamakTerrainPlane : public palTerrainPlane {
public:
	palTokamakTerrainPlane();
	void Init(Float x, Float y, Float z, Float min_size);
	palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material);
protected:
	FACTORY_CLASS(palTokamakTerrainPlane,palTerrainPlane,Tokamak,1)
};


class palTokamakOrientatedTerrainPlane :  public palOrientatedTerrainPlane {
public:
	palTokamakOrientatedTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);
	virtual palMatrix4x4& GetLocationMatrix() {return palOrientatedTerrainPlane::GetLocationMatrix();}
	virtual void SetMaterial(palMaterial *material);
protected:
	FACTORY_CLASS(palTokamakOrientatedTerrainPlane,palOrientatedTerrainPlane,Tokamak,1)
};

class palTokamakTerrainMesh : virtual public palTerrainMesh {
public:
	palTokamakTerrainMesh();
	void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
	palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material);
protected:
	FACTORY_CLASS(palTokamakTerrainMesh,palTerrainMesh,Tokamak,1)
};

class palTokamakTerrainHeightmap : virtual public palTerrainHeightmap, private palTokamakTerrainMesh {
public:
	palTokamakTerrainHeightmap();
	void Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap);
	palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material);
protected:
	FACTORY_CLASS(palTokamakTerrainHeightmap,palTerrainHeightmap,Tokamak,1)
};



class palTokamakPSDSensor : public palPSDSensor {
protected:
	class PSDControllerCB: public neRigidBodyControllerCallback
	{
		friend class palTokamakPSDSensor;
	public:
		void RigidBodyControllerCallback(neRigidBodyController * controller, float unused)
		{
			neRigidBody * rb = controller->GetRigidBody();
			m_pSensor->m_distance=m_pSensor->m_ptokSensor->GetDetectDepth();
		}
		palTokamakPSDSensor *m_pSensor;
	};
	friend class PSDControllerCB;
public:
	palTokamakPSDSensor();
	void Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range); //position, direction
	Float GetDistance();

protected:
	neSensor *m_ptokSensor;
	neRigidBodyController *m_ptokController;
	PSDControllerCB m_cb;
	//internal return data:
	Float m_distance;
	FACTORY_CLASS(palTokamakPSDSensor,palPSDSensor,Tokamak,1)
};

class palTokamakContactSensor: public palContactSensor {
public:
	palTokamakContactSensor();
	void Init(palBody *body); //location and size?
	void GetContactPosition(palVector3& contact);
	palVector3 m_Contact;
protected:
	FACTORY_CLASS(palTokamakContactSensor,palContactSensor,Tokamak,1);
};


#ifdef STATIC_CALLHACK
extern void pal_tokamak_call_me_hack();
#endif

#endif
