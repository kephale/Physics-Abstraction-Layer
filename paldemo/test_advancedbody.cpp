#include "test_advancedbody.h"


FACTORY_CLASS_IMPLEMENTATION(Test_AdvancedBody);


void Test_AdvancedBody::Update() {
	unsigned int i;
	float time = PF->GetActivePhysics()->GetTime()*0.3;
	for (i=0;i<bodies.size();i++) {
		palGenericBody* pb= NULL;
		pb = dynamic_cast<palGenericBody*>(bodies[i]);
		if (pb) {
			//animate our kinematic friends.
			if (pb->IsKinematic())
				pb->SetPosition(fmod(i*0.3,2)*cos(time+i*0.3),1,sin(time+i*0.3));
		}
	}
};	

void Test_AdvancedBody::Input(SDL_Event E) {
		int i,j;
		Float x,y,z;
		palGenericBody* pb= NULL;
		palBoxGeometry* pbg = NULL;
		palSphereGeometry* psg = NULL;
		palCapsuleGeometry* pcg = NULL;

		palMatrix4x4 m;
		switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
			case SDLK_1:
				//create a generic body
				pb = dynamic_cast<palGenericBody*>(PF->CreateObject("palGenericBody"));
				if (pb == NULL) {
					printf("Error: Could not create a generic body\n");
					return;
				} 
				//create the position matrix
				mat_identity(&m);
				x= sfrand()*3;
				y = sfrand()*2+5.0f;
				z = sfrand()*3;
				mat_translate(&m,x,y,z);
				//intialize the generic body at our location, and give it a mass of 1.
				pb->Init(m);
				pb->SetMass(1.0f);
				//create a geometry
				pbg = dynamic_cast<palBoxGeometry*>(PF->CreateObject("palBoxGeometry"));
				//no change to matrix m -  We are working in GLOBAL coordinates.
				//this means our geometries position will equal the bodies position
				pbg->Init(m,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
				//now connect the geometry to the body.
				pb->ConnectGeometry(pbg);

				BuildGraphics(pb);
				break;
			case SDLK_2:
				pb = dynamic_cast<palGenericBody*>(PF->CreateObject("palGenericBody"));
				if (pb == NULL) {
					printf("Error: Could not create a generic body\n");
					return;
				} 
				mat_identity(&m);
				mat_translate(&m,sfrand()*3,sfrand()*2+5.0f,sfrand()*3);
				pb->Init(m);
				pb->SetMass(1.0f);
				psg = dynamic_cast<palSphereGeometry*>(PF->CreateObject("palSphereGeometry"));
				psg->Init(m,ufrand()*0.5f+0.1f,1);
				pb->ConnectGeometry(psg);
				BuildGraphics(pb);
				break;
			case SDLK_3:
				pb = dynamic_cast<palGenericBody*>(PF->CreateObject("palGenericBody"));
				if (pb == NULL) {
					printf("Error: Could not create a generic body\n");
					return;
				} 
				mat_identity(&m);
				mat_translate(&m,sfrand()*3,sfrand()*2+5.0f,sfrand()*3);
				mat_rotate(&m,90,0,0,1); //make the capsule horizontal
				pb->Init(m);
				pb->SetMass(1.0f);
				pcg = dynamic_cast<palCapsuleGeometry*>(PF->CreateObject("palCapsuleGeometry"));
				x = ufrand()+0.5f;
				pcg->Init(m,x*0.3,x,1);
				pb->ConnectGeometry(pcg);
				//now offset the center of mass in the x-direction by the length
				m._41 -= x;
				//disabled for now!
//				pb->SetCenterOfMass(m);
				BuildGraphics(pb);
				break;
			case SDLK_4:
				{
				int dist;
				dist = 2;
				float mult;
				mult = 1.0f;
				for (j=-dist;j<dist;j++)
					for (i=-dist;i<dist;i++) {
						CreateBody("palBox",i*mult,5.0f,j*mult,mult*0.25,mult*0.25,mult*0.25,1.0f);
					}
				}
				break;
			case SDLK_5:
				if (bodies.size()>0) {
						int r= rand() % bodies.size();
						pb = dynamic_cast<palGenericBody*>(bodies[r]);
						if (pb) {
							pbg = dynamic_cast<palBoxGeometry*>(PF->CreateObject("palBoxGeometry"));
							//get the pose of the body
							m = pb->GetLocationMatrix();
							//shift it a bit for our new geom
							mat_translate(&m,sfrand(),sfrand(),sfrand());
							pbg->Init(m,ufrand()+0.2f,ufrand()+0.2f,ufrand()+0.2f,1);
							pb->ConnectGeometry(pbg);
							//rebuild the graphical representation
							DeleteGraphics(pb);
							BuildGraphics(pb);
						}
				}
				break;
			case SDLK_6:
				if (bodies.size()>0) {
						int r= rand() % bodies.size();
						pb = dynamic_cast<palGenericBody*>(bodies[r]);
						if (pb) {
							if (pb->GetGeometries().size()>1) {
								pb->RemoveGeometry(pb->GetGeometries()[1]);
								//rebuild the graphical representation
								DeleteGraphics(pb);
								BuildGraphics(pb);
							}
						}
				}
				break;
			case SDLK_7:
				if (bodies.size()>0) {
						int r= rand() % bodies.size();
						pb = dynamic_cast<palGenericBody*>(bodies[r]);
						if (pb) {
							if (pb->IsKinematic() == false)
								//pb->SetKinematic(true);
								pb->SetDynamicsType(PALBODY_KINEMATIC);
							else
								//pb->SetKinematic(false);
								pb->SetDynamicsType(PALBODY_DYNAMIC);
						}
				}
				break;
#if 0
			case SDLK_r:
				{
				int dist;
				dist = 3;
				float mult;
				mult = 1.0f;
				for (j=-dist;j<dist;j++)
					for (i=-dist;i<dist;i++) {
						pb = CreateBody("palStaticBox",i*mult,1.0f,j*mult,0.25,0.25,0.25,1.0f);
					}
				pb = 0;
				}
				break;
			case SDLK_5:
				x = sfrand()*3;
				y = sfrand()*2+3.0f;
				z = sfrand()*3;
				pcb = dynamic_cast<palCompoundBodyBase *>(PF->CreateObject("palCompoundBody"));
				if (!pcb)
					return;
				dynamic_cast<palCompoundBody *>(pcb)->Init(x,y,z);
			case SDLK_t:
				if (!pcb) {
					x = sfrand()*3;
					y = sfrand()*2+3.0f;
					z = sfrand()*3;
					pcb = dynamic_cast<palCompoundBodyBase *>(PF->CreateObject("palStaticCompoundBody"));
					if (!pcb)
						return;
					dynamic_cast<palStaticCompoundBody *>(pcb)->Init(x,y,z);
				}
				if (pcb) {
					palBoxGeometry *pbg;
					pbg = pcb->AddBox();
					if (pbg) {
						palMatrix4x4 m;
						mat_identity(&m);
						mat_translate(&m,1+x,y,z);
						pbg->Init(m,1,1,1,1);
					}
					pbg = pcb->AddBox();
					if (pbg) {
						palMatrix4x4 m;
						mat_identity(&m);
						mat_translate(&m,-1+x,y,z);
						pbg->Init(m,1,1,1,1);
					}
					pcb->Finalize();
					BuildGraphics(pcb);
				} else {
					printf("Error: Could not create a compound body\n");
				}
				pb = pcb;
				break;
			case SDLK_6:
				palConvex *pcv;
				pcv = NULL;
				pcv=dynamic_cast<palConvex *>(PF->CreateObject("palConvex"));
				if (pcv) {
					Float pVerts[(36+36+1)*3];
					int nVerts = (36+36+1);
					MakeConvexCone(pVerts);
					pcv->Init(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,pVerts,nVerts,1);
				//	float radius=0.5f*ufrand()+0.05f;
				//	pc->Init(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,radius,radius+ufrand()+0.1f,1);
					BuildGraphics(pcv);
				} else {
					printf("Error: Could not create a convex object\n");
				} 
				pb = pcv;
				break;
			case SDLK_y:
				{
				palStaticConvex *pcv;
				pcv = NULL;
				pcv=dynamic_cast<palStaticConvex *>(PF->CreateObject("palStaticConvex"));
				if (pcv) {
					Float pVerts[(36+36+1)*3];
					int nVerts = (36+36+1);
					MakeConvexCone(pVerts);
					pcv->Init(sfrand()*3,sfrand()*2+3.0f,sfrand()*3,pVerts,nVerts);
				//	float radius=0.5f*ufrand()+0.05f;
				//	pc->Init(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,radius,radius+ufrand()+0.1f,1);
					BuildGraphics(pcv);
				} else {
					printf("Error: Could not create a static convex object\n");
				} 
				pb = pcv;
				}
				break;
			case SDLK_7:
//				palCompoundBody *pcb;
				pcb = NULL;
				pcb = dynamic_cast<palCompoundBody *>(PF->CreateObject("palCompoundBody"));
				if (pcb) {
					
					Float x = sfrand()*3;
					Float y = sfrand()*2+5.0f;
					Float z = sfrand()*3;
					
					dynamic_cast<palCompoundBody *>(pcb)->Init(x,y,z);
					

					Float pVerts[(36+36+1)*3];
					int nVerts = (36+36+1);
					MakeConvexCone(pVerts);
				
					palConvexGeometry *pcg = 0;
					pcg = pcb->AddConvex();
					if (pcg) {
						palMatrix4x4 m;
						mat_identity(&m);
						mat_translate(&m,1+x,y,z);
						pcg->Init(m,pVerts,nVerts,1);
					}
					pcg = pcb->AddConvex();
					if (pcg) {
						palMatrix4x4 m;
						mat_identity(&m);
						mat_translate(&m,-1+x,y,z);
						pcg->Init(m,pVerts,nVerts,1);
					}

					pcb->Finalize();
					BuildGraphics(pcb);
				} else {
					printf("Error: Could not create a convex object\n");
				} 
				pb = pcb;
				break;	
#endif
			case SDLK_8:
				{
					if (bodies.size()>0) {
						int r= rand() % bodies.size();
						pb = dynamic_cast<palGenericBody*>(bodies[r]);
							if (pb) {
							pb->SetPosition(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,ufrand()*M_PI,ufrand()*M_PI,ufrand()*M_PI);
							pb->SetActive(true);
							}
					}
				}
				break;
			case SDLK_9:
				if (bodies.size()>0) {
				DeleteGraphics(bodies[0]);
				delete bodies[0];
				
				bodies.erase(bodies.begin());
				}
				break;
			} 
			if (pb) {
				bodies.push_back(pb);
			}
			break;
		}
	}
