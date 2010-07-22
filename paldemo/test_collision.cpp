#include "test_collision.h"
#include "../pal/palCollision.h"

FACTORY_CLASS_IMPLEMENTATION(Test_Collision);
/*
void MakeConvexCone(Float *pVerts) {
	int i;
	for (i=0;i<36;i++) {
		pVerts[i*3+0] = sin(i*10*DEG2RAD);
		pVerts[i*3+1] = cos(i*10*DEG2RAD);
		pVerts[i*3+2] = 0;
	}
	for (i=36;i<36+36;i++) {
		pVerts[i*3+0] = 0.5*sin(i*10*DEG2RAD);
		pVerts[i*3+1] = 0.5*cos(i*10*DEG2RAD);
		pVerts[i*3+2] = 0.5;
	}
	i=36+36;
	pVerts[i*3+0] = 0;
	pVerts[i*3+1] = 0;
	pVerts[i*3+2] = 1;
}
*/

void Test_Collision::AdditionalRender() {
	palPhysics*pf = PF->GetActivePhysics();
	if (!pf) return;
	palCollisionDetection *pcd = dynamic_cast<palCollisionDetection *>(pf);
	if (!pcd) return;

	
	glPointSize(6.0);
	glLineWidth(3.0);
	for (float a=0;a<M_PI*2;a+=0.1f) {

		float x = cosf(a);
		float y = 0.2;
		float z = sinf(a);
		palRayHit hit;
		if (a<M_PI)
			pcd->RayCast(x*10,y,z*10,-x,0,-z,20,hit);
		else {
			x = a - M_PI;
			y = 5;
			z = 0;
			pcd->RayCast(x,y,z,0,-1,0,10,hit);
		}
		if (hit.m_bHitPosition) {
			glColor3f(1,1,1);
			glBegin(GL_LINES);
			glVertex3f(x,y,z);
			glVertex3fv(hit.m_vHitPosition._vec);
			glEnd();

			glDisable(GL_DEPTH_TEST);
			glBegin(GL_POINTS);
			glVertex3f(x,y,z);
			glVertex3fv(hit.m_vHitPosition._vec);
			glEnd();
			glEnable(GL_DEPTH_TEST);
		}
		if (hit.m_bHitNormal) {
			glColor3f(0,0,1);
			glBegin(GL_LINES);
			glVertex3fv(hit.m_vHitPosition._vec);
			palVector3 end_norm;
			vec_add(&end_norm,&hit.m_vHitPosition,&hit.m_vHitNormal);
			glVertex3fv(end_norm._vec);
			glEnd();
		}
	}

	glDisable(GL_DEPTH_TEST);
	for (unsigned int i=0;i<bodies.size();i++) {
		
		palContact c;
		pcd->GetContacts(bodies[i],c);
		for (unsigned int j=0;j<c.m_ContactPoints.size();j++) {
			
		
			glColor3f(1,1,1);
			glBegin(GL_POINTS);
			glVertex3fv( c.m_ContactPoints[j].m_vContactPosition._vec);
			glEnd();
			
		}
	}
	glEnable(GL_DEPTH_TEST);
	glPointSize(1.0);
}

void Test_Collision::Input(SDL_Event E) {
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
				pb = CreateBody("palStaticBox",sfrand()*3,sfrand()*2+5.0f,sfrand()*3,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
				if (pb == NULL) {
					printf("Error: Could not create a static box\n");
				} 
				break;
			case SDLK_2:
				palSphere *ps;
				ps = NULL;
				ps=dynamic_cast<palSphere *>(PF->CreateObject("palSphere"));
				if (ps) {
					ps->Init(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,0.5f*ufrand()+0.05f,1);
					BuildGraphics(ps);
				} else {
					printf("Error: Could not create a sphere\n");
				} 
				pb = ps;
				break;
			case SDLK_w:
				pb = CreateBody("palStaticSphere",sfrand()*3,sfrand()*2+5.0f,sfrand()*3,0.5*ufrand()+0.05f,0,0,1);
				if (pb == NULL) {
					printf("Error: Could not create a static sphere\n");
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
						bodies.push_back(dynamic_cast<palBody*>(pb));
						palCollisionDetection *pcd = dynamic_cast<palCollisionDetection *>(PF->GetActivePhysics());
						if (!pcd) {
							printf("failed to create collision subsystem\n");
						} else {
							pcd->NotifyCollision(pb,true);
						}
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
			case SDLK_7:
				{
				float xp = sfrand()*3;
				float yp = sfrand()*3;
				pb = CreateBody("palBox",xp,4.0f,yp,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
					return;
				} 
				pb->SetGroup(1);
				pb = CreateBody("palBox",xp,6.0f,yp,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
				pb->SetGroup(1);

				float x= xp;
				float y= 8;
				float z= yp;
				pcb = dynamic_cast<palCompoundBodyBase *>(PF->CreateObject("palCompoundBody"));
				if (!pcb)
					return;
				dynamic_cast<palCompoundBody *>(pcb)->Init(x,y,z);
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
				pcb->SetGroup(1);
				BuildGraphics(pcb);

				PF->GetActivePhysics()->SetGroupCollision(1,1,false);
				}
				break;	
			case SDLK_8:
				{
					if (bodies.size()>0) {
						int r= rand() % bodies.size();
						bodies[r]->SetPosition(sfrand()*3,sfrand()*2+5.0f,sfrand()*3,ufrand()*M_PI,ufrand()*M_PI,ufrand()*M_PI);
						bodies[r]->SetActive(true);
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
				bodies.push_back(dynamic_cast<palBody*>(pb));
				palCollisionDetection *pcd = dynamic_cast<palCollisionDetection *>(PF->GetActivePhysics());
				if (!pcd) {
					printf("failed to create collision subsystem\n");
				} else {
					pcd->NotifyCollision(pb,true);
				}
			}
			break;
		}
	}
