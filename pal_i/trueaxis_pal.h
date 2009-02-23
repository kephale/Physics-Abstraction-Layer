#ifndef TRUE_AXIS_PAL_H
#define TRUE_AXIS_PAL_H

#define TRUE_AXIS_PAL_SDK_VERSION_MAJOR 0
#define TRUE_AXIS_PAL_SDK_VERSION_MINOR 0
#define TRUE_AXIS_PAL_SDK_VERSION_BUGFIX 18

//(c) Adrian Boeing 2005, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. True Axis implementation.
		This enables the use of True Axis SDK via PAL.
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.0.19: 18/02/09 - Public set/get for True Axis functionality & documentation
		Version 0.0.18: 19/10/07 - Version number request
		Version 0.0.17: 26/07/07 - orientated plane
		Version 0.0.16: 15/07/07 - Body sleep
		Version 0.0.15: 22/06/07 - Added set linear and angular velocities.
		Version 0.0.14: 09/11/06 - Updated for v1.2.0.2
		Version 0.0.13: 22/02/05 - PSD sensor
		Version 0.0.12: 20/02/05 - Spherical link, TerrainMesh, TerrainHeightmap, begin RevoluteLink.
		Version 0.0.11: 18/02/05 - Cleanup body&geom
		Version 0.0.1 : 17/02/05 - basic:Body, Geom, Box Geom, Sphere Geom, Cylinder Geom, Box, Sphere, Cylinder, TerrainPlane
	TODO:
	notes:
*/
#include "Physics/Physics.h" 
#include "Physics/DynamicObject.h" 
#include "Physics/StaticObject.h" 
#include "Physics/CollisionObjectAABBMesh.h" 
#include "Physics/CollisionObjectCombo.h" 

#include "../pal/pal.h"
#include "../pal/palFactory.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4250) //dominance
//#ifndef NDEBUG
//#pragma comment(lib, "TAPhysicsDMD.lib")
//#pragma comment(lib, "TACommonDMD.lib")
//#ifdef MICROSOFT_VC_7
//#pragma comment(lib, "TA_VC7.lib")
//#endif
//#ifdef MICROSOFT_VC_8
//#pragma comment(lib, "TA_VC8.lib")
//#endif
//#else
//#pragma comment(lib, "TAPhysicsMD.lib")
//#pragma comment(lib, "TACommonMD.lib")
//#ifdef MICROSOFT_VC_7
//#pragma comment(lib, "TA_VC7.lib")
//#endif
//#ifdef MICROSOFT_VC_8
//#pragma comment(lib, "TA_VC8.lib")
//#endif
//#endif
#endif

class palTrueAxisPhysics: public palPhysics {
public:
	palTrueAxisPhysics();
	virtual void Init(Float gravity_x, Float gravity_y, Float gravity_z);
	virtual void Cleanup();
	const char* GetVersion();
	const char* GetPALVersion();
	//extra methods provided by TA abilities:
protected:
	virtual void Iterate(Float timestep);

	FACTORY_CLASS(palTrueAxisPhysics,palPhysics,TrueAxis,1)
};


class palTrueAxisBody : virtual public palBody {
public:
	palTrueAxisBody();
	~palTrueAxisBody();

	virtual void SetPosition(palMatrix4x4& location);
	virtual palMatrix4x4& GetLocationMatrix();
#if 0
	virtual void SetForce(Float fx, Float fy, Float fz);
	virtual void GetForce(palVector3& force);
	virtual void SetTorque(Float tx, Float ty, Float tz);
	virtual void GetTorque(palVector3& torque);
#endif
	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

	virtual void SetActive(bool active);

	virtual void SetMaterial(palMaterial *material);

	virtual void SetMass(Float mass);
	//TA::PhysicsObject *m_pObj;
	TA::DynamicObject *m_pDObj;
	int m_nUserGroupID;
	int m_nUserGroupItemID;
protected:
	void BuildBody(Float fx, Float fy, Float fz);
};

class palTrueAxisGeometry : virtual public palGeometry {
public:
	palTrueAxisGeometry();
	~palTrueAxisGeometry();
	TA::CollisionObjectCombo *m_pcoc;
};

class palTrueAxisBoxGeometry : public palTrueAxisGeometry, public palBoxGeometry  {
public:
	palTrueAxisBoxGeometry();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);

protected:
	FACTORY_CLASS(palTrueAxisBoxGeometry,palBoxGeometry,TrueAxis,1)
};

class palTrueAxisTerrain : virtual public palTerrain {
public:
	palTrueAxisTerrain();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material) {};
	TA::StaticObject* m_pSObj;
};


