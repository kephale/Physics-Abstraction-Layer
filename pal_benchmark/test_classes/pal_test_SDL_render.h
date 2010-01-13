#include "test_lib/test_lib.h"
#include "pal_test.h"

extern bool g_graphics;

class PALTestSDLRenderer {
public:
	PALTestSDLRenderer() {
		g_quit = false;
		angle = M_PI*0.4f;
		distance = 12;
	}
	bool g_quit;
	SDLGLObject *pSDLGLplane;

	float angle;
	float distance;
	virtual void SetCamera() {
		g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
		g_eng->SetViewMatrix(distance*cos(angle),2,distance*sin(angle),0,-1,0,0,1,0);
	}

	void Main(PALTest *pt, SDLGLObject *terrain = 0, float starting_distance = 12 ) {
		distance = starting_distance;


		SDL_Event	E;
		if (g_graphics) {
			g_eng = new SDLGLEngine; //create the graphics engine
			g_eng->Init(640,480);	
		}

		pSDLGLplane = terrain;
		bool mouse_down=false;
		
		if (g_graphics)
			while (!g_quit) {
				if(SDL_PollEvent(&E)) {
					switch(E.type) {
				case SDL_QUIT:
					g_quit=true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					mouse_down=true;
					break;
				case SDL_MOUSEBUTTONUP:
					mouse_down=false;
					break;
				case SDL_MOUSEMOTION:
					if (mouse_down)
						angle+=E.motion.xrel*0.01f;
					break;
				case SDL_KEYDOWN:
					if (E.key.keysym.sym == SDLK_KP_PLUS) {
						distance -= 0.5f;
					}
					if (E.key.keysym.sym == SDLK_KP_MINUS) {
						distance += 0.5f;
					}
					break;
					}
				} else {

					pt->Update();


					//clear the screen, setup the camera
					g_eng->Clear();
					SetCamera();
					//display all our graphics objects
					for (unsigned int i=0;i<g_Graphics.size();i++) {
						g_Graphics[i]->Display();
					}

					//draw the ground
					if (pSDLGLplane)
						pSDLGLplane->Render();

					//flip the screen
					g_eng->Flip();
				}

			} //end while
		else //if graphics
			while (!pt->Done()) {
				pt->Update();
			}
	}
};