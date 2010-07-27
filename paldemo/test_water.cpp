#define NOMINMAX 
#include "test_water.h"

#include "../pal/palFluid.h"
#include "../pal/palCollision.h"

FACTORY_CLASS_IMPLEMENTATION(Test_Water);
// Fluid globals


PAL_VECTOR<palSPHFluid *> g_ParticleFluids;
PAL_VECTOR<palDampendShallowFluid *> g_GridFluids;

//propeller actuators for case '1' (Mako)
palPropeller *prop0;
palPropeller *prop1;
palPropeller *prop2;
palPropeller *prop3;


void render_water() {
	for (unsigned int f=0;f<g_GridFluids.size();f++) {
		g_GridFluids[f]->Update();
	glColor3f(1,1,1);
	glBegin(GL_POINTS);
	palVector3 *pp = g_GridFluids[f]->GetFluidVertices();
		for (int i=0; i<g_GridFluids[f]->GetNumVertices(); i++)
		{
			glVertex3f(	pp[i].x,
				pp[i].y,
				pp[i].z);
		}
		glEnd();
	/*
		palVector3 *v = g_GridFluids[f]->GetFluidVertices();
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, v);
		glDrawArrays(GL_POINTS,0,g_GridFluids[f]->GetNumVertices());
		glDisableClientState(GL_VERTEX_ARRAY);
	*/
	
	}
}

void Test_Water::AdditionalRender() {

	
//	update_water(0.8,0.01);
	render_water();

	if (g_ParticleFluids.size()==0) return;
	
	glColor3f(1,1,1);
	glBegin(GL_POINTS);
	
	for (unsigned int j=0; j<g_ParticleFluids.size();j++) {
		palVector3 *pp = g_ParticleFluids[j]->GetParticlePositions();
		for (int i=0; i<g_ParticleFluids[j]->GetNumParticles(); i++)
		{
			glVertex3f(	pp[i].x,
				pp[i].y,
				pp[i].z);
		}
	}
	glEnd();

}

void Test_Water::Init(int terrain_type) {
		CreateTerrain(terrain_type);
}

void Test_Water::Update() 
 {
	
	 for (unsigned int i=0;i<act.size();i++)
		 act[i]->Apply();
	 for (unsigned int i=0;i<bodies.size();i++)
		 bodies[i]->SetActive(true);

	};	


