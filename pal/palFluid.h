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
		Version 0.2.01: 05/02/09 - Non square grid fluid bugfix
		Version 0.2.0 : 04/02/09 - Added particle fluids and grid fluids.
		Version 0.1.02: 05/11/08 - Further documentation
		Version 0.1.01: 05/09/08 - Doxygen
		Version 0.1   : 30/12/07 - Original (alpha)
	</pre>
	\todo
		- Documentation
		- Support fluid materials
*/
#include "palBase.h"

/** A grid based fluid class. (Eulerian View)
This simulates a fluid in a grid structure.
*/
class palGridFluid : public palFactoryObject {
public:
	/** Update the fluid state*/
	virtual void Update() = 0;
};

/** A 2D "heightmap" fluid based on finite difference dampend shallow water equations
*/
class palDampendShallowFluid : public palGridFluid {
public:
	palDampendShallowFluid();
	virtual ~palDampendShallowFluid();
	/** Initializes the fluid to a given grid size with each grid cell having a specified size. Must be called before any other function.
	\param dimX The x-dimension of the grid
	\param dimY The y-dimension of the grid
	\param cellSize The real world size of the cell in the grid. The total water dimensions is [dimX*cellSize, dimY*cellSize]
	\param density The fluid density (eg: 1000)
	\param dampingFluid The damping coefficient for the fluid itself - controls the fluid's energy levels (try 0.04 - 0.0001)
	\param dampingBody_linear Linear damping (simplified version of palLiquidDrag)
	\param dampingBody_angular Angular damping (simplified version of palLiquidDrag)
	*/
	virtual void Init(int dimX = 128, int dimY = 128, float cellSize = 0.08, float density = 1000, float dampingFluid = 0.01, float dampingBody_linear = 0.02, float dampingBody_angular = 0.04);
	/** Updates the fluid.
	*/
	virtual void Update();

	virtual int Get_DimensionsX(){return m_DimX;}
	virtual int Get_DimensionsY(){return m_DimY;}
	virtual Float GetCellSize() {return m_CellSize;}
	virtual Float *GetFluidHeights() {return m_ReadBuffer;}

	virtual palVector3* GetFluidVertices();
	virtual int GetNumVertices();
private:
	void SwitchBuffers();
	void UpdateFluid();
	void UpdateInteraction(int step=4, float WaterDepth = 5);

	palVector3 *m_Vertices;
	int m_DimX,m_DimY;
	float m_FluidDampingFactor;
	float m_BodyDampingFactor_Linear;
	float m_BodyDampingFactor_Angular;
	float m_CellSize;
	float m_Density;
	float *m_Waterbuf0;
	float *m_Waterbuf1;
	float *m_ReadBuffer;
	float *m_WriteBuffer;
	int m_Count;
	int m_VertexCount;
	FACTORY_CLASS(palDampendShallowFluid,palDampendShallowFluid,*,1)
};

/** A particle based fluid class. (Lagrangian View)
This simulates a fluid composed of a number of particles.
*/
class palParticleFluid : public palFactoryObject {
public:
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


/** A smoothed particle hydrodynamics based fluid class.
*/
class palSPHFluid : public palParticleFluid {
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
};
#endif
