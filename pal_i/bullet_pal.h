#ifndef BULLET_PAL_H
#define BULLET_PAL_H

#define BULLET_PAL_SDK_VERSION_MAJOR 0
#define BULLET_PAL_SDK_VERSION_MINOR 2
#define BULLET_PAL_SDK_VERSION_BUGFIX 1

//(c) Adrian Boeing 2006, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Bullet implementation.
		This enables the use of Bullet via PAL.
	Author:
		Adrian Boeing
	Revision History:
	Version 0.2.01: 16/04/09 - Soft body tetrahedron
	Version 0.2.00: 15/04/09 - Soft body cloth
	Version 0.1.06: 18/02/09 - Public set/get for Bullet functionality & documentation
	Version 0.1.05: 14/11/08 - Bugfixed generic link to support static bodies
	Version 0.1.04: 29/10/08 - Bugfixed collision detection body
	Version 0.1.03: 10/10/08 - Fixed revolute and spherical link limits and deconstructors.
	Version 0.1.02: 07/10/08 - Multithreaded disable macro (BULLET_SINGLETHREAD)
	Version 0.1.01: 30/09/08 - PAL Version
	Version 0.1.00: 24/09/08 - Static convex body
	Version 0.0.99: 05/09/08 - Updated for Bullet 2.70, multithreaded solver
	Version 0.0.98: 14/07/08 - Compound body finalize mass & inertia method
	Version 0.0.97: 06/07/08 - Collision detection raycast
	Version 0.0.96: 05/07/08 - Collision Detection initial
	Version 0.0.95: 26/05/08 - Collision group support
	Version 0.0.94: 03/05/08 - Static compound body
	Version 0.0.93: 09/04/08 - Angular Motor
	Version 0.0.92: 13/02/08 - Static box&sphere orientation fix
	Version 0.0.91: 26/12/07 - Static sphere, capsule
	Version 0.0.9 : 17/12/07 - Base body, compound body position fix, static box, base link support
	Version 0.0.87: 15/12/07 - Body deletion.
	Version 0.0.86: 20/11/07 - PSD fix.
	Version 0.0.85: 10/11/07 - Fixed orientated plane bug
	Version 0.0.84: 09/11/07 - Fixed geometery and body location bugs
	Version 0.0.83: 07/11/07 - Added compound body
	Version 0.0.82: 28/10/07 - Updated for Bullet 2.62RC2
	Version 0.0.81: 19/10/07 - Version number request, new force system
	Version 0.0.8 : 17/10/07 - Added Generic Constraint
	Version 0.0.7 : 15/10/07 - Added PSD sensor
	Version 0.0.6 : 18/08/07 - Convex geom and body and vehicle
	Version 0.0.54: 01/08/07 - Updated for Bullet 2.55
	Version 0.0.53: 25/07/07 - Orientated plane
	Version 0.0.52: 15/07/07 - body sleep
	Version 0.0.51: 22/06/07 - body set velocity linear & angular
	Version 0.0.5 : 10/05/06 - Update for Bullet 2.50
	Version 0.0.4 : 17/11/06 - materials, terrain heightmap
	Version 0.0.3 : 16/11/06 - terrain mesh, spherical, revolute and prismatic link.
	Version 0.0.2 : 14/11/06 - boxgeom fix, sphere geom, cylinder geom, sphere, cylinder, terrainplane
	Version 0.0.1 : 13/11/06 - physics, body, boxgeom, box
	TODO:
		- raycasts
		- fix prismatic link config
		- link limits
		- collision accuracy
		- sawp terrainplane to use btStaticPlaneShape
	notes:
*/

#define BULLET_SINGLETHREAD
#include "../pal/palFactory.h"
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>
#include "../pal/palCollision.h"
#include "../pal/palSolver.h"
#include "../pal/palSoftBody.h"

