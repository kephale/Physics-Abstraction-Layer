#ifndef NOVODEX_PAL_H
#define NOVODEX_PAL_H

#define NOVODEX_PAL_SDK_VERSION_MAJOR 0
#define NOVODEX_PAL_SDK_VERSION_MINOR 2
#define NOVODEX_PAL_SDK_VERSION_BUGFIX 0
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/** \file novodex_pal.h
		Adrian Boeing
	\brief
	Abstract:
		PAL - Physics Abstraction Layer. NovodeX/PhysX implementation.
		This enables the use of NovodeX via PAL.
	\author
	Author:
		Adrian Boeing
    \version
	Revision History:
		Version 0.3.0 : 08/12/09 - Generic body completed.
		Version 0.2.0 : 16/05/09 - Softbodies cloth and deformable
		Version 0.1.02: 18/02/09 - Public set/get for NovodeX functionality & documentation
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
#include <pal/pal.h>
#include <pal/palFactory.h>
#include <pal/palFluid.h>
#include <pal/palCollision.h>
#include <pal/palSolver.h>
#include <framework/errorlog.h>

#if !defined(PAL_DISABLE_FLUID) && !defined(NX_DISABLE_FLUIDS)
#define NOVODEX_ENABLE_FLUID
#endif

#include <NxPhysics.h>
#if defined(_MSC_VER)
////#pragma comment(lib, "NxFoundation.lib")
////#pragma comment(lib, "NxPhysics.lib")
//#pragma comment(lib, "PhysXLoader.lib")
//#pragma comment(lib, "NxCooking.lib")
#pragma warning(disable : 4250) //dominance
#endif


/**
*  /brief   Purpose: Error Reporting system used for ageia to tell you messed up...
*                    passed into the ageia init routine
*/
class palNovodexErrorReportingSystem : public NxUserOutputStream, public StatusObject
{
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	palNovodexErrorReportingSystem() {}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void reportError(NxErrorCode code, const char *message, const char* file, int line)
	{
		if (code < NXE_DB_INFO)
		{
			ErrorLog::GetInstance()->SetInfo(file,line,this,"Error");
			ErrorLog::GetInstance()->Error("PhysX is stating error %s \n", message);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NxAssertResponse reportAssertViolation(const char *message, const char *file,int line)
	{
		ErrorLog::GetInstance()->SetInfo(file,line,this,"Error");
		ErrorLog::GetInstance()->Error("PhysX is stating error %s \n", message);
		return NX_AR_CONTINUE;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void print(const char *message)
	{
		SET_DEBUG(message);
	}

};


class palNovodexMaterialUnique : public palMaterialUnique {
public:
	palNovodexMaterialUnique();
	~palNovodexMaterialUnique();
	void Init(PAL_STRING name, const palMaterialDesc& desc);

	virtual void SetParameters(const palMaterialDesc& matDesc);

	NxMaterialDesc	m_MaterialDesc;
	NxMaterial*	m_pMaterial;
	NxMaterialIndex m_Index;
protected:
	FACTORY_CLASS(palNovodexMaterialUnique,palMaterialUnique,Novodex,2);
};

/** Novodex Physics Class
	Additionally Supports:
		- Collision Detection
		- Solver System
*/
class palNovodexPhysics: public palPhysics, public palCollisionDetectionExtended, public palSolver {
public:
	palNovodexPhysics();
	void Init(palPhysicsDesc& desc);
	void Cleanup();

	const char* GetPALVersion();
	const char* GetVersion();

	//Novodex specific:
	/** Returns the current Novodex Scene in use by PAL
		\return A pointer to the current NxScene
	*/
	NxScene* NxGetScene();
	/** Returns the Novodex SDK used by PAL
		\return A pointer to the NxPhysicsSDK
	*/
	NxPhysicsSDK* NxGetPhysicsSDK();

	//colision detection functionality
	virtual void SetCollisionAccuracy(Float fAccuracy);
	virtual void SetGroupCollision(palGroup a, palGroup b, bool enabled);
	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit);
	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range,
	         palRayHitCallback& callback, palGroupFlags groupFilter = ~0);
	virtual void NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled);
	virtual void NotifyCollision(palBodyBase *pBody, bool enabled);
	virtual void GetContacts(palBodyBase *pBody, palContact& contact);
	virtual void GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact);

	//solver functionality
	virtual void StartIterate(Float timestep);
	virtual bool QueryIterationComplete();
	virtual void WaitForIteration();
	virtual void SetFixedTimeStep(Float fixedStep);
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
private:
	void PopulateDebugDraw();
	palNovodexErrorReportingSystem m_UserReport;
	Float m_fFixedTimeStep;
	bool m_bSetUseHardware;
	int m_iSetSubsteps;
	int m_iSetPe;
};

