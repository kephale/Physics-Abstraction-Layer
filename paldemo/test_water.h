#ifndef TEST_WATER
#define TEST_WATER

#include "test.h"

//drop an object test
class Test_Water : public Test {
public:
	Test_Water() {
	}
	
	void Init(int terrain_type) ;
	std::string GetName() {
		return "Water";
	}
	std::string GetDescription() {
		return "Tests fluids. Press 4 to create a container, press W to create a particle liquid, press 5 to drop a sphere into the container. Press 1 to create a body with directly applied fluid forces. Press h to create a grid-based fluid, and press 3 to drop different bodies into it.";
	}
	void Input(SDL_Event E);
	void Update();
	void AdditionalRender();
protected:
	PAL_VECTOR<palBody*> bodies; //vector of all bodies for deletion
	PAL_VECTOR<palActuator *> act;
	FACTORY_CLASS(Test_Water,Test_Water,palTests,2);
};

#endif
