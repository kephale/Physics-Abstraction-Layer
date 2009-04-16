#ifndef PALSOFTBODY_H
#define PALSOFTBODY_H
//(c) Adrian Boeing 2009, see liscence.txt (BSD liscence)
/** \file palsoftBody.h
	\brief
		PAL - Physics Abstraction Layer
		Soft Body
	\author
		Adrian Boeing
    \version
	<pre>
	Revision History:
		Version 0.1 : 15/04/09 - Softbody patches and Tetrahedral
	</pre>
	\todo
*/

#include "palBodies.h"

class palSoftBody : virtual public palBody {
public:
	virtual int GetNumParticles() = 0;
	virtual palVector3* GetParticlePositions() = 0;
};

/** A soft body patch.
	This is usefull for simulating cloth. 
	This may also simulate other closed patch systems such as balloons, depending on whether the physics engine supports this.
*/
class palPatchSoftBody :  virtual public palSoftBody {
public:
	/** Initializes the soft body patches with triangles. 
	\param pParticles A pointer to the particles positions (or vertices/nodes) that describe the patch mesh
	\param pMass A pointer to the mass for each particle (or vertex) This can be null to indicate defaults. Zero mass indicates a fixed particle/node.
	\param nParticles The number of particles (vertices). (ie: the total number of Floats / 3)
	\param pIndices A pointer to the indices which describe the patch mesh
	\param nIndices The number of indices. (ie: the number of triangles * 3)
	*/
	virtual void Init(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices) = 0;
	/**  Soft body systems typically have a seperate solver. With the SetIterations you can set the number of iterations for the soft body solver.
	\param  nIterations the number of iterations for the softbody solver.
	*/
	virtual void SetIterations(const int nIterations) = 0;
};

/** A soft body object.
	This is usefull for simulating deformable objects, such as a teddy bear.
	The mesh must be provided as connected tetrahedrons. 
*/
class palTetrahedralSoftBody : virtual public palSoftBody {
public:
	/** Initializes the soft body with tetrahedral. Each tetraheron must contain 4 nodes indexed in a counter-clockwise order
	\param pParticles A pointer to the particles positions (or vertices/nodes) that describe the mesh
	\param pMass A pointer to the mass for each particle (or vertex) This can be null to indicate defaults. 
	\param nParticles The number of particles (vertices). (ie: the total number of Floats / 3)
	\param pIndices A pointer to the indices which describe the tetrahedral mesh
	\param nIndices The number of indices. (ie: the number of tetrahedrons * 4)
	*/
	virtual void Init(const Float *pParticles, const Float *pMass, const int nParticles, const int *pIndices, const int nIndices) = 0;
protected:
	virtual int *ConvertTetrahedronToTriangles(const int *pIndices, const int nIndices);
};

#endif