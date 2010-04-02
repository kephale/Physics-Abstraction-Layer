#include "test_lib/test_lib.h"
#include "../test_classes/pal_test_SDL_render.h"
#include "../test_classes/collision_test.h"

PALTest *pt = 0;
bool g_graphics = true;

//test specific:
float g_max_time= 999999;
PAL_Collision_Test<> *pct = 0;

int main(int argc, char *argv[]) {
	
	if ( argc != 5 )
	{
		printf("\nYou did not supply 4 arguments. example: ./test_collision g Bullet 5 0.0001\n");
		printf("\toptions:\n");
		printf("\t1st argument: 'g' = graphics ON. 'n' = graphics OFF.\n");
		printf("\t2nd argument: Name of physics engine to use: ie: Bullet, Newton, ODE, Tokamak, etc\n");
		printf("\t3rd argument: Max time to run for (applies only when Graphics are OFF )\n");
		printf("\t4th argument: Step Size\n");
		printf("exiting...\n");
		exit(0);
	}
	
	PF->LoadPALfromDLL(); 
	pt = pct = new PAL_Collision_Test<>;
#if 1
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
		} else if ( argv[1][0]=='n') {
			g_graphics=false;
			std::string outputFileString = std::string("collision_") + argv[2] + ".txt";
			printf("Graphics OFF\n");
		}
		g_engine = argv[2];
		PF->SelectEngine(argv[2]);
		g_max_time=atof(argv[3]);
		step_size = atof(argv[4]);
	}
#endif
	pt->SetMaxSimTime(g_max_time);
	pt->SetStepSize(step_size);
	PF->SelectEngine(g_engine);
//	PF->SelectEngine("Bullet"); //Y
//	PF->SelectEngine("Jiggle"); //Y (fails)
//	PF->SelectEngine("Newton"); //Y
//	PF->SelectEngine("ODE"); //X
//	PF->SelectEngine("Novodex"); //X
//	PF->SelectEngine("Tokamak"); //Y (fails)
//	PF->SelectEngine("TrueAxis"); //Y

	pt->CreatePhysics();

	/*
	//hack for novodex, but it still fails, so why bother?
	palTerrainMesh *ptm = 0;
	for (int i=0;i<4;i++) {
		ptm = PF->CreateTerrainMesh();
		ptm->Init(0,0,0,verts,5,&inds[i*3],3*1);
	}
	*/
		
	SDLGLObject *pSDLMesh = new SDLGLObject;
	pSDLMesh->SetPosition(0,0,0);
	pSDLMesh->Construct(true,false,5,3*4);

	pSDLMesh->SetData(0,0, 0,1);
	pSDLMesh->SetData(1,-1, 0,0);
	pSDLMesh->SetData(2,0,-1,0);
	pSDLMesh->SetData(3,1, 0,0);
	pSDLMesh->SetData(4,0, 0,-1);
	
	pSDLMesh->SetColor(0,1,0,0);
	pSDLMesh->SetColor(1,0,1,0);
	pSDLMesh->SetColor(2,0,0,1);
	pSDLMesh->SetColor(3,1,1,1);
	pSDLMesh->SetColor(4,1,0,1);

	pSDLMesh->SetIndex(0,0,1,2);
	pSDLMesh->SetIndex(1,2,0,3);
	pSDLMesh->SetIndex(2,1,2,4);
	pSDLMesh->SetIndex(3,4,3,2);

	PALTestSDLRenderer r;
	r.Main(pt,pSDLMesh,2);
	
	//write out the data
	if (!g_graphics) {
		std::string result = std::string("collision_") + g_engine + ".txt";
		FILE *fout = fopen(result.c_str(),"w");
		for (unsigned int ds = 0; ds<pct->data.size(); ds++) {
			if (pct->data[ds]<0)
				fprintf(fout,"FAILED,");
			else
				fprintf(fout,"%f,",pct->data[ds]);
		}
		fclose(fout);
	}

	delete g_eng;

	PF->Cleanup();

	printf("test_collision finished\n");
	
	return 0;
};
