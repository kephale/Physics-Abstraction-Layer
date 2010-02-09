#ifndef PALBODIES_H
#define PALBODIES_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/** \file palBodies.h
	\brief
		PAL - Physics Abstraction Layer
		Bodies & Geometries
	\author
		Adrian Boeing
    \version
	<pre>
	Revision History:
		Version 0.3.9 : 20/02/09 - Generic body
		Version 0.3.82: 26/09/08 - Merged body type enum
		Version 0.3.81: 13/07/08 - Compound body finalize mass & inertia method
		Version 0.3.8 : 12/01/08 - Compound body base split
		Version 0.3.72: 13/11/07 - Explicit convex compound body support
		Version 0.3.71: 23/10/07 - Split geometry.h
		Version 0.3.7 : 18/08/07 - Added convex geom and body
		Version 0.3.61: 22/06/07 - Set linear and angular velocities
		Version 0.3.6 : 18/02/05 - Cleanup bodies
		Version 0.3.51: 13/09/04 - Angular impulse, positional impulse
		Version 0.3.5 : 05/09/04 - Impulse
		Version 0.3.43: 19/08/04 - Geometries fix
		Version 0.3.42: 29/07/04 - bugfix, docu update, geometries update
		Version 0.3.4 : 27/07/04 - Doxygen documentation
		Version 0.3.3 : 06/07/04 - add force, forcepos, torque
		Version 0.3.2 : 05/07/04 - geometries & compound body, redid generic init
		Version 0.3   : 04/07/04 - Split from pal.h
	</pre>
	\todo
		- Convex mesh mass/density/inertia calcs (I had an old paper on this once.. as in non electronic, early journal article.. see buoyancy code in subsim probably has some leftover references..) alt:Gauss's Theorem volume
		- Confirm Capsule inertia calculations (infact confirm all)
		- allow correction for inertia calculations if rotated.
		- Confirm suming, is it mass or area? (transfer of axis of inertia)
		- rewrite code usign ODE inertia calculations , if these can be confirmed
		- default init with a translation matrix.
*/

#include "palBodyBase.h"


/** The body class.
	A body represents a physical object in the physics engine. A body has location, mass (and inertia), and material properties.
	A body is usually accompianied by a geometry which represents the shape of the body.
*/
class palBody : virtual public palBodyBase {
public:
	palBody();

	/** Sets the position of the body
	\param x The x-coordinate of the body (world)
	\param y The y-coordinate of the body (world)
	\param z The z-coordinate of the body (world)
	*/
	void SetPosition(Float x, Float y, Float z);

	/** Sets the orientation of the body via a 4x4 transformation matrix.
	The location matrix may not include scaleing properties
	\param location The transformation matrix
	*/
	virtual void SetPosition(palMatrix4x4& location);

	/** Sets the position and orientation of the body
	\param x The x-coordinate of the body (world)
	\param y The y-coordinate of the body (world)
	\param z The z-coordinate of the body (world)
	\param roll The roll (rotation) about the x-axis of the body (CHECK!)
	\param pitch The pitch (rotation) about the y-axis of the body (CHECK!)
	\param yaw The yaw (rotation) about the z-axis of the body (CHECK!)
	*/
	void SetPosition(Float x, Float y, Float z, Float roll, Float pitch, Float yaw);

	/** Sets the orientation of the body
	\param roll The roll (rotation) about the x-axis of the body (CHECK!)
	\param pitch The pitch (rotation) about the y-axis of the body (CHECK!)
	\param yaw The yaw (rotation) about the z-axis of the body (CHECK!)
	*/
	void SetOrientation(Float roll, Float pitch, Float yaw);



#if 0
	/** Retrieves the position and orientation of the body as a 4x4 transformation matrix.
	*/
	virtual palMatrix4x4& GetLocationMatrix() = 0;
	/** Sets the material applied to this body.
	A material pointer can be retrieved using the palMaterials::GetMaterial() method.
	*/
	virtual void SetMaterial(palMaterial *material);
#endif
#if 0
	/** Sets the force acting on a body.
	In classical physics force is related to the acceleration of a body by \f$F=m.a\f$, where F is the force, m is the Mass, and a is the Acceleration.
	Sets the force vector applied to a body, regardless of what force was previously calculated by the physics engine.
	\param fx The force vector (x)
	\param fy The force vector (y)
	\param fz The force vector (z)
	*/
	virtual void SetForce(Float fx, Float fy, Float fz);

