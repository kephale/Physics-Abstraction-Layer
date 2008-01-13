#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info

#include "../pal/palFactory.h"
#include "graphics.h"

//#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC80.CRT' version='8.0.50608.0' processorArchitecture='x86' publicKeyToken='1fc8b3b9a1e18e3b'\"")


bool g_quit = false;
//SDLGLEngine *g_eng;

float frand() {
	return rand()/(float)RAND_MAX;
}

#include "resource.h"

BOOL MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	DWORD ret;
	
	switch (uMsg)
	{
	case WM_CREATE:
		return TRUE;
		break;
	case WM_INITDIALOG:
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Bullet");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Dynamechs [disabled]");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Jiggle");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Meqon [deprecated]");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Newton");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Novodex");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"ODE");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Open Tissue [disabled]");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Tokamak");
#ifdef MICROSOFT_VC_6
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"True Axis [disabled]");
#else
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"True Axis");
#endif
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_SETCURSEL,0,0);
		return TRUE;
		break;
	case WM_COMMAND:	
		switch (LOWORD(wParam)) {
		case IDOK:
			ret=SendDlgItemMessage(hWnd,IDC_LIST1,LB_GETCURSEL,0,0);
			switch (ret) {
			case 0:
				PF->SelectEngine("Bullet");
				break;
			case 1:
				PF->SelectEngine("Dynamechs");
				break;
			case 2:
				PF->SelectEngine("Jiggle");
				break;
			case 3:
				PF->SelectEngine("Meqon");
				break;
			case 4:
				PF->SelectEngine("Newton");
				break;
			case 5:
				PF->SelectEngine("Novodex");
				break;
			case 6:
				PF->SelectEngine("ODE");
				break;
			case 7:
				PF->SelectEngine("OpenTissue");
				break;
			case 8:
				PF->SelectEngine("Tokamak");
				break;
			case 9:
				PF->SelectEngine("TrueAxis");
				break;
			}
			EndDialog(hWnd, 0);	
			break;	
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);	
		exit(0);
		return TRUE;
		break;
	}
	return FALSE;
}



int main(int argc, char *argv[]) {

	//win32 specific code:
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);
	//use the dialoge box to select the physics engine

	SDL_Event	E;
	g_eng = new SDLGLEngine; //create the graphics engine
	g_eng->Init(640,480);	

	palPhysics *pp = 0;
	
	pp = PF->CreatePhysics();
	if (!pp) {
		MessageBox(NULL,"Could not start physics!","Error",MB_OK);
		return -1;
	}
	//initialize gravity
	pp->Init(0,-9.8f,0);

	//initialize materials
	palMaterials *pm = PF->CreateMaterials();
	if (pm)
		pm->NewMaterial("wood",0.2,0.1,0.1); 

	//initialize the ground
	palTerrainPlane *pt= PF->CreateTerrainPlane();
	if (pt) {
		pt->Init(0,0,0,20.0f);
		//set the material
		if (pm)
			pt->SetMaterial(pm->GetMaterial("wood"));
	}

	//make the ground graphical object
	SDLGLPlane *pSDLGLplane = new SDLGLPlane;
	pSDLGLplane->Create(0,0,0,20,20);

#define HEIGHT 4
	//make the pyramid
	for (int y=0;y<HEIGHT;y++)	{
		for (int x=0;x<y;x++) {
			palBox *pb;
			pb = PF->CreateBox();
			pb->Init(x,((HEIGHT-1)-y)+0.5f,0,1,1,1,3);
			pb->SetMaterial(pm->GetMaterial("wood"));
			BuildGraphics(pb);		
			if (x) {
				pb = PF->CreateBox();
				pb->Init(-x,((HEIGHT-1)-y)+0.5f,0,1,1,1,3);
				pb->SetMaterial(pm->GetMaterial("wood"));
				BuildGraphics(pb);		
			}
		}
	}
	
	//make the controllable sphere
	palSphere *control_sphere = PF->CreateSphere();
	if (control_sphere ) {
		control_sphere->Init(0,1,3,0.4f,6);
		if (pm)
			control_sphere->SetMaterial(pm->GetMaterial("wood"));
		BuildGraphics(control_sphere);		
	}


	bool mouse_down=false;
	float angle = M_PI*0.4f;
	float distance = 12;
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
						
					}
					if (E.key.keysym.sym == SDLK_KP_PLUS) {
						distance -= 0.5f;
					}
					if (E.key.keysym.sym == SDLK_KP_MINUS) {
						distance += 0.5f;
					}
					//make a new shape?
					if (E.key.keysym.sym == SDLK_z) {
						palBox *pb = PF->CreateBox();
						if (pb) {
							pb->Init(2*frand()-1,7,2*frand()-1,frand()+0.2f,frand()+0.2f,frand()+0.2f,2+frand()*15);
							pb->SetMaterial(pm->GetMaterial("wood"));
							BuildGraphics(pb);
						}
					}
					if (E.key.keysym.sym == SDLK_x) {
						palSphere *pb = PF->CreateSphere();
						if (pb) {
							pb->Init(2*frand()-1,7,2*frand()-1,0.5*frand()+0.2f,2+frand()*15);
							pb->SetMaterial(pm->GetMaterial("wood"));
							BuildGraphics(pb);
						}
					}

					if (E.key.keysym.sym == SDLK_c) {
						palCapsule *pb = PF->CreateCapsule();
						if (pb) {
							pb->Init(2*frand()-1,7,2*frand()-1,0.5*frand()+0.2f,frand()+0.2f,2+frand()*15);
							pb->SetMaterial(pm->GetMaterial("wood"));
							BuildGraphics(pb);
						}
					}
					//push the sphere?
					if (control_sphere)
					switch (E.key.keysym.sym) {
					case SDLK_w:
						control_sphere->ApplyImpulse(0,0,-5);
						break;
					case SDLK_s:
						control_sphere->ApplyImpulse(0,0, 5);
						break;
					case SDLK_a:
						control_sphere->ApplyImpulse(-5,0,0);
						break;
					case SDLK_d:
						control_sphere->ApplyImpulse( 5,0,0);
						break;
					}
					
					break;
				}
		} else {

			//update physics
			if (pp)
				pp->Update(0.01f);

		
			//clear the screen, setup the camera
			g_eng->Clear();
			g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
			g_eng->SetViewMatrix(distance*cos(angle),9,distance*sin(angle),0,0,0,0,1,0);
	
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

	delete g_eng;

	PF->Cleanup();

	return 0;
};