#include "graphics.h"
#include <pal/pal.h>
#include <pal/palFactory.h>
#include <iostream>
#include <unistd.h>
#include <pal/palStringable.h>

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

void EventLoop(palPhysics* physics) {
	bool done = false;
	while (!(done = handleInput())) {
		physics->Update(0.1);

		g_eng->Clear();
		for (unsigned int i = 0; i < g_Graphics.size(); i++) {
			g_Graphics[i]->Display();
			palBodyBase* body = g_Graphics[i]->m_pBody;
			if (body) {
				std::cout << i << ":\t" << *(g_Graphics[i]->m_pBody) << std::endl;
			}
		}
		g_eng->Flip();
		std::cout << "*** TICK ***" << std::endl;
		sleep(1);
	}
}

int main(int argc, char* argv[]) {
#ifndef PAL_STATIC
	PF->LoadPhysicsEngines();
#endif

	PF->SelectEngine("Bullet");
	palPhysics *pp = PF->CreatePhysics(); //create the main physics class
	if (pp == NULL) {
		std::cerr << "Failed to create the physics engine. Check to see if you spelt the engine name correctly, or that the engine DLL is in the right location" << std::endl;
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
	g_eng->SetViewMatrix(20.0f, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// make our objects
	palTerrainPlane *pt = PF->CreateTerrainPlane(); //create the ground
	pt->Init(0,0,0,50.0f); //initialize it, set its location to 0,0,0 and minimum size to 50
	BuildGraphics(pt);
	
	palBox *pb = PF->CreateBox(); //create a box
	pb->Init(0,5.0f,0, 1,1,1, 1);
	GraphicsObject* gObject = BuildGraphics(pb);
	gObject->m_pBody = pb;

	EventLoop(pp);

	PF->Cleanup();
}
