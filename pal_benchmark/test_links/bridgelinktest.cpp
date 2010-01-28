#include "test_lib/test_lib.h"
#include "../test_classes/pal_test_SDL_render.h"
#define TIMEBRIDGE
#include "../test_classes/bridge_test.h"

#ifdef WIN32
#include "../BWHighResolutionTimer/BWTimerWindows.h"
#else
#include "../BWHighResolutionTimer/BWTimerUnix.h"
#endif

PALTest *pt = 0;
bool g_graphics = true;

//test specific:
float g_max_time= 999999;
int num = 10;
PAL_Bridge_Test<> *pct = 0;

class BridgeRenderer : public PALTestSDLRenderer {
	virtual void SetCamera() {
		
		g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
		g_eng->SetViewMatrix(distance*cos(angle),1.5*num,distance*sin(angle),0,0.5*num,0,0,1,0);
	}
};

int main(int argc, char *argv[]) {
	
	if ( argc != 5 )
	{
		printf("Linking Test");
		printf("\nYou did not supply 4 arguments. example: ./test_links g Bullet 6 10\n");
		printf("\toptions:\n");
		printf("\t1st argument: 'g' = graphics ON. 'n' = graphics OFF.\n");
		printf("\t2nd argument: Name of physics engine to use: ie: Bullet, Newton, ODE, Tokamak, etc\n");
		printf("\t3rd argument: Number )\n");
		printf("\t4th argument: Max Time\n");
		printf("exiting...\n");
		exit(0);
	}
	
	PF->LoadPALfromDLL(); 
	pt = pct = new PAL_Bridge_Test<>;

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
		else if ( argv[1][0]=='n' ){
			g_graphics=false;
			printf("Graphics OFF\n");
		}
			
		PF->SelectEngine(argv[2]);
		num = atoi(argv[3]);
		g_max_time=atof(argv[4]);
	}

	pt->SetMaxSimTime(g_max_time);
	//pt->SetStepSize(step_size);

	pct->num = num;
	
	BWObjects::HighResolutionTimer *t;
#ifdef WIN32
	t = new BWObjects::WindowsTimer();
#else
	t = new BWObjects::UnixTimer();
#endif
	t->Start();
		pt->CreatePhysics();	// Where everything gets rendered and physics calculated
		BridgeRenderer r;
		r.Main(pt,0,num*2);
	t->Stop();
	
	double timerResult = t->GetElapsedTimeInMilliseconds();
	
	if (!g_graphics) {
		std::string result = std::string("links_") + argv[2] + "_" + argv[3] + ".txt";
		FILE *fout = fopen(result.c_str(),"w");
		fprintf(fout,"%f\n",pct->g_error_sum);
		fclose(fout);
		printf("Error Results printed to: %s\n", result.c_str());
		
		std::string result_time = std::string("links_time_") + argv[2] + "_" + argv[3] + ".txt";
		FILE *resultsFile = fopen(result_time.c_str(),"w");
		fprintf(resultsFile,"Result in Milliseconds:\n");
		fprintf(resultsFile,"%f\n", timerResult );
		fclose(resultsFile);
		printf("Timing Results printed to: %s\n", result_time.c_str());
	}

	delete g_eng;

	PF->Cleanup();

	printf("test_links finished\n");
	
	return 0;
};
