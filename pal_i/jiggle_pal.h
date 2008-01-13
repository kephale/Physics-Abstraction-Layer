#ifndef JIGGLE_PAL_H
#define JIGGLE_PAL_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Jiggle implementation.
		This enables the use of JiggleLib via PAL.
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.0.32: 19/10/07 - Version number request
		Version 0.0.31: 25/07/07 - merge with Danny Rowlhouse version, completed port to 0.83 version, palOrientatedPlane
		Version 0.0.3 : 24/07/07 - version 0.83 support (materials)
		Version 0.0.22: 15/07/07 - body sleep
		Version 0.0.21: 21/06/07 - palJiggleBody::SetPosition bug found, needs bugfix, terrain mesh, body set linear&angular velocity
		Version 0.0.2 : 09/11/06 - Support for the publicly available JigLib 0.71
		Version 0.0.1 : 31/12/04 - Physics, body, geom, boxgeom, box, terrainplane, spheregeom, cylinder geom, sphere,
						cylinder, materials
	TODO:
		- fix forces
		-revolute&spherical link fixs
		-implement prismatic link
	notes:
*/
#include <jiglib.hpp>

#include "../pal/pal.h"
#include "../pal/palFactory.h"

#define JIGLIB_V 830

#if defined(_MSC_VER)
#ifndef NDEBUG
#pragma comment(lib,"JigLibDebug.lib")
#else
#if (JIGLIB_V >= 830)
#pragma comment(lib,"jiglibRelease.lib")
#else
#pragma comment(lib,"jiglib.lib")
#endif
#endif
#pragma warning(disable : 4250)
#endif

#if (JIGLIB_V >= 830)
class palJiggleMaterialUnique : public palMaterialUnique {
public:
	palJiggleMaterialUnique();
	void Init(STRING name,Float static_friction, Float kinetic_friction, Float restitution); 
	int m_idx;
	JigLib::tMaterialProperties m_jProp;
protected:
	FACTORY_CLASS(palJiggleMaterialUnique,palMaterialUnique,Jiggle,2);
};
#endif

class palJigglePhysics: public palPhysics {
public:
	palJigglePhysics();
	virtual void Init(Float gravity_x, Float gravity_y, Float gravity_z);
	virtual void Cleanup();
	const char* GetVersion();
	//extra methods provided by Jiggle abilities:
protected:
	virtual void Iterate(Float timestep);

  FACTORY_CLASS(palJigglePhysics,palPhysics,Jiggle,1)
};


class palJiggleBody : virtual public palBody {
public:
	palJiggleBody();

	virtual void SetPosition(palMatrix4x4& location);
	virtual palMatrix4x4& GetLocationMatrix();

#if 0
	virtual void ApplyTorque(Float tx, Float ty, Float tz);
	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void AddForce(Float fx, Float fy, Float fz);
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

	virtual void SetMaterial(palMaterial *material);

	virtual void SetActive(bool active);

	JigLib::tBody *m_pjBody;
protected:

};

class palJiggleGeometry : virtual public palGeometry {
public:
	palJiggleGeometry();
	JigLib::tCollisionSkin *m_pjSkin;
};

class palJiggleBoxGeometry : public palJiggleGeometry, public palBoxGeometry  {
public:
	palJiggleBoxGeometry();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
#if (JIGLIB_V < 830)
	JigLib::tBoxSkin *m_pjBoxSkin;
#else
	JigLib::tCollisionSkin *m_pjBoxSkin;
#endif
protected:
	FACTORY_CLASS(palJiggleBoxGeometry,palBoxGeometry,Jiggle,1)
};

class palJiggleSphereGeometry : public palSphereGeometry , public palJiggleGeometry {
public:
	palJiggleSphereGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
#if (JIGLIB_V < 830)
	JigLib::tSphereSkin *m_pjSphereSkin;
	#else
JigLib::tCollisionSkin *m_pjSphereSkin;	
#endif
protected:
	FACTORY_CLASS(palJiggleSphereGeometry,palSphereGeometry,Jiggle,1)
};

class palJiggleCylinderGeometry : public palCapsuleGeometry , public palJiggleGeometry {
public:
	palJiggleCylinderGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
#if (JIGLIB_V < 830)	
	JigLib::tCapsuleSkin *m_pjCapsuleSkin;
	#else
		JigLib::tCollisionSkin *m_pjCapsuleSkin;
	
#endif
protected:
	FACTORY_CLASS(palJiggleCylinderGeometry,palCapsuleGeometry,Jiggle,1)
};

class palJiggleBox : public palBox, public palJiggleBody {
public:
	palJiggleBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by Jiggle abilities:
protected:
	FACTORY_CLASS(palJiggleBox,palBox,Jiggle,1)
};


class palJiggleSphere : public palSphere, public palJiggleBody {
public:
	palJiggleSphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);
protected:
	FACTORY_CLASS(palJiggleSphere,palSphere,Jiggle,1)
};


class palJiggleCylinder : public palCapsule, public palJiggleBody {
public:
	palJiggleCylinder();
	virtual void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
	
protected:
	FACTORY_CLASS(palJiggleCylinder,palCapsule,Jiggle,1)
};

class palJiggleTerrainPlane : public palTerrainPlane {
public:
	palJiggleTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
	virtual void InitND(Float nx,Float ny, Float nz, Float d);
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material);
protected:
#if (JIGLIB_V < 830)
	JigLib::tPlaneSkin *m_pjPlaneSkin;
#else
	JigLib::tCollisionSkin *m_pjPlaneSkin;
#endif
	FACTORY_CLASS(palJiggleTerrainPlane,palTerrainPlane,Jiggle,1)
};

class palJiggleOrientatedTerrainPlane :  public palOrientatedTerrainPlane  {
public:
	palJiggleOrientatedTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);
protected:
	JigLib::tCollisionSkin *m_pjPlaneSkin;
	FACTORY_CLASS(palJiggleOrientatedTerrainPlane,palOrientatedTerrainPlane,Jiggle,1)
};

class palJiggleTerrainMesh :  virtual public palTerrainMesh {
public:
	palJiggleTerrainMesh();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetMaterial(palMaterial *material);
protected:
#if (JIGLIB_V < 830)
	JigLib::tStaticMeshSkin *m_pjMeshSkin;
#else
	JigLib::tCollisionSkin *m_pjMeshSkin;
#endif
	FACTORY_CLASS(palJiggleTerrainMesh,palTerrainMesh,Jiggle,1)
};

class palJiggleSphericalLink : public palSphericalLink {
public:
	palJiggleSphericalLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);

	virtual void SetLimits(Float cone_limit_rad, Float twist_limit_rad);
protected:
	JigLib::tConstraint *m_pjConstraint;

	FACTORY_CLASS(palJiggleSphericalLink,palSphericalLink,Jiggle,1)
};

class palJiggleRevoluteLink: public palRevoluteLink {
public:
	palJiggleRevoluteLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad); 
protected:
	JigLib::tHingeJoint *m_pjHinge;
	FACTORY_CLASS(palJiggleRevoluteLink,palRevoluteLink,Jiggle,1)
};


#endif