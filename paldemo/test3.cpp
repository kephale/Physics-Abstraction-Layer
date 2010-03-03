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

		// ------------ Real materials ------------
		// Author: Brock Woolf (brock.woolf@gmail.com)
		// Values referenced from here: http://www.roymech.co.uk/Useful_Tables/Tribology/co_of_frict.htm
		
		// Aluminium
		matDesc_Aluminium.m_fStatic = 1.05f;
		matDesc_Aluminium.m_fKinetic = 1.40f;
		matDesc_Aluminium.m_fRestitution = 0.0f;
		// Diamond
		matDesc_Diamond.m_fStatic = 0.1f;
		matDesc_Diamond.m_fKinetic = 0.05f;
		matDesc_Diamond.m_fRestitution = 0.0f;
		// Oak
		matDesc_Oak.m_fStatic = 0.54f;
		matDesc_Oak.m_fKinetic = 0.32f;
		matDesc_Oak.m_fRestitution = 0.0f;
		// Steel
		matDesc_Steel.m_fStatic = 0.74f;
		matDesc_Steel.m_fKinetic = 0.57f;
		matDesc_Steel.m_fRestitution = 0.0f;
		// Wood
		matDesc_Wood.m_fStatic = 0.2f;
		matDesc_Wood.m_fKinetic = 0.1f;
		matDesc_Wood.m_fRestitution = 0.1f;
		// Aluminum-Steel
		matDesc_Aluminum_Steel.m_fStatic = 0.61f;
		matDesc_Aluminum_Steel.m_fKinetic = 0.47f;
		matDesc_Aluminum_Steel.m_fRestitution = 0.0f;

		pm->NewMaterial("Aluminum", matDesc_Aluminium);
		pm->NewMaterial("Diamond", matDesc_Diamond);
		pm->NewMaterial("Oak", matDesc_Oak);
		pm->NewMaterial("Steel", matDesc_Steel);
		pm->NewMaterial("Wood", matDesc_Wood);

		pm->SetMaterialInteraction("Aluminum", "Steel", matDesc_Aluminum_Steel);

		// ------------ Test materials ------------
		// Normal
		matDesc_Normal.m_fStatic = 0.5f;
		matDesc_Normal.m_fKinetic = 0.5f;
		matDesc_Normal.m_fRestitution = 0.5f;
		// Jumpy
		matDesc_Jumpy.m_fStatic = 1.0f;
		matDesc_Jumpy.m_fKinetic = 1.0f;
		matDesc_Jumpy.m_fRestitution = 1.0f;
		// Stubborn
		matDesc_Stubborn.m_fStatic = 1.0f;
		matDesc_Stubborn.m_fKinetic = 1.0f;
		matDesc_Stubborn.m_fRestitution = 0.0f;
		// LittleR
		matDesc_LittleR.m_fStatic = 1.0f;
		matDesc_LittleR.m_fKinetic = 1.0f;
		matDesc_LittleR.m_fRestitution = 0.3f;
		// Sliding
		matDesc_Sliding.m_fStatic = 1.0f;
		matDesc_Sliding.m_fKinetic = 0.1f;
		matDesc_Sliding.m_fRestitution = 1.0f;
		// Unmoving
		matDesc_Unmoving.m_fStatic = 1.0f;
		matDesc_Unmoving.m_fKinetic = 100.0f;
		matDesc_Unmoving.m_fRestitution = 1.0f;
		// LittleK
		matDesc_LittleK.m_fStatic = 1.0f;
		matDesc_LittleK.m_fKinetic = 0.9f;
		matDesc_LittleK.m_fRestitution = 1.0f;
		// Smooth
		matDesc_Smooth.m_fStatic = 0.1f;
		matDesc_Smooth.m_fKinetic = 1.0f;
		matDesc_Smooth.m_fRestitution = 1.0f;
		// Sticky
		matDesc_Sticky.m_fStatic = 10.0f;
		matDesc_Sticky.m_fKinetic = 1.0f;
		matDesc_Sticky.m_fRestitution = 1.0f;
		// LittleS
		matDesc_LittleS.m_fStatic = 0.2f;
		matDesc_LittleS.m_fKinetic = 1.0f;
		matDesc_LittleS.m_fRestitution = 1.0f;
		// Normal-Jumpy
		matDesc_NormalJumpy.m_fStatic = 1.0f;
		matDesc_NormalJumpy.m_fKinetic = 1.0f;
		matDesc_NormalJumpy.m_fRestitution = 1.0f;
		// Normal-Stubborn
		matDesc_NormalStubborn.m_fStatic = 1.0f;
		matDesc_NormalStubborn.m_fKinetic = 1.0f;
		matDesc_NormalStubborn.m_fRestitution = 0.0f;
		// Normal-LittleR
		matDesc_NormalLittleR.m_fStatic = 1.0f;
		matDesc_NormalLittleR.m_fKinetic = 1.0f;
		matDesc_NormalLittleR.m_fRestitution = 0.3f;

		pm->NewMaterial("Normal", matDesc_Normal);
		//test restitution
		pm->NewMaterial("Jumpy", matDesc_Jumpy);
		pm->NewMaterial("Stubborn", matDesc_Stubborn);
		pm->NewMaterial("LittleR", matDesc_LittleR);

		pm->SetMaterialInteraction("Normal", "Jumpy", matDesc_NormalJumpy);
		pm->SetMaterialInteraction("Normal", "Stubborn", matDesc_NormalStubborn);
		pm->SetMaterialInteraction("Normal", "LittleR", matDesc_NormalLittleR);

		//test kinetic friction
		pm->NewMaterial("Sliding", matDesc_Sliding);
		pm->NewMaterial("Unmoving", matDesc_Unmoving);
		pm->NewMaterial("LittleK", matDesc_LittleK);

		//test static friction
		pm->NewMaterial("Smooth", matDesc_Smooth);
		pm->NewMaterial("Sticky", matDesc_Sticky);
		pm->NewMaterial("LittleS", matDesc_LittleS);

		if ( pTerrain == NULL ) {
			printf("Error: Cannot choose no terrain for the Material test\n");
		}
		
		pTerrain->SetMaterial( pm->GetMaterial("Normal") );
	}

void Test_3::Input(SDL_Event E) {
		
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
