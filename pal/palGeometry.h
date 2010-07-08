#ifndef PALGEOMETRY_H
#define PALGEOMETRY_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/** \file palGeometry.h
	\brief
		PAL - Physics Abstraction Layer
		Geometries
	\author
		Adrian Boeing
    \version
	<pre>
	Revision History:
		Version 0.2.13: 22/02/09 - Virtual recalculate offset positions
		Version 0.2.12: 26/09/08 - Optional indices storage for convex object
		Version 0.2.11: 05/07/08 - Geometry query for connected body
		Version 0.2.1 : 17/12/07 - Cylinder renamed to Capsule
		Version 0.2   : 15/12/07 - Geometry mesh generation
		Version 0.1.2 : 11/12/07 - Concave geometry
		Version 0.1.1 : 18/08/07 - Convex geometry
		Version 0.1   : 23/10/07 - Split from palGeometry.h
	</pre>
	\todo
		- Concave mesh mass/density/inertia calcs
		- Convex mesh mass/density/inertia calcs (I had an old paper on this once.. as in non electronic, early journal article.. see buoyancy code in subsim probably has some leftover references..) alt:Gauss's Theorem volume
		- Confirm capsule inertia calculations (infact confirm all)
		- allow correction for inertia calculations if rotated.
		- Confirm suming, is it mass or area? (transfer of axis of inertia)
		- rewrite code usign ODE inertia calculations , if these can be confirmed
*/

/** The type of geometry (shape) of (or part of) an object.
*/
typedef enum {
	PAL_GEOM_NONE = 0, //!< No geometry/undefined geometry
	PAL_GEOM_BOX = 1, //!< Box geometry
	PAL_GEOM_SPHERE = 2,//!< Sphere geometry
	PAL_GEOM_CAPSULE = 3, //!< Capped Capsule geometry
	PAL_GEOM_CONVEX = 4,  //!< Convex geometry
	PAL_GEOM_CONCAVE = 5  //!< Concave geometry
} palGeometryType;


/** The base geometry class.
	The class used to represent the shape, or part of the shape of a body (object).
	The geometry allows automated calculation of a bodies inertia properties, and provides the information required to perform collision detection.

	The geometry includes the inertia properties, as well as a mass.
*/
class palGeometry : public palFactoryObject {
	friend class palBody;
	friend class palBodyBase;
	friend class palGenericBody;
public:
	palGeometry();
	virtual ~palGeometry();
//	void SetPosition(Float x, Float y, Float z);
//	void SetPosition(Float x, Float y, Float z, Float roll, Float pitch, Float yaw);
	virtual void GetPosition(palVector3& pos);

	/** Retrieves the position and orientation of the geometry as a 4x4 transformation matrix.
	*/
	virtual palMatrix4x4& GetLocationMatrix();

	/** Retrieves the position and orientation of the geometry, relative to the attached body as a 4x4 transformation matrix.
	*/
	virtual const palMatrix4x4& GetOffsetMatrix();

	virtual void GenericInit(palMatrix4x4& location, void *param_array) = 0;

	/** Generates a set of vertices that represent an approximation to the geometry.
		This can be used for debug rendering, or generating the mesh for a convex object representation for some physics engines
		\return An array of Floats (3 Floats is 1 Vertex), or null if the functionality is unsupported or there is an error
	*/
	virtual Float *GenerateMesh_Vertices();

	/**  Generates a set of corresponding indicies to represent the approximation to the geometry.
	*/
	virtual int *GenerateMesh_Indices();

	virtual int GetNumberOfVertices();
	virtual int GetNumberOfIndices();

	virtual void SetMass(Float mass);

	virtual Float GetMass();

	/** Returns the body the geometry is attached to, if valid.
	*/
	virtual palBodyBase *GetBaseBody();

	virtual void CalculateInertia() = 0; //should be protected... :(

	/** @return the collision margin / skin width around the shape.
	 * If it returns -1.0, that means engine default or unsupported.
	 */
	virtual Float GetMargin() const;

	/** Sets the collision margin / skin width around the shape.
	 * Don't set this to 0.0, but -1.0 means engine default.
	 * @return false if this is not supported or couldn't be set.
	 */
	virtual bool SetMargin(Float margin);
protected:
	//virtual void iGenericInit(void *param,va_list arg_ptr) = 0;
	virtual void SetPosition(palMatrix4x4& location);
	//recalculates the m_mOffset (local) matrix given the specified location and body
	virtual void ReCalculateOffset();
public:

	Float m_fInertiaXX; //inertia tensor XX,YY,ZZ (identity locations)
	Float m_fInertiaYY;
	Float m_fInertiaZZ;

	palGeometryType m_Type;
protected:

	Float *m_pVertices;
	int *m_pIndices;
	int m_nVertices;
	int m_nIndices;

