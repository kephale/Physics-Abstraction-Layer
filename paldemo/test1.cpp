#include "test1.h"

FACTORY_CLASS_IMPLEMENTATION(Test_1);

void Test_1::Input(SDL_Event E) {
		int i,j;
		Float x,y,z;
		palBodyBase *pb= NULL;
		palCompoundBodyBase *pcb = NULL;
		switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
			case SDLK_1:
				pb = CreateBody("palBox",sfrand()*3,sfrand()*2+5.0f,sfrand()*3,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
				} 
				break;
			case SDLK_q:
				pb = CreateBody("palStaticBox",sfrand()*3,sfrand()*2+3.0f,sfrand()*3,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
				if (pb == NULL) {
					printf("Error: Could not create a static box\n");
				} 
				break;
			case SDLK_2:
				palSphere *ps = PF->CreateObjectAs<palSphere>("palSphere");
				if (ps) {
					ps->Init(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,0.5f*ufrand()+0.05f,1);
					BuildGraphics(ps);
				} else {
					printf("Error: Could not create a sphere\n");
				} 
				pb = ps;
				break;
			case SDLK_w:
				pb = CreateBody("palStaticSphere",sfrand()*3,sfrand()*2+3.0f,sfrand()*3,0.5*ufrand()+0.05f,0,0,1);
				if (pb == NULL) {
					printf("Error: Could not create a static sphere\n");
				} 
				break;
			case SDLK_3:
				palCapsule *pc = PF->CreateObjectAs<palCapsule>("palCapsule");
				if (pc) {
					float radius=0.5f*ufrand()+0.05f;
					pc->Init(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,radius,radius+ufrand()+0.1f,1);
					BuildGraphics(pc);
				} else {
					printf("Error: Could not create a cylinder\n");
				} 
				pb = pc;
				break;
			case SDLK_4:
				{
					int dist;
					dist = 3;
					float mult;
					mult = 1.0f;
					for (j=-dist;j<dist;j++)
						for (i=-dist;i<dist;i++) {
							pb = CreateBody("palBox",i*mult,5.0f,j*mult,0.25,0.25,0.25,1.0f);
						}
					pb = 0;
				}
				break;
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
				{
					x = sfrand()*3;
					y = sfrand()*2+3.0f;
					z = sfrand()*3;
					palCompoundBody * pcb_local = PF->CreateObjectAs<palCompoundBody>("palCompoundBody");
					pcb = pcb_local;
					if (!pcb_local)
						return;
					pcb_local->Init(x,y,z);
				}
				// No break here
			case SDLK_t:
				if (!pcb) {
					x = sfrand()*3;
					y = sfrand()*2+3.0f;
					z = sfrand()*3;
					palStaticCompoundBody * pcb_local = PF->CreateObjectAs<palStaticCompoundBody>("palStaticCompoundBody");
					pcb = pcb_local;
					if (!pcb_local)
						return;
					pcb_local->Init(x,y,z);
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
				pcv=PF->CreateObjectAs<palConvex>("palConvex");
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
				pcv=PF->CreateObjectAs<palStaticConvex>("palStaticConvex");
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
				{
//					palCompoundBody *pcb;
					pcb = NULL;
					palCompoundBody * pcb_local = PF->CreateObjectAs<palCompoundBody>("palCompoundBody");
					pcb = pcb_local;
					if (pcb) {
						
						Float x = sfrand()*3;
						Float y = sfrand()*2+5.0f;
						Float z = sfrand()*3;

						pcb_local->Init(x,y,z);
						

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
				}
				break;	
			case SDLK_8:
				{
					if (bodies.size()>0) {
						int r= rand() % bodies.size();
						if (bodies[r]->isBody()) {
							palBody * body = static_cast<palBody*>(bodies[r]);
							body->SetPosition(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,ufrand()*M_PI,ufrand()*M_PI,ufrand()*M_PI);
							body->SetActive(true);
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