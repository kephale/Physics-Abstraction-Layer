#include <stdio.h> //for our old friend, the printf function
#include "pal/palFactory.h"
#include "pal/ConfigStatic.h"

int main(int argc, char *argv[]) {
#ifndef PAL_STATIC
	PF->LoadPALfromDLL(); //use this if you wish to load PAL from .dll or .so
	//otherwise, we can staticly link in an implementation by include the pal_i files
	//eg: tokamak_pal.cpp and tokamak_pal.h
#endif

	PF->SelectEngine(DEFAULT_ENGINE);		 // Here is the name of the physics engine you wish to use. You could replace DEFAULT_ENGINE with "Tokamak", "ODE", etc...
	palPhysics *pp = PF->CreatePhysics(); //create the main physics class
	if (pp == NULL) {
		printf("Failed to create the physics engine. Check to see if you spelt the engine name correctly, or that the engine DLL is in the right location\n");
		return 0;
	}
	palPhysicsDesc desc;
	pp->Init(desc); //initialize it, set the main gravity vector
	palTerrainPlane *pt= PF->CreateTerrainPlane(); //create the ground
	pt->Init(0,0,0,50.0f); //initialize it, set its location to 0,0,0 and minimum size to 50
	palBox *pb = PF->CreateBox(); //create a box
	pb->Init(0,1.5f ,0, 1,1,1, 1); //initialize it, set its location to 0,1.5,0 (one and a half units up in the air), set dimensions to 1x1x1 and its mass to 1
	for (int i=0;i<25;i++) { //run 25 steps of the simulation
		pp->Update(0.02f); //update the physics engine. advance the simulation time by 0.02

		palVector3 pos;
		pb->GetPosition(pos); //get the location of the box

		printf("Current box position is %6.5f at time %4.2f\n",pos.y,pp->GetTime());
	}
	PF->Cleanup(); //we are done with the physics. clean up.
}
