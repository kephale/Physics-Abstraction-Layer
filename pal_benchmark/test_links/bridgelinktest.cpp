#include "test_lib/test_lib.h"
#include "../test_classes/pal_test_sdl_render.h"
#define TIMEBRIDGE
#include "../test_classes/bridge_test.h"

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
		if (argv[1][0]=='g')
			g_graphics=true;
		else
			g_graphics=false;
		PF->SelectEngine(argv[2]);
		num = atoi(argv[3]);
		g_max_time=atof(argv[4]);
	}
	

	pt->SetMaxSimTime(g_max_time);
	//pt->SetStepSize(step_size);

	pct->num = num;

	pt->CreatePhysics();
	BridgeRenderer r;
	r.Main(pt,0,num*2);

	if (!g_graphics) {
	std::string result = std::string("links_") + argv[2] + "_" + argv[3] + ".txt";
	FILE *fout = fopen(result.c_str(),"w");
	fprintf(fout,"%f",pct->g_error_sum);
	fclose(fout);

	std::string result_time = std::string("links_time_") + argv[2] + "_" + argv[3] + ".txt";
	FILE *fout_time = fopen(result_time.c_str(),"w");
	// BW: fprintf(fout_time,"%f",pct->t.GetElapsedTime());
	fclose(fout_time);
	}

	delete g_eng;

	PF->Cleanup();

	return 0;
};