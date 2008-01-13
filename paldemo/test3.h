#ifndef TEST_3
#define TEST_3

#include "test.h"

//drop an object test
class Test_3 : public Test {
public:
	Test_3() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
		InitMaterials();
	}

	std::string GetName() {
		return "Materials";
	}
	std::string GetDescription() {
		return "Tests different material properites";
	}
	virtual void Input(SDL_Event E);
	virtual void Update() {
		;
	};		
protected:
	void InitMaterials();
	palBody *pbM[3];
	FACTORY_CLASS(Test_3,Test_3,palTests,2);
};

#endif