#if defined(_MSC_VER)
//#ifndef NDEBUG
//#pragma comment( lib, "libbulletcollision_d.lib")
//#pragma comment( lib, "libbulletdynamics_d.lib")
//#pragma comment( lib, "libbulletmath_d.lib")
//#ifndef BULLET_SINGLETHREAD
//#pragma comment( lib, "libbulletmultithreaded_d.lib")
//#endif
//#else
//#pragma comment( lib, "libbulletcollision.lib")
//#pragma comment( lib, "libbulletdynamics.lib")
//#pragma comment( lib, "libbulletmath.lib")
//#ifndef BULLET_SINGLETHREAD
//#pragma comment( lib, "libbulletmultithreaded.lib")
//#endif
//#endif
#pragma warning(disable : 4250)
#endif

class palBulletBodyBase;

/** Bullet Physics Class
	Additionally Supports:
		- Collision Detection
		- Solver System
*/
class palBulletPhysics: public palPhysics, public palCollisionDetectionExtended, public palSolver {
	friend class palBulletSoftBody;
public:
	palBulletPhysics();
	virtual void Init(palPhysicsDesc& desc);
	virtual void Cleanup();
	const char* GetPALVersion();
	const char* GetVersion();

	//extra methods provided by Bullet abilities:
	/** Returns the current Bullet World in use by PAL
		\return A pointer to the current btDynamicsWorld
	*/
	btDynamicsWorld* BulletGetDynamicsWorld() {return m_dynamicsWorld;}
	/** Returns the current Bullet Collision Dispatcher in use by PAL
		\return A pointer to the current btCollisionDispatcher
	*/
	btCollisionDispatcher* BulletGetCollsionDispatcher() {return m_dispatcher;}

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
	virtual void SetSolverAccuracy(Float fAccuracy);
	virtual void StartIterate(Float timestep);
	virtual bool QueryIterationComplete();
	virtual void WaitForIteration();
   virtual void SetFixedTimeStep(Float fixedStep);
	virtual void SetPE(int n);
	virtual void SetSubsteps(int n);
	virtual void SetHardware(bool status);
	virtual bool GetHardware(void);

	void AddRigidBody(palBulletBodyBase* body);
	void RemoveRigidBody(palBulletBodyBase* body);


	PAL_VECTOR<unsigned long> m_CollisionMasks;
protected:

	Float m_fFixedTimeStep;
	int set_substeps;
	int set_pe;

	virtual void Iterate(Float timestep);
	btDiscreteDynamicsWorld*	m_dynamicsWorld;
	btSoftBodyWorldInfo		m_softBodyWorldInfo;
	btCollisionDispatcher*	m_dispatcher;
	FACTORY_CLASS(palBulletPhysics,palPhysics,Bullet,1)
};

/** Bullet Body Base Class
*/
class palBulletBodyBase :virtual public palBodyBase {
	friend class palBulletPhysics;
	friend class palBulletRevoluteLink;
	friend class palBulletSphericalLink;
	friend class palBulletPrismaticLink;
	friend class palBulletGenericLink;
public:
	palBulletBodyBase();
	virtual ~palBulletBodyBase();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetPosition(palMatrix4x4& location);
	virtual void SetMaterial(palMaterial *material);
	virtual palGroup GetGroup() const;
	virtual void SetGroup(palGroup group);

	//Bullet specific:
	/** Returns the Bullet Body associated with the PAL body
		\return A pointer to the btRigidBody
	*/
	btRigidBody *BulletGetRigidBody() {return m_pbtBody;}

protected:
	btRigidBody *m_pbtBody;
	btDefaultMotionState *m_pbtMotionState;
	void BuildBody(const palVector3& pos, Float mass,
							palDynamicsType dynType = PALBODY_DYNAMIC,
							btCollisionShape *btShape = NULL,
							const palVector3& inertia = palVector3::Create(1.0, 1.0, 1.0));
	void AssignDynamicsType(palDynamicsType dynType, Float mass, btVector3 inertia);
};

