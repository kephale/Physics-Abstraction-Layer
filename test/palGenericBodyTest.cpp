/*
 * palGenericBodyTest.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: Chris Long
 *  Copyright (C) 2010 SET Corporation
 */

#include "palGenericBodyTest.h"
#include <gtest/gtest.h>
#include <pal/palFactory.h>
#include <vector>
#include <cmath>

palGenericBodyTest::palGenericBodyTest() {
}

palGenericBodyTest::~palGenericBodyTest() {
}

template<typename T> void push_back3(std::vector<T> &v, T x, T y, T z) {
	v.push_back(x);
	v.push_back(y);
	v.push_back(z);
}

static std::vector<Float> createMesh(float x, float y, float z, float radius, float length, int hstrip, int vslice) {
	float fX1, fY1, fX2, fY2, fX3, fY3, fX4, fY4;	// The vertex positions around each quad we calculate
	float fAngle,fY,fYNext;
	float fAngleAdd = 360.0f / (float)vslice;
	float fSineAngle = 0;
	float fSineAdd = 180.0f / (hstrip-1);
	float height = length;
	int i,j;

	std::vector<float> verts;
	verts.reserve(hstrip * vslice * 6 * 3);
	// Loop around our sphere
	for (i=0; i<hstrip; i++)
	{
		// Reset the angle for this slice
		fAngle = 0;

		fY = cosf(fSineAngle * DEG2RAD) * radius;
		fYNext = cosf((fSineAngle+fSineAdd) * DEG2RAD) * radius;

		// If we're above the midpoint, add half the height to the vertex positions.
		// Otherwise subtract half the height.
		if (i<=(hstrip/2)-1)
			fY += height/2;
		else
			fY -= height/2;
		if (i<=(hstrip/2)-2)
			fYNext += height/2;
		else
			fYNext -= height/2;

		for (j=0; j<vslice; j++)
		{
			// Calculate the X and Y position for the sphere (as if it were a circle viewed from above)
			fX1 = sinf(fAngle * DEG2RAD) * radius * sinf(fSineAngle * DEG2RAD);
			fY1 = cosf(fAngle * DEG2RAD) * radius * sinf(fSineAngle * DEG2RAD);
			fX2 = sinf((fAngle+fAngleAdd) * DEG2RAD) * radius * sinf(fSineAngle * DEG2RAD);
			fY2 = cosf((fAngle+fAngleAdd) * DEG2RAD) * radius * sinf(fSineAngle * DEG2RAD);
			fX3 = sinf(fAngle * DEG2RAD) * radius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fY3 = cosf(fAngle * DEG2RAD) * radius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fX4 = sinf((fAngle+fAngleAdd) * DEG2RAD) * radius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fY4 = cosf((fAngle+fAngleAdd) * DEG2RAD) * radius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fAngle += fAngleAdd;

			push_back3<Float>(verts, fX1, fY    , fY1);
			push_back3<Float>(verts, fX4, fYNext, fY4);
			push_back3<Float>(verts, fX2, fY    , fY2);
			push_back3<Float>(verts, fX1, fY    , fY1);
			push_back3<Float>(verts, fX3, fYNext, fY3);
			push_back3<Float>(verts, fX4, fYNext, fY4);

		}


		fSineAngle += fSineAdd;
	}
	return verts;
}

TEST_F(palGenericBodyTest, testConvexGeometry) {

	std::vector<Float> verts(createMesh(0.0f, 0.0f, 0.0f, 5.0f, 5.0f, 20, 20));
	palMatrix4x4 position;
	mat_identity(&position);
	float mass = 10.0f;

	palConvexGeometry* geometry = PF->CreateConvexGeometry(position, &verts.front(), verts.size() / 3, mass);

	palGenericBody* body = PF->CreateGenericBody(position);
	body->ConnectGeometry(geometry);

	delete body;
}
