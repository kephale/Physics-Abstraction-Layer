#ifndef TEST_2
#define TEST_2

#include "test.h"

//drop an object test
class Test_2 : public Test {
public:
	Test_2() {
	}
	
	void Init(int terrain_type) {
		CreateTerrain(terrain_type);
		m_BodyType1="palBox";
		m_BodyType2="palBox";
	}

	std::string GetName() {
		return "Link Test";
	}
	std::string GetDescription() {
		return "This tests simple link creation and enabeling link limits. Press keys 1, 2, and 3 for a Revolute, Spherical, or Prismatic link. Press keys 4, 5 and 6 to create these links with movement limits enabled.\r\na,s,d effects the body type created, representing Box, Sphere and Cylinder respectively. z, x, c effects the second body type.";
	}
	void Input(SDL_Event E) {
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



		}
		}
	}
	void Update() {
		;
	};	
	std::string m_BodyType1;
	std::string m_BodyType2;
protected:

	FACTORY_CLASS(Test_2,Test_2,palTests,2);
};

#endif