class palBulletBody :  virtual public palBody, virtual public palBulletBodyBase {
public:
	palBulletBody();
	~palBulletBody();
//	virtual void SetPosition(palMatrix4x4& location);
//	virtual palMatrix4x4& GetLocationMatrix();
//	virtual void SetMaterial(palMaterial *material);

//	virtual void SetForce(Float fx, Float fy, Float fz);
//	virtual void GetForce(palVector3& force);
//	virtual void SetTorque(Float tx, Float ty, Float tz);
//	virtual void GetTorque(palVector3& torque);

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
		palBulletBodyBase::SetPosition(location);
	}
protected:
//	void BuildBody(Float fx, Float fy, Float fz, Float mass);

};

class palBulletGenericBody :  virtual public palBulletBody, virtual public palGenericBody {
public:
	palBulletGenericBody();
	virtual ~palBulletGenericBody();
   virtual void Init(palMatrix4x4 &pos);
	virtual void SetDynamicsType(palDynamicsType dynType);
   virtual void SetGravityEnabled(bool enabled);
   virtual bool IsGravityEnabled();
   virtual void SetMass(Float mass);
   virtual void SetInertia(Float Ixx, Float Iyy, Float Izz);
   virtual void ConnectGeometry(palGeometry* pGeom);
   virtual void RemoveGeometry(palGeometry* pGeom);
	virtual bool IsDynamic();
	virtual bool IsKinematic();
	virtual bool IsStatic();
protected:
   FACTORY_CLASS(palBulletGenericBody, palGenericBody, Bullet, 1);
private:
	bool m_bGravityEnabled;
};

class palBulletCompoundBody : public palCompoundBody, public palBulletBody {
public:
	palBulletCompoundBody();
	virtual void SetPosition(palMatrix4x4& location);
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ);
protected:
	FACTORY_CLASS(palBulletCompoundBody,palCompoundBody,Bullet,1)
};

class palBulletStaticCompoundBody : public palStaticCompoundBody, public palBulletCompoundBody {
public:
	palBulletStaticCompoundBody();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void Finalize();
protected:
	FACTORY_CLASS(palBulletStaticCompoundBody,palStaticCompoundBody,Bullet,1)
};

/** Bullet Geometry Class
*/
class palBulletGeometry : virtual public palGeometry {
	friend class palBulletBodyBase;
	friend class palBulletCompoundBody;
	friend class palBulletStaticCompoundBody;
public:
	palBulletGeometry();
	~palBulletGeometry();
	//Bullet specific:
	/** Returns the Bullet Collision Shape used by PAL geometry
		\return A pointer to the btCollisionShape
	*/
	btCollisionShape* BulletGetCollisionShape() {return m_pbtShape;}
protected:
	btCollisionShape* m_pbtShape;
};

class palBulletBoxGeometry : public palBulletGeometry, public palBoxGeometry  {
public:
	palBulletBoxGeometry();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
	btBoxShape *m_pbtBoxShape;
protected:
	FACTORY_CLASS(palBulletBoxGeometry,palBoxGeometry,Bullet,1)
};

class palBulletSphereGeometry : public palSphereGeometry , public palBulletGeometry {
public:
	palBulletSphereGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
	btSphereShape *m_btSphereShape;
protected:
	FACTORY_CLASS(palBulletSphereGeometry,palSphereGeometry,Bullet,1)
};

class palBulletCapsuleGeometry : public palCapsuleGeometry , public palBulletGeometry {
public:
	palBulletCapsuleGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
	btCylinderShape *m_btCylinderShape;
protected:
	FACTORY_CLASS(palBulletCapsuleGeometry,palCapsuleGeometry,Bullet,1)
};

class palBulletBox : virtual public palBox, virtual public palBulletBody {
public:
	palBulletBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by Bullet abilities:
protected:
	FACTORY_CLASS(palBulletBox,palBox,Bullet,1)
};

class palBulletStaticBox : virtual public palStaticBox, virtual public palBulletBodyBase {
public:
	palBulletStaticBox();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth);
protected:
	FACTORY_CLASS(palBulletStaticBox,palStaticBox,Bullet,1)
};


class palBulletSphere : public palSphere, public palBulletBody {
public:
	palBulletSphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);
