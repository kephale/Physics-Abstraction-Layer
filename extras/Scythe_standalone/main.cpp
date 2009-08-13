/*
Copyright (c) 2007, Pal Ruud, Adrian Boeing

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the PAL nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <math.h>
#include <vector>

#include "ScythePhysics.h"

#include "pal/ConfigStatic.h"
#include "test_lib/test_lib.h"
#include "example/graphics.h"

bool g_quit=false;
bool g_paused=true;


int  main(int argc,char **argv)
{
	bool g_graphics=true;
	palPhysics *pp = 0;
	palBody * pBody = 0;

	// vector for pal bodies
	std::vector<palBody *> objectVector;

	GraphicsObject * graphicsObject = 0;


	float max_y=-1;
	float amax_x=0;

	if ( argc > 2 ) {

#ifndef PAL_STATIC
		PF->LoadPALfromDLL();
#endif

		PF->SelectEngine(argv[1]);

		pp = PF->CreatePhysics();
		if (!pp) {
#ifdef WIN32
			MessageBox(NULL,"Could not start physics! Is \"libpal_<physicsengine>.dll\" missing?","Error",MB_OK);
#else
			printf("Could not start physics engine %s!\n",argv[1]);
#endif
			return -1;
		}

      palPhysicsDesc desc;
		// setting gravity
		pp->Init(desc);

		printf("Loading Scythe file: %s...\r\n", argv[2] );


		std::vector<palBodyBase *> object = ScythePhysics::loadScythePhysics(argv[2]);

		// draw graphics
		for (size_t i = 0; i < object.size(); i++) {
			graphicsObject = BuildGraphics(object[i]);
			//find a maximum for auto-distance
			palVector3 pos;
			object[i]->GetPosition(pos);
			if (pos.y>max_y)
				max_y = pos.y;
			if (fabsf(pos.x)>amax_x)
				amax_x = fabsf(pos.x);
		}


	}
	else
	{
		printf("\nScythe Loader for PAL\n");
		printf("*********************\n");

		printf("Usage: pal_scythe <physics engine> <scythe_file_name>\r\n");
		printf("E.g: pal_scythe Bullet person.phs\n");
		return 0;
	}

	SDL_Event	E;
	if (g_graphics) {
	g_eng = new SDLGLEngine; //create the graphics engine
	g_eng->Init(640,480);
	}

	// creating a pal plane as ground
	palTerrainPlane *pt= PF->CreateTerrainPlane();

	if (pt) {
		pt->Init(0,-0.05,0,50.0f);

	}
	SDLGLPlane *pSDLGLplane = 0;
	if (g_graphics) {
	//make the ground graphical object
	pSDLGLplane = new SDLGLPlane;
	pSDLGLplane->Create(0,0,0,20,20);
	}


	bool mouse_down=false;
	float angle = (float)M_PI*0.4;
	float distance;
	distance = (max_y>amax_x) ? max_y*1.5f : amax_x*1.5f;
	if (argc == 4)
		distance = (float)atof(argv[3]);
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
					if (E.key.keysym.sym == SDLK_SPACE) {
						g_paused=!g_paused;
					}
					if (E.key.keysym.sym == SDLK_KP_PLUS) {
						distance -= 0.5f;
					}
					if (E.key.keysym.sym == SDLK_KP_MINUS) {
						distance += 0.5f;
					}
					break;
				}
		} else {

			if (!g_paused)
			//update physics

			if (pp)

				pp->Update(step_size);


			//clear the screen, setup the camera
			g_eng->Clear();
			g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
			g_eng->SetViewMatrix(distance*cos(angle),distance*0.8f,distance*sin(angle),0,distance*0.4f,0,0,1,0);

			//display all our graphics objects
			for (unsigned int i=0;i<g_Graphics.size();i++) {
				g_Graphics[i]->Display();
			}

			//draw the ground
			pSDLGLplane->Render();


			//flip the screen
			g_eng->Flip();

		}
	} //end while
	return 0;
}
