/*
 * palSphericalLinkTest.cpp
 *
 *  Created on: Aug 11, 2010
 *      Author: Chris Long
 *  Copyright (C) 2010 SET Corporation
 */

#include "palSphericalLinkTest.h"
#include <pal/palFactory.h>
#include <pal/palStatic.h>

palSphericalLinkTest::palSphericalLinkTest() {
	// TODO Auto-generated constructor stub

}

palSphericalLinkTest::~palSphericalLinkTest() {
	// TODO Auto-generated destructor stub
}

void palSphericalLinkTest::SetUp()
{
	PF->LoadPhysicsEngines();
	const char* engineName = getenv("PAL_PHYSICS_ENGINE");
	if (engineName == NULL) {
		engineName = "Bullet";
	}
	PF->SelectEngine(engineName);
	physics = PF->CreatePhysics();
	ASSERT_TRUE(physics != NULL);

	palPhysicsDesc desc;
	physics->Init(desc);

	palStaticBox* anchor = dynamic_cast<palStaticBox*>(PF->CreateObject("palStaticBox"));
	anchor->Init(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	palBox* floater = PF->CreateBox();
	Float floaterX = 10.0f;
	floater->Init(floaterX, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	
}

void palSphericalLinkTest::TearDown()
{
	PF->Cleanup();
}