protected:
	FACTORY_CLASS(palBulletSphere,palSphere,Bullet,1)
};

class palBulletStaticSphere : virtual public palStaticSphere, virtual public palBulletBodyBase {
public:
	palBulletStaticSphere();
	virtual void Init(palMatrix4x4 &pos, Float radius);
protected:
	FACTORY_CLASS(palBulletStaticSphere,palStaticSphere,Bullet,1)
};

class palBulletCapsule : public palCapsule, public palBulletBody {
public:
	palBulletCapsule();
	virtual void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);

protected:
	FACTORY_CLASS(palBulletCapsule,palCapsule,Bullet,1)
};

class palBulletStaticCapsule : public palStaticCapsule, public palBulletBodyBase {
public:
	palBulletStaticCapsule();
	virtual void Init(Float x, Float y, Float z, Float radius, Float length);

protected:
	FACTORY_CLASS(palBulletStaticCapsule,palStaticCapsule,Bullet,1)
};

class palBulletTerrainPlane : virtual public palTerrainPlane, virtual public palBulletBodyBase  {
public:
	palBulletTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
public:
	btBoxShape *m_pbtBoxShape;
	FACTORY_CLASS(palBulletTerrainPlane,palTerrainPlane,Bullet,1)
};


class palBulletOrientatedTerrainPlane : virtual public palOrientatedTerrainPlane, virtual public palBulletBodyBase  {
public:
	palBulletOrientatedTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);
	virtual palMatrix4x4& GetLocationMatrix() {
		return palOrientatedTerrainPlane::GetLocationMatrix();
	}
public:
	btStaticPlaneShape *m_pbtPlaneShape;
	FACTORY_CLASS(palBulletOrientatedTerrainPlane,palOrientatedTerrainPlane,Bullet,1)
};

class palBulletTerrainMesh : virtual public palTerrainMesh, virtual public palBulletBodyBase  {
public:
	palBulletTerrainMesh();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
protected:
	btBvhTriangleMeshShape *m_pbtTriMeshShape;
	PAL_VECTOR<int> m_Indices;
	PAL_VECTOR<Float> m_Vertices;
	FACTORY_CLASS(palBulletTerrainMesh,palTerrainMesh,Bullet,1)
};

class palBulletTerrainHeightmap : virtual public palTerrainHeightmap, private palBulletTerrainMesh {
public:
	palBulletTerrainHeightmap();
	virtual void Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap);
protected:
	FACTORY_CLASS(palBulletTerrainHeightmap,palTerrainHeightmap,Bullet,1)
};


class palBulletSphericalLink : public palSphericalLink {
public:
	palBulletSphericalLink();
	~palBulletSphericalLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);
	virtual void SetLimits(Float cone_limit_rad, Float twist_limit_rad);

	btGeneric6DofConstraint *m_btp2p;
protected:
	FACTORY_CLASS(palBulletSphericalLink,palSphericalLink,Bullet,1)
};

class palBulletRevoluteLink: public palRevoluteLink {
public:
	palBulletRevoluteLink();
	~palBulletRevoluteLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad);

	btHingeConstraint *m_btHinge;
protected:
	FACTORY_CLASS(palBulletRevoluteLink,palRevoluteLink,Bullet,1)
};

class palBulletPrismaticLink:  public palPrismaticLink {
public:
	palBulletPrismaticLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);

	btGeneric6DofConstraint* m_btSlider;
protected:
	FACTORY_CLASS(palBulletPrismaticLink,palPrismaticLink,Bullet,1)
};


class palBulletConvexGeometry : public palBulletGeometry, public palConvexGeometry  {
public:
	palBulletConvexGeometry() {};
	~palBulletConvexGeometry() {};
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
	btConvexHullShape *m_pbtConvexShape;
protected:
	FACTORY_CLASS(palBulletConvexGeometry,palConvexGeometry,Bullet,1)
};

