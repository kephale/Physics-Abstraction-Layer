#include "graphics.h"
#include <pal/pal.h>
#include <pal/palFactory.h>
#include <iostream>
#include <unistd.h>
#include <pal/palStringable.h>
#include <pal/palMath.h>

palBox* boxA;
palBox* boxB;
using namespace std;

bool handleInput() {
	SDL_Event event;
	bool done = false;
	while (SDL_PollEvent(&event) && !done) {
		switch (event.type) {
		case SDL_QUIT:
			done = true;
			break;
		}
	}
	return done;
}

static int degrees(float a, float b) {
    return int(atan2f(a, b) / M_PI * 180.0f);
}

void EventLoop(palPhysics* physics) {
	bool done = false;
	while (!(done = handleInput())) {
		physics->Update(0.1);

		palVector3 aPos, bPos;
		boxA->GetPosition(aPos);
        aPos.y -= 0.5;
		boxB->GetPosition(bPos);
        bPos.y -= 0.5;
		cout << "A: " << aPos << " r=" << vec_mag(&aPos) << " a=" << degrees(aPos.x, aPos.z) << "\t"
			 << "B: " << bPos << " r=" << vec_mag(&bPos) << " a=" << degrees(bPos.x, bPos.z) << endl;
		g_eng->Clear();
		for (unsigned int i = 0; i < g_Graphics.size(); i++) {
			g_Graphics[i]->Display();
			palBodyBase* body = g_Graphics[i]->m_pBody;
			/*
			if (body) {
				std::cout << i << ":\t" << *(g_Graphics[i]->m_pBody)
								<< std::endl;
			}
			*/
		}
		g_eng->Flip();
		usleep(100000);
	}
}

int main(int argc, char* argv[]) {
#ifndef PAL_STATIC
	PF->LoadPhysicsEngines();
#endif

	PF->SelectEngine("Bullet");
	palPhysics *pp = PF->CreatePhysics(); //create the main physics class
	if (pp == NULL) {
		std::cerr
						<< "Failed to create the physics engine. Check to see if you spelt the engine name correctly, or that the engine DLL is in the right location"
						<< std::endl;
		return 1;
	}

	palPhysicsDesc desc;
	pp->Init(desc);

	// init graphics system
	g_eng = new SDLGLEngine();
	if (!g_eng->Init(640, 480)) {
		std::cerr << "Failed to create rendering engine" << std::endl;
		return 2;
	}
	g_eng->SetViewMatrix(20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
					0.0f);

	// make our objects
	palTerrainPlane *pt = PF->CreateTerrainPlane(); //create the ground
	pt->Init(0, 0, 0, 50.0f); //initialize it, set its location to 0,0,0 and minimum size to 50
	palMaterials* materials = PF->CreateMaterials();
	palMaterialDesc iceDesc;
	iceDesc.m_fKinetic = 0;
	iceDesc.m_fStatic = 0;
	palMaterial* ice = materials->NewMaterial("ice", iceDesc);
	pt->SetMaterial(ice);
	BuildGraphics(pt);

	float altitude = 0.5f;
	
	boxA = PF->CreateBox(); //create a box
	boxA->Init(0, altitude, 6, 1, 1, 10, 1);
	GraphicsObject* gObjectA = BuildGraphics(boxA);

	boxB = PF->CreateBox(); //create a box
	boxB->Init(6, altitude, 0, 10, 1, 1, 1);
	GraphicsObject* gObjectB = BuildGraphics(boxB);

	palRevoluteLink* link = PF->CreateRevoluteLink(boxA, boxB, 0, altitude, 0, 0, 1, 0);
	GraphicsObject* gObjectLink = BuildGraphics(link);

	link->SetLimits(-M_PI, M_PI);
	palAngularMotor* motor = PF->CreateAngularMotor(link, 1000);
	motor->Update(2);

	EventLoop(pp);

	PF->Cleanup();
}
