#ifndef BOX2D_PAL_H
#define BOX2D_PAL_H

#define BOX2D_PAL_SDK_VERSION_MAJOR 0
#define BOX2D_PAL_SDK_VERSION_MINOR 0
#define BOX2D_PAL_SDK_VERSION_BUGFIX 31

#include <Box2D/Box2D.h>
#include <pal/palFactory.h>

//(c) Adrian Boeing 2008, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Box2D implementation.
		This enables the use of Box2D via PAL.
	Author:
		Adrian Boeing
	Revision History:
	Version 0.0.32: 10/08/09 - Changed Physics Init signature
	Version 0.0.31: 15/07/08 - Compound body finalize mass & inertia method
	Version 0.0.3 : 07/07/08 - update for Box2d v 2.0.1
	Version 0.0.2 : 13/01/08 - sliders (revolute, spherical, prismatic)
	Version 0.0.1 : 12/01/08 - physics, geoms (box, sphere, convex), body (box, sphere, convex)
	TODO:
		- compound body geom rotation
		- bugfix materials
		- set mass
	notes:
*/

// apparently they renamed things in a more recent version of Box2d
typedef b2Shape b2ShapeDef;
typedef b2PolygonShape b2PolygonDef;
typedef b2CircleShape b2CircleDef;

class palBox2DPhysics: public palPhysics {
public:
	palBox2DPhysics();
	virtual void Init(const palPhysicsDesc& desc);
	virtual void Cleanup();
	const char* GetPALVersion() const;
	const char* GetVersion() const;
	//extra methods provided by Box2D abilities:
protected:
	b2World *pb2World;
	virtual void Iterate(Float timestep);
	FACTORY_CLASS(palBox2DPhysics,palPhysics,Box2D,1)
};

class palBox2DGeometry : virtual public palGeometry {
public:
	palBox2DGeometry();
	~palBox2DGeometry();

	void GenericCreate();
	static void Flatten(palMatrix4x4 &pos); //remove z

	b2ShapeDef *pbShape;
};


class palBox2DBodyBase :virtual public palBodyBase {
public:
	palBox2DBodyBase();
	~palBox2DBodyBase();
	virtual const palMatrix4x4& GetLocationMatrix() const;
	virtual void SetPosition(const palMatrix4x4& location);
	virtual void SetMaterial(palMaterial *material);

	b2BodyDef *pbBodyDef;
	b2Body* pBody;
protected:
	void BuildBody(Float fx, Float fy, Float mass, bool dynamic = true);
};

class palBox2DBody :  virtual public palBody, virtual public palBox2DBodyBase {
public:
	palBox2DBody();
//	~palBox2DBody();

	virtual void ApplyForce(Float fx, Float fy, Float fz);
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void GetLinearVelocity(palVector3& velocity) const;
	virtual void GetAngularVelocity(palVector3& velocity_rad) const;

	virtual void SetLinearVelocity(const palVector3& velocity);
	virtual void SetAngularVelocity(const palVector3& velocity_rad);

	virtual bool IsActive() const;

	virtual void SetActive(bool active);

	using palBox2DBodyBase::SetPosition;
protected:
};

class palBox2DCompoundBody : public palCompoundBody, public palBox2DBody {
public:
	palBox2DCompoundBody();
	virtual void Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ);
protected:
	FACTORY_CLASS(palBox2DCompoundBody,palCompoundBody,Box2D,1)
};

class palBox2DBoxGeometry : public palBox2DGeometry, public palBoxGeometry  {
public:
	palBox2DBoxGeometry();
	virtual void Init(const palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);

	b2PolygonDef * pbBoxShape;
protected:
	FACTORY_CLASS(palBox2DBoxGeometry,palBoxGeometry,Box2D,1)
};


class palBox2DBox : public palBox, public palBox2DBody {
public:
	palBox2DBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by Box2D abilities:
protected:
	FACTORY_CLASS(palBox2DBox,palBox,Box2D,1)
};

class palBox2DTerrainPlane : public palTerrainPlane, public palBox2DBodyBase {
public:
	palBox2DTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
public:
	FACTORY_CLASS(palBox2DTerrainPlane,palTerrainPlane,Box2D,1)
};


class palBox2DSphereGeometry : public palSphereGeometry , public palBox2DGeometry {
public:
	palBox2DSphereGeometry();
	virtual void Init(const palMatrix4x4 &pos, Float radius, Float mass);

	b2CircleDef *pbCirShape;
protected:
	FACTORY_CLASS(palBox2DSphereGeometry,palSphereGeometry,Box2D,1)
};

class palBox2DSphere : public palSphere, public palBox2DBody {
public:
	palBox2DSphere();
	virtual void Init(Float x, Float y, Float z, Float radius, Float mass);
protected:
	FACTORY_CLASS(palBox2DSphere,palSphere,Box2D,1)
};


class palBox2DConvexGeometry : public palBox2DGeometry, public palConvexGeometry  {
public:
	palBox2DConvexGeometry();
	virtual void Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);

	b2PolygonDef *pbPolyShape;
protected:
	FACTORY_CLASS(palBox2DConvexGeometry,palConvexGeometry,Box2D,1)
};

class palBox2DConvex : public palBox2DBody, public palConvex {
public:
	palBox2DConvex();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
protected:
	FACTORY_CLASS(palBox2DConvex,palConvex,Box2D,1)
};

class palBox2DStaticBox : virtual public palStaticBox, virtual public palBox2DBodyBase {
public:
	palBox2DStaticBox();
	virtual void Init(const palMatrix4x4 &pos, Float width, Float height, Float depth);
protected:
	FACTORY_CLASS(palBox2DStaticBox,palStaticBox,Box2D,1)
};

class palBox2DStaticSphere : virtual public palStaticSphere, virtual public palBox2DBodyBase {
public:
	palBox2DStaticSphere();
	virtual void Init(const palMatrix4x4 &pos, Float radius);
protected:
	FACTORY_CLASS(palBox2DStaticSphere,palStaticSphere,Box2D,1)
};
/*
class palBox2DTerrainMesh : virtual public palTerrainMesh, virtual public palBox2DBodyBase  {
public:
	palBox2DTerrainMesh();
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
protected:
	FACTORY_CLASS(palBox2DTerrainMesh,palTerrainMesh,Box2D,1)
};
*/


class palBox2DSphericalLink : public palSphericalLink {
public:
	palBox2DSphericalLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);
//	virtual void SetLimits(Float cone_limit_rad, Float twist_limit_rad);

	b2RevoluteJointDef *m_bHinge;
	b2RevoluteJoint *m_bRJoint;
protected:
	FACTORY_CLASS(palBox2DSphericalLink,palSphericalLink,Box2D,1)
};

class palBox2DRevoluteLink: public palRevoluteLink {
public:
	palBox2DRevoluteLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
//	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad);

	b2RevoluteJointDef *m_bHinge;
	b2RevoluteJoint *m_bRJoint;
protected:
	FACTORY_CLASS(palBox2DRevoluteLink,palRevoluteLink,Box2D,1)
};

class palBox2DPrismaticLink:  public palPrismaticLink {
public:
	palBox2DPrismaticLink();
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);

	b2PrismaticJointDef *m_bSlider;
	b2PrismaticJoint *m_bPJoint;
protected:
	FACTORY_CLASS(palBox2DPrismaticLink,palPrismaticLink,Box2D,1)
};



#endif
