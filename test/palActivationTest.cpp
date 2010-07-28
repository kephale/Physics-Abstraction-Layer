/*
 * palActivationTest.cpp
 *
 *  Created on: Jul 20, 2010
 *      Author: Chris Long
 *  Copyright (C) 2010 SET Corporation
 */


#include <pal/pal.h>
#include <pal/palFactory.h>
#include <pal/palActivation.h>
#include <iostream>
#include <cstdlib>
#include <float.h>
#include <assert.h>

const float EPSILON = 0.0001;

float randFloat()
{
	return rand() / float(RAND_MAX) * FLT_MAX;
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

	palActivationSettings *activationSettings = box->asActivationSettings();
	assert(activationSettings != 0);

	float linear = randFloat();
	activationSettings->SetActivationLinearVelocityThreshold(linear);
	assert(fabsf(linear - activationSettings->GetActivationLinearVelocityThreshold()) < EPSILON);

	float angular = randFloat();
	activationSettings->SetActivationAngularVelocityThreshold(angular);
	assert(fabsf(angular - activationSettings->GetActivationAngularVelocityThreshold()) < EPSILON);

	pp->Cleanup();
	std::cout << "success" << std::endl;
	return 0;
}