	/** Gets the force acting on a body.
	\param force The force vector
	*/
	virtual void GetForce(palVector3& force) = 0;

	/** Adds a force to the body.
	This applies a force with the direction and strength represented by the input vector.
	If you have a normalized vector, you will need to multiply it by the magnitude of the force to get the desired result.
	\param fx The force vector (x)
	\param fy The force vector (y)
	\param fz The force vector (z)
	*/
	virtual void AddForce(Float fx, Float fy, Float fz); //direction of force (vector);
	/** Adds a force to the body, at a specified, global, position.
	This applies a force at a given position on the body, with the direction and strength represented by the input vector.
	This has the result of adding both a force and a torque to the body (ie: results in a spin on the object).
	\param px The position (x)
	\param py The position (y)
	\param pz The position (z)
	\param fx The force vector (x)
	\param fy The force vector (y)
	\param fz The force vector (z)
	*/
	virtual void AddForceAtPosition(Float px, Float py, Float pz, Float fx, Float fy, Float fz); //direction of force (vector);

	/** Sets the torque acting on a body.
	In classical physics, torque is related to angular acceleration by \f$T=I.\alpha\f$ where T is the Torque, I is the moment of inertia, and \f$\alpha\f$ is angular acceleration.
	*/
	virtual void SetTorque(Float tx, Float ty, Float tz); //PALI
	/** Gets the torque acting on a body
	*/
	virtual void GetTorque(palVector3& torque) = 0;

	/** Adds torque to the body
	*/
	virtual void AddTorque(Float tx, Float ty, Float tz);
#endif

	/** Applies a force to the body.
	This applies a force with the direction and strength represented by the input vector.
	If you have a normalized vector, you will need to multiply it by the magnitude of the force to get the desired result.
	The force is only valid for a single update.
	\param fx The force vector (x)
	\param fy The force vector (y)
	\param fz The force vector (z)
	*/
	virtual void ApplyForce(Float fx, Float fy, Float fz); //direction of force (vector);


	/** Applies a torque on the body.
	In classical physics, torque is related to angular acceleration by \f$T=I.\alpha\f$ where T is the Torque, I is the moment of inertia, and \f$\alpha\f$ is angular acceleration.
	The torque is only valid for a single update.
	*/
	virtual void ApplyTorque(Float tx, Float ty, Float tz);

	/** Applies a force to the body, at a specified, global, position.
	This applies a force at a given position on the body, with the direction and strength represented by the input vector.
	This has the result of adding both a force and a torque to the body (ie: results in a spin on the object).
	\param px The position (x)
	\param py The position (y)
	\param pz The position (z)
	\param fx The force vector (x)
	\param fy The force vector (y)
	\param fz The force vector (z)
	*/
	virtual void ApplyForceAtPosition(Float px, Float py, Float pz, Float fx, Float fy, Float fz); //direction of force (vector);

	/** Applys a linear impulse to the body.
	In classical physics, momentum is related to velocity by \f$p=m.v\f$ where p is the Momentum, m is the Mass, and v is the Velocity.
	An impulse is simply a change in momentum.
	\param ix The impulse vector (x)
	\param iy The impulse vector (y)
	\param iz The impulse vector (z)
	*/
	virtual void ApplyImpulse(Float ix, Float iy, Float iz);


	/** Applys an an angular impulse to the body.
	This will cause a change in the angular momentum, and subsequently a change in the angular velocity.
	\param ix The impulse vector (x)
	\param iy The impulse vector (y)
	\param iz The impulse vector (z)
	*/
	virtual void ApplyAngularImpulse(Float ix, Float iy, Float iz);


	/** Applys an impulse to the body at a specified, global, position.
	\param px The position (x)
	\param py The position (y)
	\param pz The position (z)
	\param ix The impulse vector (x)
	\param iy The impulse vector (y)
	\param iz The impulse vector (z)
	*/
	virtual void ApplyImpulseAtPosition(Float px, Float py, Float pz, Float ix, Float iy, Float iz);

	/** Gets the linear velocity of the body
	*/
	virtual void GetLinearVelocity(palVector3& velocity) = 0;
	/** Gets the angular velocity of the body
	*/
	virtual void GetAngularVelocity(palVector3& velocity_rad) = 0;

	/** Sets the linear velocity of the body
	*/
	virtual void SetLinearVelocity(palVector3 velocity) = 0;

