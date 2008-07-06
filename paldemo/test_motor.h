#ifndef TEST_MOTOR
#define TEST_MOTOR

#include "test.h"

//drop an object test
class Test_Motor : public Test {
public:
	Test_Motor() {
	}
	void CreateSet();
	void CreateChain();
	void CreateRobot();
	void Init(int terrain_type);

	std::string GetName() {
		return "Motor";
	}
	std::string GetDescription() {
		return "Simple Motors";
	}
	void Input(SDL_Event E);
	void Update() ;
protected:
	std::string m_BodyType1;
	std::string m_BodyType2;
	VECTOR<PID *> pids; //vector of all PID controllers for the motors
	VECTOR<palAngularMotor*> motors; //vector of all motors for deletion
	VECTOR<palBody*> bodies; //vector of all bodies for deletion
	FACTORY_CLASS(Test_Motor,Test_Motor,palTests,2);
};

#endif