class palBulletConcaveGeometry : public palBulletGeometry, public palConcaveGeometry  {
public:
   palBulletConcaveGeometry() {};
   ~palBulletConcaveGeometry() {};
   virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
   btBvhTriangleMeshShape *m_pbtTriMeshShape;
protected:
   PAL_VECTOR<int> m_Indices;
   PAL_VECTOR<Float> m_Vertices;
   FACTORY_CLASS(palBulletConcaveGeometry,palConcaveGeometry,Bullet,1)
};


class palBulletConvex : public palBulletBody, public palConvex {
public:
	palBulletConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
protected:
	FACTORY_CLASS(palBulletConvex,palConvex,Bullet,1)
};


class palBulletStaticConvex: public palStaticConvex, public palBulletBodyBase {
public:
	palBulletStaticConvex();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices);
protected:
	FACTORY_CLASS(palBulletStaticConvex,palStaticConvex,Bullet,1)
};

class palBulletPSDSensor : public palPSDSensor {
public:
	palBulletPSDSensor();
	void Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range); //position, direction
	Float GetDistance();
protected:
	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;
	FACTORY_CLASS(palBulletPSDSensor,palPSDSensor,Bullet,1)
};


class palBulletGenericLink : public palGenericLink {
public:
	palBulletGenericLink();
	~palBulletGenericLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
		palVector3 linearLowerLimits,
		palVector3 linearUpperLimits,
		palVector3 angularLowerLimits,
		palVector3 angularUpperLimits);
protected:
	btGeneric6DofConstraint* genericConstraint;
	FACTORY_CLASS(palBulletGenericLink,palGenericLink,Bullet,1)
};

class palBulletAngularMotor : public palAngularMotor {
public:
	palBulletAngularMotor();
	virtual void Init(palRevoluteLink *pLink, Float Max);
	virtual void Update(Float targetVelocity);
	virtual void Apply();
protected:
	btHingeConstraint *m_bhc;
	FACTORY_CLASS(palBulletAngularMotor,palAngularMotor,Bullet,1)
};

class palBulletSoftBody: virtual public palSoftBody {
public:
	virtual palMatrix4x4& GetLocationMatrix() {return m_mLoc;};
	virtual void GetLinearVelocity(palVector3& velocity) {};

	virtual void GetAngularVelocity(palVector3& velocity_rad) {};

	virtual void SetLinearVelocity(palVector3 velocity) {};

	virtual void SetAngularVelocity(palVector3 velocity_rad) {};

	virtual bool IsActive() {return true;}

	virtual void SetActive(bool active) {};

	virtual int GetNumParticles();
	virtual palVector3* GetParticlePositions();

	btSoftBody* m_pbtSBody;
	PAL_VECTOR<palVector3> pos;
protected:
	void BulletInit(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices);
};

class palBulletPatchSoftBody: public palPatchSoftBody, public palBulletSoftBody  {
public:
	palBulletPatchSoftBody();
	virtual void Init(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices);
	virtual void SetIterations(const int nIterations) {};

	FACTORY_CLASS(palBulletPatchSoftBody,palPatchSoftBody,Bullet,1)
};


class palBulletTetrahedralSoftBody : public palTetrahedralSoftBody, public palBulletSoftBody  {
public:
	palBulletTetrahedralSoftBody();
	virtual void Init(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices);
	FACTORY_CLASS(palBulletTetrahedralSoftBody,palTetrahedralSoftBody,Bullet,1)
};

inline short int convert_group(palGroup group) {
	short int btgroup = 1 << group;
	//We don't need to use the built in group set, it's optional, and not exposed in pal.
	//btgroup = btgroup << 5;

	if (btgroup == 0)
	{
		return 1;
	}
	return btgroup;
}

inline palGroup convert_to_pal_group(short int v)
{
	static const unsigned int b[] = {0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0,
				0xFF00FF00, 0xFFFF0000};
	palGroup r = (v & b[0]) != 0;
	for (unsigned i = 3; i > 0; i--)
	{
		r |= ((v & b[i]) != 0) << i;
	}
	return r;
}


#ifdef STATIC_CALLHACK
#include "bullet_pal_static_include.h"
#endif

#endif
