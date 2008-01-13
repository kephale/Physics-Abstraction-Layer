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
		Version 0.2   : 12/01/08 - Compound body
		Version 0.1.2 : 26/12/07 - Sphere, Convex, and Capsule base
		Version 0.1.1 : 16/12/07 - Box base
		Version 0.1   : 11/12/07 - Original
	</pre>
	\todo
*/
#include "palBase.h"
#include "palMaterials.h"
#include "palGeometry.h"

#ifdef MICROSOFT_VC
#pragma warning( disable : 4250 ) //temporarily disable dominance warnings
#endif

/** The base body class.
	A body represents a object in the physics engine. 
	A body has location and may have material properties. 
	A body is usually accompianied by a geometry which represents the shape of the body. 
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

public:
	VECTOR<palGeometry *> m_Geometries; //!< The geometries which the body is constucted from

	//todo: make protected, fix pal link
	Float m_fPosX;
	Float m_fPosY;
	Float m_fPosZ;
protected:
	


	virtual void SetPosition(Float x, Float y, Float z);
	/**
	Sets the position and orientation of the body via a 4x4 transformation matrix.
	Optional override implemenation for engines that support setting the location matrix for static bodies
	\param location The transformation matrix
	*/
	virtual void SetPosition(palMatrix4x4& location); 
	palMaterial *m_pMaterial;
	palMatrix4x4 m_mLoc;
	virtual void SetGeometryBody(palGeometry *pgeom);

	void Cleanup() ; //deletes all geometries and links which reference this body
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
	virtual palGeometry *AddGeometry(STRING type); //public?

	/**
	Finalizes the construction of the compound body.
	This function must be called after all the desired geometries have been attached to the body.
	The inertia tensor is calculated via the parallel axis theorem 
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
//	virtual void impGenericInit(void *param, va_list arg_ptr); //and kill genericinit
//	Float m_fWidth;
//	Float m_fHeight;
//	Float m_fDepth;
};

/** The base convex class.
*/

class palConvexBase : virtual public palBodyBase {
public:
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);
};

/** The base sphere class.
*/
class palSphereBase : virtual public palBodyBase {
public:
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
	/** \return The radius of the sphere.*/
	Float GetRadius();
//	palSphereGeometry *m_pSphereGeom;
//	Float m_fRadius;
};

/** The base capsule class.
*/
class palCapsuleBase: virtual public palBodyBase {
public:
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
	/** \return The radius of the capsule.*/
	Float GetRadius();
	/** \return The length of the capsule.*/
	Float GetLength();
protected:
//	palCapsuleGeometry *m_pCapsuleGeom;
};

#endif
