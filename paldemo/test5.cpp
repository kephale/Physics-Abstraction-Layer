#include "test5.h"

FACTORY_CLASS_IMPLEMENTATION(Test_5);


void Test_5::Update() {
	unsigned int i;
	for (i=0;i<psds.size();i++)
		printf("psd[%d]=%f\n",i,psds[i]->GetDistance());
	for (i=0;i<contacts.size();i++) {
		palVector3 c;
		contacts[i]->GetContactPosition(c);
		printf("contact[%d]=%f %f %f\n",i,c.x,c.y,c.z);
	}

}


void Test_5::AdditionalRender() {
	unsigned int i;
	for (i=0;i<psds.size();i++) {
		//glColor3f(1,1,1);
		//glBegin(GL_LINES);
		//palVector3 bpos;
		//glEnd();
	}
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
#if 0
				psd->Init(pb,0,1,0, 0,1,0, 20);
#else
				psd->Init(pb,1,1.5,0, 1,0,0, 20);
#endif
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
				{
				pb = dynamic_cast<palBody *>( CreateBody("palBox",0,2,0, 1,1,1, 1));
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
				} 
				palPSDSensor *psd;
				psd = PF->CreatePSDSensor();
				if (!psd)
					return;
#if 1
				psd->Init(pb,0,1,0, 0,1,0, 20);
#else
				psd->Init(pb,1,1.5,0, 1,0,0, 20);
#endif
				psds.push_back(psd);
				gpb=pb;
				}
				break;
			case SDLK_4:
				pb = dynamic_cast<palBody *>( CreateBody("palBox",0,2,0, 1,1,1, 1));
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
				} 
				palContactSensor *pcs;
				pcs = PF->CreateContactSensor();
				if (!pcs)
					return;
				pcs->Init(pb);
				contacts.push_back(pcs);
				gpb=pb;
				break;
			} 
			break;
		}
	}