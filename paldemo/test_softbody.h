#ifndef TEST_SB
#define TEST_SB

#include "test.h"

//drop an object test
class Test_SoftBody : public Test {
public:
	Test_SoftBody() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
	}

	std::string GetName() {
		return "Soft Body Object Dropping";
	}
	std::string GetDescription() {
		return "EXPERIMENTAL";
	}
	void Input(SDL_Event E);
	void Update() {
		;
	};	
	void AdditionalRender();
protected:

	PAL_VECTOR<palBodyBase*> bodies; //vector of all bodies for deletion

	FACTORY_CLASS(Test_SoftBody,Test_SoftBody,palTests,2);
};

#endif