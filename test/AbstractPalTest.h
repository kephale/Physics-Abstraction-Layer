/*
 * AbstractPalTest.h
 *
 *  Created on: Sep 7, 2010
 *      Author: chris
 */

#ifndef ABSTRACTPALTEST_H_
#define ABSTRACTPALTEST_H_

#include <gtest/gtest.h>
#include <pal/pal.h>

class AbstractPalTest: public testing::Test
{
public:
	AbstractPalTest();
	AbstractPalTest(const PAL_STRING& engineName);
	virtual ~AbstractPalTest();

protected:
	virtual void SetUp();
	virtual void TearDown();

	palPhysics* physics;
	PAL_STRING engineName;
};

#endif /* ABSTRACTPALTEST_H_ */
