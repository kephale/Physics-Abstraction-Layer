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
		return "Tests particle fluids. Press 4 to create a container, press W to create a particle liquid, press 5 to drop a sphere into the container.";
	}
	void Input(SDL_Event E);
	void Update();
	void AdditionalRender();
protected:
	VECTOR<palBody*> bodies; //vector of all bodies for deletion
	VECTOR<palActuator *> act;
	FACTORY_CLASS(Test_Water,Test_Water,palTests,2);
};

#endif