#ifndef PALBODYBASE_H
#define PALBODYBASE_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/** \file palBodyBase.h
	\brief
		PAL - Physics Abstraction Layer.
		Body base functionality (body & geom - static or dynamic)
	\author
		Adrian Boeing
	\version
	<pre>
		Version 0.2.12: 01/10/08 - Optional indices for convex
		Version 0.2.11: 26/09/08 - Merged body type enum
		Version 0.2.1 : 26/05/08 - Collision groups
		Version 0.2   : 12/01/08 - Compound body
		Version 0.1.2 : 26/12/07 - Sphere, Convex, and Capsule base
		Version 0.1.1 : 16/12/07 - Box base
		Version 0.1   : 11/12/07 - Original
	</pre>
	\todo
*/
#include "palBase.h"
#include "palMaterials.h"
//#include "palGeometry.h"
class palGeometry;
class palSphereGeometry;
class palCapsuleGeometry;
class palBoxGeometry;
class palConvexGeometry;

#ifdef MICROSOFT_VC
#pragma warning( disable : 4250 ) //temporarily disable dominance warnings
#endif


/** The type of PAL body object.
This enumeration can be used to determine the type of object, otherwise a custom type must be found by using dynamic casts
This includes dynamic bodies, static bodies, and terrain types.
*/
typedef enum {
	PAL_BODY_NONE = 0, //!< Undefined body type
	PAL_BODY_BOX = 1,  //!<  Box body type
	PAL_BODY_SPHERE = 2, //!< Sphere body type
	PAL_BODY_CAPSULE = 3, //!< Capsule body type
	PAL_BODY_CONVEX = 4, //!< Convex body type
	PAL_BODY_COMPOUND = 5, //!< Compound body type
	PAL_BODY_GENERIC = 6, //!< Generic body type (static, kinematic or dynamic, compound)
	PAL_STATIC_NONE = 0, //!< Undefined body type
	PAL_STATIC_BOX = 101,  //!<  Box body type
	PAL_STATIC_SPHERE = 102, //!< Sphere body type
	PAL_STATIC_CAPSULE = 103, //!< Capsule body type
	PAL_STATIC_CONVEX = 104, //!< Convex body type
	PAL_STATIC_COMPOUND = 105, //!< Compound body type
	PAL_TERRAIN_NONE = 0, //!< Undefined terrain type
	PAL_TERRAIN_PLANE = 201, //!< Planar (flat) terrain type
	PAL_TERRAIN_HEIGHTMAP = 202, //!< Heightmap terrain type
	PAL_TERRAIN_MESH = 203, //!< Mesh terrain type - functionality determind by implementation
} palBaseBodyType;

typedef palBaseBodyType palBodyType;
typedef palBaseBodyType palTerrainType;
typedef palBaseBodyType palStaticBodyType;

/** The base body class.
	A body represents a object in the physics engine.
	A body has location and may have material properties.
	A body is usually accompanied by a geometry which represents the shape of the body.
	The base body does not need to have a mass, it can be a static object.
*/
class palBodyBase :  public palFactoryObject {
public:
	palBodyBase();

	/** Retrieves the position and orientation of the body as a 4x4 transformation matrix.
	*/
	virtual palMatrix4x4& GetLocationMatrix() = 0;

	/** Retrieves the position of the body as a 3 dimensional vector.
	\param pos A three dimensional vector representing the bodies position
	*/
	//i should kill this function
	virtual void GetPosition(palVector3& pos);

	/** Sets the material applied to this body.
	A material pointer can be retrieved using the palMaterials::GetMaterial() method.
	*/
	virtual void SetMaterial(palMaterial *material);

	/** @return the collision group this body belongs to.
	*/
	palGroup GetGroup() const;

	/** Sets the collision group this body belongs to.
	*/
	virtual void SetGroup(palGroup group);

	/// @return true if collsion masks as supported
	bool SupportsMasks();

	/** Sets the collision mask this body belongs to.
	*/
	virtual bool SetMask(palMask mask);

	/** @return the collision mask for this body.  It will always return all bits on if it is unsupported.
	*/
	palMask GetMask() const;

