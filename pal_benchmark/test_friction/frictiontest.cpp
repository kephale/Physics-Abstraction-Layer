//#include "../pal_i/jiggle_pal.h"
#include "test_lib/test_lib.h"
//#include "../example/debugconsole.h"


float frand() {
	return rand()/(float)RAND_MAX;
}

void printmatrix(float *mat) {
	for (int i=0;i<16;i++) {
		printf("%f ",mat[i]);
		if (i%4 == 3) 
			printf("\n");
	}
}

bool g_quit = false;

//test specific:
bool g_graphics = true;
float g_runningaverage = 0;
float g_runningaverage_accel = 0;
int g_nrun=0;
float g_start_time= 0.5f;
float g_max_time= 999999;
bool g_force_active = false;
float theta = 0;
float g_mat_stat = 0.2f;
float g_mat_kin = 0.1f;

int main(int argc, char *argv[]) {
#if 1
	PF->LoadPALfromDLL(); 

	if (argc<2) {
	//win32 specific code:
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);
	//use the dialoge box to select the physics engine
	} else {
		if (argv[1][0]=='g')
			g_graphics=true;
		else
			g_graphics=false;
		PF->SelectEngine(argv[2]);
		theta = atof(argv[3]);
		g_start_time = atof(argv[4]);
		g_max_time = atof(argv[5]);
		g_mat_stat = atof(argv[6]);
		g_mat_kin = atof(argv[7]);
	}
#else
	DebugConsole dc;
	theta = 0.4;
//	PF->SelectEngine("ODE");
//	PF->SelectEngine("Novodex");
//	PF->SelectEngine("Jiggle");
	PF->SelectEngine("Bullet");
//	PF->SelectEngine("Newton");
#endif
	
	
	SDL_Event	E;
	if (g_graphics) {
	g_eng = new SDLGLEngine; //create the graphics engine
	g_eng->Init(640,480);	
	}

	palPhysics *pp = 0;
	
	pp = PF->CreatePhysics();
	if (!pp) {
		MessageBox(NULL,"Could not start physics!","Error",MB_OK);
		return -1;
	}


	Float gravityMag = -9.8;
	//initialize gravity
	pp->Init(0,gravityMag,0);

	//initialize materials
	palMaterials *pm = PF->CreateMaterials();
	if (pm) {
		pm->NewMaterial("test",g_mat_stat,g_mat_kin,0.0); 
		//pm->NewMaterial("test",0.5,0.7,0.0); 
	}



	palOrientatedTerrainPlane *pot= dynamic_cast<palOrientatedTerrainPlane *>(PF->CreateObject("palOrientatedTerrainPlane"));
	if (pot) {
		pot->Init(0,0,0, sin(theta),cos(theta),0, 75.0f);
		if (pm)
			pot->SetMaterial(pm->GetMaterial("test"));
	}




	SDLGLPlane *pSDLGLplane = 0;
	if (g_graphics) {
	//make the ground graphical object
	pSDLGLplane = new SDLGLPlane;
	pSDLGLplane->Create(0,0,0,50,50);
	}
	
	VECTOR<palBox *> boxes;
	palBox *pb;

	pb = PF->CreateBox();
	pb->Init(0,5,0, 5.f,1,5.f,10);
	if (pm)
		pb->SetMaterial(pm->GetMaterial("test"));
	BuildGraphics(pb);		

	pb->SetPosition(0,0.5,0,theta,0,0);

	palVector3 vel_last;
	palVector3 pos_last;
	Timer t;

	bool mouse_down=false;
	float angle = M_PI*0.4f;
	float distance = 12;
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

		
			//clear the screen, setup the camera
			g_eng->Clear();
			g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
			g_eng->SetViewMatrix(distance*cos(angle),5,distance*sin(angle),0,0.25,0,0,1,0);
	
			//display all our graphics objects
			for (unsigned int i=0;i<g_Graphics.size();i++) {
				g_Graphics[i]->Display();
			}

			//draw the ground
			if (pot) {
				pSDLGLplane->SetPosition(pot->GetLocationMatrix()._mat);
			} 
			pSDLGLplane->Render();
			
			//flip the screen
			g_eng->Flip();

			pb->SetActive(true);

			//velocity
			palVector3 velocity;
			pb->GetLinearVelocity(velocity);
			
			//acceleration
			palVector3 accel;			
			vec_sub(&accel,&vel_last,&velocity);
			vec_mul(&accel,1/step_size);

			//tangent unit vector			
			palVector3 vt_unit;
			vt_unit.x=cos(theta);
			vt_unit.y=sin(theta);
			vt_unit.z=0;

			float FGN = gravityMag*cos(theta); //normal gravity force
			float FGT = gravityMag*sin(theta); //tangent gravity force

			//acceleration in tangent direction
			float a = vec_dot(&accel,&vt_unit);