	palMatrix4x4 m_mLoc;
	palMatrix4x4 m_mOffset; //offset from body
	palBodyBase *m_pBody;
	Float m_fMass;
};

/** A sphere geometry.
	This represents a sphere, at a given position with a given radius and mass.
	<img src="../pictures/sphere.jpg" alt="sphere">
	The diagram indicates the central point of the sphere, as well as its radius.
*/
class palSphereGeometry : virtual public palGeometry {
public:
	palSphereGeometry();
	//palSphereGeometry(palBody *pbody);
	/** Initializes the sphere
	\param pos The transformation matrix representing the position and orientation of the sphere
	\param radius The sphere's radius
	\param mass The sphere's mass
	*/
	virtual void Init(palMatrix4x4 &pos, Float radius, Float mass);
//	virtual void Init(Float x, Float y, Float z, Float radius, Float mass);
	virtual void CalculateInertia();
	virtual void GenericInit(palMatrix4x4& location, void *param_array);
	Float m_fRadius; //< The radius of the sphere
protected:
	int hstrip;
	int vslice;
	virtual Float *GenerateMesh_Vertices();
	virtual int *GenerateMesh_Indices();
private:
	void push_back3(Float *v, Float x, Float y, Float z);
	int tppos; //temp
};

/** A box geometry
	This represents a box (eg: cube, rectangular prism) at a given position, with a given width, height, depth and mass.
	<img src="../pictures/cube.jpg" alt="box">
	The diagram shows the central point of the box, as well as the width,height,and depth of the box.
*/
class palBoxGeometry : virtual public palGeometry {
public:
	//palBoxGeometry(palBody *pbody);
	/** Initializes the box
	The center of the box is specified by its position.
	\param pos The transformation matrix representing the position and orientation of the box
	\param width The width of the box
	\param height The height of the box
	\param depth depth The depth of the box
	\param mass The box's mass
	*/
	virtual void Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass);
//	virtual void Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass);
	virtual void CalculateInertia();
	virtual void GenericInit(palMatrix4x4& location, void *param_array);

	/// Depending on when axis is up, x,y, and z maps differently to width depth, and height
	virtual palVector3 GetXYZDimensions() const;

	Float m_fWidth; //< The width of the box
	Float m_fHeight; //< The height of the box
	Float m_fDepth; //< The depth of the box
protected:
	//virtual void iGenericInit(void *param,va_list arg_ptr);

	virtual Float *GenerateMesh_Vertices();
	virtual int *GenerateMesh_Indices();
	virtual int GetNumberOfVertices();
	virtual int GetNumberOfIndices();
};

/** A capped cylinder geometry
	This represents a capped cylinder at a given position, with a given radius, length and mass
	A capped cylinder, is just like a normal cylinder, however it has rounded end pieces (ie: two hemispheres, one at the top, one at the bottom), as opposed to flat ones.
	<img src="../pictures/capsule.jpg" alt="cylinder">
	The diagram indicates the central point of the cylinder, as well as its length and radius.
	The default orientation of the cylinder is such that the length is specified along the "y" axis.
	The total length of the cylinder is the specified length plus double the radius.
*/
class palCapsuleGeometry : virtual public palGeometry {
public:
	palCapsuleGeometry();
	//palCapsuleGeometry(palBody *pbody);
	/**
	Initializes the capsule
	\param pos The transformation matrix representing the position and orientation of the Capsule
	\param radius The radius of the Capsule
	\param length The length of the Capsule
	\param mass The Capsule's mass
	????NOTE: HOW IS THE CENTER OF THE Capsule DEFINED? DIAGRAM.
	*/
	virtual void Init(palMatrix4x4 &pos, Float radius, Float length, Float mass);
//	virtual void Init(Float x, Float y, Float z, Float radius, Float length, Float mass);
	virtual void CalculateInertia();
	virtual void GenericInit(palMatrix4x4& location, void *param_array);
	Float m_fRadius; //< The radius of the Capsule
	Float m_fLength; //< The length of the Capsule
protected:
	int hstrip;
	int vslice;
	virtual Float *GenerateMesh_Vertices();
	virtual int *GenerateMesh_Indices();
private:
	void push_back3(Float *v, Float x, Float y, Float z);
	int tppos; //temp
};
/*
//moment of inertia: parallel axis theorem:
//Itotal = ICM + M*d² (ie: original inertia tensor plus mass times distance squared)
class palCompoundBody : public palBody {
	virtual void Init(Float x, Float y, Float z);
//	this:
//	virtual void AddGeometry(palGeometry *pgeom);//?
//	or:
//	virtual palGeometry *AddGeomoetry(palGeomType type); //NYET! need to pass back sphere, box, or capsule >=/
//	so:(?)
	virtual palSphereGeometry *AddSphere(); //NO (unless the pbody is made part of the constructor, and is created as a reference. >=])
//  so:
//  AddSphere(x,y,z,radius,mass); //eeevil

  //AddGeomoetry(PAL_STRING type,Float params?); /// eeeeevvil
  //AddGeomoetry(palSphereGeometry((this), x,y,z,radius mass);

  PAL_VECTOR<palGeometry *> m_Geometries;
};
*/