/** Novodex Geometry Class
*/
class palNovodexGeometry : virtual public palGeometry {
	friend class palNovodexBodyBase;
	friend class palNovodexCompoundBody;
	friend class palNovodexStaticCompoundBody;
	friend class palNovodexGenericBody;
public:
	palNovodexGeometry();
//	virtual palMatrix4x4& GetLocationMatrix(); //unfinished!

	Float GetMargin() const;
	bool SetMargin(Float margin);

	//Novodex specific:
	/** Returns the Novodex Shape Descriptor used by PAL geometry
		\return A pointer to the NxShapeDesc
	*/
	NxShapeDesc* NxGetShapeDesc() {return m_pShape;}

	/** Returns the Novodex Shape used by PAL bodies
		\return A pointer to the NxShape
	*/
	NxShape* NxGetShape() {return m_pCreatedShape;}

protected:
	virtual void ReCalculateOffset();
	NxShape* m_pCreatedShape;
	NxShapeDesc* m_pShape;
};

class palNovodexBoxGeometry : public palNovodexGeometry, public palBoxGeometry  {
public:
	palNovodexBoxGeometry();
	~palNovodexBoxGeometry();
	void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass); //unfinished!


protected:
	NxBoxShapeDesc *m_pBoxShape;
	FACTORY_CLASS(palNovodexBoxGeometry,palBoxGeometry,Novodex,1)
};

/** Novodex Body Base Class
*/
class palNovodexBodyBase :virtual public palBodyBase {
	friend class palNovodexPhysics;
	friend class palNovodexRevoluteLink;
	friend class palNovodexSphericalLink;
	friend class palNovodexPrismaticLink;
	friend class palNovodexGenericLink;
	friend class palNovodexSpring;
	friend class palNovodexRevoluteSpringLink;
public:
	palNovodexBodyBase();
	~palNovodexBodyBase();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetPosition(palMatrix4x4& location);
	virtual void SetMaterial(palMaterial* material);
	virtual void SetGroup(palGroup group);

	Float GetSkinWidth() const;
	bool SetSkinWidth(Float skinWidth);

	//Novodex specific:
	/** Returns the Novodex Actor associated with the PAL body
		\return A pointer to the NxActor
	*/
	NxActor *NxGetActor() {return m_Actor;}
protected:
	NxActor *m_Actor;
	NxBodyDesc m_BodyDesc;
	NxActorDesc m_ActorDesc;

	// this is an internal utility function so that m_fSkinWidth doesn't need to be protected
	Float GetSavedSkinWidth() const { return m_fSkinWidth; }
protected:
	void BuildBody(Float mass, bool dynamic = true);
private:
	Float m_fSkinWidth;
};

class palNovodexBody : virtual public palBody, virtual public palNovodexBodyBase {
public:
	palNovodexBody();
	~palNovodexBody();

	virtual void SetPosition(palMatrix4x4& location);
//	palMatrix4x4& GetLocationMatrix();
#if 0
	void SetForce(Float fx, Float fy, Float fz);
	void GetForce(palVector3& force);

	void SetTorque(Float tx, Float ty, Float tz);
	void GetTorque(palVector3& torque);

	void AddForce(Float fx, Float fy, Float fz);
	void AddTorque(Float tx, Float ty, Float tz);

#endif
	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	//virtual void ApplyForceAtPosition(Float px, Float py, Float pz, Float fx, Float fy, Float fz); //direction of force (vector);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

  //@return if the body is sleeping
   virtual bool IsActive();

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

protected:
	NxSphereShapeDesc *m_pSphereShape;
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

protected:
	NxCapsuleShapeDesc *m_pCapShape;
	//recalculates the m_mOffset (local) matrix given the specified location and body
	virtual void ReCalculateOffset();
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
	friend class palNovodexConvex;
	friend class palNovodexStaticConvex;
public:
	palNovodexConvexGeometry();
	~palNovodexConvexGeometry();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);

protected:
	NxConvexMeshDesc  *m_pConvexMesh;
	NxConvexShapeDesc *m_pConvexShape;
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


class palNovodexConcaveGeometry : public palNovodexGeometry, public palConcaveGeometry  {
public:
	palNovodexConcaveGeometry();
	~palNovodexConcaveGeometry();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);

protected:
	NxTriangleMeshDesc  *m_pConcaveMesh;
	NxTriangleMeshShapeDesc *m_pConcaveShape;
	FACTORY_CLASS(palNovodexConcaveGeometry,palConcaveGeometry,Novodex,1)
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

/** Novodex Link Class
*/
class palNovodexLink : virtual public palLink {
public:
	palNovodexLink();

