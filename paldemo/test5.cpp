#include "test5.h"

FACTORY_CLASS_IMPLEMENTATION(Test_5);


void Test_5::Update() {
	for (unsigned int i=0;i<psds.size();i++)
		printf("psd[%d]=%f\n",i,psds[i]->GetDistance());
}

palBody *gpb;

void Test_5::Input(SDL_Event E) {
		int i,j;
		palBody *pb= NULL;
		switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
			case SDLK_9:
				printf("impulse\n");
				gpb->ApplyAngularImpulse(0,10,0);
				break;
			case SDLK_1:
				pb = dynamic_cast<palBody *>( CreateBody("palBox",0,2,0, 1,1,1, 1));
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
				} 
				palPSDSensor *psd;
				psd = PF->CreatePSDSensor();
				if (!psd)
					return;
				psd->Init(pb,0,0,0,1,0,0,20);
				psds.push_back(psd);
				gpb=pb;
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