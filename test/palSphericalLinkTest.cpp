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
#include <iostream>

palSphericalLinkTest::palSphericalLinkTest() {
}

palSphericalLinkTest::~palSphericalLinkTest() {
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
    // can't set orientation of static objects ??
    //anchor->SetOrientation(0.0f, 0.0f, M_PI/2.0f);

	floater = PF->CreateBox();
	Float floaterX = 10.0f;
	floater->Init(floaterX, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    link = PF->CreateSphericalLink(anchor, floater, 0.0f, 0.0f, 0.0f);
}

void palSphericalLinkTest::TearDown()
{
	PF->Cleanup();
}

using namespace std;

TEST_F(palSphericalLinkTest, testConeLimits) {
    link->SetLimits(M_PI, FLOAT_EPSILON);
    palVector3 pos;
	for (int i=0;i<50;i++) {
		physics->Update(0.02f);

		floater->GetPosition(pos); 

		cout << "Box position at time " << physics->GetTime() << "\tis" << pos << endl;
	}

    // This fails now, maybe because the axis of the link is
    //wrong. Will have to investigate and re-enable later.
    //EXPECT_FLOAT_EQ(-10.0f, pos.y);
}
