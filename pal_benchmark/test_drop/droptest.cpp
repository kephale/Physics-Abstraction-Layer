#include "../../PAL/test_lib/test_lib.h"
#include "../test_classes/pal_test_sdl_render.h"
#include "../test_classes/drop_test.h"

bool g_quit = false;
PALTest *pt = 0;
bool g_graphics = true;

//test specific:
float g_max_time= 999999;
PAL_Drop_Test<> *pct = 0;

class DropRenderer : public PALTestSDLRenderer {
	virtual void SetCamera() {
		
		g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
		palVector3 p;
		pct->ps->GetPosition(p);
		g_eng->SetViewMatrix(distance*cos(angle),2,distance*sin(angle),0,p.y,0,0,1,0);
	}
};

int main(int argc, char *argv[]) {
	PF->LoadPALfromDLL(); 
	pt = pct = new PAL_Drop_Test<>;
	if (argc<2) {
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
		g_max_time=atof(argv[3]);
	}

	pt->SetMaxSimTime(g_max_time);
	//pt->SetStepSize(step_size);

	pt->CreatePhysics();

	DropRenderer r;
	r.Main(pt,0);
	
	if (!g_graphics) {
	std::string result = std::string("drop_") + argv[2] + ".txt";
	FILE *fout = fopen(result.c_str(),"w");
	for (int ds = 0; ds<pct->data.size(); ds++) {
			fprintf(fout,"%f,",pct->data[ds]);
	}
	fclose(fout);
	}

	delete g_eng;

	PF->Cleanup();

	return 0;
};