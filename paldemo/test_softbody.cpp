#include "test_softbody.h"
#include "../pal/palSoftBody.h"

FACTORY_CLASS_IMPLEMENTATION(Test_SoftBody);



PAL_VECTOR<palSoftBody *> g_SoftBodies;

void Test_SoftBody::AdditionalRender() {

	if (g_SoftBodies.size()==0) return;

	glColor3f(1,1,1);
	glBegin(GL_POINTS);
	
	for (unsigned int j=0; j<g_SoftBodies.size();j++) {
		palVector3 *pp = g_SoftBodies[j]->GetParticlePositions();
		for (int i=0; i<g_SoftBodies[j]->GetNumParticles(); i++)
		{
			glVertex3f(	pp[i].x,
				pp[i].y,
				pp[i].z);
		}
	}
	glEnd();

}

void Test_SoftBody::Input(SDL_Event E) {
		int i,j;
		Float x,y,z;
		palBodyBase *pb= NULL;
		palCompoundBodyBase *pcb = NULL;
		switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
			case SDLK_1:
				{
					palPatchSoftBody *psb = dynamic_cast<palPatchSoftBody *>(PF->Construct("palPatchSoftBody"));
					float v[8*8*3];
					int xDim = 8;
					int yDim = 8;
					int nv = xDim*yDim;
				
					for (i=0;i<xDim;i++)
						for (j=0;j<yDim;j++) {
							v[(i+j*xDim)*3+0] = (i *0.5f); //x
							v[(i+j*xDim)*3+1] = (j*0.5f)+2; //y 
							v[(i+j*xDim)*3+2] = 0; //z
						}

					int x,y,iTriIndex = 0;
					int ind[8*8*6];
					for (y=0;y < yDim-1;y++)
					for (x=0;x < xDim-1;x++) {
								ind[iTriIndex*3+0]=(y*xDim)+x;
								ind[iTriIndex*3+1]=(y*xDim)+xDim+x;
								ind[iTriIndex*3+2]=(y*xDim)+x+1;
								// Move to the next triangle in the array
								iTriIndex += 1;

								ind[iTriIndex*3+0]=(y*xDim)+x+1;
								ind[iTriIndex*3+1]=(y*xDim)+xDim+x;
								ind[iTriIndex*3+2]=(y*xDim)+x+xDim+1;
								// Move to the next triangle in the array
								iTriIndex += 1;
							}
					psb->Init(v,0,nv,ind,iTriIndex*3);
					g_SoftBodies.push_back(psb);
				}
				break;
			case SDLK_2:
				{
					palPatchSoftBody *psb = dynamic_cast<palPatchSoftBody *>(PF->Construct("palPatchSoftBody"));
					PAL_VECTOR<float> verts;
					int n = 48;
					float size = 1;
					for(int i=0;i<n;i++)
					{
						float	p=0.5,t=0;
						for(int j=i;j;p*=0.5,j>>=1) if(j&1) t+=p;
						float	w=2*t-1;
						float	a=(M_PI+2*i*M_PI)/n;
						float	s=sqrt(1-w*w);
						verts.push_back(s*cos(a)*size);
						verts.push_back(s*sin(a)*size + size*2+1);
						verts.push_back(w*size);
					}
					int *pInd;
					int nInd;
					palConvexGeometry::GenerateHull_Indices(&verts[0],verts.size()/3,&pInd,nInd);
					psb->Init(&verts[0],0,verts.size()/3,pInd,nInd);
					g_SoftBodies.push_back(psb);
				}
				break;
		case SDLK_3:
				{
					palTetrahedralSoftBody *psb = dynamic_cast<palTetrahedralSoftBody *>(PF->Construct("palTetrahedralSoftBody"));
					float wx=2;
					float wy=2;
					float wz=2;
					float h =1;
					int numX = (int)(wx / h) + 1;
					int numY = (int)(wy / h) + 1;
					int numZ = (int)(wz / h) + 1;
					PAL_VECTOR<float> verts;
					int i,j,k;
					for (i = 0; i <= numX; i++) {
						for (j = 0; j <= numY; j++) {
							for (k = 0; k <= numZ; k++) {
								verts.push_back(h*i - h * numX * 0.5f);
								verts.push_back(h*j	+ h * numY +1);
								verts.push_back(h*k - h * numZ * 0.5f); 
							}
						}
					}
					int i1,i2,i3,i4,i5,i6,i7,i8;
					int nInd = numX*numY*numZ*5*4; 
					int *pInd = new int[nInd];
					int *id = pInd;
					for (i = 0; i < numX; i++) {
						for (j = 0; j < numY; j++) {
							for (k = 0; k < numZ; k++) {
								i5 = (i*(numY+1) + j)*(numZ+1) + k; i1 = i5+1;
								i6 = ((i+1)*(numY+1) + j)*(numZ+1) + k; i2 = i6+1;
								i7 = ((i+1)*(numY+1) + (j+1))*(numZ+1) + k; i3 = i7+1;
								i8 = (i*(numY+1) + (j+1))*(numZ+1) + k; i4 = i8+1;

								if ((i + j + k) % 2 == 1) {
									*id++ = i1; *id++ = i2; *id++ = i6; *id++ = i3;
									*id++ = i6; *id++ = i3; *id++ = i7; *id++ = i8;
									*id++ = i1; *id++ = i8; *id++ = i4; *id++ = i3;
									*id++ = i1; *id++ = i6; *id++ = i5; *id++ = i8;
									*id++ = i1; *id++ = i3; *id++ = i6; *id++ = i8;
								}
								else {
									*id++ = i2; *id++ = i5; *id++ = i1; *id++ = i4;
									*id++ = i2; *id++ = i7; *id++ = i6; *id++ = i5;
									*id++ = i2; *id++ = i4; *id++ = i3; *id++ = i7;
									*id++ = i5; *id++ = i7; *id++ = i8; *id++ = i4;
									*id++ = i2; *id++ = i5; *id++ = i4; *id++ = i7;
								}
							}
						}
					}
					psb->Init(&verts[0],0,verts.size()/3,pInd,nInd);
					g_SoftBodies.push_back(psb);
				}
				break;
#if 0
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
				pb = CreateBody("palStaticSphere",sfrand()*3,sfrand()*2+3.0f,sfrand()*3,0.5*ufrand()+0.05f,0,0,1);
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
#endif
			case SDLK_4:
				{
				int dist;
				dist = 3;
				float mult;
				mult = 1.0f;
				for (j=-dist;j<dist;j++)
					for (i=-dist;i<dist;i++) {
						pb = CreateBody("palBox",i*mult,5.0f,j*mult,0.5,0.5,0.5,1.0f);
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
			case SDLK_8:
				{
					if (bodies.size()>0) {
						int r= rand() % bodies.size();
						palBody *body = dynamic_cast<palBody*>(bodies[r]);
							if (body) {
						
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