	/**
	 * Sets a pointer to a user defined object or value.
	 * @param dataPtr the pointer value to set.
	 */
	void SetUserData(void *dataPtr);

	/**
	 * @return The user data pointer.
	 */
	void *GetUserData();
public:
	PAL_VECTOR<palGeometry *> m_Geometries; //!< The geometries which the body is constructed from

	//todo: make protected, fix pal link
	Float m_fPosX;
	Float m_fPosY;
	Float m_fPosZ;

	palBaseBodyType m_Type; //!< The type of body
protected:

	void SetSupportsMasks(bool value);

	virtual void SetPosition(Float x, Float y, Float z);
	/**
	Sets the position and orientation of the body via a 4x4 transformation matrix.
	Optional override implementation for engines that support setting the location matrix for static bodies
	\param location The transformation matrix
	*/
	virtual void SetPosition(palMatrix4x4& location);
	palMaterial *m_pMaterial;
	palMatrix4x4 m_mLoc;
	palGroup m_Group;
	palMask m_Mask;
	virtual void SetGeometryBody(palGeometry *pgeom);

	void Cleanup() ; //deletes all geometries and links which reference this body
private:
	void *m_pUserData;
	bool m_bSupportsMasks;
};

class palCompoundBodyBase : virtual public palBodyBase {
public:
	/**
	Adds a sphere geometry to the compound body.
	\return Returns a newly constructed sphere geometry which must be initialised with the appropriate data.
	*/
	virtual palSphereGeometry *AddSphere();
	/**
	Adds a box geometry to the compound body
	\return Returns a newly constructed box geometry which must be initialised with the appropriate data.
	*/
	virtual palBoxGeometry *AddBox();
	/**
	Adds a capped cylinder geometry to the compound body
	\return Returns a newly constructed capped cylinder geometry which must be initialised with the appropriate data.
	*/
	virtual palCapsuleGeometry *AddCapsule();
	/**
	Adds a convex geometry to the compound body
	\return Returns a newly constructed convex geometry which must be initialised with the appropriate data.
	*/
	virtual palConvexGeometry *AddConvex();

	/**
	Adds a custom geometry type to the compound body
	\param type A string representing the name of the palGeometry object that is to be constructed and attached to the compound body
	\return Returns the newly constructed object, or null upon failure
	*/
	virtual palGeometry *AddGeometry(PAL_STRING type); //public?

	/**
	Finalizes the construction of the compound body.
	This function must be called after all the desired geometries have been attached to the body.
	It must also be called before any other method is invoked, other than the AddGeometry methods and the Init method.

	A valid compound body call would have a structure following this procedure:
	1. Init
	2. AddGeometry/Box/Sphere/Capsule/Convex
	3. Finalize
	4. Other (eg: SetMaterial)

	The inertia tensor is automatically calculated via the parallel axis theorem
	*/
	virtual void Finalize() = 0;
};

#include "palGeometry.h"

/** The base box class.
*/
class palBoxBase : virtual public palBodyBase {
public:
	/** \return The width of the box.*/
	Float GetWidth();
	/** \return The height of the box.*/
	Float GetHeight();
	/** \return The depth of the box.*/
	Float GetDepth();
protected:
	//do the default construction
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
//	palBoxGeometry *m_pBoxGeom;
//	virtual void impGenericInit(void *param, va_list arg_ptr); //and kill generic init
//	Float m_fWidth;
//	Float m_fHeight;
//	Float m_fDepth;
};

/** The base convex class.
*/

class palConvexBase : virtual public palBodyBase {
public:
protected:
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);
};

/** The base sphere class.
*/
class palSphereBase : virtual public palBodyBase {
public:
	/** \return The radius of the sphere.*/
	Float GetRadius();
protected:
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
//	palSphereGeometry *m_pSphereGeom;
//	Float m_fRadius;
};

/** The base capsule class.
*/
class palCapsuleBase: virtual public palBodyBase {
public:
	/** \return The radius of the capsule.*/
	Float GetRadius();
	/** \return The length of the capsule.*/
	Float GetLength();
protected:
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
//	palCapsuleGeometry *m_pCapsuleGeom;
};

#endif
