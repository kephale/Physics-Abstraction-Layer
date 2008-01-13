#include "test6.h"

FACTORY_CLASS_IMPLEMENTATION(Test_6);

void Test_6::Create_palRagDoll(float *pos) {
	palSphericalLink *neck; 
	palSphericalLink *shoulder1;
	palSphericalLink *shoulder2;
	palSphericalLink *hip1;
	palSphericalLink *hip2;
	palRevoluteLink *elbow1;
	palRevoluteLink *elbow2;
	palRevoluteLink *knee1;
	palRevoluteLink *knee2;
//	palRevoluteLink *wrist1;
//	palRevoluteLink *wrist2;
	palRevoluteLink *ankle1;
	palRevoluteLink *ankle2;

			palBox *head;
			palBox *foot1;
			palBox *foot2;
			palSphere *body;
//			palSphere *hand1;
//			palSphere *hand2;
			palCapsule *arm1;
			palCapsule *arm2;
			palCapsule *arm3;
			palCapsule *arm4;
			palCapsule *leg1;
			palCapsule *leg2;
			palCapsule *leg3;
			palCapsule *leg4;
			head = NULL;
			body = NULL;
			arm1 = NULL;
			arm2 = NULL;
			arm3 = NULL;
			arm4 = NULL;
			leg1 = NULL;
			leg2 = NULL;
			leg3 = NULL;
			leg4 = NULL;
			foot1 = NULL;
			foot2 = NULL;
//			hand1 = NULL;
//			hand2 = NULL;

			
			head = dynamic_cast<palBox *>(PF->CreateObject("palBox"));
			foot1 = dynamic_cast<palBox *>(PF->CreateObject("palBox"));
			foot2 = dynamic_cast<palBox *>(PF->CreateObject("palBox"));
			body = dynamic_cast<palSphere *>(PF->CreateObject("palSphere"));
//			hand1 = dynamic_cast<palSphere *>(PF->CreateObject("palSphere"));
//			hand2 = dynamic_cast<palSphere *>(PF->CreateObject("palSphere"));
			arm1 = dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
			arm2 = dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
			arm3 = dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
			arm4 = dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
			leg1 = dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
			leg2 = dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
			leg3 = dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
			leg4 = dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));

			
			//float pos[3];
			int i;
/*
			for (i=0;i<3;i++)
				pos[i]=sfrand()*3;
				pos[1]+=3.0f;
*/
			if ((head)&&(body)&&(arm1)&&(arm2)&&(arm3)&&(arm4)&&(leg1)&&(leg2)&&(leg3)&&(leg4)&&(foot1)&&(foot2)) {
				
			} else {
				printf("Error: Could not create a object\n");
				return;
			} 

			head->Init(pos[0],	      pos[1]+0.5,	pos[2],		0.3,  0.3,  0.3,  1);
				body->Init(pos[0],	      pos[1],		pos[2],		0.38, 1);
				arm1->Init(pos[0]-0.38,   pos[1]+0.25,  pos[2],		0.1,  0.25, 1);
				arm2->Init(pos[0]+0.38,   pos[1]+0.25,  pos[2],		0.1,  0.25, 1);
				arm3->Init(pos[0]-0.38,   pos[1]+0.6,	pos[2],		0.1,  0.25, 1);
				arm4->Init(pos[0]+0.38,   pos[1]+0.6,	pos[2],		0.1,  0.25, 1);
				leg1->Init(pos[0]-0.1,    pos[1]-0.6,   pos[2],		0.1,  0.3,  1);
				leg2->Init(pos[0]+0.1,    pos[1]-0.6,   pos[2],		0.1,  0.3,  1);
				leg3->Init(pos[0]-0.1,    pos[1]-1.1,   pos[2],		0.1,  0.3,  1);
				leg4->Init(pos[0]+0.1,    pos[1]-1.1,   pos[2],		0.1,  0.3,  1);
//				hand1->Init(pos[0]-0.38,  pos[1]+0.72,  pos[2],		0.13, 1);
//				hand2->Init(pos[0]+0.38,  pos[1]+0.72,  pos[2],		0.13, 1);
				foot1->Init(pos[0]-0.1,   pos[1]-1.25,  pos[2]+0.1, 0.2,  0.2,  0.4,  1);
				foot2->Init(pos[0]+0.1,   pos[1]-1.25,  pos[2]+0.1, 0.2,  0.2,  0.4,  1);

				neck = NULL;
				shoulder1 = NULL;
				shoulder2 = NULL;
				hip1 = NULL;
				hip2 = NULL;
				elbow1 = NULL;
				elbow2 = NULL;
				knee1 = NULL;
				knee2 = NULL;
//				wrist1 = NULL;
//				wrist2 = NULL;
				ankle1 = NULL;
				ankle2 = NULL;

				neck = dynamic_cast<palSphericalLink *>(PF->CreateObject("palSphericalLink"));
				shoulder1 = dynamic_cast<palSphericalLink *>(PF->CreateObject("palSphericalLink"));
				shoulder2 = dynamic_cast<palSphericalLink *>(PF->CreateObject("palSphericalLink"));
				hip1 = dynamic_cast<palSphericalLink *>(PF->CreateObject("palSphericalLink"));
				hip2 = dynamic_cast<palSphericalLink *>(PF->CreateObject("palSphericalLink"));
				elbow1 = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
				elbow2 = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
				knee1 = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
				knee2 = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
