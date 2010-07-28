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
#include <cassert>


template <typename T> void assertEquals(T expect, T actual,
										const std::string& message) {
	if (expect != actual) {
		std::cerr << "expecting '" << expect << "' but got '" << actual
				  << "':" << message << std::endl;
		abort();
	}
}

int main(int argc, char* argv[])
{
	PF->LoadPhysicsEngines();
	PF->SelectEngine("Bullet");		 // Here is the name of the physics engine you wish to use. You could replace DEFAULT_ENGINE with "Tokamak", "ODE", etc...
	palPhysics *pp = PF->CreatePhysics(); //create the main physics class
	if (pp == NULL) {
		std::cerr << "Failed to create the physics engine. Check to see if you spelt the engine name correctly, or that the engine DLL is in the right location" << std::endl;
		return 1;
	}
	palPhysicsDesc desc;
	pp->Init(desc); //initialize it, set the main gravity vector

	palBox* box = PF->CreateBox();
	box->Init(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	Float x = rand() / float(RAND_MAX) * FLT_MAX;
	Float y = rand() / float(RAND_MAX) * FLT_MAX;
	float z = rand() / float(RAND_MAX) * FLT_MAX;

	box->SetPosition(x, y, z);
	palVector3 pos;
	box->GetPosition(pos);
	assertEquals(pos.x, x, "SetPosition or GetPosition failed");
	assert(pos.y == y);
	assert(pos.z == z);

	pp->Cleanup();
	std::cout << "success" << std::endl;
	return 0;
}
