#ifndef ODE_PAL_H
#define ODE_PAL_H

#define ODE_PAL_SDK_VERSION_MAJOR 0
#define ODE_PAL_SDK_VERSION_MINOR 1
#define ODE_PAL_SDK_VERSION_BUGFIX 9

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. ODE implementation.
		This enables the use of ODE via PAL.
	Author:
		Adrian Boeing
	Revision History:
		Version 0.1.10: 16/09/09 - AB: Fixed some bugs, introduced a new bug to the compound body (4x3 vs 4x4)
		Version 0.1.09: 18/02/09 - Public set/get for ODE functionality & documentation
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

#include <pal/pal.h>
#include <pal/palFactory.h>
#include <pal/palCollision.h>
#include <pal/palActivation.h>

#include <ode/ode.h>

#if defined(_MSC_VER)
#pragma warning(disable : 4250)

//#ifndef NDEBUG
//#if defined(dSINGLE)
//#pragma comment( lib, "ode_singled.lib" )
//#else
//#pragma comment( lib, "ode_doubled.lib" )
//#endif
//#else
//#if defined(dSINGLE)
//#pragma comment( lib, "ode_single.lib" )
//#else
//#pragma comment( lib, "ode_double.lib" )
//#endif
//#endif

#endif //_MSC_VER

#define ODE_MATINDEXLOOKUP int

class palODEMaterials : public palMaterials {
public:
	palODEMaterials();
// todo: fill this in! =
// have a opdebodyid-> material name ? map (or index => better!)
	virtual palMaterialUnique *NewMaterial(PAL_STRING name, const palMaterialDesc& desc);
	static void InsertIndex(dGeomID odeGeom, palMaterial *mat);

	static palMaterial *GetODEMaterial(dGeomID odeGeomA, dGeomID odeGeomB);

protected:
	static ODE_MATINDEXLOOKUP* GetMaterialIndex(dGeomID odeGeom);
	virtual void SetIndex(int posx, int posy, palMaterial *pm);
	virtual void SetNameIndex(PAL_STRING name);

	static PAL_VECTOR<PAL_STRING> g_MaterialNames;
	static std_matrix<palMaterial *> g_Materials;
	static PAL_MAP <dGeomID, ODE_MATINDEXLOOKUP> g_IndexMap; //or make this part of the global?  this is evil.
	FACTORY_CLASS(palODEMaterials,palMaterials,ODE,2);
};

/** ODE Physics Class
	Additionally Supports:
		- Collision Detection
*/
class palODEPhysics: public palPhysics, public palCollisionDetectionExtended {
public:
	palODEPhysics();
	void Init(const palPhysicsDesc& desc);
	void SetGravity(Float gravity_x, Float gravity_y, Float gravity_z);


	//colision detection functionality
	virtual void SetCollisionAccuracy(Float fAccuracy);

	virtual void SetGroupCollision(palGroup a, palGroup b, bool enabled);
	void SetGroupCollisionOnGeom(unsigned long bits, unsigned long otherBits, dGeomID geom, bool collide);

	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit);
	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz,
				Float range, palRayHitCallback& callback, palGroupFlags groupFilter = ~0);
	virtual void NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled);
	virtual void NotifyCollision(palBodyBase *pBody, bool enabled);
	void CleanupNotifications(palBodyBase* geom);
	virtual void GetContacts(palBodyBase *pBody, palContact& contact) const;
	virtual void GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact) const;
	virtual void ClearContacts();

//	void SetDefaultMaterial(palMaterial *pmat);
//	void SetGroundPlane(bool enabled, Float size);
	const char* GetPALVersion() const;
	const char* GetVersion() const;
	virtual palCollisionDetection* asCollisionDetection() { return this; }

	//ODE specific:
	/** Returns the current ODE World in use by PAL
		\return A pointer to the current ODE dWorldID
	*/
	dWorldID ODEGetWorld() const;
	/** Returns the current ODE Space in use by PAL
		\return A pointer to the current ODE dSpaceID
	*/
	dSpaceID ODEGetSpace() const;

	void Cleanup();

	PAL_VECTOR<unsigned long> m_CollisionMasks;

