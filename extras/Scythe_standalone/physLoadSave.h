#pragma once

#include <fstream>
#include "Vector3.h"

namespace Scythe
{
//USE THIS IF YOU NEED IT-
//typedef float Vec3[3];
//typedef Vec3 Mat33[3];
//OTHERWISE DEFINE AS YOUR OWN MATH CLASSES-
//typedef Vector3 Vec3;
//typedef Matrix Mat33;

class PhysicsActor;
class PhysicsPrimitive;
class PhysicsModel;
class PhysicsLight;
class PhysicsJoint;
class PhysicsRootObject;

//most objects have these basic properties
class PhysicsObjectRoot
{
public:
	PhysicsObjectRoot();
	~PhysicsObjectRoot(){}

	Vec3			pos;		//offset from physentity center, which is 0,0,0
	Vec3			axis[3];
	int			type;
	float			custom[8];
};
class PhysicsSpring
{
public:
	PhysicsSpring() : active(false), value(0), spring(0), damper(0){}
	~PhysicsSpring(){}

	bool		active;
	float		value;
	float		spring;
	float		damper;
};
class PhysicsSoftLimit
{
	//similar to spring, I know
public:
	PhysicsSoftLimit(): active(false){}
	~PhysicsSoftLimit(){}

	bool		active;
	float		value;
	float		spring;
	float		damper;
	float		restitution;
};
class PhysicsMotor
{
public:
	PhysicsMotor() : active(false), targetVelocity(0), maxForce(0), freeSpinning(false) {}
	~PhysicsMotor(){}

	bool		active;
	float		targetVelocity;
	float		maxForce;
	bool		freeSpinning;
};
class PhysicsDrive
{
public:
	PhysicsDrive(): active(false){}
	~PhysicsDrive(){}

	bool	active;
	int		type;
	float	damping;
	float	forceLimit;
	float	spring;
};
class PhysicsJoint : public PhysicsObjectRoot
{
public:
	PhysicsJoint(){}
	~PhysicsJoint(){}

	int				function;		//only 1 per joint
	float			limits[4];
	bool			limitsOn;
	int			actors[2];		//the actors that are joined
	float			breakable[2];
	bool			selfCollision;
	int			maxIterations;
	//point in plane limit = all local positions
	bool			pointForActor2;	
	Vec3			pointPos;
	Vec3			planeNormal[3];		//can have up to 3 limit planes
	Vec3			planePos[3];
	//
	int				projectionMode;
	float			projectionDist;		//only hinge, ball and 6DOF
	float			projectionAngle;
	float			stiffness;			//for newton, not physX
	float			empty1;				//in case of future need
	float			empty2;
	float			empty3;
	//hinge only
	PhysicsMotor	motor;
	//hinge + ball
	PhysicsSpring	twist;
	//ball
	PhysicsSpring	swing;
	PhysicsSpring	dist;
	char			name[300];
};
class Physics6DOF : public PhysicsJoint
{
public:
	Physics6DOF(){}
	~Physics6DOF(){}

	PhysicsSoftLimit	linearLimit;
	PhysicsSoftLimit	swingLimit[2];
	PhysicsSoftLimit	twistLimit[2];
	PhysicsDrive		linearDrive[3];		///xyz
	bool				slerpDriveMode;
	PhysicsDrive		twistDrive;
	PhysicsDrive		swingDrive;
	PhysicsDrive		slerpDrive;
	Vec3				drivePosTarget;
	Mat33			driveOrientationTarget;
	Vec3				driveVelocityTarget;
	Vec3				driveAngularVelocityTarget;

};
class PhysicsLight : public PhysicsObjectRoot
{
public:
	PhysicsLight(){}
	~PhysicsLight(){}

	Vec3			RGB;
	Vec3			specular;
	float			range;
	float			innerCone;
	float			outerCone;
	float			falloff;
};
class PhysicsModel : public PhysicsObjectRoot
{
public:
	PhysicsModel(){ hasSkeleton = false; }
	~PhysicsModel(){}

	char				modelName[200];
	Vec3				scale;
	bool				hasSkeleton;
};
class PhysicsMesh
{
public:
	PhysicsMesh(){ numVerts = 0; verts=0; numTris=0; tris=0; normals = 0; }
	~PhysicsMesh() {	delete [] verts;	delete [] normals;	delete [] tris; }

