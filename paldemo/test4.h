#ifndef TEST_4
#define TEST_4

#include "test.h"

//drop an object test
class Test_4 : public Test {
public:
	Test_4() {
	}
	
	void Init(int terrain_type) {
		m_PyrHeight = 4;
		m_JengaHeight = 4;
		m_WallHeight = 4;
		CreateTerrain(terrain_type);
		InitMaterials();
		m_UseMaterials=true;
	}

	std::string GetName() {
		return "Stacking";
	}
	std::string GetDescription() {
		return "Object stacking test. Press keys 1, 2, and 3 for a Pyramid Stack, 'Jenga' Stack, or a Wall Stack respectively. Keys a,s,d will increase the stacks height, and z,x,c will decrease the stack height. This will only have an effect when constructed. Pressing the space bar will launch a cube at the stack.";
	}
	void Input(SDL_Event E);
	void Update() {
		;
	};	
protected:

	void SetMat(palBody *pb);

	bool m_UseMaterials;

	void InitMaterials();

	void BuildCards();
	void BuildJengaStack(int height,int width, float dimW,float dimH,
									float posX,float posY,float posZ);
	
	void BuildWall(int height, int width,
                            float dimX,float dimY,float dimZ,
                            float posX, float posY, float posZ);
	int m_PyrHeight;
	int m_JengaHeight;
	int m_WallHeight;
	FACTORY_CLASS(Test_4,Test_4,palTests,2);
};

#endif