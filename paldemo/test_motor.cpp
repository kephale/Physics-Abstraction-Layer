#include "test_motor.h"

FACTORY_CLASS_IMPLEMENTATION(Test_Motor);

bool g_drive = false;

void Test_Motor::CreateChain(int xyz) {
	m_BodyType1="palStaticBox";
	m_BodyType2="palBox";
	
	palBodyBase *pb_last = 0;
	palBodyBase *pb = 0;

	pb_last = CreateBody(m_BodyType1.c_str(),0,2,0,1,1,1,0);
	for (int i=1;i<5;i++) {
		switch (xyz) {
			case 0:
				pb = CreateBody(m_BodyType2.c_str(),i*1.5f,2,0,0.5,0.5,0.5,1);
				break;
			case 1:
				pb = CreateBody(m_BodyType2.c_str(),0,2+i*1.5f,0,0.5,0.5,0.5,1);
				break;
			case 2:
				pb = CreateBody(m_BodyType2.c_str(),0,2,i*1.5f,0.5,0.5,0.5,1);
				break;
		}

		palRevoluteLink *prl;
		prl = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
		if (prl == NULL) {
			printf("Error: Could not create a Revolute link\n");
			return;
		}
		switch (xyz) {
			case 0:
				prl->Init(pb_last,pb,i*1.5f - 1,2,0,0,0,1);
				break;
			case 1:
				prl->Init(pb_last,pb,0,i*1.5f - 1 + 2,0,0,0,1);
				break;
			case 2:
				prl->Init(pb_last,pb,0,2,i*1.5f - 1,1,0,0);
				break;
		}

#if 1
	palAngularMotor *pam = dynamic_cast<palAngularMotor *>(PF->CreateObject("palAngularMotor"));
	if (!pam) {
		printf("Error: Could not create a Angular Motor\n");
		return;
	}
	motors.push_back(pam);
	desired.push_back(0);
	PID *pid;
	pid = new PID;
	pid->Init(50,0.1,0);
	pids.push_back(pid);
	pam->Init(prl,500.0f);
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
	desired.push_back(0);
	PID *pid;
	pid = new PID;
	pid->Init(1,0,0);
	pids.push_back(pid);
	pam->Init(prl,7.5f);
#endif
	
}



void Test_Motor::CreateRobot() {
	g_drive = true;
	palBodyBase *pb_last = 0;
	palBody *pb = 0;

	pb_last = CreateBody("palStaticBox",0,0.5,0,1,1,1,0);
	pb_last->SetGroup(1);
	

	for (int i=0;i<3;i++) {
		if (i == 0) //base
			pb = dynamic_cast<palBody *>(CreateBody("palBox",0,1.25,0,0.75,0.5,0.75,0.3));
		else
			pb = dynamic_cast<palBody *>(CreateBody("palBox",i,1.5,0,1,0.5,0.25,0.1));
		pb->SetGroup(1);
		palRevoluteLink *prl;
		prl = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
		if (prl == NULL) {
			printf("Error: Could not create a Revolute link\n");
			return;
		}

		if (i==0)
			prl->Init(pb_last,pb,0,1,0,0,1,0);
		else
			prl->Init(pb_last,pb,i-0.5f,1.5,0,0,0,1);

#if 1
		palAngularMotor *pam = dynamic_cast<palAngularMotor *>(PF->CreateObject("palAngularMotor"));
		if (!pam) {
			printf("Error: Could not create a Angular Motor\n");
			return;
		}
		motors.push_back(pam);
		desired.push_back(0);
		PID *pid;
		pid = new PID;
		pid->Init(2,0.1,0);
		pids.push_back(pid);
		pam->Init(prl,50.0f);
#endif

		pb_last = pb;
	}
	PF->GetActivePhysics()->SetGroupCollision(1,1,false);
}

void Test_Motor::Init(int terrain_type) {
	CreateTerrain(terrain_type);
	
}

void Test_Motor::Update()
{
	int i;
	for (i=0;i<bodies.size();i++) {
		bodies[i]->SetActive(true);
	}
	for (i=0;i<motors.size();i++) {
		printf("motor[%d]: desired: [%+6.4f] actual:[%+6.4f] diff:[%+4.2f]\n",i,desired[i],motors[i]->GetLink()->GetAngle(),diff_angle(desired[i],motors[i]->GetLink()->GetAngle()));
		float pid_out = pids[i]->Update(diff_angle(desired[i],motors[i]->GetLink()->GetAngle()),0.01);
		motors[i]->Update(pid_out);
		motors[i]->Apply();
	}
	if (g_drive)
	if (desired.size()>0)
	if (fmodf(PF->GetActivePhysics()->GetTime(),40)>20) {
		desired[0] = 0.25f; if (desired.size()>2) {
		desired[1] = -0.75f;
		desired[2] = 1.5f;}
	} else {
		desired[0] =-0.25f; if (desired.size()>2) {
		desired[1] = 0.0f;
		desired[2] = 0.0f; }
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
					CreateChain(0);
					break;
					case SDLK_3:
					CreateChain(1);
					break;
					case SDLK_4:
					CreateChain(2);
					break;

				case SDLK_9:
					CreateRobot();
				break;
				case SDLK_SPACE:
					palBox *pb;
				pb = dynamic_cast<palBox *>(CreateBody("palBox",sfrand()*3,ufrand()*3+1.5f,4,0.25f,0.25f,0.25f,5));
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
					return;
				} 
				pb->ApplyImpulse(0,0,-35);
				BuildGraphics(pb);		
				break;
			
			} 
		break;
	}
}