protected:
	void Iterate(Float timestep);

	FACTORY_CLASS(palODEPhysics,palPhysics,ODE,1)
	bool m_initialized;
};

/** The ODE Body class
*/
class palODEBody : virtual public palBody, virtual public palActivationSettings
{
	friend class palODERevoluteLink;
	friend class palODESphericalLink;
	friend class palODEPrismaticLink;
	friend class palODEBoxGeometry;
	friend class palODESphereGeometry;
	friend class palODECapsuleGeometry;
	friend class palODEConvexGeometry;
	friend class palODEConcaveGeometry;
public:
	palODEBody();
	~palODEBody();
	virtual void SetPosition(Float x, Float y, Float z);
	virtual void SetPosition(const palMatrix4x4& location);
#if 0
	virtual void SetForce(Float fx, Float fy, Float fz);
	virtual void GetForce(palVector3& force) const;

	virtual void AddForce(Float fx, Float fy, Float fz);
	virtual void AddTorque(Float tx, Float ty, Float tz);

	virtual void SetTorque(Float tx, Float ty, Float tz);
	virtual void GetTorque(palVector3& torque);

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float ix, Float iy, Float iz);
#endif

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	virtual void GetLinearVelocity(palVector3& velocity) const;
	virtual void GetAngularVelocity(palVector3& velocity_rad) const;

	virtual void SetLinearVelocity(const palVector3& velocity);
	virtual void SetAngularVelocity(const palVector3& velocity_rad);

  //@return if the body is active or sleeping
	virtual bool IsActive() const;

	virtual void SetActive(bool active);

	virtual void SetGroup(palGroup group);

	virtual void SetMaterial(palMaterial *material);

	//virtual void a() {};
	virtual const palMatrix4x4& GetLocationMatrix() const;

	virtual palActivationSettings* asActivationSettings() { return this; }

	/***** Pal Activation ****/
	virtual Float GetActivationLinearVelocityThreshold() const;
   virtual void SetActivationLinearVelocityThreshold(Float);

   virtual Float GetActivationAngularVelocityThreshold() const;
   virtual void SetActivationAngularVelocityThreshold(Float);

   virtual Float GetActivationTimeThreshold() const;
   virtual void SetActivationTimeThreshold(Float);

   virtual const std::bitset<DUMMY_ACTIVATION_SETTING_TYPE>& GetSupportedActivationSettings() const;
   /***** Pal Activation ****/

	//ODE specific:
	/** Returns the ODE body associated with the PAL body
		\return The ODE dBodyID
	*/
	dBodyID ODEGetBody() const {return odeBody;}
	/**
	 *  Same as IsCollisionResponseEnabled.  Added a fast inline so the internal code won't have to call a virtual method
	 *  for every potential collision
	 */
	inline bool ODEGetCollisionResponseEnabled() const { return m_bCollisionResponseEnabled; }
protected:
	dBodyID odeBody; // the ODE body
	bool m_bCollisionResponseEnabled;
protected:
	void BodyInit(Float x, Float y, Float z);
	virtual void SetGeometryBody(palGeometry *pgeom);
	void RecalcMassAndInertia();
private:
	static const std::bitset<DUMMY_ACTIVATION_SETTING_TYPE> SUPPORTED_SETTINGS;
};

/** The ODE Geometry class
*/
class palODEGeometry : virtual public palGeometry {
	friend class palODECompoundBody;
	friend class palODEPhysics;
	friend class palODEBody;
public:
	palODEGeometry();
	~palODEGeometry();
	virtual const palMatrix4x4& GetLocationMatrix() const; //unfinished!
	//ode abilites:
	void SetPosition(const palMatrix4x4 &pos);
	virtual void SetMaterial(palMaterial *material);
	//ODE specific:
	/** Returns the ODE geometry associated with the PAL geometry
		\return The ODE dGeomID
	*/
	dGeomID ODEGetGeom() const {return odeGeom;}

	virtual void CalculateMassParams(dMass& odeMass, Float massScalar) const = 0;

protected:
	void ReCalculateOffset();
	dGeomID odeGeom; // the ODE geometries representing this body
};

