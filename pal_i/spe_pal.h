#ifndef SPE_PAL_H
#define SPE_PAL_H

#define SPE_PAL_SDK_VERSION_MAJOR 0
#define SPE_PAL_SDK_VERSION_MINOR 0
#define SPE_PAL_SDK_VERSION_BUGFIX 43

#include <SPE.h>
#include "../pal/palFactory.h"
#include "../pal/palFluid.h"

//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. SPE implementation.
		This enables the use of Simple Physics Engine via PAL.
	Author:
		Adrian Boeing
	Revision History:
	Version 0.0.43: 18/02/09 - Public set/get for SPE functionality & documentation
	Version 0.0.42: 30/09/08 - PAL Versioning
	Version 0.0.41: 09/04/08 - Version 3.0 compatible
	Version 0.0.4 : 29/01/08 - PSD sensor
	Version 0.0.3 : 31/12/07 - fluid velocity, geom deconstructor & bugfix
	Version 0.0.2 : 30/12/07 - physics, fluid, static box, capusle, sphere
	Version 0.0.1 : 29/12/07 - physics, body, all geom, box, capsule, sphere, (convex), terrain plane, terrain mesh
	TODO:
		-bugfix convex
		-body forces/impulses
		-version
		-body sleep
	notes:
*/

#if defined(_MSC_VER)
//#pragma comment( lib, "SPE.lib")
#endif


/** SPE Physics Class
	Additionally Supports:
*/
class palSPEPhysics: public palPhysics {
public:
	palSPEPhysics();
	virtual void Init(Float gravity_x, Float gravity_y, Float gravity_z);
	virtual void Cleanup();
	const char* GetPALVersion();
	const char* GetVersion();
	//extra methods provided by SPE abilities:
	//SPE specific:
	/** Returns the current SPE World in use by PAL
		\return The current LPSPEWORLD
	*/
	LPSPEWORLD SPEGetWorld();
protected:
	virtual void Iterate(Float timestep);
	LPSPEWORLD pWorld;
	FACTORY_CLASS(palSPEPhysics,palPhysics,SPE,1)
};

/** SPE Geometry Class
*/
class palSPEGeometry : virtual public palGeometry {
public:
	palSPEGeometry();
	~palSPEGeometry();

	void GenericCreate();

	//SPE specific:
	/** Returns the SPE Shape used by PAL geometry
		\return Returns LPSPESHAPE
	*/
	LPSPESHAPE SPEGetShape() {return pShape;}
protected:
	LPSPESHAPE pShape;
};

/** SPE Body Base Class
*/
class palSPEBodyBase :virtual public palBodyBase {
public:
	palSPEBodyBase();
	~palSPEBodyBase();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetPosition(palMatrix4x4& location);
	virtual void SetMaterial(palMaterial *material);

	//SPE specific:
	/** Returns the SPE Rigid Body associated with the PAL body
		\return Returns LPSPERIGIDBODY
	*/
	LPSPERIGIDBODY SPEGetRigidBody() {return pBody;}
protected:
	LPSPERIGIDBODY pBody;
	void BuildBody(Float fx, Float fy, Float fz, Float mass, bool dynamic = true);
};

class palSPEBody :  virtual public palBody, virtual public palSPEBodyBase {
public:
	palSPEBody();
//	~palSPEBody();

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

	virtual void IsActive();
	virtual void SetActive(bool active);

	virtual void SetPosition(palMatrix4x4& location) {
		palSPEBodyBase::SetPosition(location);
	}
protected:
};



class palSPEBoxGeometry : public palSPEGeometry, public palBoxGeometry  {
public:
	palSPEBoxGeometry() {};
protected:
	FACTORY_CLASS(palSPEBoxGeometry,palBoxGeometry,SPE,1)
};

class palSPESphereGeometry : public palSphereGeometry , public palSPEGeometry {
public:
	palSPESphereGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
protected:
	FACTORY_CLASS(palSPESphereGeometry,palSphereGeometry,SPE,1)
};

class palSPECapsuleGeometry : public palCapsuleGeometry , public palSPEGeometry {
public:
	palSPECapsuleGeometry() {};
protected:
	FACTORY_CLASS(palSPECapsuleGeometry,palCapsuleGeometry,SPE,1)
};

class palSPEConvexGeometry : public palSPEGeometry, public palConvexGeometry  {
public:
	palSPEConvexGeometry();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palSPEConvexGeometry,palConvexGeometry,SPE,1)
};

class palSPEBox : public palBox, public palSPEBody {
public:
	palSPEBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by SPE abilities:
protected:
	FACTORY_CLASS(palSPEBox,palBox,SPE,1)
};

class palSPESphere : public palSphere, public palSPEBody {
public:
	palSPESphere();
	virtual void Init(Float x, Float y, Float z, Float radius, Float mass);
protected:
	FACTORY_CLASS(palSPESphere,palSphere,SPE,1)
};


class palSPECapsule : public palCapsule, public palSPEBody {
public:
	palSPECapsule();
	virtual void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
protected:
	FACTORY_CLASS(palSPECapsule,palCapsule,SPE,1)
};

class palSPEConvex : public palSPEBody, public palConvex {
public:
	palSPEConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palSPEConvex,palConvex,SPE,1)
};


class palSPEStaticBox : virtual public palStaticBox, virtual public palSPEBodyBase {
public:
	palSPEStaticBox();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth);
protected:
	FACTORY_CLASS(palSPEStaticBox,palStaticBox,SPE,1)
};


class palSPEStaticSphere : virtual public palStaticSphere, virtual public palSPEBodyBase {
public:
	palSPEStaticSphere();
	virtual void Init(palMatrix4x4 &pos, Float radius);
protected:
	FACTORY_CLASS(palSPEStaticSphere,palStaticSphere,SPE,1)
};


class palSPEStaticCapsule : public palStaticCapsule, public palSPEBodyBase {
public:
	palSPEStaticCapsule();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length);

protected:
	FACTORY_CLASS(palSPEStaticCapsule,palStaticCapsule,SPE,1)
};

class palSPETerrainPlane : public palTerrainPlane, public palSPEBodyBase {
public:
	palSPETerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
public:
	FACTORY_CLASS(palSPETerrainPlane,palTerrainPlane,SPE,1)
};

class palSPETerrainMesh : virtual public palTerrainMesh, virtual public palSPEBodyBase  {
public:
	palSPETerrainMesh();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
protected:
	FACTORY_CLASS(palSPETerrainMesh,palTerrainMesh,SPE,1)
};


class palSPEPSDSensor : public palPSDSensor {
public:
	palSPEPSDSensor();
	void Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range); //position, direction
	Float GetDistance();
protected:
	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;
	FACTORY_CLASS(palSPEPSDSensor,palPSDSensor,SPE,1)
};


/////////
class palSPEFluid : public palSPHFluid {
public:
	palSPEFluid();

	virtual void Init();
	virtual void AddParticle(Float x, Float y, Float z, Float vx, Float vy, Float vz);
	virtual int GetNumParticles();
	virtual palVector3* GetParticlePositions();
	virtual void Finalize();

	PAL_VECTOR<palVector3> pos;
	LPSPEFLUID		pFluid;

	FACTORY_CLASS(palSPEFluid,palSPHFluid,SPE,2)
};

#endif
