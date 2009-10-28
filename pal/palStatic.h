#ifndef PALSTATIC_H
#define PALSTATIC_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*! \file palBodyBase.h
	\brief
		PAL - Physics Abstraction Layer. 
		Static body functionality
	\author
		Adrian Boeing
	\version
	<pre>
		Version 0.1.22: 26/09/08 - Merged body type enum
		Version 0.1.21: 17/01/08 - Documentation
		Version 0.1.2 : 12/01/08 - Static compound
		Version 0.1.1 : 18/12/07 - Convex
		Version 0.1   : 16/12/07 - Original
	</pre>
	\todo
*/

#include "palBodyBase.h"


/** The base static body.
	This is only included to simplify abstraction with certain physics engines.
*/
class palStatic : virtual public palBodyBase {
public:
};


/** A static box. It has no mass.
	This class represents a simple box (eg: cube, rectangular prism) at a given position, with a given width, height, and depth.
	<img src="../pictures/cube.jpg" alt="box">
	The diagram shows the central point of the box, as well as the width,height,and depth of the box.
*/
class palStaticBox : virtual public palBoxBase, virtual public palStatic {
public:
	/**	Initializes the box.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param width The width of the box
	\param height The height of the box
	\param depth The depth of the box
	*/
	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth);

	/**	Initializes the box.
	\param pos The transformation matrix representing the position and orientation of the box
	\param width The width of the box
	\param height The height of the box
	\param depth The depth of the box
	*/
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth);
};

class palStaticConvex : virtual public palConvexBase, virtual public palStatic {
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
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices);
	
	/**
	Initializes the convex body. 
	\param pos The transformation matrix representing the position and orientation of the convex body
	\param pVertices The vertices describing the shape
	\param nVertices The number of vertices (ie: the total number of Floats / 3)
	\param mass The objects's mass
	*/
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices);

	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
};

/** A static sphere. It has no mass.
	This class represents a simple sphere at a given position, with a given radius.
	<img src="../pictures/sphere.jpg" alt="sphere">
	The diagram indicates the central point of the sphere, as well as its radius.
*/
class palStaticSphere : virtual public palSphereBase, virtual public palStatic {
public:
	/** Initializes the sphere.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param radius The sphere's radius
	\param mass The sphere's mass
	*/
	virtual void Init(Float x, Float y, Float z, Float radius);

	/** Initializes the sphere.
	\param pos The transformation matrix representing the position and orientation of the sphere
	\param radius The sphere's radius
	\param mass The sphere's mass
	*/
	virtual void Init(palMatrix4x4 &pos, Float radius);
protected:
};

/** A static capped cylinder. It has no mass.
	This class represents a simple capped cylinder at a given position, with a given radius, and length.
	<img src="../pictures/capsule.jpg" alt="cylinder">
	The diagram indicates the central point of the cylinder, as well as its length and radius.
	The default orientation of the cylinder is such that the length is specified along the "y" axis.
*/
class palStaticCapsule: virtual public palCapsuleBase, virtual public palStatic {
public:
	/** Initializes the capped cylinder.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param radius The radius of the capsule
	\param length The length of the capsule
	\param mass The capsule's mass
	*/
	virtual void Init(Float x, Float y, Float z, Float radius, Float length);

	/** Initializes the capped cylinder.
	\param pos The transformation matrix representing the position and orientation of the capsule
	\param radius The radius of the capsule
	\param length The length of the capsule
	\param mass The capsule's mass
	*/
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length);
protected:
};

class palStaticCompoundBody : virtual public palCompoundBodyBase, virtual public palStatic {
public:
	palStaticCompoundBody();
	/**
	Initializes the compound body at a given position.
	The effective orientation of the body is specified via the orientation of the geometries.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	*/
	virtual void Init(Float x, Float y, Float z);

	/**
	Initializes the compound body at a given position.
	The effective orientation of the body is specified via the orientation of the geometries.
	\param pos The transformation matrix representing the position and orientation of the compound body itself
	*/
	virtual void Init(palMatrix4x4 &pos);

	virtual palMatrix4x4& GetLocationMatrix();
	/**
	Finalizes the construction of the static compound body.
	This function must be called after all the desired geometries have been attached to the body.
	*/
	virtual void Finalize();
	
protected:
	PAL_VECTOR<palStatic *> m_DefaultFinalizeBodies;

	
	FACTORY_CLASS(palStaticCompoundBody,palStaticCompoundBody,*,1);
};

#endif


