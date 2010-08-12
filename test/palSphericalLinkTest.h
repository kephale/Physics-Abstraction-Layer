/*
 * palSphericalLinkTest.h
 *
 *  Created on: Aug 11, 2010
 *      Author: Chris Long
 *  Copyright (C) 2010 SET Corporation
 */

#ifndef PALSPHERICALLINKTEST_H_
#define PALSPHERICALLINKTEST_H_

#include <gtest/gtest.h>
#include <pal/pal.h>

class palSphericalLinkTest : public ::testing::Test {
public:
	palSphericalLinkTest();
	virtual ~palSphericalLinkTest();
	virtual void SetUp();
	virtual void TearDown();

	palPhysics* physics;
        palSphericalLink* link;
        palBox* floater;
};

#endif /* PALSPHERICALLINKTEST_H_ */
