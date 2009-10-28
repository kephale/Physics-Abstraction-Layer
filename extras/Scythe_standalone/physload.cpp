

//Scythe builds
//	#include "stdafx.h"

#include "Vector3.h"
//Vec3 and Mat33 are the basic types used by scythe, that you will need to define
//here we just typedef to our own math classes
typedef Scythe::Vector3 Vec3;
typedef Scythe::Matrix Mat33;

#include "physLoadSave.h"
#include <memory.h>

namespace Scythe {
	typedef Vector3 NxVec3;
	typedef Matrix NxMat33;


/*
WARNING!!!
NOT THE SAME FILE AS THE SDK VERSION, KEEP THEM BOTH UPDATED
*/



PhysicsPrimitive::PhysicsPrimitive()
{
	meshData = 0; 
	fluidDrain		= false;
	triggerOnEnter	= false;
	triggerOnLeave	= false;
	triggerOnStay	= false;
	strcpy(cookedFile	, "");
}
PhysicsActor::PhysicsActor() {
	numPrimitives = 0;
	primitives	= 0;
	numModels	= 0;
	models	= 0;
	numLights	= 0;
	lights		= 0;
	function	= 0;
	boneModelID		= -1;
	bonePos	= Vec3(0,0,0);
	boneAxis[0]	= Vec3(1,0,0);
	boneAxis[1]	= Vec3(0,1,0);
	boneAxis[2]	= Vec3(0,0,1);
	isRootBone	= false;

	group			= 0;
	staticEntity	= false;
	density		= 1;
	angularDamping	= 0.05f;
	linearDamping	= 0;
	mass			= 0;
	sleepVelocity[0] = -1;
	sleepVelocity[1] = -1;
	centerOfMass[0]		= 0;
	centerOfMass[1]		= 0;
	centerOfMass[2]		= 0;

	disableFluidCollision	= false;
	disableGravity		= false;
	maxIterations		= 4;
	manualCOM = false;
}
PhysicsEntity::PhysicsEntity() {
	fileVersion = 1.4f;
	numActors = 0;
	actors = 0;
	function = 0;
	numJoints = 0;
	numCustomJoints = 0;
	customJoints = 0;
	joints = 0;
	numStrayPrimitives = 0;
	strayPrimitives = 0;
	numStrayModels = 0;
	strayModels = 0;
	numStrayLights = 0;
	strayLights = 0;
	skeletalModels = 0;
	numSkeletalModels = 0;
}
PhysicsActor::~PhysicsActor() {
	if(numLights > 0)
		delete [] lights;
	if(numModels > 0)
		delete [] models;
	if(numPrimitives > 0)
		delete [] primitives;
}
PhysicsEntity::~PhysicsEntity() {
	if(actors && numActors > 0) {
		delete [] actors;
	}
	if(joints && numJoints > 0)
		delete [] joints;
	if(strayPrimitives && numStrayPrimitives > 0)
		delete [] strayPrimitives;
	if(strayModels && numStrayModels > 0)
		delete [] strayModels;
	if(strayLights && numStrayLights > 0)
		delete [] strayLights;
}

PhysicsObjectRoot::PhysicsObjectRoot() {
}


int PhysicsEntity::loadRootObject(std::ifstream* filein, PhysicsObjectRoot* roo)
{
	filein->read((char*) &roo->axis[0], sizeof(NxVec3));
	filein->read((char*) &roo->axis[1], sizeof(NxVec3));
	filein->read((char*) &roo->axis[2], sizeof(NxVec3));
	filein->read((char*) &roo->pos, sizeof(NxVec3));
	filein->read((char*) &roo->type, sizeof(int));
	filein->read((char*) &roo->custom, sizeof(float)*8);
return 1;
}
int PhysicsEntity::readPrimitive(std::ifstream* fileout, PhysicsPrimitive* p, float fileVersion)
{
	loadRootObject(fileout, p);

	fileout->read((char*) &p->group, sizeof(int));
	fileout->read((char*) &p->size, sizeof(NxVec3));
	fileout->read((char*) &p->materialIndex, sizeof(int));
	fileout->read((char*) &p->density, sizeof(float));
	fileout->read((char*) &p->mass, sizeof(float));
	fileout->read((char*) &p->skinWidth, sizeof(float));

	int strSize = 0;
	fileout->read((char*) &strSize, sizeof(int));
	memset(p->meshFile, '\0', sizeof(char)*400);
	fileout->read((char*) p->meshFile, sizeof(char )*strSize);

	if(fileVersion >= 1.2f) {
		strSize = 0;
		memset(p->cookedFile, '\0', sizeof(char)*400);
		fileout->read((char*) &strSize, sizeof(int));
		fileout->read((char*)p->cookedFile, sizeof(char)*strSize);
	}

	fileout->read((char*) &p->useMeshFileOnly, sizeof(bool));
	fileout->read((char*) &p->useMeshData, sizeof(bool));

	if((fileVersion < 1.2f && p->useMeshData) || (fileVersion >= 1.2f && p->type == PRIMITIVE_MESH))
	{
		p->meshData = new PhysicsMesh;
		fileout->read((char*) &p->meshData->isConvex, sizeof(bool));
		fileout->read((char*) &p->meshData->numVerts, sizeof(int));
		fileout->read((char*) &p->meshData->numTris, sizeof(int));
		p->meshData->verts = new float [p->meshData->numVerts*3];
		p->meshData->normals = new float [p->meshData->numVerts*3];
		p->meshData->tris	= new unsigned short [p->meshData->numTris*3];
		fileout->read((char*) p->meshData->verts, sizeof(float)*p->meshData->numVerts * 3);
		fileout->read((char*) p->meshData->normals, sizeof(float)*p->meshData->numVerts * 3);
		fileout->read((char*) p->meshData->tris, sizeof(unsigned short)*p->meshData->numTris * 3);
		if(fileVersion >= 1.2f) {
			fileout->read((char*) &p->meshData->doubleSided, sizeof(bool));
			fileout->read((char*) &p->meshData->smooth, sizeof(bool));
		}
	}

	if(fileVersion >= 1.2f)
	{
		fileout->read((char*) &p->fluidDrain, sizeof(bool));
		fileout->read((char*) &p->triggerOnEnter, sizeof(bool));
		fileout->read((char*) &p->triggerOnStay, sizeof(bool));
		fileout->read((char*) &p->triggerOnLeave, sizeof(bool));
	}

	return 1;
}



int		PhysicsEntity::loadFile(const char* name)
{
	//load from the file
	return loadingPart( name);
}


//This is the main function responsible for loading the data from a phs file
int	 PhysicsEntity::loadingPart(const char* name)
{
	//At this stage, all pos are in world coordinates

	//we assume its a .phs file

	char 	name2[256];
	strcpy(name2, name);

	std::ifstream filein(name2, std::ios::binary);

	if(!filein.is_open())
		return -1;

	filein.read((char*) &fileVersion, sizeof(float));

	//ACTORS
	filein.read((char*) &numActors, sizeof(int));
	actors = new PhysicsActor[numActors];
	for(int i=0 ; i < numActors ; i++) {
		//the counts
		filein.read((char*) &actors[i].numPrimitives, sizeof(int));
		filein.read((char*) &actors[i].numModels, sizeof(int));
		filein.read((char*) &actors[i].numLights, sizeof(int));
		//actor data
		filein.read((char*) &actors[i].function, sizeof(int));
		filein.read((char*) &actors[i].pos, sizeof(NxVec3));		//actors pos, is same as model[0] pos
		filein.read((char*) &actors[i].type, sizeof(int));
		filein.read((char*) &actors[i].group, sizeof(int));
		filein.read((char*) &actors[i].staticEntity, sizeof(bool));
		filein.read((char*) &actors[i].density, sizeof(float));
		filein.read((char*) &actors[i].angularDamping, sizeof(float));
		filein.read((char*) &actors[i].linearDamping, sizeof(float));
		filein.read((char*) &actors[i].custom, sizeof(float)*8);
		filein.read((char*) &actors[i].mass, sizeof(float));
		filein.read((char*) &actors[i].sleepVelocity, sizeof(float)*2);
		filein.read((char*) &actors[i].centerOfMass, sizeof(NxVec3));
		filein.read((char*) &actors[i].centerOfMassOrientation, sizeof(NxMat33));

		if(fileVersion >= 1.1f) {
			filein.read((char*) &actors[i].manualCOM, sizeof(bool));
		}
		strcpy(actors[i].name, "Actor");
		if(fileVersion >= 1.2f) {
			int strSize;
			filein.read((char*) &strSize, sizeof(int));
			memset(actors[i].name, '\0', 300);
			filein.read((char*) actors[i].name, sizeof(char )*strSize);
			filein.read((char*) &actors[i].disableFluidCollision, sizeof(bool));
			filein.read((char*) &actors[i].disableGravity, sizeof(bool));
		}
		actors[i].maxIterations = 4;
		if(fileVersion >= 1.3f) {
			filein.read((char*) &actors[i].maxIterations, sizeof(int));
		}


		actors[i].boneModelID = -1;
		memset(actors[i].boneName, '\0', 300);
		if(fileVersion >= 1.4f) {
			filein.read((char*) &actors[i].boneModelID, sizeof(int));
			int strSize = 0;
			filein.read((char*) &strSize, sizeof(int));
			filein.read((char*) actors[i].boneName, sizeof(char )*strSize);
			filein.read((char*) &actors[i].bonePos, sizeof(NxVec3));
			filein.read((char*) &actors[i].boneAxis[0], sizeof(NxVec3));
			filein.read((char*) &actors[i].boneAxis[1], sizeof(NxVec3));
			filein.read((char*) &actors[i].boneAxis[2], sizeof(NxVec3));
			filein.read((char*) &actors[i].isRootBone, sizeof(bool));
		}


		//actors primitives
		actors[i].primitives = new PhysicsPrimitive[actors[i].numPrimitives];
		for(int ii = 0 ; ii < actors[i].numPrimitives ; ii++) 
		{
			readPrimitive(&filein, &actors[i].primitives[ii], fileVersion);
		}
		//models
		actors[i].models = new PhysicsModel[actors[i].numModels];
		for(int ii = 0 ; ii < actors[i].numModels ; ii++) 
		{
			loadRootObject(&filein, &actors[i].models[ii]);
			memset(actors[i].models[ii].modelName, '\0', 200);
			int strSize;
			filein.read((char*) &strSize, sizeof(int));
			filein.read((char*) actors[i].models[ii].modelName, sizeof(char )*strSize);
//			filein >> actors[i].models[ii].modelName;
//			filein.read((char*) &actors[i].models[ii].pos, sizeof(NxVec3));	//not used, models pos is actors pos
			filein.read((char*) &actors[i].models[ii].scale, sizeof(NxVec3));
		}
		//lights
		actors[i].lights = new PhysicsLight[actors[i].numLights];
		for(int ii = 0 ; ii < actors[i].numLights ; ii++) 
		{
			filein.read((char*) &actors[i].lights[ii], sizeof(PhysicsLight));
		}
	}
	numSkeletalModels = 0;
	if(fileVersion >= 1.4f)
	{
		//skeletal models are in their own list
		filein.read((char*) &numSkeletalModels, sizeof(int));
		skeletalModels = new PhysicsModel[numSkeletalModels];
		for(int ii = 0 ; ii < numSkeletalModels ; ii++) {
			loadRootObject(&filein, &skeletalModels[ii]);
			memset(skeletalModels[ii].modelName, '\0', 200);
			int strSize;
			filein.read((char*) &strSize, sizeof(int));
			filein.read((char*) skeletalModels[ii].modelName, sizeof(char )*strSize);
			filein.read((char*) &skeletalModels[ii].scale, sizeof(NxVec3));
		}
	}

	//joints
	filein.read((char*) &numJoints, sizeof(int));
	joints = new PhysicsJoint[numJoints];
	for(int i=0 ; i < numJoints ; i++) {
		//the counts
		memset(joints[i].name, '\0', 300);
		if(fileVersion < 1.2f) {
			strcpy(joints[i].name, "Joint");
			filein.read((char*) &joints[i], sizeof(PhysicsJoint)-(sizeof(char)*300));
		}
		if(fileVersion >= 1.2f) {
			filein.read((char*) &joints[i], sizeof(PhysicsJoint));
		}
	}
	//custom joints
	filein.read((char*) &numCustomJoints, sizeof(int));
	customJoints = new Physics6DOF[numCustomJoints];
	for(int i = 0 ; i < numCustomJoints ; i++) {
		//the counts
		filein.read((char*) &customJoints[i], sizeof(Physics6DOF));
	}

	//NOW THE STRAYS
	filein.read((char*) &numStrayPrimitives, sizeof(int));
	strayPrimitives = new PhysicsPrimitive[numStrayPrimitives];
	for(int ii = 0 ; ii < numStrayPrimitives ; ii++) 
	{
		readPrimitive(&filein, &strayPrimitives[ii], fileVersion);
	}

	filein.read((char*) &numStrayModels, sizeof(int));
	strayModels = new PhysicsModel[numStrayModels];
	for(int ii = 0 ; ii < numStrayModels ; ii++) {
		loadRootObject(&filein, &strayModels[ii]);
		memset(strayModels[ii].modelName, '\0', 200);
		int strSize;
		filein.read((char*) &strSize, sizeof(int));
		filein.read((char*) strayModels[ii].modelName, sizeof(char )*strSize);
		filein.read((char*) &strayModels[ii].scale, sizeof(NxVec3));
	}

	filein.read((char*) &numStrayLights, sizeof(int));
	strayLights = new PhysicsLight[numStrayLights];
	for(int ii = 0 ; ii < numStrayLights ; ii++) 
	{
		filein.read((char*) &strayLights[ii], sizeof(PhysicsLight));
	}

	filein.close();

	return 1;
}

#ifndef __SCYTHE
}
#endif