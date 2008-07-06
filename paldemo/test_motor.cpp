#include "test_motor.h"

FACTORY_CLASS_IMPLEMENTATION(Test_Motor);

void Test_Motor::CreateChain() {
	m_BodyType1="palStaticBox";
	m_BodyType2="palBox";
	
	palBodyBase *pb_last = 0;
	palBodyBase *pb = 0;

	pb_last = CreateBody(m_BodyType1.c_str(),0,4,0,1,1,1,0);
	for (int i=1;i<5;i++) {
		pb = CreateBody(m_BodyType2.c_str(),i*1.5f,4,0,0.5,0.5,0.5,1);

		palRevoluteLink *prl;
		prl = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
		if (prl == NULL) {
			printf("Error: Could not create a Revolute link\n");
			return;
		}
		prl->Init(pb_last,pb,i*1.5f - 1,4,0,0,0,1);

#if 1
	palAngularMotor *pam = dynamic_cast<palAngularMotor *>(PF->CreateObject("palAngularMotor"));
	if (!pam) {
		printf("Error: Could not create a Angular Motor\n");
		return;
	}
	motors.push_back(pam);
	PID *pid;
	pid = new PID;
	pid->Init(2,0.1,0);
	pids.push_back(pid);
	pam->Init(prl,7.5f);
#endif

		pb_last = pb;
	}

}


void Test_Motor::CreateSet() {
	m_BodyType1="palBox";
	m_BodyType2="palBox";
	palBodyBase *pb1;
	palBodyBase *pb2;
	float pos[3];		
	float dim1[3];				
	float dim2[3];
	int i;
	for (i=0;i<3;i++)
		pos[i]=sfrand()*3;
	for (i=0;i<3;i++)
		dim1[i]=ufrand()+0.1f;
	
	for (i=0;i<3;i++)
		dim2[i]=ufrand()+0.1f;

	//ensure box 1 bigger than 2
	dim1[1] += dim2[1];
	//set correct pos
	pos[1]=dim1[1]*0.5f;

	pb1 = CreateBody(m_BodyType1.c_str(),
		pos[0],
		pos[1],
		pos[2],
		dim1[0],
		dim1[1],
		dim1[2],
		ufrand()+1);
	pb2 = CreateBody(m_BodyType2.c_str(),
		pos[0]+dim1[0]*0.5f+dim2[0]*0.5f,
		pos[1],
		pos[2],
		dim2[0],
		dim2[1],
		dim2[2],
		ufrand()+0.1);
	palRevoluteLink *prl;
	prl = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
	if (prl == NULL) {
		printf("Error: Could not create a Revolute link\n");
		return;
	}
	prl->Init(pb1,pb2,pos[0]+dim1[0]*0.5f,pos[1],pos[2],0,0,1);
	
#if 1
	palAngularMotor *pam = dynamic_cast<palAngularMotor *>(PF->CreateObject("palAngularMotor"));
	if (!pam) {
		printf("Error: Could not create a Angular Motor\n");
		return;
	}
	motors.push_back(pam);
	PID *pid;
	pid = new PID;
	pid->Init(1,0,0);
	pids.push_back(pid);
	pam->Init(prl,7.5f);
#endif
	
}

void Test_Motor::CreateRobot() {

	//float foot_left[] = {4.7,3.6,2.5};
	//CreateBody("palBox",0,foot_left[1]*0.5,0,foot_left[0],foot_left[1],foot_left[2],1);

}

void Test_Motor::Init(int terrain_type) {
	CreateTerrain(terrain_type);
	
}

void Test_Motor::Update()
{
	for (int i=0;i<motors.size();i++) {
		float pid_out = pids[i]->Update(0,motors[i]->GetLink()->GetAngle(),0.01);
		motors[i]->Update(pid_out);
		motors[i]->Apply();
	}
};	

void Test_Motor::Input(SDL_Event E) {
	switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
				case SDLK_1:
					CreateSet();
				break;
				case SDLK_2:
					CreateChain();
					break;

				case SDLK_9:
					CreateRobot();
				break;
			} 
		break;
	}
}