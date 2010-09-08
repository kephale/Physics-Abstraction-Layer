/*
 * AbstractPalTest.cpp
 *
 *  Created on: Sep 7, 2010
 *      Author: chris
 */

#include "AbstractPalTest.h"
#include <pal/palFactory.h>

AbstractPalTest::AbstractPalTest() {
}

AbstractPalTest::AbstractPalTest(const PAL_STRING& physicsEngine) :
	engineName(physicsEngine) {
}

AbstractPalTest::~AbstractPalTest() {
}

void AbstractPalTest::SetUp() {
	if (engineName.size() == 0) {
		char* engineNameFromEnv = getenv("PAL_PHYSICS_ENGINE");
		if (engineNameFromEnv) {
			engineName = engineNameFromEnv;
		}
		else {
			engineName = "Bullet";
		}
	}
	PF->LoadPhysicsEngines();
	PF->SelectEngine(engineName);
	physics = PF->CreatePhysics();
	ASSERT_TRUE(physics != NULL);

	palPhysicsDesc desc;
	physics->Init(desc);

}

void AbstractPalTest::TearDown() {
	PF->Cleanup();
}
