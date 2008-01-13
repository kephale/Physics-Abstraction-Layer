#include "test3.h"

FACTORY_CLASS_IMPLEMENTATION(Test_3);

void Test_3::InitMaterials() {
		palMaterials *pm=NULL;
		//create the materials collection
		pm = PF->CreateMaterials();		
		if (pm == NULL) {
			printf("could not create materials!\n");
			return;
		}
		
		//real materials
		pm->NewMaterial("Aluminum", 1.05, 1.4,0);
		pm->NewMaterial("Diamond", 0.1, 0.05, 0);
		pm->NewMaterial("Oak", 0.54, 0.32, 0);
		pm->NewMaterial("Steel", 0.74, 0.57, 0);
		pm->NewMaterial("Wood",0.2,0.1,0.1);

		pm->SetMaterialInteraction("Aluminum", "Steel", 0.61, 0.47, 0);

		//test materials:

		pm->NewMaterial("Normal",0.5f, 0.5f, 0.5f);
		//test restitution
		pm->NewMaterial("Jumpy",1, 1, 1);
		pm->NewMaterial("Stubborn", 1, 1, 0);
		pm->NewMaterial("LittleR", 1,1,0.3);

		pm->SetMaterialInteraction("Normal", "Jumpy",1, 1, 1);
		pm->SetMaterialInteraction("Normal", "Stubborn", 1, 1, 0);
		pm->SetMaterialInteraction("Normal", "LittleR", 1, 1, 0.3);

		//test kinetic friction
		pm->NewMaterial("Sliding", 1, 0.1,1);
		pm->NewMaterial("Unmoving", 1, 100, 1);
		pm->NewMaterial("LittleK", 1,0.9,1);

		//test static friction
		pm->NewMaterial("Smooth", 0.1, 1, 1);
		pm->NewMaterial("Sticky", 10, 1, 1);
		pm->NewMaterial("LittleS", 0.2, 1, 1);

		pTerrain->SetMaterial(pm->GetMaterial("Normal") );
	}

void Test_3::Input(SDL_Event E) {
		int i,j;
		palBodyBase *pb= NULL;
		palMaterials *pm = PF->CreateMaterials();
		switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
			case SDLK_1:
				//restitution test
				pb = CreateBody("palBox",-2,3,0,1,1,1,1);
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
					return;
				}
				palMaterial *pmu=NULL;
				pmu=pm->GetMaterial("Stubborn");
				if (pmu==NULL) {
					printf("could not get material\n");
					return;
				}
				printf("pmu stubborn is %f %f %f\n",pmu->m_fStatic,pmu->m_fKinetic,pmu->m_fRestitution);
					
				pb->SetMaterial(pmu);

				pb = CreateBody("palBox",0,3,0,1,1,1,1);
				pmu=pm->GetMaterial("LittleR");
				if (pmu==NULL) {
					printf("could not get material\n");
					return;
				}
				printf("pmu littleR is %f %f %f\n",pmu->m_fStatic,pmu->m_fKinetic,pmu->m_fRestitution);
				pb->SetMaterial(pmu);

				pb = CreateBody("palBox",2,3,0,1,1,1,1);
				pmu=pm->GetMaterial("Jumpy");
				if (pmu==NULL) {
					printf("could not get material\n");
					return;
				}
				pb->SetMaterial(pmu);
				
				break;			
			} 
			break;
		}
	}