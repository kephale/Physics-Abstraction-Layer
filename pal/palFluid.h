#ifndef PALFLUID_H
#define PALFLUID_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/** \file palFluid.h
	\brief
		PAL - Physics Abstraction Layer. 
		Fluid particles functionality
	\author
		Adrian Boeing
	\version
	<pre>
		Version 0.1.02: 05/11/08 - Further documentation
		Version 0.1.01: 05/09/08 - Doxygen
		Version 0.1   : 30/12/07 - Original (alpha)
	</pre>
	\todo
		- Documentation
		- Support fluid materials
*/
#include "palBase.h"

/** A particle based fluid class.
This simulates a fluid composed of a number of particles.
*/
class palFluid : public palFactoryObject {
public:
	/** Initializes the fluid.
	This must be the first call to create the fluid structures. This can then be followed by AddParticle and then Finalize.
	Pseudo-Example:
	<pre>
	palFluid *pf;
	pf->Init();
	pf->AddParticle();
	pf->Finalize();
	</pre>
	*/
	virtual void Init() = 0;
	/** Adds a particle to the fluid.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param vx The velocity (x)
	\param vy The velocity (y)
	\param vz The velocity (z)
	*/
	virtual void AddParticle(Float x, Float y, Float z, Float vx, Float vy, Float vz) = 0;
	/** Gets the number of particles in the fluid.
	\return The number of active particles.
	*/
	virtual int GetNumParticles() = 0;
//	virtual palVector3& GetParticlePosition(int i) = 0;

	/** Gets the particle positions in the fluid.
	\return An array containing the particle positions.
	*/
	virtual palVector3* GetParticlePositions() = 0;

	/** Finalizes the construction of the fluid.
	This must be called after the fluid has been initialized, and all the particles have been added to the fluid.
	*/
	virtual void Finalize() = 0;
};

#endif