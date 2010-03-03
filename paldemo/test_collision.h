#ifndef TEST_COLLISION
#define TEST_COLLISION

#include "test.h"

//drop an object test
class Test_Collision : public Test {
public:
	Test_Collision() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
	}

	std::string GetName() {
		return "Collision subsytem";
	}
	std::string GetDescription() {
		return "This test visualizes the contact points and raycasts from the collision subsystem. Simply drop a box, sphere, or cylinder onto the underlying terrain by pressing the following keys:\r\n 1 - Drops a box\r\n 2 - Drops a sphere\r\n 3 - Drops a cylinder.\r\n 4 - Drops a grid of cubes.\r\n 5 - Drops a compound body\r\n 6 - Drops a convex body\r\n 7 - Drops a set of objects with collision disabled\r\n 8 - Randomly repositions a random body\r\n 9 - Deletes a body";
	}
	void Input(SDL_Event E);
	void Update() {
		;
	};	
	void AdditionalRender();
protected:
	PAL_VECTOR<palBody*> bodies; //vector of all bodies for deletion
	FACTORY_CLASS(Test_Collision,Test_Collision,palTests,2);
};

#endif
