// DAE2XML written by John W. Ratcliff August 2, 2006
// modified by Adrian Boeing 2007 for PAL support
/*
	Abstract:
		PAL - Physics Abstraction Layer. 
		Stand alone COLLADA loader
		This demonstrates loading a COLLADA file with PAL
	Author: 
		Adrian Boeing
	Revision History:
	Version 0.0.2: 16/07/08 - Updated for PAL SVN
	Version 0.0.1: 24/10/07 - Original public release
	TODO:
		- fix generic links
	notes:
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <math.h>
#include "pal/ConfigStatic.h"
#include "test_lib/test_lib.h"

#include "DAE2XML_ColladaPhysics.h"

bool g_quit=false;
bool g_paused=true;


// This console program demonstrates how to load a COLLADA 1.4.1 physics file and
// load it in PAL
int  main(int argc,char **argv)
{
	bool g_graphics=true;
	palPhysics *pp = 0;

	if ( argc > 2 )
	{

#ifndef PAL_STATIC
	PF->LoadPALfromDLL(); 
#endif
	
	PF->SelectEngine(argv[1]);
	
	pp = PF->CreatePhysics();
	if (!pp) {
		MessageBox(NULL,"Could not start physics!","Error",MB_OK);
		return -1;
	}

	printf("Loading COLLADA file: %s\r\n", argv[2] );
	DAE2XML::ColladaPhysics *cp = DAE2XML::loadColladaPhysics(argv[2]);
	DAE2XML::loadPAL(cp);

    DAE2XML::releaseColladaPhysics(cp);
	}
	else
	{
		printf("DAE2XML written by John W. Ratcliff August 2, 2006\n");
		printf("modified by Adrian Boeing 2007 for PAL support\n");

		printf("Usage: DAE2XML <physics engine> <collada_file_name>\r\n");
		printf("eg: DAE2XML Novodex deer_bound.dae\n");
		printf("eg: DAE2XML Bullet hornbug.dae\n");
		return 0;
	}

	SDL_Event	E;
	if (g_graphics) {
	g_eng = new SDLGLEngine; //create the graphics engine
	g_eng->Init(640,480);	
	}


	palTerrainPlane *pt= PF->CreateTerrainPlane();
	if (pt) {
		pt->Init(0,0,0,50.0f);
	}
	SDLGLPlane *pSDLGLplane = 0;
	if (g_graphics) {
	//make the ground graphical object
	pSDLGLplane = new SDLGLPlane;
	pSDLGLplane->Create(0,0,0,20,20);
	}
			

	bool mouse_down=false;
	float angle = M_PI*0.4f;
	float distance = 7;
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
			g_eng->SetViewMatrix(distance*cos(angle),4,distance*sin(angle),0,0,0,0,1,0);
	
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
}
