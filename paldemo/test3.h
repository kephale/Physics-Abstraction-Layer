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

	// Properties for real material types
	palMaterialDesc matDesc_Aluminium;
	palMaterialDesc matDesc_Diamond;
	palMaterialDesc matDesc_Oak;
	palMaterialDesc matDesc_Steel;
	palMaterialDesc matDesc_Wood;
	palMaterialDesc matDesc_Aluminum_Steel;

	// Properties for other material types
	palMaterialDesc matDesc_Normal;
	palMaterialDesc matDesc_Jumpy;
	palMaterialDesc matDesc_Stubborn;
	palMaterialDesc matDesc_LittleR;
	palMaterialDesc matDesc_Sliding;
	palMaterialDesc matDesc_Unmoving;
	palMaterialDesc matDesc_LittleK;
	palMaterialDesc matDesc_Smooth;
	palMaterialDesc matDesc_Sticky;
	palMaterialDesc matDesc_LittleS;
	// Interaction types
	palMaterialDesc matDesc_NormalJumpy;
	palMaterialDesc matDesc_NormalStubborn;
	palMaterialDesc matDesc_NormalLittleR;
};

#endif
