#include "test2.h"

FACTORY_CLASS_IMPLEMENTATION(Test_2);


void Test_2::Input(SDL_Event E) {
		palBodyBase *pb1;
		palBodyBase *pb2;

		bool limits=false;

		float pos[3];		
		float dim1[3];				// position it start falling and size of object
		float dim2[3];
		int i;
		for (i=0;i<3;i++)
			pos[i]=sfrand()*3;
		pos[1]+=5.0f;						
		for (i=0;i<3;i++)
			dim1[i]=ufrand()+0.1f;
		for (i=0;i<3;i++)
			dim2[i]=ufrand()+0.1f;

		bool nswap_xy = true; //do not swap xy
			

		switch(E.type) {
		case SDL_KEYDOWN:
		switch (E.key.keysym.sym) {
			case SDLK_z:
				m_BodyType1 = "palBox";
				break;
			case SDLK_x:
				m_BodyType1 = "palSphere";
				break;
			case SDLK_c:
				m_BodyType1 = "palCapsule";
				break;
			case SDLK_a:
				m_BodyType2 = "palBox";
				break;
			case SDLK_s:
				m_BodyType2 = "palSphere";
				break;
			case SDLK_d:
				m_BodyType2 = "palCapsule";
				break;
			case SDLK_v:
				m_BodyType1 = "palStaticBox";
				break;
			case SDLK_b:
				m_BodyType1 = "palStaticSphere";
				break;
			case SDLK_n:
				m_BodyType1 = "palStaticCapsule";
				break;
			case SDLK_4:
				limits=true;
			case SDLK_7:
				if (!limits) {
					pos[0]=2;	pos[1]=3;	pos[2]=0;
					dim1[0]=1;	dim1[1]=1;	dim1[2]=1;
					dim2[0]=1;	dim2[1]=1;	dim2[2]=1;
					CreateBody(m_BodyType1.c_str(),
							pos[0],
							pos[1]-(dim1[1]*1.5),
							pos[2],
							dim1[0],
							dim1[1],
							dim1[2],
							1);
				}
			case SDLK_1:					// a revolute link test 
				pb1 = CreateBody(m_BodyType1.c_str(),
							pos[0],
							pos[1],
							pos[2],
							dim1[0],
							dim1[1],
							dim1[2],
							1);
				pb2 = CreateBody(m_BodyType2.c_str(),
							pos[0]+dim1[0]*0.5f+dim2[0]*0.5f,
							pos[1],
							pos[2],
							dim2[0],
							dim2[1],
							dim2[2],
							1);
				palRevoluteLink *prl;
				prl = dynamic_cast<palRevoluteLink *>(PF->CreateObject("palRevoluteLink"));
				if (prl == NULL) {
					printf("Error: Could not create a Revolute link\n");
					return;
				}
				prl->Init(pb1,pb2,pos[0]+dim1[0]*0.5f,pos[1],pos[2],0,0,1);
				if (limits)
					prl->SetLimits(-ufrand()*M_PI,ufrand()*M_PI);
				break;
			case SDLK_5:
				limits=true;
			case SDLK_8:
				if (!limits) {
					pos[0]=2;	pos[1]=3;	pos[2]=0;
					dim1[0]=1;	dim1[1]=1;	dim1[2]=1;
					dim2[0]=1;	dim2[1]=1;	dim2[2]=1;
					CreateBody(m_BodyType1.c_str(),
							pos[0],
							pos[1]-(dim1[1]*1.5),
							pos[2],
							dim1[0],
							dim1[1],
							dim1[2],
							1);
				}
			case SDLK_2:
				pb1 = CreateBody(m_BodyType1.c_str(),
							pos[0],
							pos[1],
							pos[2],
							dim1[0],
							dim1[1],
							dim1[2],
							1);
				pb2 = CreateBody(m_BodyType2.c_str(),
							pos[0]+dim1[0]*0.5f+dim2[0]*0.5f,
							pos[1],
							pos[2],
							dim2[0],
							dim2[1],
							dim2[2],
							1);
				palSphericalLink *psl;
				psl = dynamic_cast<palSphericalLink *>(PF->CreateObject("palSphericalLink"));
				if (psl == NULL) {
					printf("Error: Could not create a spherical link\n");
					return;
				}
				psl->Init(pb1,pb2,pos[0]+dim1[0]*0.5f,pos[1],pos[2]);
				if (limits)
					psl->SetLimits(ufrand()*M_PI,ufrand()*M_PI);
				break;
			
		case SDLK_6:
			limits=true;
		case SDLK_3:					// a prismatic link test
			
				pb1 = CreateBody(m_BodyType1.c_str(),
							pos[0],
							pos[1],
							pos[2],
							dim1[0],
							dim1[1],
							dim1[2],
							1);
				
				palPrismaticLink *ppl;
				ppl = dynamic_cast<palPrismaticLink *>(PF->CreateObject("palPrismaticLink"));
				if (ppl == NULL) {
					printf("Error: Could not create a prismatic link\n");
					return;
				}

				//if (ufrand()>0.5) {
				if (1) {
				pb2 = CreateBody(m_BodyType2.c_str(),
							pos[0]+dim1[0]*0.5f+dim2[0]*0.5f,
							pos[1],
							pos[2],
							dim2[0],
							dim2[1],
							dim2[2],
							1);
				ppl->Init(pb1,pb2,pos[0]+dim1[0]*0.5f,pos[1],pos[2],1,0,0);
				} else {
							pb2 = CreateBody(m_BodyType2.c_str(),
							pos[0],
							pos[1],
							pos[2]+dim1[2]*0.5f+dim2[2]*0.5f,
							dim2[0],
							dim2[1],
							dim2[2],
							1);
				ppl->Init(pb1,pb2,pos[0],pos[1],pos[2]+dim1[2]*0.5f,0,0,1);
				}
				break;
		case SDLK_o:
			nswap_xy = false;
			pos[1]-=5.0f; //readjust drop height
			pos[0]+=5.0f;
		case SDLK_0:{
				dim2[0]=dim1[0];
				pb1 = CreateBody(m_BodyType1.c_str(),
							nswap_xy ? pos[0] : pos[1],
							nswap_xy ? pos[1] : pos[0],
							pos[2],
							nswap_xy ? dim1[0] : dim1[1],
							nswap_xy ? dim1[1] : dim1[0],
							dim1[2],
							1);
				pb1->SetGroup(1);
				
				palSphericalLink *psl;
				for (i=1;i<5;i++) {
					pb2 = CreateBody(m_BodyType2.c_str(),
						nswap_xy ? pos[0]+(dim1[0]*0.5f+dim2[0]*0.5f)*i : pos[1],
						nswap_xy ? pos[1] : pos[0]+(dim1[0]*0.5f+dim2[0]*0.5f)*i,
						pos[2],
						nswap_xy ? dim2[0] : dim2[1],
						nswap_xy ? dim2[1] : dim2[0],
						dim2[2],
						1);
					pb2->SetGroup(1);

					psl = dynamic_cast<palSphericalLink *>(PF->CreateObject("palSphericalLink"));
					if (psl == NULL) {
						printf("Error: Could not create a spherical link\n");
						return;
					}
					psl->Init(pb1,pb2,
						nswap_xy ? pos[0]+dim1[0]*(0.5f+i-1) : pos[1],
						nswap_xy ? pos[1] : pos[0]+dim1[0]*(0.5f+i-1),
						pos[2]);
					//pb1->SetGroup(1);
					pb2->SetGroup(1);
					pb1=pb2;
				}
				PF->GetActivePhysics()->SetGroupCollision(1,1,false);
					}
			break;

		}
		}
	}