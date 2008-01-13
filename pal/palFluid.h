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
		Version 0.1   : 30/12/07 - Original (alpha)
	</pre>
	\todo
		- Support fluid materials
*/
#include "palBase.h"

class palFluid : public palFactoryObject {
public:
	virtual void Init() = 0;
	virtual void AddParticle(Float x, Float y, Float z, Float vx, Float vy, Float vz) = 0;
	virtual int GetNumParticles() = 0;
//	virtual palVector3& GetParticlePosition(int i) = 0;
	virtual palVector3* GetParticlePositions() = 0;
	virtual void Finalize() = 0;
};

#endif