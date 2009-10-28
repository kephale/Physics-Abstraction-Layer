#ifndef TEST_1
#define TEST_1

#include "test.h"

//drop an object test
class Test_AdvancedBody : public Test {
public:
	Test_AdvancedBody() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
	}

	std::string GetName() {
		return "Object Dropping - Advanced";
	}
	std::string GetDescription() {
		return "NOTE: VERY FEW PHYSICS ENGINES SUPPORT THIS DEMO\r\n Create static, kinematic and dynamic bodies with altering geometries and center of masses.\r\n 1 - Drops a box\r\n 2 - Drops a sphere\r\n 3 - Drops a capsule with an offset center of mass\r\n 4 - Drops a grid of cubes.\r\n 5 - Adds a box geometry to a body\r\n 7 - Sets a body to be kinematic and moves (animates) it in a circle\r\n 8 - Randomly repositions a random body\r\n 9 - Deletes a body";
	}
	void Input(SDL_Event E);
	void Update();
protected:

	PAL_VECTOR<palBodyBase*> bodies; //vector of all bodies for deletion

	FACTORY_CLASS(Test_AdvancedBody,Test_AdvancedBody,palTests,2);
};

#endif