class palODEBoxGeometry : virtual public palBoxGeometry, virtual public palODEGeometry {
public:
	palODEBoxGeometry();
	void Init(const palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
	virtual void CalculateMassParams(dMass& odeMass, Float massScalar) const;
protected:
	FACTORY_CLASS(palODEBoxGeometry,palBoxGeometry,ODE,1)
};

class palODESphereGeometry : virtual public palSphereGeometry, virtual public palODEGeometry {
public:
	palODESphereGeometry();
	void Init(const palMatrix4x4 &pos, Float radius, Float mass);
	virtual void CalculateMassParams(dMass& odeMass, Float massScalar) const;
protected:
	FACTORY_CLASS(palODESphereGeometry,palSphereGeometry,ODE,1)
};

class palODECapsuleGeometry : virtual public palCapsuleGeometry, virtual public palODEGeometry {
public:
	palODECapsuleGeometry();
	void Init(const palMatrix4x4 &pos, Float radius, Float length, Float mass);
	virtual const palMatrix4x4& GetLocationMatrix() const;
	virtual void CalculateMassParams(dMass& odeMass, Float massScalar) const;
protected:
	void ReCalculateOffset();
	FACTORY_CLASS(palODECapsuleGeometry,palCapsuleGeometry,ODE,1)
private:
	unsigned int m_upAxis;
};

class palODEConvexGeometry : virtual public palConvexGeometry, virtual public palODEGeometry  {
public:
	palODEConvexGeometry();
	virtual void Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
	virtual void Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
	virtual void CalculateMassParams(dMass& odeMass, Float massScalar) const;
protected:
	FACTORY_CLASS(palODEConvexGeometry,palConvexGeometry,ODE,1)
};

class palODEConcaveGeometry : virtual public palConcaveGeometry, virtual public palODEGeometry  {
public:
	palODEConcaveGeometry();
   virtual void Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
	virtual void CalculateMassParams(dMass& odeMass, Float massScalar) const;
protected:
	FACTORY_CLASS(palODEConcaveGeometry,palConcaveGeometry,ODE,1)
};

class palODEBox : virtual public palBox, virtual public palODEBody {
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
	virtual void Init(const palMatrix4x4 &pos, Float width, Float height, Float depth);
	virtual const palMatrix4x4& GetLocationMatrix() const {
		return m_mLoc;
	}
protected:
	FACTORY_CLASS(palODEStaticBox,palStaticBox,ODE,1)
};


class palODESphere : virtual public palSphere, virtual public palODEBody {
public:
	palODESphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);
	//extra methods provided by ODE abilities:
	void SetMass(Float mass);
	//void SetRadius(Float radius);
protected:
	FACTORY_CLASS(palODESphere,palSphere,ODE,1)
};



class palODECylinder : virtual public palCapsule, virtual public palODEBody {
public:
	palODECylinder();
	void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
	//extra methods provided by ODE abilities:
	//void SetRadiusLength(Float radius, Float length);
	void SetMass(Float mass);
protected:
	FACTORY_CLASS(palODECylinder,palCapsule,ODE,1)
};

class palODEGenericBody : virtual public palODEBody, virtual public palGenericBody {
public:
	palODEGenericBody();

	virtual void Init(const palMatrix4x4 &pos);
	virtual void SetDynamicsType(palDynamicsType dynType);

	virtual void SetGravityEnabled(bool enabled);
	virtual bool IsGravityEnabled() const;

	virtual void SetCollisionResponseEnabled(bool enabled);
	virtual bool IsCollisionResponseEnabled() const;

	virtual void SetMass(Float mass);
	virtual void SetInertia(Float Ixx, Float Iyy, Float Izz);

	virtual void SetLinearDamping(Float);
	virtual Float GetLinearDamping() const;

	virtual void SetAngularDamping(Float);
	virtual Float GetAngularDamping() const;

	virtual void SetMaxAngularVelocity(Float maxAngVel);
	virtual Float GetMaxAngularVelocity() const;

	virtual void ConnectGeometry(palGeometry* pGeom);
	virtual void RemoveGeometry(palGeometry* pGeom);

	virtual bool IsDynamic() const;
	virtual bool IsKinematic() const;
	virtual bool IsStatic() const;

protected:
	FACTORY_CLASS(palODEGenericBody, palGenericBody, ODE, 1);
};

