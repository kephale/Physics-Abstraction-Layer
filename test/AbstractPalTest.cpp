/*
 * AbstractPalTest.cpp
 *
 *  Created on: Sep 7, 2010
 *      Author: chris
 */

#include "AbstractPalTest.h"
#include <pal/palFactory.h>
#include <pal/palException.h>

const PAL_STRING AbstractPalTest::PAL_DEFAULT_ENGINE("Bullet");

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
			engineName = PAL_DEFAULT_ENGINE;
		}
	}
	PF->LoadPhysicsEngines();
	PF->SelectEngine(engineName);
	physics = PF->CreatePhysics();
	if (!physics) {
		throw new palException("Could not load physics engine");
	}

	palPhysicsDesc desc;
	physics->Init(desc);

}

void AbstractPalTest::TearDown() {
	PF->Cleanup();
}
