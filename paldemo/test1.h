#ifndef TEST_1
#define TEST_1

#include "test.h"

//drop an object test
class Test_1 : public Test {
public:
	Test_1() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
	}

	std::string GetName() {
		return "Object Dropping";
	}
	std::string GetDescription() {
		return "This is one of the simplest tests of PAL. Simply drop a box, sphere, or cylinder onto the underlying terrain by pressing the following keys:\r\n 1 - Drops a box\r\n 2 - Drops a sphere\r\n 3 - Drops a cylinder.\r\n 4 - Drops a grid of cubes.\r\n 5 - Drops a compound body\r\n 6 - Drops a convex body\r\n 7 - Drops a compound convex body\r\n 8 - Randomly repositions a random body\r\n 9 - Deletes a body";
	}
	void Input(SDL_Event E);
	void Update() {
		;
	};	
protected:

	PAL_VECTOR<palBodyBase*> bodies; //vector of all bodies for deletion

	FACTORY_CLASS(Test_1,Test_1,palTests,2);
};

#endif