	int			numVerts;
	float*			verts;
	int			numTris;
	unsigned short*	tris;
	float*			normals;
	bool			isConvex;		//else static
	bool			smooth;
	bool			doubleSided;
};
class PhysicsPrimitive : public PhysicsObjectRoot
{
public:
	PhysicsPrimitive();
	~PhysicsPrimitive(){ if(meshData) delete meshData; }

	int			group;
	Vec3			size;
	int			materialIndex;
	float			density;
	float			mass;
	float			skinWidth;

	bool			useMeshData;
	PhysicsMesh*	meshData;

	bool			useMeshFileOnly;
	
	//the original model file used to create the mesh
	char			meshFile[400];
	//this is the collision mesh filename
	//it can either be used on its own without the above mesh data to 
	//load a pre-cooked mesh file, or used with the above mesh data
	//as a unique identifier to assist with instancing
	char			cookedFile[400];

	bool			triggerOnEnter;
	bool			triggerOnStay;
	bool			triggerOnLeave;
	bool			fluidDrain;
};
class PhysicsActor : public PhysicsObjectRoot
{
public:
	PhysicsActor();
	~PhysicsActor();

	int				id;
	char				name[300];
	//list of primitives
	int				numPrimitives;
	PhysicsPrimitive*		primitives;
	int				numModels;
	PhysicsModel*		models;
	int				numLights;
	PhysicsLight*		lights;
	//bone
	int				boneModelID;
	char				boneName[300];
	Vec3				bonePos;
	Vec3				boneAxis[3];
	bool				isRootBone;		//true if this bone is the root (ie hips)

	int				function;
	//properties
	int				group;
	bool				staticEntity;
	float				density;
	float				angularDamping;
	float				linearDamping;
	float				mass;

	bool				manualCOM;			//indicates that we use the custom center of mass
	Vec3				centerOfMass;		//This is a GLOBAL position
	Mat33			centerOfMassOrientation;

	float				sleepVelocity[2];

	bool			disableGravity;
	bool			disableFluidCollision;
	int			maxIterations;
};

//THE HIERACHY:
//physicsEntity
	//list of actors
		//list of primitives
		//list of models
		//list of lights
	//list of joints
		//limits
		//springs
		//motors
	//list of custom joints
		//inherits from joint
	//list of stray primitives
	//list of stray models
	//list of stray lights
	

//our main structure that represents a physics entity
//used in file format to represent cars, ragdolls, everything
class PhysicsEntity
{
public:
	PhysicsEntity();
	~PhysicsEntity();
	
	float				fileVersion;

	//MAIN DATA TO BE LOADED
	//list of actors
	int				numActors;
	PhysicsActor*		actors;
		//list of primitives		//prims, models, lights, all part of the actor, an actor is just a solid object man
		//list of models
		//list of lights
		//list of functions
	//function (not currently used)
	int				function;
	//list of joints
	int				numJoints;
	PhysicsJoint*		joints;
	int				numCustomJoints;
	Physics6DOF*		customJoints;

	PhysicsModel*	skeletalModels;
	int			numSkeletalModels;

	//EDITOR ONLY LOADS THIS DATA -non actors, dont worry about loading them
	int			numStrayPrimitives;
	PhysicsPrimitive*	strayPrimitives;
	int			numStrayModels;
	PhysicsModel*	strayModels;
	int			numStrayLights;
	PhysicsLight*	strayLights;


	int			loadFile(const char* name);			//load a scene into game

protected:
	void			convertSceneToZAxis(bool loading = false);
	int			loadingPart(const char* name);
	int			copyToScene(const Vec3 position);				//used for loading into game
	
	int			loadRootObject(std::ifstream* filein, PhysicsObjectRoot* roo);
	int			readPrimitive(std::ifstream* fileout, PhysicsPrimitive* p, float fileVersion);
};

enum { PRIMITIVE_BOX=3, PRIMITIVE_SPHERE=4, PRIMITIVE_CAPSULE=5, PRIMITIVE_CYLINDER, PRIMITIVE_CONE, PRIMITIVE_ELIPSE, PRIMITIVE_MESH, PRIMITIVE_NULL };							//primitives[].model  if primitive else is model index
enum { LIGHT_SPOT, LIGHT_POINT };
enum { JOINT_HINGE=1, JOINT_BALL, JOINT_CYLINDRICAL, JOINT_SLIDING, JOINT_FIXED, JOINT_DISTANCE, JOINT_POINTONLINE, JOINT_CUSTOM, JOINT_UNIVERSAL };

}