//				wrist1 = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
//				wrist2 = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
				ankle1 = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
				ankle2 = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));

				if (neck == NULL) {
					printf("Error: Could not create a neck\n");
					return;
				}
				if (shoulder1 == NULL) {
					printf("Error: Could not create a shoulder1\n");
					return;
				}
				if (shoulder2 == NULL) {
					printf("Error: Could not create a shoulder2\n");
					return;
				}
				if (hip1 == NULL) {
					printf("Error: Could not create a hip1\n");
					return;
				}
				if (hip2 == NULL) {
					printf("Error: Could not create a hip2\n");
					return;
				}
				if (elbow1 == NULL) {
					printf("Error: Could not create a elbow1\n");
					return;
				}
				if (elbow2 == NULL) {
					printf("Error: Could not create a elbow2\n");
					return;
				}
				if (knee1 == NULL) {
					printf("Error: Could not create a knee1\n");
					return;
				}
				if (knee2 == NULL) {
					printf("Error: Could not create a knee2\n");
					return;
				}
/*				if (wrist1 == NULL) {
					printf("Error: Could not create a wrist1\n");
					return;
				}
				if (wrist2 == NULL) {
					printf("Error: Could not create a wrist2\n");
					return;
				}*/
				if (ankle1 == NULL) {
					printf("Error: Could not create a ankle1\n");
					return;
				}
				if (ankle2 == NULL) {
					printf("Error: Could not create a ankle2\n");
					return;
				}

				neck	 ->Init(body,  head,  pos[0],	  pos[1]+0.4,  pos[2]);
                shoulder1->Init(body,  arm1,  pos[0]-0.3, pos[1]+0.1,  pos[2]);
				shoulder2->Init(body,  arm2,  pos[0]+0.3, pos[1]+0.1,  pos[2]);
				hip1	 ->Init(body,  leg1,  pos[0],	  pos[1]-0.6,  pos[2]);
				hip2	 ->Init(body,  leg2,  pos[0],	  pos[1]-0.6,  pos[2]);	
				elbow1	 ->Init(arm1,  arm3,  pos[0]-0.3, pos[1]+0.3,  pos[2],  0, 0, 1);
				elbow2	 ->Init(arm2,  arm4,  pos[0]+0.3, pos[1]+0.3,  pos[2],  0, 0, 1);
				knee1	 ->Init(leg1,  leg3,  pos[0]-0.1, pos[1]-0.9,  pos[2],  0, 0, 1);
				knee2	 ->Init(leg2,  leg4,  pos[0]+0.1, pos[1]-0.9,  pos[2],  0, 0, 1);
//				wrist1	 ->Init(hand1, arm3,  pos[0]-0.3, pos[1]+0.5,  pos[2],  0, 0, 1);
//				wrist2	 ->Init(hand2, arm4,  pos[0]+0.3, pos[1]+0.5,  pos[2],  0, 0, 1);
				ankle1	 ->Init(foot1, leg3,  pos[0]-0.1, pos[1]-1.2,  pos[2],  0, 0, 1);
				ankle2	 ->Init(foot2, leg4,  pos[0]+0.1, pos[1]-1.2,  pos[2],  0, 0, 1);

				BuildGraphics(head);
				BuildGraphics(body);
				BuildGraphics(arm1);
				BuildGraphics(arm2);
				BuildGraphics(arm3);
				BuildGraphics(arm4);
				BuildGraphics(leg1);
				BuildGraphics(leg2);
				BuildGraphics(leg3);
				BuildGraphics(leg4);
//				BuildGraphics(hand1);
//				BuildGraphics(hand2);
				BuildGraphics(foot1);
				BuildGraphics(foot2);

	}




void Test_6::Update() {
/*	for (unsigned int i=0;i<psds.size();i++)
		printf("psd[%d]=%f\n",i,psds[i]->GetDistance());
*/
}

palBody *t6_gpb;

void Test_6::Input(SDL_Event E) {
		int i,j;
		palBody *pb= NULL;
		switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
			case SDLK_9:
				printf("impulse\n");
				t6_gpb->ApplyAngularImpulse(0,10,0);
				break;
			case SDLK_1:
				/*
				pb = CreateBody("palBox",0,2,0, 1,1,1, 1);
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
				} 
				t6_gpb=pb;*/{
					float pos[3];
					pos[0] = sfrand()*3;
					pos[1] = ufrand()*3+3;
					pos[2] = sfrand()*3;
				Create_palRagDoll(pos);
					   }
				break;
			case SDLK_2:
				palSphere *ps;
				ps = NULL;
				ps=dynamic_cast<palSphere *>(PF->CreateObject("palSphere"));
				if (ps) {
					ps->Init(5+sfrand()*2,4,0,0.3,1);
				//	ps->Init(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,0.5f*ufrand()+0.05f,1);
					BuildGraphics(ps);
				} else {
					printf("Error: Could not create a sphere\n");
				} 
				break;
			case SDLK_3:
				palCapsule *pc;
				pc = NULL;
				pc=dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
				if (pc) {
					float radius=0.5f*ufrand()+0.05f;
					pc->Init(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,radius,radius+ufrand()+0.1f,1);
					BuildGraphics(pc);
				} else {
					printf("Error: Could not create a cylinder\n");
				} 
				break;
			} 
			break;
		}
	}