	//Novodex specific:
	/** Returns the Novodex Joint associated with the PAL link
		\return A pointer to the NxJoint
	*/
	NxJoint *NxGetJoint();
protected:
	NxJointDesc *m_Jdesc;
	NxJoint *m_Joint;
};

class palNovodexRevoluteLink: public palRevoluteLink, public palNovodexLink {
	friend class palNovodexAngularMotor;
public:
	palNovodexRevoluteLink();
	~palNovodexRevoluteLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	void SetLimits(Float lower_limit_rad, Float upper_limit_rad);

protected:
	NxRevoluteJoint *m_RJoint;
	NxRevoluteJointDesc *m_RJdesc;
	FACTORY_CLASS(palNovodexRevoluteLink,palRevoluteLink,Novodex,1)
};


class palNovodexRevoluteSpringLink: public palRevoluteSpringLink, public palNovodexLink {
	friend class palNovodexAngularMotor;
public:
	palNovodexRevoluteSpringLink();
	~palNovodexRevoluteSpringLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad);

	virtual void SetSpring(const palSpringDesc& springDesc);
	virtual void GetSpring(palSpringDesc& springDescOut);

protected:
	NxRevoluteJoint *m_RJoint;
	NxRevoluteJointDesc *m_RJdesc;
	FACTORY_CLASS(palNovodexRevoluteSpringLink,palRevoluteSpringLink,Novodex,1)
};

class palNovodexSphericalLink : public palSphericalLink, public palNovodexLink {
public:
	palNovodexSphericalLink();
	~palNovodexSphericalLink();
	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);

	void SetLimits(Float cone_limit_rad, Float twist_limit_rad);

protected:
	NxSphericalJoint  *m_SJoint;
	NxSphericalJointDesc *m_SJdesc;
	FACTORY_CLASS(palNovodexSphericalLink,palSphericalLink,Novodex,1)
};

class palNovodexPrismaticLink:  public palPrismaticLink, public palNovodexLink {
public:
	palNovodexPrismaticLink();
	~palNovodexPrismaticLink();

	void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);

protected:
	NxPrismaticJoint *m_PJoint;
	NxPrismaticJointDesc *m_PJdesc;
	FACTORY_CLASS(palNovodexPrismaticLink,palPrismaticLink,Novodex,1)
};

class palNovodexGenericLink : public palGenericLink, public palNovodexLink {
public:
	palNovodexGenericLink();
	virtual ~palNovodexGenericLink();
	void Init(palBodyBase *parent, palBodyBase *child, palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
		palVector3 linearLowerLimits,
		palVector3 linearUpperLimits,
		palVector3 angularLowerLimits,
		palVector3 angularUpperLimits);
	NxD6Joint* NovodexGetD6Joint() { return m_DJoint; }
	NxD6JointDesc* NovodexGetD6JointDesc() { return m_DJdesc; }
protected:
	NxD6Joint* m_DJoint;
	NxD6JointDesc *m_DJdesc;
	FACTORY_CLASS(palNovodexGenericLink,palGenericLink,Novodex,1)
};

class palNovodexRigidLink : public palRigidLink, public palNovodexLink {
public:
    palNovodexRigidLink();
    virtual ~palNovodexRigidLink();
    void Init(palBodyBase *parent, palBodyBase *child);
protected:
    NxFixedJoint* m_fixedJoint;
    NxFixedJointDesc* m_fixedJointDesc;
	FACTORY_CLASS(palNovodexRigidLink,palRigidLink,Novodex,1)
};

class palNovodexTerrain : virtual public palTerrain, public palNovodexBodyBase {
public:
	palNovodexTerrain();
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

class palNovodexGenericLinkSpring : public palGenericLinkSpring {
public:
	typedef palGenericLinkSpring BaseClass;

	palNovodexGenericLinkSpring();

	virtual void Init(palGenericLink* link);

	virtual void SetLinearSpring(unsigned axis, const palSpringDesc& spring);

	virtual void GetLinearSpring(unsigned axis, palSpringDesc& out) const;

	virtual void SetAngularSpring(unsigned axis, const palSpringDesc& spring);

	virtual void GetAngularSpring(unsigned axis, palSpringDesc& out) const;

	virtual void Apply();

	palNovodexGenericLink* NovodexGetLink() { return m_pNovodexLink; }
private:
	palNovodexGenericLink* m_pNovodexLink;
	FACTORY_CLASS(palNovodexGenericLinkSpring,palGenericLinkSpring,Novodex,1);
};


class palNovodexGenericBody : virtual public palGenericBody, virtual public palNovodexBody {
public:
	palNovodexGenericBody();
	virtual void Init(palMatrix4x4 &pos);
	virtual void SetDynamicsType(palDynamicsType dynType);
	virtual void SetMass(Float mass);
	virtual void SetInertia(Float Ixx, Float Iyy, Float Izz);
	virtual void SetGravityEnabled(bool enabled);
	virtual bool IsGravityEnabled() const;
	virtual void SetCollisionResponseEnabled(bool enabled);
	virtual bool IsCollisionResponseEnabled() const;

#if 0
	virtual void SetCenterOfMass(palMatrix4x4& loc);
#endif