void Test_Water::Input(SDL_Event E) {
		int i,j,k;
		palBodyBase *pb= NULL;
		switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
			case SDLK_p:
				//Sleep(2000);
			break;
			case SDLK_i:
				{
					if (prop0) {
					prop2->SetVoltage(500);
					prop3->SetVoltage(500);
					}
					//front = 2
					//back == 3
				}
				break;
			case SDLK_j:
				{
					if (prop0) 
					prop0->SetVoltage(500);
				}
				break;
			case SDLK_l:
				{
					if (prop1) 
					prop1->SetVoltage(500);
				}
				break;
			case SDLK_k:
				{
					if (prop0) {
					prop0->SetVoltage(0);
					prop1->SetVoltage(0);
					prop2->SetVoltage(0);
					prop3->SetVoltage(0);
					}
				}
				break;
			case SDLK_w:
				{
					int size = 5;
					float d = 0.1f;
					palSPHFluid *pf = dynamic_cast<palSPHFluid * >(PF->CreateObject("palSPHFluid"));
					if (!pf) return;
					pf->Init();
					for (k=0;k<size;k++) {
						for (j=0;j<size*16;j++) {
							for (i=0;i<size;i++) {
								pf->AddParticle(i*d-(size*d)*0.5f,j*d+4,k*d-(size*d)*0.5f,0,0,0);
					}
					}
					}
					pf->Finalize();
					g_ParticleFluids.push_back(pf);
				}
				break;
			case SDLK_h:
				{
					palDampendShallowFluid *pf = new palDampendShallowFluid;//dynamic_cast<palDampendShallowFluid * >(PF->CreateObject("palDampendShallowFluid"));
					if (!pf) return;
					pf->Init(128,128,0.06,1000,0.01,0.5,0.07);
					g_GridFluids.push_back(pf);
				}
				break;
			case SDLK_1:
				{
				//pb = CreateBody("palBox",sfrand()*3,sfrand()*0.5f+0.1f,sfrand()*3,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
				pb = CreateBody("palBox",0,0,0,1.8,0.5,0.5,420);

				palFakeBuoyancy *pfb = new palFakeBuoyancy;//dynamic_cast<palFakeBuoyancy *>( PF->CreateObject("palFakeBuoyancy") );
				pfb->Init(dynamic_cast<palBody*>(pb),998.29);
				act.push_back(pfb);

				palLiquidDrag *pld = new palLiquidDrag;
				pld->Init(dynamic_cast<palBody*>(pb),0.25,0.4,998.29);
				act.push_back(pld);

				prop0 = new palPropeller;
				prop0->Init(dynamic_cast<palBody*>(pb),0,0,0.25, 1,0,0, 0.05);
				act.push_back(prop0);

				prop1 = new palPropeller;
				prop1->Init(dynamic_cast<palBody*>(pb),0,0,-0.25, 1,0,0, 0.05);
				act.push_back(prop1);

				prop2 = new palPropeller;
				prop2->Init(dynamic_cast<palBody*>(pb),0.75,0,0, 0,-1,0, 0.05);
				act.push_back(prop2);

				prop3 = new palPropeller;
				prop3->Init(dynamic_cast<palBody*>(pb),-0.75,0,0, 0,-1,0, 0.05);
				act.push_back(prop3);

				if (pb == NULL) {
					printf("Error: Could not create a box\n");
				} 
				break;
				}
			case SDLK_2:
				palSphere *ps;
				ps = NULL;
				ps=dynamic_cast<palSphere *>(PF->CreateObject("palSphere"));
				if (ps) {
					float r = 0.5f*ufrand()+0.05f;
					float v = r*r*r*4/3.0f*3.14;
					ps->Init(sfrand()*1,sfrand()*2+5.0f,sfrand()*1,r,v*ufrand()*1.5);
					BuildGraphics(ps);
				} else {
					printf("Error: Could not create a sphere\n");
				} 
				pb = ps;
				break;
			/*case SDLK_3:
				{
				pb = CreateBody("palBox",sfrand()*3,sfrand()*0.5f+0.1f,sfrand()*3,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
				}
				break;*/
			case SDLK_3:
				{
				pb = CreateBody("palBox",-2,2,0,1,1,1, 200); //very buoyant
				pb = CreateBody("palBox", 0,0,0,1,1,2,2*900); //buoyant v= 1x1x1, m = pVg, p = 1000
				pb = CreateBody("palBox", 2,2,0,1,1,1,4000); //sinks!
				}
				break;
			case SDLK_5:
				{
					float mult;
					mult = 0.5f;
					float r = mult*0.25;
					float v = r*r*r*(4/3.0f)*3.14;
					pb = CreateBody("palSphere",sfrand()*0.01f,4.0f,0,r,mult,mult,v*1000);
				}
				break;
				case SDLK_6:
				{
					float w=ufrand()*0.25+0.1;
					float h=ufrand()*0.25+0.1;
					float d=ufrand()*0.25+0.1;
					float v= w*h*d;
					pb = CreateBody("palBox",sfrand()*0.01f,4.0f,0,w,h,d,v*1000);
				}
				break;
			case SDLK_4:
				{
				int dist;
				dist = 1;
				float mult;
				mult = 0.7f;
				float ymult=1.5f;
				for (j=-dist;j<=dist;j++)
					for (i=-dist;i<=dist;i++) {
						if ((i==0) && (j==0))
							;//pb = CreateBody("palSphere",0,5.0f,0,mult*0.25f,mult,mult,1000.0f);
						else
							pb = CreateBody("palStaticBox",i*mult,ymult*0.5f,j*mult,mult,ymult,mult,0,true);
					}
				
				}
				pb = 0;
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
			}
			break;
		}
	}
