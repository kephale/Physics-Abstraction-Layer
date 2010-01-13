#include "test_lib/test_lib.h"
#include "../test_classes/pal_test_sdl_render.h"
#include "../test_classes/restitution_test.h"

PALTest *pt = 0;
bool g_graphics = true;
//test specific:
float g_max_time= 999999;
PAL_Restitution_Test<> *pct = 0;

int main(int argc, char *argv[]) {
	PF->LoadPALfromDLL(); 
	pt = pct = new PAL_Restitution_Test<>;

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
		g_max_time=atof(argv[3]);
	}

	pt->SetMaxSimTime(g_max_time);
	//pt->SetStepSize(step_size);
	pt->CreatePhysics();
	PALTestSDLRenderer r;
	r.Main(pt);
	
	if (!g_graphics) {
	STRING result = STRING("restitution_path_") + argv[2] + ".txt";
	FILE *fout = fopen(result.c_str(),"w");
	int i;
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
	}

	
/*	if (!g_graphics) {
		
		
	STRING result_time = STRING("stack_time_") + argv[2] + "_" + argv[3] + ".txt";
	FILE *fout_time = fopen(result_time.c_str(),"w");
	fprintf(fout_time,"%f",t.GetElapsedTime());
	fclose(fout_time);
	}
	*/
	

	delete g_eng;

	PF->Cleanup();

	return 0;
};