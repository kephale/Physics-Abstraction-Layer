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
#include <pal_i/bullet/bullet_pal.h>
#include <iostream>

palSphericalLinkTest::palSphericalLinkTest() {
	// TODO Auto-generated constructor stub

}

palSphericalLinkTest::~palSphericalLinkTest() {
	// TODO Auto-generated destructor stub
}

void palSphericalLinkTest::SetUp() {
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
	/*
	palBulletPhysics* bulletPhysics = dynamic_cast<palBulletPhysics*>(physics);
	bulletPhysics->SetFixedTimeStep(0.02f);
	bulletPhysics->SetSubsteps(5);
	*/

	anchor = dynamic_cast<palStaticBox*>(PF->CreateObject("palStaticBox"));
	anchor->Init(0.0f, 6.0f, 10.0f, 1.0f, 1.0f, 1.0f);
#ifdef TRY_ROTATE
	// DEBUG
	palBulletBodyBase* bbb = dynamic_cast<palBulletBodyBase*>(anchor);
	palMatrix4x4 location = bbb->GetLocationMatrix();
	mat_rotate(&location, -M_PI/2.0f, 0.0f, 0.0f, 1.0f);
	bbb->SetPosition(location);
#endif

	floater = PF->CreateBox();
	Float floaterY = 6.0f;
	Float floaterZ = 0.f; //sqrtf(100.f - floaterY*floaterY);
	floater->Init(0.0f, floaterY, floaterZ, 1.0f, 1.0f, 1.0f, 1.0f);

	linkPos = palVector3(0.f, 6.f, 5.f);
	link = PF->CreateSphericalLink(anchor, floater, 0.0f, 6.0f, 5.0f);
}

void palSphericalLinkTest::TearDown() {
	PF->Cleanup();
}

using namespace std;

const string getAngles(palBulletSphericalLink* link) {
	ostringstream result;
	result << "(";
	for (int i = 0; i < 3; i++) {
		btScalar radians = link->m_btp2p->getAngle(i);
		result << btDegrees(radians);
		if (i != 2) {
			result << ", ";
		}
	}
	result << ")";
	return result.str();
}

#define PROP_OUT(obj, pname) "\t" << #pname << "=" << (obj).pname

const string getAngleInfo(palBulletSphericalLink* link) {
	ostringstream result;
	for (int i = 0; i < 3; i++) {
		btRotationalLimitMotor* limit = link->m_btp2p->getRotationalLimitMotor(i);
		result << "#" << i << "[";
		result << PROP_OUT(*limit, m_currentLimitError);
		result << PROP_OUT(*limit, m_currentPosition);
		result << PROP_OUT(*limit, m_currentLimit);
		result << PROP_OUT(*limit, m_accumulatedImpulse);
		result << "]";
	}
	return result.str();
}

TEST_F(palSphericalLinkTest, testConeLimits)
{
	cout.precision(4);
	//link->SetLimits(M_PI, FLOAT_EPSILON);
	link->SetLimits(M_PI / 2.f, 0.0f); // negative numbers mean unconstrained
	palVector3 floaterPos;
	palVector3 anchorPos;
	anchor->GetPosition(anchorPos);

	float stepsize = 0.005f;
	float duration = 5.f;
	int iterations = ceilf(duration / stepsize);
	palBulletSphericalLink* sLink = dynamic_cast<palBulletSphericalLink*>(link);
	for (int i = 0; i < iterations; i++) {
		physics->Update(stepsize);

		floater->GetPosition(floaterPos);
		palVector3 distVector;
		vec_sub(&distVector, &linkPos, &floaterPos);
		btScalar distance = vec_mag(&distVector);
		btScalar angle = sLink->m_btp2p->getAngle(0);

		cout << "Box position at time " << physics->GetTime() << "\tis " << floaterPos
						<< "\t" << distance << "\tangle=" << angle << " ("
						<< btDegrees(angle) << ")"
						//getAngleInfo(dynamic_cast<palBulletSphericalLink*> (link))
						<< endl;
	}

	// This fails now, maybe because the axis of the link is
	//wrong. Will have to investigate and re-enable later.
	EXPECT_FLOAT_EQ(-10.0f, floaterPos.y);
}
