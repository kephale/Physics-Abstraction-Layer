#include "test_lib/test_lib.h"
#include "../test_classes/pal_test_SDL_render.h"
#include "../test_classes/restitution_test.h"
#include "../palBenchmark/irrlicht_globals.cpp"

#ifdef _WIN32
#include "../BWHighResolutionTimer/BWTimerWindows.h"
#else
#include "../BWHighResolutionTimer/BWTimerUnix.h"
#endif

PALTest *pt = 0;
bool g_graphics = true;
//test specific:
float g_max_time= 999999;
PAL_Restitution_Test<> *pct = 0;

int main(int argc, char *argv[]) {
	
	if ( argc != 4 )
	{
		printf("Restitution Test");
		printf("\nYou did not supply 3 arguments. example: ./test_restitution g Bullet 10\n");
		printf("\toptions:\n");
		printf("\t1st argument: 'g' = graphics ON. 'n' = graphics OFF.\n");
		printf("\t2nd argument: Name of physics engine to use: ie: Bullet, Newton, ODE, Tokamak, etc\n");
		printf("\t3rd argument: Max Time to run for (infinite in graphics mode)\n");
		printf("exiting...\n");
		exit(0);
	}
	
	PF->LoadPALfromDLL(); 
	pt = pct = new PAL_Restitution_Test<>;

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
		}
		PF->SelectEngine(argv[2]);
		g_max_time=atof(argv[3]);
	}

	BWObjects::HighResolutionTimer *t;
#ifdef _WIN32
	t = new BWObjects::WindowsTimer();
#else
	t = new BWObjects::UnixTimer();
#endif
	
	t->Start();
		pt->SetMaxSimTime(g_max_time);
		//pt->SetStepSize(step_size);
		pt->CreatePhysics();
		PALTestSDLRenderer r;
		r.Main(pt);
	t->Stop();
	
	double timerResult = t->GetElapsedTimeInMilliseconds();
	
	if (!g_graphics) {
		std::string result = std::string("restitution_path_") + argv[2] + ".txt";
		FILE *fout = fopen(result.c_str(),"w");
		unsigned int i;
		for (i=0;i<pct->pos_01.size();i++) {
			fprintf(fout,"%f,",pct->pos_01[i]);
		}
		fprintf(fout,"\n");
		for (i=0;i<pct->pos_05.size();i++) {
			fprintf(fout,"%f,",pct->pos_05[i]);
		}
		fprintf(fout,"\n");
		for (i=0;i<pct->pos_10.size();i++) {
			fprintf(fout,"%f,",pct->pos_10[i]);
		}
		fprintf(fout,"\n");
		fclose(fout);
		printf("Restitution path results output to: \t%s\n", result.c_str());
	}

	if (!g_graphics) {
		std::string result_time = std::string("restitution_stack_time_") + argv[2] + "_" + argv[3] + ".txt";
		FILE *fout_time = fopen(result_time.c_str(),"w");
		fprintf(fout_time,"Result in Milliseconds:\n");
		fprintf(fout_time,"%f\n", timerResult );
		fclose(fout_time);
		printf("Stack Time Results output to: \t\t%s\n", result_time.c_str());
	}
	
	delete g_eng;

	PF->Cleanup();
	
	printf("test_restitution finished\n");

	return 0;
};
