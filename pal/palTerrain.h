#ifndef PALTERRAIN_H
#define PALTERRAIN_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*! \file palTerrain.h
	\brief
		PAL - Physics Abstraction Layer. 
		Terrain
	\author
		Adrian Boeing
	\version
	<pre>
		Version 0.3.4 : 28/02/09 - Added plane init in (a,b,c,d) form
		Version 0.3.31: 26/09/08 - Merged body type enum
		Version 0.3.3 : 25/07/07 - Orientated terrain plane
		Version 0.3.2 : 08/08/04 - Cleaned up & safe (hm) & types
		Version 0.3.1 : 28/07/04 - Doxygen documentation
		Version 0.3   : 04/07/04 - Split from pal.h 
	</pre>
	\todo
		- Allow specification of the orientation of the heightmap
		- Allow multiple terrains
*/
#include "palStatic.h"


/** The base terrain class.
The terrain classes maintain a static description of the environment for collision purposes.
Terrain is not necessarily restricted to "the ground", terrain can represent any static environment (eg: a house).
*/
class palTerrain : virtual public palStatic {
public:
	palTerrain();
	palTerrainType GetType();
protected:
	
};

/** A plane.
	This defines the terrain as a plane in the xz-plane.
	The representation of the plane is implementation dependent, and may be constructed from a thin box.
*/
class palTerrainPlane : virtual public palTerrain {
public:
	palTerrainPlane();
	/** Initializes a plane.
	\param x Position of the plane (x)
	\param y Position of the plane (y)
	\param z Position of the plane (z)
	\param min_size The minimum size of the plane
	*/
	virtual void Init(Float x, Float y, Float z, Float min_size);
	Float GetMinimumSize();
protected:
	void GenerateDefaultBoxGeom(Float height);
	Float m_fSize;
};

/** A plane.
	This defines the terrain as a plane defined by a point and a normal vector
	The representation of the plane is implementation dependent, and may be constructed from a thin box.
	Not all physics systems may support this.
*/
class palOrientatedTerrainPlane : virtual public palTerrain  {
public:
	palOrientatedTerrainPlane();
	/** Initializes a plane.
	\param x Position of the plane (x)
	\param y Position of the plane (y)
	\param z Position of the plane (z)
	\param nx Normal of the plane (x)
	\param ny Normal of the plane (y)
	\param nz Normal of the plane (z)
	\param min_size The minimum size of the plane
	*/
	virtual void Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size);

	virtual void Init(Float nx, Float ny, Float nz, Float d, Float min_size);
	virtual palMatrix4x4& GetLocationMatrix();
	Float m_fNormX;
	Float m_fNormY;
	Float m_fNormZ;
	Float GetMinimumSize();
protected:
	Float m_fSize;
	void CalcualteOrientationMatrixFromNormals();
	Float CalculateD();
};

/** A Heightmap
	This defines the terrain as a heightmap. The heightmap is in the xz-plane.
	<img src="../pictures/heightfield.jpg" alt="heightmap">
	The diagram illustrates a height map. The heights are spaced on an evenly partitioned grid, where each height in the input data represents a corresponding height in the resulting mesh.
*/
class palTerrainHeightmap : virtual public palTerrain {
public:
	palTerrainHeightmap();
	virtual ~palTerrainHeightmap();
	/** Initializes a heightmap terrain.
	\param x Position of the heightmapped terrain (x)
	\param y Position of the heightmapped terrain (y)
	\param z Position of the heightmapped terrain (z)
	\param width The width (size as a distance) of the terrain
	\param depth The depth (size as a distance) of the terrain
	\param terrain_data_width The number of heightmap values used to describe a slice of the terrains width.
	\param terrain_data_depth The number of heightmap values used to describe a slice of the terrains depth.
	\param pHeightmap A pointer to an array of Float values of size (terrain_data_width*terrain_data_depth) which contains all the heights of the terrain.
	*/
	virtual void Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap);
	const Float *GetHeightMap();
	Float GetWidth();
	Float GetDepth();
	int GetDataWidth();
	int GetDataDepth();
protected:
	Float m_fWidth;
	Float m_fDepth;
	int m_iDataWidth;
	int m_iDataDepth;
	Float *m_pHeightmap;
};

/** A triangle mesh 
	This defines the terrain as a triangle mesh. The structure of the mesh which is supported is dependent on the underlying physics engine implementation. Typically this represents a connected list of triangles.
*/
class palTerrainMesh : virtual public palTerrain {
public:
	palTerrainMesh();
	/** Initializes a triangle mesh terrain. 
	The triangle mesh consists of:
		- A set of vertices, which describe the location of corners in an object.
		- A set of indices, which describes how the corners are connected to form triangle surfaces in an object.
	The vertices are an array of Float values. Each vertex is assumed to be a group of 3 Float values.
	The indices are an array of integer (int) values.

	\param x Position of the mesh terrain (x)
	\param y Position of the mesh terrain (y)
	\param z Position of the mesh terrain (z)
	\param pVertices A pointer to the vertices which describe the mesh
	\param nVertices The number of vertices. (ie: the total number of Floats / 3)
	\param pIndices A pointer to the indices which describe the mesh
	\param nIndices The number of indices. (ie: the number of triangles * 3)
	*/
	virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices);
	//nVertices = number of vertices (as in number of 3 float collections, ie: the total number of floats / 3)
	//nIndives = number of indices ( as in 3* the number of triangles)
	int m_nVertices;
	int m_nIndices;
	Float *m_pVertices;
	int *m_pIndices;
};


#endif