	/** Sets the angular velocity of the body
	*/
	virtual void SetAngularVelocity(palVector3 velocity_rad) = 0;

	/**
	 * @return true if this body is active, ie not sleeping or frozen
	 */
	virtual bool IsActive() = 0;

	/** Sets the body as active or sleeping
	*/
	virtual void SetActive(bool active) = 0;

//	virtual void GenericInit(palMatrix4x4& pos, void *param_array) = 0;
//	virtual void GenericInit(void *param, ...) = 0;
	//virtual void impGenericInit(void *param,va_list arg_ptr) = 0;
	//api version 2: (?)

	//virtual void AddForce(Float px, Float py, Float pz, Float fx, Float fy, Float fz); //direction of force (vector);
	//apply impulse
	//add torque

	Float m_fForceX;
	Float m_fForceY;
	Float m_fForceZ;

	Float m_fTorqueX;
	Float m_fTorqueY;
	Float m_fTorqueZ;
/*
	Float m_fPosX;
	Float m_fPosY;
	Float m_fPosZ;
*/
	Float m_fMass; //!< The total mass of the body



protected:
	/**
	 * Iterates over the vector of geometries and calculates the total moment of inertia
	 * @return a vector3 with the identity locations of the inertia tensor.
	 */
	palVector3 CalcInertiaSum(float& summedMass);

	void Cleanup() ; //deltes all geometries and links which reference this body
};




//typically either palCompoundBody or palMesh will be implemented. mesh >= compound body
/** A compound body, for representing a body composed of multiple geometries.
	This represents a given number of elementary geometry types which combine to create a more complex compound body.
	For very complex objects, consider using a mesh to represent the geometry, if it is supported by the physics engine.

	Geometries must be added to a body via its Add functions, and then the Finalize function must be called to compound the body.

	<img src="../pictures/compoundbody.jpg" alt="compound">
	The diagram indicates the central point of the geometries used to construct the body, and the central reference point of the body.
*/
class palCompoundBody : virtual public palBody, virtual public palCompoundBodyBase {
public:
	/**
	Initializes the compound body at a given position.
	The effective orientation of the body is specified via the orientation of the geometries.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	*/
	virtual void Init(Float x, Float y, Float z);
	virtual void GenericInit(palMatrix4x4& pos, void *param_array);
	virtual void Finalize();
	virtual void Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ) = 0;
protected:
	void SumInertia();
	Float m_fInertiaXX; //inertia tensor XX,YY,ZZ (identity locations)
	Float m_fInertiaYY;
	Float m_fInertiaZZ;
};


/** A convex body.
	This class represents a convex object at a given position, with a given set of points.
	TODO: picture
*/
class palConvex : virtual public palBody, virtual public palConvexBase {
public:
	/**
	Initializes the convex body.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param pVertices The vertices describing the shape
	\param nVertices The number of vertices (ie: the total number of Floats / 3)
	\param mass The objects's mass
	*/
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);

	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
protected:
	virtual void GenericInit(palMatrix4x4& pos, void *param_array) {};
};

/** A box.
	This class represents a simple box (eg: cube, rectangular prism) at a given position, with a given width, height, depth and mass.
	<img src="../pictures/cube.jpg" alt="box">
	The diagram shows the central point of the box, as well as the width,height,and depth of the box.
*/
class palBox : virtual public palBody, virtual public palBoxBase {
public:
	/**	Initializes the box.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param width The width of the box
	\param height The height of the box
	\param depth The depth of the box
	\param mass The box's mass

	The boxes orientation can be set by employing a SetPosition function.
	*/
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	//virtual void GenericInit(void *param, ...);
	virtual void GenericInit(palMatrix4x4& pos, void *param_array);
protected:
};

/** A sphere.
	This class represents a simple sphere at a given position, with a given radius and mass.
	<img src="../pictures/sphere.jpg" alt="sphere">
	The diagram indicates the central point of the sphere, as well as its radius.
*/
class palSphere : virtual public palBody, virtual public palSphereBase {
public:
	/** Initializes the sphere.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param radius The sphere's radius
	\param mass The sphere's mass
	*/
	virtual void Init(Float x, Float y, Float z, Float radius, Float mass);
	//void GenericInit(void *param, ...);
	virtual void GenericInit(palMatrix4x4& pos, void *param_array);
protected:
};

