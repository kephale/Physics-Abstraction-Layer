#include "test_gravity.h"


FACTORY_CLASS_IMPLEMENTATION(Test_Gravity);

void Test_Gravity::Init(int terrain_type) {
	//CreateTerrain(terrain_type);
	palPhysics *pp = PF->GetActivePhysics();
	//remove old physics configu
	pp->Cleanup();
	//init again with Z up
	palPhysicsDesc desc;
	desc.m_vGravity.x = 0;
	desc.m_vGravity.y = 0;
	desc.m_vGravity.z = -9.8;
	pp->Init(desc);
	//create the ground as an orientated plane

	palOrientatedTerrainPlane *pot = dynamic_cast<palOrientatedTerrainPlane *>(PF->CreateObject("palOrientatedTerrainPlane"));
	pot->Init(0,0,-5,0,0,1,10);

	//Manually create the graphics for this plane..
	SDLGLPlane *pSGplane;
	pSGplane = new SDLGLPlane;
	pSGplane->Create(0,0,-5,10,10);
	palMatrix4x4 m;
	mat_identity(&m);
	mat_set_rotation(&m,M_PI*0.5,0,0);
	pSGplane->SetPosition(m._mat);

	GraphicsObject *g;
	g = new GraphicsObject; //create a graphics object 
	g->m_Graphics.push_back(pSGplane);
	g_Graphics.push_back(g);
	//Finished with graphics
}

void Test_Gravity::Update() {
	unsigned int i;
	float time = PF->GetActivePhysics()->GetTime()*0.3;

};	

void Test_Gravity::Input(SDL_Event E) {
	int i,j;
	Float x,y,z;
	palBodyBase* pb= NULL;

	palMatrix4x4 m;
	switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
				case SDLK_1:
					pb = CreateBody("palBox",sfrand()*3,sfrand()*3,sfrand()*2+5,ufrand()+0.1f,ufrand()+0.1f,ufrand()+0.1f,1);
					if (pb == NULL) {
						printf("Error: Could not create a box\n");
					} 
					break;
				case SDLK_2:
					palSphere *ps;
					ps = NULL;
					ps=dynamic_cast<palSphere *>(PF->CreateObject("palSphere"));
					if (ps) {
						ps->Init(sfrand()*3,sfrand()*3,sfrand()*2+5,0.5f*ufrand()+0.05f,1);
						BuildGraphics(ps);
					} else {
						printf("Error: Could not create a sphere\n");
					} 
					pb = ps;
					break;
				case SDLK_3:
					palCapsule *pc;
					pc = NULL;
					pc=dynamic_cast<palCapsule *>(PF->CreateObject("palCapsule"));
					if (pc) {
						float radius=0.5f*ufrand()+0.05f;
						pc->Init(0,0,0,radius,radius+ufrand()+0.1f,1);
						pc->SetPosition(sfrand()*3,sfrand()*3,sfrand()*2+5,1.5,0,0);
						BuildGraphics(pc);
					} else {
						printf("Error: Could not create a cylinder\n");
					} 
					pb = pc;
					break;
			}
			if (pb) {
				bodies.push_back(pb);
			}
			break;
	}
}
