/*
 * rigidLinkTest.cpp
 *
 *  Created on: Apr 9, 2010
 *      Author: Chris Long
 *  Copyright (C) 2010 SET Corporation
 */

#include <pal/pal.h>
#include <pal/palFactory.h>
#include <iostream>
#include <cmath>
#include <pal_i/bullet/bullet_pal.h>

const int STEPS = 20;

int main(int argc, char* argv[])
{
	PF->LoadPALfromDLL(0);
	PF->SelectEngine("Bullet");		 // Here is the name of the physics engine you wish to use. You could replace DEFAULT_ENGINE with "Tokamak", "ODE", etc...
	palPhysics *pp = PF->CreatePhysics(); //create the main physics class
	if (pp == NULL) {
		std::cerr << "Failed to create the physics engine. Check to see if you spelt the engine name correctly, or that the engine DLL is in the right location" << std::endl;
		return 1;
	}
	palPhysicsDesc desc;
	//desc.m_vGravity = palVector3(0.0f, 0.0f, 0.0f);
	pp->Init(desc); //initialize it, set the main gravity vector

	palBox* boxA = PF->CreateBox();
	boxA->Init(0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	boxA->SetOrientation(M_PI, 0, 0);

	palBox* boxB = PF->CreateBox();
	boxB->Init(0.0f, 0.0f, 5.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	boxB->SetOrientation(M_PI, M_PI, M_PI);

	palRigidLink* link = PF->CreateRigidLink(boxA, boxB);

	palTerrainPlane *pt= PF->CreateTerrainPlane(); //create the ground
	pt->Init(0,0,0,50.0f); //initialize it, set its location to 0,0,0 and minimum size to 50

	palBulletRigidLink* brLink = dynamic_cast<palBulletRigidLink*>(link);

	for (int i = 0; i < STEPS; i++) {
		pp->Update(0.05);

		palVector3 aPos;
		boxA->GetPosition(aPos);
		palVector3 bPos;
		boxB->GetPosition(bPos);

		std::cout << i << "\t" << aPos << "\t" << bPos << "\t" << brLink->GetAngle() << "\t"
				<< brLink->m_fLowerLimit << "\t" << brLink->m_fUpperLimit << std::endl;
	}
	pp->Cleanup();
	return 0;
}
