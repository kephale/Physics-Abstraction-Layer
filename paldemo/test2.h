#ifndef TEST_2
#define TEST_2

#include "test.h"

//link test
class Test_2 : public Test {
public:
	Test_2() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
		m_BodyType1="palBox";
		m_BodyType2="palBox";
	}

	std::string GetName() {
		return "Link Test";
	}
	std::string GetDescription() {
		return "This tests simple link creation and enabeling link limits. Press keys 1, 2, and 3 for a Revolute, Spherical, or Prismatic link. Press keys 4, 5 and 6 to create these links with movement limits enabled.\r\na,s,d effects the body type created, representing Box, Sphere and Cylinder respectively. z, x, c effects the second body type.";
	}
	void Input(SDL_Event E);
	void Update() {
		;
	};	
	std::string m_BodyType1;
	std::string m_BodyType2;
protected:

	FACTORY_CLASS(Test_2,Test_2,palTests,2);
};

#endif