//(c) Adrian Boeing 2008, see liscence.txt (BSD liscence)
/*! \file palSettings.h
	\brief
		PAL - Physics Abstraction Layer. 
		Settings and configuration
	\author
		Adrian Boeing
	\version
	<pre>
		Version 0.1   : 26/05/08 - Original planning
	</pre>
	\todo
*/

/*
Parameters:

Simulation System:
proc - maximum number of proccessors physics may use
threads - maximum number of threads physics may spawn


Integrator/Solver:
mode - fixed or variable timesteps
order - eg: Euler,RK4 (higher is more accurate)
substeps - general maximum sub-steps solver may employ
substeps_collision - maximum collision sub-steps
substeps_contact - maximum contact point sub steps
substeps_relaxation - maximum relaxation substeps

coriolis - enable/disable calculation of coriolis forces

joint_stiffness - extra energy inserted to maintain connected joints

sleep_linear - Threshold of velocity to sleep
sleep_angular - Threshold of angular velocity to sleep
damping_linear - Linear damping factor 
damping_angular - Angular damping factor

Collision:
space_partition - Set the datastructure: Uniform grid, BSP, Octtree, KD tree, etc.
type - Enable/disable continuous collision detection 
skin_width - Specifies by how much shapes can interpenetrate. 

Memory:
solver_buffer - Maximumum buffer size for solver
max_rigid_bodies - Maximum number of rigid bodies in scene
max_geometries - Maximum number of geometries
max_animated - Maximum number of kinematically animated object
max_constraints - Maximum number of constrains/links in scene
max_particles - Maximum number of particles in scene

//custom allocator & free?

Other:



//specifics: Newton
NewtonSetPlatformArchitecture
NewtonSetSolverModel 
NewtonSetFrictionModel 
NewtonSetWorldSize 
NewtonBodyCoriolisForcesMode 
NewtonBodySetContinuousCollisionMode 
NewtonBodySetFreezeTreshold 
NewtonJointSetStiffness 

Ageia:
NX_SKIN_WIDTH  
NX_DEFAULT_SLEEP_LIN_VEL_SQUARED 
NX_DEFAULT_SLEEP_ANG_VEL_SQUARED 
NX_BOUNCE_THRESHOLD 
NX_CONTINUOUS_CD 
NX_CCD_EPSILON 
NX_SOLVER_CONVERGENCE_THRESHOLD
NX_IMPLICIT_SWEEP_CACHE_SIZE 
NX_CONSTANT_FLUID_MAX_PACKETS
NX_CONSTANT_FLUID_MAX_PARTICLES_PER_STEP

GaString bp = params["broadPhase"].asString();
	scene_desc.broadPhase = (bp == "coherent")?NX_BROADPHASE_COHERENT:
							(bp == "quadratic")?NX_BROADPHASE_QUADRATIC:
							(bp == "full")?NX_BROADPHASE_FULL:NX_BROADPHASE_FORCE_DWORD;

	GaString ts = params["stepMethod"].asString();
	scene_desc.timeStepMethod = (ts == "fixed")?NX_TIMESTEP_FIXED:
								(ts == "variable")?NX_TIMESTEP_VARIABLE:NX_NUM_TIMESTEP_METHODS;

TrueAxis:
enum SpacialDivisionType
	{
		SPACIAL_DIVISION_TYPE_DYNAMIC_OCTREE = 0,
		SPACIAL_DIVISION_TYPE_CTREE = 1,
		SPACIAL_DIVISION_TYPE_XY_COLLISION_GRID = 2,
		SPACIAL_DIVISION_TYPE_XZ_COLLISION_GRID = 3,
		SPACIAL_DIVISION_TYPE_YZ_COLLISION_GRID = 4,
	};

ODE:
If you have a spring constant kp and damping constant kd, then the corresponding ODE constants are: 
ERP = h kp / (h kp + kd)
CFM = 1 / (h kp + kd)
*/

class palSettings :  public palFactoryObject {
public:
	palSettings();
	virtual void SetWorldExtents(
		Float fMinX, Float fMinY, Float fMinZ,
		Float fMaxX, Float fMaxY, Float fMaxZ);

	virtual void SetPhysicsAccuracy(Float fAccuracy);//0 - fast, 1 - accurate

	virtual void SetSolverAccuracy(Float fAccuracy);//0 - fast, 1 - accurate
	virtual void SetCollisionAccuracy(Float fAccuracy);//0 - fast, 1 - accurate

	virtual bool QuerySupport(PAL_STRING parameter);
	virtual void Set(PAL_STRING parameter, PAL_STRING value);

	PAL_MAP<PAL_STRING, PAL_STRING> m_Settings;

};