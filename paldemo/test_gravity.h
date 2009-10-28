#ifndef TEST_GRAVITY
#define TEST_GRAVITY

#include "test.h"

//drop an object test
class Test_Gravity : public Test {
public:
	Test_Gravity() {
	}
	
	void Init(int terrain_type);

	std::string GetName() {
		return "Gravity";
	}
	std::string GetDescription() {
		return "NOTE: EXPERIMENTAL\r\nGravity test - Z up.\r\n 1 - Drops a box\r\n 2 - Drops a sphere\r\n 3 - Drops a capsule\r\n";
	}
	void Input(SDL_Event E);
	void Update();
protected:

	PAL_VECTOR<palBodyBase*> bodies; //vector of all bodies for deletion

	FACTORY_CLASS(Test_Gravity,Test_Gravity,palTests,2);
};

#endif