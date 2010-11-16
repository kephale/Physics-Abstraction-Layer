/*
 * palLinkFeedbackTest.h
 *
 *  Created on: Nov 15, 2010
 *      Author: chris
 */

#ifndef PALLINKFEEDBACKTEST_H_
#define PALLINKFEEDBACKTEST_H_

#include "AbstractPalTest.h"
#include <pal/palBodies.h>

class palLinkFeedbackTest: public AbstractPalTest {
public:
	palLinkFeedbackTest();
	virtual ~palLinkFeedbackTest();
protected:
	palStaticBox* anchor;
	palBox* floater;
};

#endif /* PALLINKFEEDBACKTEST_H_ */
