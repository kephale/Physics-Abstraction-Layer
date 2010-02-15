#include "../palBenchmark/main.h"
#include "test_lib/test_lib.h"
#include "../test_classes/pal_test_SDL_render.h"
#include "../test_classes/drop_test.h"
#include "../palBenchmark/paltest.h"

#ifdef WIN32
#include "../BWHighResolutionTimer/BWTimerWindows.h"
#else
#include "../BWHighResolutionTimer/BWTimerUnix.h"
#endif

bool g_quit = false;
PALTest *pt = 0;
bool g_graphics = true;

//test specific:
float g_max_time= 999999;
PAL_Drop_Test<PALTestScene> *pct = 0;

class DropRenderer : public PALTestSDLRenderer {
	virtual void SetCamera() {
		
		g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
		palVector3 p;
		pct->ps->GetPosition(p);
		g_eng->SetViewMatrix(distance*cos(angle),2,distance*sin(angle),0,p.y,0,0,1,0);
	}
};

int main(int argc, char *argv[]) {
	
	if ( argc != 4 )
	{
		printf("\nYou did not supply 3 arguments. example: ./test_drop g Bullet 5\n");
		printf("\toptions:\n");
		printf("\t1st argument: 'g' = graphics ON. 'n' = graphics OFF.\n");
		printf("\t2nd argument: Name of physics engine to use: ie: Bullet, Newton, ODE, Tokamak, etc\n");
		printf("\t3rd argument: Max time to run for (applies only when Graphics are OFF )\n");
		printf("exiting...\n");
		exit(0);
	}
	
	PF->LoadPALfromDLL(); 
	pt = pct = new PAL_Drop_Test<PALTestScene>;
	
	if (argc<2) {
#ifdef _WIN32
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);
#endif
	//use the dialoge box to select the physics engine
	} else {
		if (argv[1][0]=='g') {
			g_graphics=true;
#ifndef  _WIN32	// Mac & Linux
            g_device = createDevice(video::EDT_OPENGL, dimension2d<s32>(640, 480),
                                    16, false, false, false, 0);
#else						// Windows
            g_device = createDevice(video::EDT_OPENGL,
                                    (const core::dimension2d<u32>&)dimension2d<s32>(640, 480), 16,
                                    false, false, false, 0);
#endif
            
            g_driver = g_device->getVideoDriver();
            g_smgr = g_device->getSceneManager();
            g_gui = g_device->getGUIEnvironment();
			printf("Graphics ON\n");
			printf("No Results. Results only output to textfile in 'n' (no graphics) mode\n");
		} else if (argv[1][0] == 'n')  {
			printf("Graphics OFF\n");
			printf("Results will be output to: drop_%s.txt\n", argv[2]);
			g_graphics=false;
		}
		PF->SelectEngine(argv[2]);
        g_engines.push_back(argv[2]);
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
		for (unsigned int ds = 0; ds < pct->data.size(); ds++) {
				fprintf(fout,"%f,",pct->data[ds]);
		}
		fclose(fout);
	}

	delete g_eng;
	PF->Cleanup();

	printf("test_drop finished\n");
	
	return 0;
};
