#ifndef OPENTISSUE_PAL_H
#define OPENTISSUE_PAL_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. OpenTissue implementation.
		This enables the use of OpenTissue via PAL.
	Author:
		Adrian Boeing
	Revision History:
	Version 0.0.4 : 31/12/07 - update to OT version 0.992 (deprecated some functionality), minmax bugfix, palOpenTissueBodyBase
	Version 0.0.3 : 01/07/07 - unique materials
	Version 0.0.2 : 23/06/07 - physics, body impl, boxgeom, box, plane, sphere geom, sphere
	Version 0.0.1 : 14/05/07 - physics, body, boxgeom header
	TODO:
	   typedef SDF_Geometry<mesh_type,map_type> sdf_geometry_type;
	 boxgeom: pos&orientation
	 body: force, impulse, torque
	notes:
		EXPERIMENTAL IMPLEMENTATION
*/
#define NOMINMAX

#include <OpenTissue/configuration.h>

#include <OpenTissue/dynamics/multibody/retro/util/math/retro_default_math_policy.h>
#include <OpenTissue/dynamics/multibody/retro/util/math/retro_optimized_ublas_math_policy.h>
#include <OpenTissue/dynamics/multibody/retro/retro.h> //needs cg

#include <OpenTissue/math/random.h>

#include <OpenTissue/geometry/util/compute_box_mass_properties.h>
#include <OpenTissue/geometry/util/compute_sphere_mass_properties.h>

#include <OpenTissue/map/io/map_binary_read.h>

#include "..\pal\pal.h"
#include "..\pal\palFactory.h"

template<typename types>
  class MyCollisionDetection
    : public OpenTissue::RetroCollisionDetection<
    types
    , OpenTissue::RetroSpatialHashing
    //, OpenTissue::RetroSweepNPrune
    , OpenTissue::RetroGeometryHandler
    , OpenTissue::retro::RetroNoContactDeterminationPolicy
    , OpenTissue::RetroSingleGroupAnalysis
    >
  {};

  template< typename types  >
  class MyStepper
    : public OpenTissue::retro::RetroConstraintBasedShockPropagationStepper<
    types
    , OpenTissue::retro::RetroProjectedGaussSeidel<typename types::math_policy >
    >
  {};

//typedef OpenTissue::DefaultTypes OTTypes;
 typedef OpenTissue::retro::RetroTypes<
//        OpenTissue::retro::default_ublas_math_policy<double>
    OpenTissue::retro::optimized_ublas_math_policy<double>
    , OpenTissue::retro::RetroNoSleepyPolicy
    , MyStepper
    , MyCollisionDetection
    , OpenTissue::retro::RetroExplicitSeparateErrorCorrectionFixedStepSimulator
  > OTTypes;

 typedef OTTypes::body_type OTBody;
 typedef OTTypes::node_traits::box_type OTBox;
 typedef OTTypes::node_traits::sphere_type OTSphere;

 typedef OpenTissue::retro::RetroGravity<OTTypes>              OTGravity;

 typedef OTTypes::math_policy::real_type  OTReal;
 typedef OTTypes::material_type OTMaterial;

class palOpenTissueMaterialUnique : public palMaterialUnique {
public:
	palOpenTissueMaterialUnique();
	void Init(STRING name,Float static_friction, Float kinetic_friction, Float restitution);
	OTMaterial m_material;
	int m_idx;
protected:
	FACTORY_CLASS(palOpenTissueMaterialUnique,palMaterialUnique,OpenTissue,2);
};

class palOpenTissuePhysics: public palPhysics {
public:
	palOpenTissuePhysics();
	virtual void Init(palPhysicsDesc& desc);
	virtual void Cleanup();
	const char* GetVersion();
	//extra methods provided by OpenTissue abilities:
protected:
	virtual void Iterate(Float timestep);
	FACTORY_CLASS(palOpenTissuePhysics,palPhysics,OpenTissue,1)
	OTTypes::simulator_type              m_simulator;

	OTReal m_timestep;
};

class palOpenTissueBodyBase :virtual public palBodyBase {
public:
	palOpenTissueBodyBase();
	~palOpenTissueBodyBase();
	virtual palMatrix4x4& GetLocationMatrix();
	virtual void SetPosition(palMatrix4x4& location);
	virtual void SetMaterial(palMaterial *material);
protected:
	void BuildBody(Float fx, Float fy, Float fz, Float mass, bool dynamic = true);
	OTBody *m_potBody;
};

class palOpenTissueBody : virtual public palBody, virtual public palOpenTissueBodyBase {
public:
	palOpenTissueBody();


	virtual void SetPosition(palMatrix4x4& location) {
		palOpenTissueBodyBase::SetPosition(location);
	}

	virtual void SetForce(Float fx, Float fy, Float fz);
	virtual void GetForce(palVector3& force);

	virtual void ApplyImpulse(Float fx, Float fy, Float fz);
	virtual void ApplyAngularImpulse(Float fx, Float fy, Float fz);

	virtual void SetTorque(Float tx, Float ty, Float tz);
	virtual void GetTorque(palVector3& torque);

	virtual void GetLinearVelocity(palVector3& velocity);
	virtual void GetAngularVelocity(palVector3& velocity_rad);

	virtual void SetLinearVelocity(palVector3 velocity);
	virtual void SetAngularVelocity(palVector3 velocity_rad);

	virtual bool IsActive();
	virtual void SetActive(bool active);

protected:

};

class palOpenTissueBoxGeometry : public palBoxGeometry  {
public:
	palOpenTissueBoxGeometry();
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
	OTBox *m_potBox;
protected:
	FACTORY_CLASS(palOpenTissueBoxGeometry,palBoxGeometry,OpenTissue,1)
};

class palOpenTissueSphereGeometry : public palSphereGeometry {
public:
	palOpenTissueSphereGeometry();
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
	OTSphere *m_potSphere;
protected:
	FACTORY_CLASS(palOpenTissueSphereGeometry,palSphereGeometry,OpenTissue,1)
};



class palOpenTissueBox : public palBox, public palOpenTissueBody {
public:
	palOpenTissueBox();
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//extra methods provided by OpenTissue abilities:
protected:
	FACTORY_CLASS(palOpenTissueBox,palBox,OpenTissue,1)
};

class palOpenTissueSphere : public palSphere, public palOpenTissueBody {
public:
	palOpenTissueSphere();
	void Init(Float x, Float y, Float z, Float radius, Float mass);
protected:
	FACTORY_CLASS(palOpenTissueSphere,palSphere,OpenTissue,1)
};



class palOpenTissueTerrainPlane : virtual public palTerrainPlane, virtual public palOpenTissueBodyBase {
public:
	palOpenTissueTerrainPlane();
	virtual void Init(Float x, Float y, Float z, Float min_size);
protected:
	OTBox m_floorbox;
	FACTORY_CLASS(palOpenTissueTerrainPlane,palTerrainPlane,OpenTissue,1)
};

/*
class palOpenTissueSphericalLink : public palSphericalLink {
public:
	palOpenTissueSphericalLink();
	virtual void Init(palBody *parent, palBody *child, Float x, Float y, Float z);
	virtual void SetLimits(Float cone_limit_rad, Float twist_limit_rad);
protected:
	OTTypes::socket_type            m_socket_A;
	OTTypes::socket_type            m_socket_B;
	OTTypes::ball_type m_ball;

	FACTORY_CLASS(palOpenTissueSphericalLink,palSphericalLink,OpenTissue,1)
};
*/
#endif
