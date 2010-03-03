#ifndef TEST_5
#define TEST_5

#include "test.h"
#include <vector>

//drop a ragdoll test
class Test_6 : public Test {
public:
	Test_6() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
	}

	std::string GetName() {
		return "Ragdoll";
	}
	std::string GetDescription() {
		return "Drop a rag doll";
	}
	void Input(SDL_Event E);
	void Update();
private:
	void Create_palRagDoll(float *pos);
	FACTORY_CLASS(Test_6,Test_6,palTests,2);

};

#endif