/** A capped cylinder.
	This class represents a simple capped cylinder at a given position, with a given radius, length and mass.
	<img src="../pictures/capsule.jpg" alt="cylinder">
	The diagram indicates the central point of the cylinder, as well as its length and radius.
	The default orientation of the cylinder is such that the length is specified along the "y" axis.
*/
class palCapsule: virtual public palBody, virtual public palCapsuleBase {
public:
	/** Initializes the capped cylinder.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param radius The radius of the capsule
	\param length The length of the capsule
	\param mass The capsule's mass
	*/
	virtual void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
	//void GenericInit(void *param, ...);
	virtual void GenericInit(palMatrix4x4& pos, void *param_array);
protected:
//	palCapsuleGeometry *m_pCapsuleGeom;
};

/*
class palTriangleMesh : virtual public palBody {
public:
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
	int m_nVertices;
	int m_nIndices;
	Float *m_pVertices;
	int *m_pIndices;
};
*/


enum palDynamicsType {
	PALBODY_DYNAMIC,
	PALBODY_STATIC,
	PALBODY_KINEMATIC
};

class palAutoDeactivation {
public:
	virtual bool IsAutoDeactivateEnabled() const = 0;
	virtual void SetAutoDeactivateEnabled(bool) = 0;
};

/** A generic rigid body, for representing a body (static, kinematic, or dynamic) composed of multiple geometries.
	This is a rigid body that supports every possible representation. It has similar functionality to a compound body except that additionally:
	1. The body type can be changed between static, kinematic and dynamic.
	2. The mass properties of the body can be adjusted at any point in the simulation
	3. Geometries can be added and removed at any time (ie: Finialize() is not required)

	The constructed body is dynamic by default.

	This body is only supported by the most advanced physics engines. (ie: very few)
	If you are seeking maximum portability avoid using the generic body.
*/
class palGenericBody : virtual public palBody {
public:
	palGenericBody();
	/** Initializes the body
	*/
	virtual void Init(palMatrix4x4& pos);

	/** Sets the body to be dynamic, static, or kinematic
	*/
	virtual void SetDynamicsType(palDynamicsType dynType);

	/** Sets if gravity should be enabled or disabled on this body
   */
   virtual void SetGravityEnabled(bool enabled) = 0;

   /**
    * @return true up gravity is enabled on this body.
    */
   virtual bool IsGravityEnabled() = 0;

	/** Sets the mass of the body (note: results of this function for non-dynamic bodies are undefined)
	*/
	virtual void SetMass(Float mass);

	/**	Sets the inertia tensor's principal moments of inertia
	These are the diagonal elements of the inertia tensor.
	*/
	virtual void SetInertia(Float Ixx, Float Iyy, Float Izz);

	/**  @return the diagonal values of the inertia tensor
   */
   virtual void GetInertia(Float& Ixx, Float& Iyy, Float& Izz);

	//This is hard to implement in many engines.  Leaving out for now.
#if 0
	/** Sets the center of mass position in global coordinates.
	*/
	virtual void SetCenterOfMass(palMatrix4x4& loc);
#endif

	/**	Connects a constructed geometry to the body
		(This is similar to the compound body's palCompoundBodyBase::AddGeometry() call, however no finalize is required)
	*/
	virtual void ConnectGeometry(palGeometry* pGeom);

	/**	Returns the geometries connected to the body
	*/
	virtual const PAL_VECTOR<palGeometry *>& GetGeometries();

	/**	Returns the number of geometries connected to the body
	*/
	virtual unsigned int GetNumGeometries();

	/**	Removes the geometry connected to the body
	*/
	virtual void RemoveGeometry(palGeometry* pGeom);

	virtual bool IsDynamic() const {return GetDynamicsType() == PALBODY_DYNAMIC;}
	virtual bool IsKinematic() const {return GetDynamicsType() == PALBODY_KINEMATIC;}
	virtual bool IsStatic() const {return GetDynamicsType() == PALBODY_STATIC;}

	palDynamicsType GetDynamicsType() const { return m_eDynType; };
protected:
	palDynamicsType m_eDynType;

	palMatrix4x4 m_mCOM;

	Float m_fInertiaXX;
	Float m_fInertiaYY;
	Float m_fInertiaZZ;
private:
	void InternalConnectGeometry(palGeometry* pGeom);
	void InternalDisconnectGeometry(palGeometry* pGeom);
	bool m_bInitialized;
};

#endif
