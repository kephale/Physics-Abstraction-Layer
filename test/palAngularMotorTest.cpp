/*
 * palBodyTest.cpp
 *
 *  Created on: Jul 1, 2010
 *      Author: Chris Long
 *  Copyright (C) 2010 SET Corporation
 */


#include <pal/pal.h>
#include <pal/palFactory.h>
#include <iostream>
#include <cstdlib>
#include <float.h>
#include <assert.h>
#include <dlfcn.h>

int main(int argc, char* argv[])
{
	std::cout << dlopen("libpal_bullet.so", RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND) << std::endl;
	PF->LoadPALfromDLL(".");
	PF->SelectEngine("Bullet");		 // Here is the name of the physics engine you wish to use. You could replace DEFAULT_ENGINE with "Tokamak", "ODE", etc...
	palPhysics *pp = PF->CreatePhysics(); //create the main physics class
	if (pp == NULL) {
		std::cerr << "Failed to create the physics engine. Check to see if you spelt the engine name correctly, or that the engine DLL is in the right location" << std::endl;
		return 1;
	}
	palPhysicsDesc desc;
	pp->Init(desc); //initialize it, set the main gravity vector

	palFactoryObject* obj = PF->CreateObject("palAngularMotor");
	palAngularMotor* angularMotor = dynamic_cast<palAngularMotor*>(obj);

	assert(angularMotor != 0);
	std::cout << "angularMotor = " << angularMotor << std::endl;
	
	pp->Cleanup();
	std::cout << "success" << std::endl;
	return 0;
}
