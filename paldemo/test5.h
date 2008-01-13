#ifndef TEST_5
#define TEST_5

#include "test.h"
#include <vector>

//drop an object test
class Test_5 : public Test {
public:
	Test_5() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
	}

	std::string GetName() {
		return "PSD";
	}
	std::string GetDescription() {
		return "Experimental";
	}
	void Input(SDL_Event E);
	void Update();
	std::vector<palPSDSensor *> psds;
	FACTORY_CLASS(Test_5,Test_5,palTests,2);
};

#endif