	virtual void SetLinearDamping(Float);
	virtual Float GetLinearDamping() const;

	virtual void SetAngularDamping(Float);
	virtual Float GetAngularDamping() const;

	virtual void SetMaxAngularVelocity(Float maxAngVel);
	virtual Float GetMaxAngularVelocity() const;

	virtual void SetCenterOfMass_LocalTransform(palMatrix4x4 loc);
	virtual void ConnectGeometry(palGeometry* pGeom);
	virtual void RemoveGeometry(palGeometry* pGeom);
	virtual bool IsDynamic();
	virtual bool IsKinematic();
	virtual bool IsStatic();
protected:
	/**
	 * Creates the NX Actor, and will delete an existing actor if it exists.  This has to happen because
	 * swapping between static and non-static requires a recreation of the actor.
	 */
	void CreateNxActor(palMatrix4x4& pos);
	FACTORY_CLASS(palNovodexGenericBody,palGenericBody,Novodex,1);

	bool m_bInitialized;
};

//extrastuff:

/** Novodex Spring Class
	(experimental)
*/
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
/** Novodex SPH Fluid implementation
*/
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

#include "../pal/palSoftBody.h"


class palNovodexTetrahedralSoftBody : public palTetrahedralSoftBody {
public:
	palNovodexTetrahedralSoftBody();
	virtual palMatrix4x4& GetLocationMatrix() {return m_mLoc;};
	virtual void GetLinearVelocity(palVector3& velocity) {};

	virtual void GetAngularVelocity(palVector3& velocity_rad) {};

	virtual void SetLinearVelocity(palVector3 velocity) {};

	virtual void SetAngularVelocity(palVector3 velocity_rad) {};

	virtual bool IsActive() {return true;}

	virtual void SetActive(bool active) {};

	virtual int GetNumParticles();
	virtual palVector3* GetParticlePositions();

	virtual void Init(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices);
	FACTORY_CLASS(palNovodexTetrahedralSoftBody,palTetrahedralSoftBody,Novodex,1)
private:
	bool cookMesh(NxSoftBodyMeshDesc& desc);
	void releaseMeshDescBuffers(const NxSoftBodyMeshDesc& desc);
	void allocateReceiveBuffers(int numVertices, int numTetrahedra);

	NxSoftBody *mSoftBody;
	NxSoftBodyMesh *mSoftBodyMesh;
	NxMeshData mReceiveBuffers;
};


class palNovodexPatchSoftBody: public palPatchSoftBody {
	// Structure for the rendering buffer
	struct RenderBufferVertexElement
	{
		NxVec3 position;
		NxVec3 normal;
	//	float texCoord[2];
	};

public:
	palNovodexPatchSoftBody();
	virtual ~palNovodexPatchSoftBody();
	virtual palMatrix4x4& GetLocationMatrix() {return m_mLoc;};
	virtual void GetLinearVelocity(palVector3& velocity) {};

	virtual void GetAngularVelocity(palVector3& velocity_rad) {};

	virtual void SetLinearVelocity(palVector3 velocity) {};

	virtual void SetAngularVelocity(palVector3 velocity_rad) {};

	virtual bool IsActive() {return true;}

	virtual void SetActive(bool active) {};

	virtual int GetNumParticles();
	virtual palVector3* GetParticlePositions();

	virtual void Init(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices);
	virtual void SetIterations(const int nIterations) {};

	PAL_VECTOR<palVector3> pos;
	FACTORY_CLASS(palNovodexPatchSoftBody,palPatchSoftBody,Novodex,2)
private:
	bool cookMesh(NxClothMeshDesc& desc);
	void releaseMeshDescBuffers(const NxClothMeshDesc& desc);
	void allocateReceiveBuffers(int numVertices, int numTriangles);

	int m_nParticles;

	NxMeshData mReceiveBuffers;
	NxCloth *mCloth;
	NxClothMesh *mClothMesh;

	RenderBufferVertexElement* mVertexRenderBuffer;
	NxU32* mIndexRenderBuffer;

	NxU32 mMaxVertices;
	NxU32 mMaxIndices;
	NxU32 mNumIndices;
	NxU32 mNumParentIndices;
	NxU32 mNumVertices;
	NxU32 mLastNumVertices;

	NxU32 mMeshDirtyFlags;
	bool mTeared;
};

#ifdef STATIC_CALLHACK
#include "novodex_pal_static_include.h"
#endif

#endif
