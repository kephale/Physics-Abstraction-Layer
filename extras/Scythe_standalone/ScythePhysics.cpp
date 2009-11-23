/*
Copyright (c) 2007, Pal Ruud, Adrian Boeing

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the PAL nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ScythePhysics.h"


std::vector<palBodyBase *> object;
std::map<std::string, palBodyBase *> objectnames;
palBodyBase * pBody = 0;
const float PI = 3.14159;

palBodyBase *ScythePhysics::GetBody(char *name) {
	std::map<std::string, palBodyBase *>::iterator itr;
	itr = objectnames.find(name);
	if (itr!=objectnames.end()) {
		return itr->second;
	}
	return 0;
}

void ScythePhysics::SetPosition(palMatrix4x4 &mat,	Scythe::PhysicsObjectRoot &primitive) {
				mat._11 = primitive.axis[0].x;
				mat._12 = primitive.axis[0].y;
				mat._13 = primitive.axis[0].z;
				mat._14 = 0.0;

				mat._21 = primitive.axis[1].x;
				mat._22 = primitive.axis[1].y;
				mat._23 = primitive.axis[1].z;
				mat._24 = 0.0;

				mat._31 = primitive.axis[2].x;
				mat._32 = primitive.axis[2].y;
				mat._33 = primitive.axis[2].z;
				mat._34 = 0.0;


				// position of primitives
				mat._41 = primitive.pos.x;
				mat._42 = primitive.pos.y;
				mat._43 = primitive.pos.z;
				mat._44 = 1.0;

}

std::vector<palBodyBase *> ScythePhysics::loadScythePhysics(const char * fileName) {

	Scythe::PhysicsEntity * physEntity = new Scythe::PhysicsEntity();

	int load = physEntity->loadFile(fileName);

	if (load < 0) {
		printf("Could not load Scythe physics file: %s\n", fileName);
		exit(0);
	}
	
	// number of actors
	int numAct = physEntity->numActors;
	int numJoints = physEntity->numJoints;
	printf("Number of joints=%d\n", numJoints);

	printf("Number of actors=%d\n", numAct);

	
	
	// actors
	for (int j = 0; j < numAct; j++) {
		int numPrim = physEntity->actors[j].numPrimitives;
		printf("Actor[%d]: '%s'\n", j, physEntity->actors[j].name);
		printf("Actor[%d]: Number of primitives=%d\n", j, numPrim);

		
		if (numPrim < 2) {
			// primitives
			for (int i = 0; i < numPrim; i++) {
				// box = 3
				if (physEntity->actors[j].primitives[i].type == 3) {
					palBoxBase * pbBox = 0;
					if (physEntity->actors[j].staticEntity == true)
						pbBox = dynamic_cast<palBoxBase *>(PF->CreateObject("palStaticBox"));
					else
						pbBox = PF->CreateBox();

					palBox *pBox = dynamic_cast<palBox *>(pbBox);

			
					// if mass is zero, use density to get scythe examples running
					if (pBox)
					if (physEntity->actors[j].primitives[i].mass == 0) {
						pBox->Init(
								physEntity->actors[j].primitives[i].pos.x,		// pos x
								physEntity->actors[j].primitives[i].pos.y,		// pos y
								physEntity->actors[j].primitives[i].pos.z,		// pos z
								physEntity->actors[j].primitives[i].size.x,		// width
								physEntity->actors[j].primitives[i].size.y,		// depth
								physEntity->actors[j].primitives[i].size.z,		// height
								physEntity->actors[j].primitives[i].density);		// mass
					}
					else {
						pBox->Init(
							physEntity->actors[j].primitives[i].pos.x,		// pos x
							physEntity->actors[j].primitives[i].pos.y,		// pos y
							physEntity->actors[j].primitives[i].pos.z,		// pos z
							physEntity->actors[j].primitives[i].size.x,		// width
							physEntity->actors[j].primitives[i].size.y,		// depth
							physEntity->actors[j].primitives[i].size.z,		// height
							physEntity->actors[j].primitives[i].mass);		// mass
					}
					
					if (!pBox) {
						palStaticBox *psBox = dynamic_cast<palStaticBox *>(pbBox);
						psBox->Init(
								physEntity->actors[j].primitives[i].pos.x,		// pos x
								physEntity->actors[j].primitives[i].pos.y,		// pos y
								physEntity->actors[j].primitives[i].pos.z,		// pos z
								physEntity->actors[j].primitives[i].size.x,		// width
								physEntity->actors[j].primitives[i].size.y,		// depth
								physEntity->actors[j].primitives[i].size.z);	// height
					}

					pBody = pbBox;
					
				}
				// sphere = 4
				else if (physEntity->actors[j].primitives[i].type == 4) {
					palSphere * pSphere = PF->CreateSphere();
					printf("Mass=%f (sphere)\n", physEntity->actors[j].mass);
					printf("Density=%f (sphere)\n", physEntity->actors[j].density);
					// if mass is zero, use density to get scythe examples running
					if (physEntity->actors[j].primitives[i].mass == 0) {
						pSphere->Init(
							physEntity->actors[j].primitives[i].pos.x,		// pos x
							physEntity->actors[j].primitives[i].pos.y,		// pos y
							physEntity->actors[j].primitives[i].pos.z,		// pos z
							physEntity->actors[j].primitives[i].size.x/2,		// radius
							physEntity->actors[j].primitives[i].density);		// mass
					}
					else {
						pSphere->Init(
							physEntity->actors[j].primitives[i].pos.x,		// pos x
							physEntity->actors[j].primitives[i].pos.y,		// pos y
							physEntity->actors[j].primitives[i].pos.z,		// pos z
							physEntity->actors[j].primitives[i].size.x/2,		// radius
							physEntity->actors[j].primitives[i].mass);		// mass
					}

					pBody = pSphere;
					

				}
				// capsule = 5 or cylinder = 6
				else if (physEntity->actors[j].primitives[i].type == 5 || physEntity->actors[j].primitives[i].type == 6) {
					palCapsule * pCapsule = PF->CreateCapsule();
					printf("Mass=%f (capsule/cylinder)\n", physEntity->actors[j].mass);
					printf("Density=%f (capsule/cylinder)\n", physEntity->actors[j].density);
					// if mass is zero, use density to get scythe examples running
					if (physEntity->actors[j].primitives[i].mass == 0) {
						pCapsule->Init(
							physEntity->actors[j].primitives[i].pos.x,		// pos x
							physEntity->actors[j].primitives[i].pos.y,		// pos y
							physEntity->actors[j].primitives[i].pos.z,		// pos z
							physEntity->actors[j].primitives[i].size.x/2,		// radius
							physEntity->actors[j].primitives[i].size.y,		// length
							physEntity->actors[j].primitives[i].density);		// mass
					}
					else {
						pCapsule->Init(
							physEntity->actors[j].primitives[i].pos.x,		// pos x
							physEntity->actors[j].primitives[i].pos.y,		// pos y
							physEntity->actors[j].primitives[i].pos.z,		// pos z
							physEntity->actors[j].primitives[i].size.x/2,		// radius
							physEntity->actors[j].primitives[i].size.y,		// length
							physEntity->actors[j].primitives[i].mass);		// mass
					}

					pBody = pCapsule;
					
	
				}
				// cone = 7
				else if (physEntity->actors[j].primitives[i].type == 7) {
					printf("!!! Found unknown/unsupported primitive, ignoring it !!!\n");
				}
				// elipse = 8
				else if (physEntity->actors[j].primitives[i].type == 8) {
					printf("!!! Found unknown/unsupported primitive, ignoring it !!!\n");
				}
				// mesh = 9
				else if (physEntity->actors[j].primitives[i].type == 9) {
					palConvex * pConvex = dynamic_cast<palConvex *>(PF->CreateObject("palConvex"));
					pConvex->Init(
						physEntity->actors[j].primitives[i].pos.x,				// pos x
						physEntity->actors[j].primitives[i].pos.y,				// pos y
						physEntity->actors[j].primitives[i].pos.z,				// pos z
						physEntity->actors[j].primitives[i].meshData->verts,		// vertices
						physEntity->actors[j].primitives[i].meshData->numVerts,	// number of vertices
						physEntity->actors[j].primitives[i].mass);				// mass

					pBody = pConvex;

					// setting rotation right
					pConvex->SetOrientation(0,PI/2,0);
					

				}
			
				// null = 10
				else if (physEntity->actors[j].primitives[i].type == 10) {
					printf("!!! Found unknown/unsupported primitive, ignoring it !!!\n");
				}
				else {
					printf("!!! Found unknown/unsupported primitive, ignoring it !!!\n");
				}

				// set rotation
				palMatrix4x4 mat;
				SetPosition(mat,physEntity->actors[j].primitives[i]);
				
				palBody *pdBody = 0;
				pdBody = dynamic_cast<palBody *> (pBody);
				if (pdBody)
					pdBody->SetPosition(mat);

					
				// adding primitive to vector
				object.push_back(pBody);
				objectnames.insert(std::make_pair(physEntity->actors[j].name,pBody));
			}
		}
		// compound body
		else {
			palCompoundBodyBase * pCompoundBody = 0;
			palStaticCompoundBody *pStatic = 0;
			palCompoundBody *pDynamic = 0;

	if (physEntity->actors[j].staticEntity == true)
				pCompoundBody = dynamic_cast<palCompoundBodyBase *>(PF->CreateObject("palStaticCompoundBody"));
			else//*/
				pCompoundBody = PF->CreateCompoundBody();
	
			pStatic = dynamic_cast<palStaticCompoundBody *>(pCompoundBody);
			pDynamic = dynamic_cast<palCompoundBody *>(pCompoundBody);
			
			printf("Creating ");
			if (pStatic)
				printf("static ");
			printf("compound body consisting of %d primitives...\n", numPrim);
			
	// initialise compound body, important!
			if (pStatic)
				pStatic->Init(physEntity->actors[j].pos.x, physEntity->actors[j].pos.y, physEntity->actors[j].pos.z);
			else
				pDynamic->Init(physEntity->actors[j].pos.x, physEntity->actors[j].pos.y, physEntity->actors[j].pos.z);
				
			if (!pCompoundBody) {
				printf("Failed to create compound body!\n");
				object.push_back(0);
				break;
			}
		

			for (int i = 0; i < numPrim; i++) {

				palMatrix4x4 mat;
				SetPosition(mat,physEntity->actors[j].primitives[i]);

				switch (physEntity->actors[j].primitives[i].type) {
					// box
				case 3:
					{
						palBoxGeometry * pBoxGeom = pCompoundBody->AddBox();
						if (physEntity->actors[j].primitives[i].mass == 0) {
							pBoxGeom->Init(mat, 
								physEntity->actors[j].primitives[i].size.x,
								physEntity->actors[j].primitives[i].size.y, 
								physEntity->actors[j].primitives[i].size.z,
								physEntity->actors[j].primitives[i].density);
						}
						else {
							pBoxGeom->Init(mat, 
								physEntity->actors[j].primitives[i].size.x,
								physEntity->actors[j].primitives[i].size.y, 
								physEntity->actors[j].primitives[i].size.z,
								physEntity->actors[j].primitives[i].mass);		
						}

					}
					break;
					// sphere
				case 4:
					{
						palSphereGeometry * pSphereGeom = pCompoundBody->AddSphere();

						if (physEntity->actors[j].primitives[i].mass == 0) {
							pSphereGeom->Init(
								mat,
								physEntity->actors[j].primitives[i].size.x/2,		// radius
								physEntity->actors[j].primitives[i].density);
						}
						else {
							pSphereGeom->Init(
								mat,
								physEntity->actors[j].primitives[i].size.x/2,		// radius
								physEntity->actors[j].primitives[i].mass);
						}

					}
					break;
					//cylinder and capsule
				case 5:
				case 6:
					{
						palCapsuleGeometry * pCylGeom = pCompoundBody->AddCapsule();
		
						
						if (physEntity->actors[j].primitives[i].mass == 0) {
							pCylGeom->Init(
								mat,
								physEntity->actors[j].primitives[i].size.x/2,		// radius
								physEntity->actors[j].primitives[i].size.y,		// length
								physEntity->actors[j].primitives[i].density);
						}
						else {
							pCylGeom->Init(
								mat,
								physEntity->actors[j].primitives[i].size.x/2,		// radius
								physEntity->actors[j].primitives[i].size.y,		// length
								physEntity->actors[j].primitives[i].mass);
						}
					}
					break;
					// mesh
				case 9:
					{
						palConvexGeometry *pConvexGeom = pCompoundBody->AddConvex();
						
						if (physEntity->actors[j].primitives[i].mass == 0) {
							pConvexGeom->Init(
								mat,
								physEntity->actors[j].primitives[i].meshData->verts,
								physEntity->actors[j].primitives[i].meshData->numVerts,
								physEntity->actors[j].primitives[i].density);
						}
						else {
							pConvexGeom->Init(
								mat,
								physEntity->actors[j].primitives[i].meshData->verts,
								physEntity->actors[j].primitives[i].meshData->numVerts,
								physEntity->actors[j].primitives[i].mass);
						}

						break;

					}
					break;
				default:
					{
						printf("!!! Found unknown/unsupported primitive, ignoring it !!!\n");
					}

				}
				
				//pCompoundBody->Finalize();
				//pBody = pCompoundBody;
				
				

				//set the position of the compound body (only position)
				//mat_identity(&mat);
				//mat_set_translation(&mat,physEntity->actors[j].pos.x,physEntity->actors[j].pos.y,physEntity->actors[j].pos.z);
				//pBody->SetPosition(mat);

				//set the position of the compound body (position and orientation)
				//SetPosition(mat,physEntity->actors[j]);
				//pBody->SetPosition(mat);

				//object.push_back(pBody);
			}
			if (!pStatic) {
				printf("finalizing compound %f\n",physEntity->actors[j].mass);
				pDynamic->Finalize(physEntity->actors[j].mass,1,1,1);
			}
			else
				pCompoundBody->Finalize();

			pBody = pCompoundBody;
			object.push_back(pBody);
			objectnames.insert(std::make_pair(physEntity->actors[j].name,pBody));
		}
		
	}

	// joints
	for (int i = 0; i < numJoints; i++) {

		if (physEntity->joints[i].actors[0] == -1 || physEntity->joints[i].actors[1] == -1) {
			printf("Joint is -1\n");
			continue;
		}
		if (object[physEntity->joints[i].actors[0]] == 0 || object[physEntity->joints[i].actors[1]] == 0 ) {
			printf("Joint connects to non-existant body\n");
			continue;
		}
		
		switch (physEntity->joints[i].type) {
			// hinge = 1
			case 1:
				{
					palRevoluteLink * pRevLink = PF->CreateRevoluteLink();
					printf("Joint %d: Actor %d and %d (hinge)\n", i, physEntity->joints[i].actors[0], physEntity->joints[i].actors[1]);
					pRevLink->Init(object[physEntity->joints[i].actors[0]], object[physEntity->joints[i].actors[1]],
						physEntity->joints[i].pos.x, physEntity->joints[i].pos.y, physEntity->joints[i].pos.z,
						physEntity->joints[i].axis[1].x, physEntity->joints[i].axis[1].y, physEntity->joints[i].axis[1].z);
				}
				break;
			// ball = 2
			case 2:
				{
					palSphericalLink * pShereLink = PF->CreateSphericalLink();
					printf("Joint %d: Actor %d and %d (ball)\n", i, physEntity->joints[i].actors[0], physEntity->joints[i].actors[1]);
					pShereLink->Init(object[physEntity->joints[i].actors[0]], object[physEntity->joints[i].actors[1]], 
						physEntity->joints[i].pos.x, physEntity->joints[i].pos.y, physEntity->joints[i].pos.z);
					//printf("pos x:%f, pos y:%f, pos z:%f\n", physEntity->joints[i].pos.x, physEntity->joints[i].pos.y, physEntity->joints[i].pos.z);
				}
				break;
			default:
				printf("!!! Found unknown/unsupported joint, ignoring it !!!\n");
				break;
		}
	}


	delete physEntity;

	return object;
}