class palODEStaticConvex: public palStaticConvex{
public:
	palODEStaticConvex();
	virtual void Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices);
	virtual void Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
	virtual const palMatrix4x4& GetLocationMatrix() const {return m_mLoc;}
protected:
	FACTORY_CLASS(palODEStaticConvex,palStaticConvex,ODE,1)
};

class palODEStaticCylinder:public palStaticCapsule {
public:
	palODEStaticCylinder();
	virtual void Init(const palMatrix4x4 &pos, Float radius, Float length);
	virtual const palMatrix4x4& GetLocationMatrix() const {return m_mLoc;}
protected:
	FACTORY_CLASS(palODEStaticCylinder,palStaticCapsule,ODE,1)
};

class palODEStaticSphere:public palStaticSphere {
public:
	palODEStaticSphere();
	virtual void Init(const palMatrix4x4 &pos, Float radius);
	virtual const palMatrix4x4& GetLocationMatrix() const {return m_mLoc;}
protected:
	FACTORY_CLASS(palODEStaticSphere,palStaticSphere,ODE,1)
};

/** The ODE Link class
*/
class palODELink : virtual public palLink {
public:
	palODELink();

	//ODE specific:
	/** Returns the ODE joint associated with the PAL link
		\return The ODE dJointID
	*/
	dJointID ODEGetJointID() const {
		return odeJoint;
	}
protected:
	dJointID odeJoint; //the ODE joint
	dJointID odeMotorJoint; //the ODE motorised joint
};

class palODESphericalLink : virtual public palSphericalLink, virtual public palODELink {
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

class palODERevoluteLink: virtual public palRevoluteLink, virtual public palODELink {
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


class palODEPrismaticLink: virtual public palPrismaticLink, virtual public palODELink {
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
	virtual const palMatrix4x4& GetLocationMatrix() const;
//protected:
	dGeomID odeGeom; // the ODE geometries representing this body
};

class palODETerrainPlane : virtual public palTerrainPlane, virtual public palODETerrain {
public:
	palODETerrainPlane();
	void Init(Float x, Float y, Float z, Float min_size);
	virtual const palMatrix4x4& GetLocationMatrix() const;
protected:
	FACTORY_CLASS(palODETerrainPlane,palTerrainPlane,ODE,1)
};

class palODEOrientatedTerrainPlane : virtual  public palOrientatedTerrainPlane, virtual public palODETerrain  {
public:
	palODEOrientatedTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);
	virtual const palMatrix4x4& GetLocationMatrix() const {return palOrientatedTerrainPlane::GetLocationMatrix();}
protected:
	FACTORY_CLASS(palODEOrientatedTerrainPlane,palOrientatedTerrainPlane,ODE,1)
};

class palODETerrainMesh : virtual public palTerrainMesh, virtual public palODETerrain {
public:
	palODETerrainMesh();
	void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
//	palMatrix4x4& GetLocationMatrix() const;
protected:
	FACTORY_CLASS(palODETerrainMesh,palTerrainMesh,ODE,1)
};

class palODETerrainHeightmap : virtual public palTerrainHeightmap, virtual private palODETerrainMesh {
public:
	palODETerrainHeightmap();
	void Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap);
//	palMatrix4x4& GetLocationMatrix() const;
protected:
	FACTORY_CLASS(palODETerrainHeightmap,palTerrainHeightmap,ODE,1)
};

class palODEConvex : virtual public palODEBody, virtual public palConvex {
public:
	palODEConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
protected:
	FACTORY_CLASS(palODEConvex,palConvex,ODE,1)
};

class palODECompoundBody : virtual public palCompoundBody, virtual public palODEBody {
public:
	palODECompoundBody();
	virtual void Init(Float x, Float y, Float z);
//	virtual void SetPosition(palMatrix4x4& location);
//	virtual palMatrix4x4& GetLocationMatrix() const;
	virtual void Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ);
protected:
	FACTORY_CLASS(palODECompoundBody,palCompoundBody,ODE,1)
};

class palODEStaticCompoundBody:public palStaticCompoundBody {
public:
	palODEStaticCompoundBody();
	virtual const palMatrix4x4& GetLocationMatrix() const {
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
	Float GetDistance() const;
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