/** A convex geometry
	This represents a convex polygon shape at a given position constructed from a set of points.
	TODO: picture.
*/
class palConvexGeometry : virtual public palGeometry {
public:
	/**
	Initializes the convex shape.
	A convex shape constains a set of vertices, which describe the location of corners in an object.
	These vertices form a 'point cloud' describing the shape.

	The vertices are an array of Float values. Each vertex is assumed to be a group of 3 Float values.

	\param pos The transformation matrix representing the position and orientation of the convex object
	\param pVertices The vertices describing the shape
	\param nVertices The number of vertices (ie: the total number of Floats / 3)
	\param mass The objects's mass
	*/
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass);

	/**
	Initializes the convex shape with index information.
	A convex shape constains a set of vertices, which describe the location of corners in an object.
	These vertices form a 'point cloud' describing the shape.

	The vertices are an array of Float values. Each vertex is assumed to be a group of 3 Float values.
	The indices are an array of integer (int) values. These may be ignored by the physics engine.

	\param pos The transformation matrix representing the position and orientation of the convex object
	\param pVertices The vertices describing the shape
	\param nVertices The number of vertices (ie: the total number of Floats / 3)
	\param pIndices A pointer to the indices which describe the mesh
	\param nIndices The number of indices. (ie: the number of triangles * 3)
	\param mass The objects's mass
	*/
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);

	virtual void SetIndices(const int *pIndices, int nIndices);
	PAL_VECTOR<Float> m_vfVertices;

	static void GenerateHull_Indices(const Float *const srcVerts, const int nVerts, int **outIndices, int& nIndices);
protected:
	virtual void CalculateInertia();
	virtual void GenericInit(palMatrix4x4& location, void *param_array) {};
private:
	void Subexpressions(Float &w0,Float &w1,Float &w2,Float &f1,Float &f2,Float &f3,Float &g0,Float &g1,Float &g2);
	void ComputeIntegral(palVector3 p[], int tmax, int index[], Float& mass, palVector3& cm);

	virtual Float *GenerateMesh_Vertices();
	virtual int *GenerateMesh_Indices();
	virtual int GetNumberOfVertices();
};


/** A concave geometry
	This represents a concave polygon shape at a given position constructed from a set of vertices and indicies describing triangles.
	TODO: picture.
*/
class palConcaveGeometry : virtual public palGeometry {
public:
	palConcaveGeometry();
	virtual ~palConcaveGeometry();
	/** Initializes a triangle mesh.
	The triangle mesh consists of:
		- A set of vertices, which describe the location of corners in an object.
		- A set of indices, which describes how the corners are connected to form triangle surfaces in an object.
	The vertices are an array of Float values. Each vertex is assumed to be a group of 3 Float values.
	The indices are an array of integer (int) values.

	\param pos The transformation matrix representing the position and orientation of the concave object
	\param pVertices A pointer to the vertices which describe the mesh
	\param nVertices The number of vertices. (ie: the total number of Floats / 3)
	\param pIndices A pointer to the indices which describe the mesh
	\param nIndices The number of indices. (ie: the number of triangles * 3)
	\param mass The objects's mass
	*/
	virtual void Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass);

protected:
	Float *m_pUntransformedVertices;
	virtual void CalculateInertia();
	virtual void GenericInit(palMatrix4x4& location, void *param_array) {};
	virtual Float *GenerateMesh_Vertices();
};
#if 0
/** A plane geometry.
	This represents a plane, defined by a point and a normal vector
	TODO: picture.
	todo: figure out normal from a given matrix
*/
class palPlaneGeometry : virtual public palGeometry {
public:
	Float m_fNormX;
	Float m_fNormY;
	Float m_fNormZ;
	Float m_fD;
	/** Initializes a plane.
	\param pos The transformation matrix representing the position and orientation of the concave object
	\param nx Normal of the plane (x)
	\param ny Normal of the plane (y)
	\param nz Normal of the plane (z)
	\param min_size The minimum size of the plane
	*/
	virtual void Init(palMatrix4x4 &pos, Float nx, Float ny, Float nz, Float min_size);
protected:
	Float m_fSize;
	virtual void CalcualteOrientationMatrixFromNormals();
	virtual Float CalculateD();
	virtual void CalculateInertia();
};
#endif

#endif
