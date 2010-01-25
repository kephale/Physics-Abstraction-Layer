#include "../test_classes/pal_test_sdl_render.h"
#define TIMESTACK
#include "../test_classes/stack_test.h"

//test specific:
bool g_graphics = true;
float g_error_sum = 0;
float g_max_time= 999999;
bool g_force_active = false;

PALTest *pt = 0;
PAL_Stack_Test<> *pct = 0;

int num = 15;

class StackRenderer : public PALTestSDLRenderer {
	virtual void SetCamera() {
		g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
		g_eng->SetViewMatrix(distance*cos(angle),num,distance*sin(angle),0,num*0.25,0,0,1,0);
	}
};
	

int main(int argc, char *argv[]) {
	
	if ( argc != 7 )
	{
		printf("Stack Test");
		printf("\nYou did not supply 6 arguments. example: ./test_stack n Bullet 4 100 n 0.001\n");
		printf("\toptions:\n");
		printf("\t1st argument: 'g' = graphics ON. 'n' = graphics OFF.\n");
		printf("\t2nd argument: Name of physics engine to use: ie: Bullet, Newton, ODE, Tokamak, etc\n");
		printf("\t3rd argument: Number\n");
		printf("\t4th argument: Max Time\n");
		printf("\t5th argument: Force active. 'a' = active, 'n' = not active\n");
		printf("exiting...\n");
		exit(0);
	}
	
	PF->LoadPALfromDLL(); 

	pt = pct = new PAL_Stack_Test<>;

	if (argc<2) {
	//win32 specific code:
#ifdef _WIN32
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);
#endif
	//use the dialoge box to select the physics engine
	} else {
		if (argv[1][0]=='g') {
			g_graphics=true;			
			printf("Graphics ON\n");
			printf("No Results. Results only output to textfile in 'n' (no graphics) mode\n");
		}
		else if (argv[1][0]=='n'){
			g_graphics=false;
			printf("Graphics OFF\n");
			std::string result_file = std::string("stack_time_") + argv[2] + "_" + argv[3] + ".txt";
			printf("Results will be output to: %s\n", result_file.c_str() );
		}
		
		PF->SelectEngine(argv[2]);
		num = atoi(argv[3]);
		g_max_time=atof(argv[4]);
		if (argv[5][0]=='a') {
			g_force_active=true;
			printf("Force is ACTIVE\n");
		}
		else {
			g_force_active=false;
			printf("Force is NOT ACTIVE\n");
		}
			
		step_size = atof(argv[6]);
	}
	pct->num = num;
	pct->g_force_active = g_force_active;
	pt->SetMaxSimTime(g_max_time);
	pt->SetStepSize(step_size);

	pt->CreatePhysics();
	
	SDLGLPlane *pSDLGLplane = 0;
	/*
	if (g_graphics) {
	//make the ground graphical object
	pSDLGLplane = new SDLGLPlane;
	pSDLGLplane->Create(0,0,0,20,20);
	}*/

	StackRenderer r;
	r.distance = num * 2;
	r.Main(pt,pSDLGLplane);
	
	if (!g_graphics) {
		std::string result_time = std::string("stack_time_") + argv[2] + "_" + argv[3] + ".txt";
		FILE *fout_time = fopen(result_time.c_str(),"w");
		// BW: fprintf(fout_time,"%f",pct->t.GetElapsedTime());
		fclose(fout_time);
	}

	delete g_eng;

	PF->Cleanup();

	return 0;
};