//			printf("A  :%f [%f,%f%,%f] [%f]\n",a,accel.x,accel.y,accel.z,pp->GetTime());
		
			float FT = a-FGT; //tangent force
			float ratio = fabsf(FT)/fabsf(FGN);

			//average
			if (pp->GetTime()>g_start_time) {
				g_runningaverage+=ratio;
				g_nrun++;
			}

			//some stats:
			printf("tan(t) :%f\n",tan(theta));
			printf("FT/FN  :%f\n",ratio);
			printf("time   :%f\n",pp->GetTime());
			printf("runavg :%f\n",g_runningaverage/g_nrun);
			printf("===\n");
			//update physics
			if (pp)
				pp->Update(step_size);
			vel_last = velocity;
		}
	} //end while
	else //if no graphics
	while (pp->GetTime()<g_max_time) {

			pb->SetActive(true);

			palVector3 pos;
			pb->GetPosition(pos);

			palVector3 velocity;
#if 1		//how will we get the velocity?
			pb->GetLinearVelocity(velocity);
#else
			vec_sub(&velocity,&pos_last,&pos);
			vec_mul(&velocity,1/step_size);
#endif
			//calculate acceleration
			palVector3 accel;			
			vec_sub(&accel,&vel_last,&velocity);
			vec_mul(&accel,1/step_size);

			//tangent unit vector
			palVector3 vt_unit;
			vt_unit.x=cos(theta);
			vt_unit.y=sin(theta);
			vt_unit.z=0;


			float FGN = gravityMag*cos(theta); //normal gravity force
			float FGT = gravityMag*sin(theta); //tangent gravity force

			//acceleration in direction of unit vector
			float a = vec_dot(&accel,&vt_unit);
			
			float FT = a-FGT;
			float ratio = fabsf(FT)/fabsf(FGN);

			//stats:
//			printf("tan(t) :%f\n",tan(theta));
//			printf("FT/FN  :%f\n",ratio);
//			printf("time   :%f\n",pp->GetTime());

			//average
			if (pp->GetTime()>g_start_time) {
				g_runningaverage+=ratio;
				g_nrun++;
				g_runningaverage_accel += a;
			}

			//update physics
			t.StartSample();
			if (pp)
				pp->Update(step_size);
			t.EndSample();

			//update last velocity & position
			vel_last = velocity;
			pos_last = pos;

	}

	if (!g_graphics) {
		STRING result = STRING("friction_") + argv[2] + "_" + argv[3] + ".txt";
		FILE *fout = fopen(result.c_str(),"w");
		fprintf(fout,"%f",g_runningaverage/g_nrun);
		fflush(fout);
		fclose(fout);

		result = STRING("friction_accel_") + argv[2] + "_" + argv[3] + ".txt";
		fout = fopen(result.c_str(),"w");
		fprintf(fout,"%f",g_runningaverage_accel/g_nrun);
		fflush(fout);
		fclose(fout);
	}


	delete g_eng;

	PF->Cleanup();

//	system("pause");
	return 0;
};