class palTrueAxisOrientatedTerrainPlane : public palOrientatedTerrainPlane, public palTrueAxisTerrain {
public:
	palTrueAxisOrientatedTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);
	virtual palMatrix4x4& GetLocationMatrix() {return palOrientatedTerrainPlane::GetLocationMatrix();}
protected:
	FACTORY_CLASS(palTrueAxisOrientatedTerrainPlane,palOrientatedTerrainPlane,TrueAxis,1)
};

class palTrueAxisTerrainPlane : public palTerrainPlane, public palTrueAxisTerrain {
public:
	palTrueAxisTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
//	virtual palMatrix4x4& GetLocationMatrix();
//	virtual void SetMaterial(palMaterial *material);
protected:
	FACTORY_CLASS(palTrueAxisTerrainPlane,palTerrainPlane,TrueAxis,1)
};

class palTrueAxisTerrainMesh : public palTerrainMesh, public palTrueAxisTerrain {
public:
	palTrueAxisTerrainMesh();
	void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
protected:
	FACTORY_CLASS(palTrueAxisTerrainMesh,palTerrainMesh,TrueAxis,1)
};

class palTrueAxisTerrainHeightmap : virtual public palTerrainHeightmap, private palTrueAxisTerrainMesh {
public:
	palTrueAxisTerrainHeightmap();
	virtual void Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap);
protected:
	FACTORY_CLASS(palTrueAxisTerrainHeightmap,palTerrainHeightmap,TrueAxis,1)
};
class palTrueAxisBox : public palBox, public palTrueAxisBody {
public:
	palTrueAxisBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
protected:
	FACTORY_CLASS(palTrueAxisBox,palBox,TrueAxis,1)
};

class palTrueAxisSphereGeometry : public palSphereGeometry , public palTrueAxisGeometry {
public:
	palTrueAxisSphereGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
protected:
	FACTORY_CLASS(palTrueAxisSphereGeometry,palSphereGeometry,TrueAxis,1)
};

class palTrueAxisSphere : public palSphere, public palTrueAxisBody {
public:
	palTrueAxisSphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);
protected:
	FACTORY_CLASS(palTrueAxisSphere,palSphere,TrueAxis,1)
};


class palTrueAxisCylinderGeometry : public palCapsuleGeometry , public palTrueAxisGeometry {
public:
	palTrueAxisCylinderGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);

protected:
	FACTORY_CLASS(palTrueAxisCylinderGeometry,palCapsuleGeometry,TrueAxis,1)
};

class palTrueAxisCylinder : public palCapsule, public palTrueAxisBody {
public:
	palTrueAxisCylinder();
	virtual void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
	
protected:
	FACTORY_CLASS(palTrueAxisCylinder,palCapsule,TrueAxis,1)
};

class palTrueAxisLink : virtual public palLink {
protected:
	void DisableContacts(palTrueAxisBody *parent, palTrueAxisBody *child);
};

class palTrueAxisSphericalLink : public palSphericalLink , public palTrueAxisLink {
public:
	palTrueAxisSphericalLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);

	virtual void SetLimits(Float cone_limit_rad, Float twist_limit_rad);
protected:

	FACTORY_CLASS(palTrueAxisSphericalLink,palSphericalLink,TrueAxis,1)
};

class palTrueAxisRevoluteLink: public palRevoluteLink , public palTrueAxisLink {
public:
	palTrueAxisRevoluteLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad); 
protected:
	FACTORY_CLASS(palTrueAxisRevoluteLink,palRevoluteLink,TrueAxis,1)
};

class palTrueAxisPSDSensor : public palPSDSensor {
public:
	palTrueAxisPSDSensor();
	void Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range); //position, direction
	Float GetDistance();
protected:
	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;
	FACTORY_CLASS(palTrueAxisPSDSensor,palPSDSensor,TrueAxis,1)
};
/*
class palNewtonContactSensor: public palContactSensor {
public:
	palNewtonContactSensor();
	void Init(palBody *body); //location and size?
	void GetContactPosition(palVector3& contact);
	palVector3 m_Contact;
protected:
	FACTORY_CLASS(palNewtonContactSensor,palContactSensor,Newton,1);
};
*/

class palTrueAxisConvexGeometry : public palTrueAxisGeometry, public palConvexGeometry  {
public:
	palTrueAxisConvexGeometry();
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palTrueAxisConvexGeometry,palConvexGeometry,TrueAxis,1)
};


class palTrueAxisConvex : public palTrueAxisBody, public palConvex {
public:
	palTrueAxisConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palTrueAxisConvex,palConvex,TrueAxis,1)
};

#endif