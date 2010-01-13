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
	
	PF->LoadPALfromDLL(); 

	pt = pct = new PAL_Stack_Test<>;

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
		num = atoi(argv[3]);
		g_max_time=atof(argv[4]);
		if (argv[5][0]=='a')
			g_force_active=true;
		else
			g_force_active=false;
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
	STRING result_time = STRING("stack_time_") + argv[2] + "_" + argv[3] + ".txt";
	FILE *fout_time = fopen(result_time.c_str(),"w");
	fprintf(fout_time,"%f",pct->t.GetElapsedTime());
	fclose(fout_time);
	}

	delete g_eng;

	PF->Cleanup();

	return 0;
};