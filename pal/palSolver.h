#ifndef PALSOLVER_H
#define PALSOLVER_H
//(c) Adrian Boeing 2008, see liscence.txt (BSD liscence)
/** \file palSolver.h
	\brief
		PAL - Physics Abstraction Layer.
		Solver Subsystem
	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.0.21:05/09/08 - Doxygen documentation
		Version 0.0.2: 03/07/08 - Final solver planning
		Version 0.0.1: 26/05/08 - Solver planning
*/
#include "palBase.h"

/** The solver class
This exposes the physics engine solver subsystem, allowing the use of multiprocessor or hardware devices for accelerated physics.
The solver subsystem calculates the new position of the physics objects (integrator) and ensures all constraints are met (solver).
To use the physics engine in parallel, you must first configure the solver, (ie: SetPE, SetSubsteps) then create the physics engine.
To perform a multithreaded update, call the StartIterate method, then perform some other calculations until QueryIterationComplete returns true.
The performance enhancements and support gained is engine and hardware specific.
*/
class palSolver {
public:
	palSolver();

	/**	Sets the accuracy of the solver
	\param fAccuracy Ranges from 0..1, 0 indicates fast and inaccurate, 1 indicates accurate and slow.
	*/
	virtual void SetSolverAccuracy(Float fAccuracy) = 0;//0 - fast, 1 - accurate
#if 0
	/**
	Returns the current simulation time
	*/
	virtual Float GetTime() = 0;

	/**
	Returns the last timestep
	*/
	virtual Float GetLastTimestep() = 0;

	/**
	This advances the physics simulation by the specified ammount of time.
	The best usage of this parameter is determined by the physics engine implementation. Consult the implementation documentation, or the physics engine documentation.
	The safest method is to treat the timestep as a constant.

	This updates PAL information for each step
	\param timestep The increment in time since the last update
	*/
	virtual void Update(Float timestep) = 0;
#endif
	/**
	This advances the physics simulation by the specified ammount of time.

	This simply steps the entire simulation.
	eg:
	StartIterate();
	WaitForIteration();
	\param timestep The increment in time since the last update
	*/
	virtual void Iterate(Float timestep) = 0;

	/**
	This starts the calculation of the next state of the physics simulation by the specified ammount of time.

	\param timestep The increment in time since the last update
	*/
	virtual void StartIterate(Float timestep) = 0;

   /** Sets the size of the physics timestep.  Even if the timestep from iterate varies, the physics engine
       will still only step in this size increment, and try to accumulate the error for addition
   */
   virtual void SetFixedTimeStep(Float fixedStep) = 0;

   /**
	This queries whether the calculation of the next state of the physics simulation has been completed.
	*/
	virtual bool QueryIterationComplete() = 0;

	/**
	This waits for the the calculation of the next state of the physics simulation to complete.
	*/
	virtual void WaitForIteration() = 0;

	/** Set the number of concurrent physics processing elements the physics simulation may use.
	eg: Threads.
	*/
	virtual void SetPE(int n) = 0;

	/** Sets the number of substeps the solver may use.
	*/
	virtual void SetSubsteps(int n) = 0;

	/** Sets whether to take advantage of special hardware
	eg: GPU, or CELL acceleration. This may cause restrictions on what and how much can be simulated
	*/
	virtual void SetHardware(bool status) = 0;

	/** Queries whether the physics is running on special hardware
	*/
	virtual bool GetHardware(void) = 0;
};

class palSolverDefault {
public:
   /**
   This starts the calculation of the next state of the physics simulation by the specified ammount of time.

   \param timestep The increment in time since the last update
   */
   virtual void StartIterate(Float timestep);

   /** Sets the size of the physics timestep.  Even if the timestep from iterate varies, the physics engine
       will still only step in this size increment, and try to accumulate the error for addition
   */
   virtual void SetFixedTimeStep(Float fixedStep);

   /**
   This queries whether the calculation of the next state of the physics simulation has been completed.
   */
   virtual bool QueryIterationComplete();

   /**
   This waits for the the calculation of the next state of the physics simulation to complete.
   */
   virtual void WaitForIteration();
};
#endif
