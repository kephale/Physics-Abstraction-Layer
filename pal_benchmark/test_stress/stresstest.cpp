#include "test_lib/test_lib.h"

bool g_quit = false;

//test specific:
bool g_graphics = true;
float g_max_time= 999999;

float ufrand() {
	return rand()/(float)RAND_MAX;
}
float sfrand() {
	return (ufrand()-0.5f)*2.0f;
}

SDLGLObject *g_terrain_graphics = 0;

void CreatePool(float height,float length_down,float length_up,float width_down,float width_up) {

	float lext1 = 0 - ((length_up - length_down)/2);		// length extra from the coordinate below
	float lext2 = 0 + ((length_up - length_down)/2);		// length extra from the coordinate below
	float wext1 = 0 - ((width_up - width_down)/2);			// width extra from the coordinate below
	float wext2 = 0 + ((width_up - width_down)/2);			// width extra from the coordinate below 
	float ver[3*16];
	int ind1[3*18];	

	// all the coordinates of the point
	ver[0]=0;						ver[1]=0;				ver[2]=0;							// 1st coordinate
	ver[3]=length_down;				ver[4]=0;				ver[5]=0;							// 2nd coordinate 
	ver[6]=0;						ver[7]=0;				ver[8]=width_down;					// 3rd coordinate
	ver[9]=length_down;				ver[10]=0;				ver[11]=width_down;					//		.
	ver[12]=0;						ver[13]=height;			ver[14]=wext1;						//		.
	ver[15]=length_down;			ver[16]=height;			ver[17]=wext1;						//		.
	ver[18]=lext1;					ver[19]=height;			ver[20]=0;							//		.
	ver[21]=length_down + lext2;	ver[22]=height;			ver[23]=0;							//		.
	ver[24]=lext1;					ver[25]=height;			ver[26]=width_down;					//		.
	ver[27]=length_down + lext2;	ver[28]=height;			ver[29]=width_down;					//		.
	ver[30]=0;						ver[31]=height;			ver[32]=width_down + wext2;			//		.
	ver[33]=length_down;			ver[34]=height;			ver[35]=width_down + wext2;			//		.
	ver[36]=lext1;					ver[37]=height;			ver[38]=wext1;						//		.
	ver[39]=length_down + lext2;	ver[40]=height;			ver[41]=wext1;						//		.
	ver[42]=lext1;					ver[43]=height;			ver[44]=width_down + wext2;			//		.
	ver[45]=length_down + lext2;	ver[46]=height;			ver[47]=width_down + wext2;			// 16th coordinate

	for (int i=0;i<48;i++) {
		if ((i%3) == 1)
			ver[i]-=height;
		if ((i%3) == 0) 
			ver[i]-=(length_down)* 0.5f;
		if ((i%3) == 2) 
			ver[i]-=(width_down ) * 0.5f;
	}

// how the point is joint together
	ind1[0]=0;				ind1[1]=3;				ind1[2]=1;				// 1st triangle
	ind1[3]=0;				ind1[4]=2;				ind1[5]=3;				// 2nd triangle
	ind1[6]=0;				ind1[7]=1;				ind1[8]=5;				// 3rd triangle
	ind1[9]=0;				ind1[10]=5;				ind1[11]=4;				// 4th triangle
	ind1[12]=2;				ind1[13]=10;			ind1[14]=3;				//		.
	ind1[15]=3;				ind1[16]=10;			ind1[17]=11;			//		.
	ind1[18]=0;				ind1[19]=8;				ind1[20]=2;				//		.
	ind1[21]=0;				ind1[22]=6;				ind1[23]=8;				//		.
	ind1[24]=1;				ind1[25]=3;				ind1[26]=9;				//		.
	ind1[27]=1;				ind1[28]=9;				ind1[29]=7;				//		.
	ind1[30]=0;				ind1[31]=4;				ind1[32]=12;			//		.
	ind1[33]=0;				ind1[34]=12;			ind1[35]=6;				//		.
	ind1[36]=2;				ind1[37]=8;				ind1[38]=14;			//		.
	ind1[39]=2;				ind1[40]=14;			ind1[41]=10;			//		.
	ind1[42]=3;				ind1[43]=15;			ind1[44]=9;				//		.
	ind1[45]=3;				ind1[46]=11;			ind1[47]=15;			//		.
	ind1[48]=1;				ind1[49]=7;				ind1[50]=13;			// 17th triangle
	ind1[51]=1;				ind1[52]=13;			ind1[53]=5;				// 18th triangle
			
	palTerrainMesh *pool;
	pool = NULL;
	pool = PF->CreateTerrainMesh();
	if (pool != NULL) {
		pool->Init(0,0,0,ver,16,ind1,3*18);		
		SDL_Mesh  *graphics_mesh = NULL;
		graphics_mesh = new SDL_Mesh;
		graphics_mesh -> Init(3*16,3*18,ver,ind1);
		g_terrain_graphics = graphics_mesh;				// drawing the 'pool'
	} else {
		printf("Error : Could not create a 'pool'\n");
	}
}

palPhysics *pp = 0;
int InitPhysics() {
	pp = PF->CreatePhysics();
	if (!pp) {
		MessageBox(NULL,"Could not start physics!","Error",MB_OK);
		return -1;
	}
	//initialize gravity
	pp->Init(0,-9.8f,0);


	CreatePool(5,5,10,5,10);		
	return 0;
}


void Resize(float *verts, int nv, float xs, float ys, float zs) {
	for (int i=0;i<nv;i++) {
		verts[i*3+0]*=xs;
		verts[i*3+1]*=ys;
		verts[i*3+2]*=zs;
	}
}

void MakeZoid(Float x, Float y, Float z) {
	palConvex *pc = dynamic_cast<palConvex *>(PF->CreateObject("palConvex"));
	if (!pc) {
		printf("failed to create convex\n");
		return;
	}
	FILE *fin = fopen("divtop.dat","r");
	if (!fin) {
		printf("failed to load convex object\n");
		return;
	}
	int nv;
	fscanf(fin,"%d",&nv);
	float *verts = new float[nv*3];
	for (int i=0;i<nv;i++) {
		fscanf(fin,"%f %f %f\n",&verts[i*3+0],&verts[i*3+1],&verts[i*3+2]);
	}
	fclose(fin);

	Resize(verts,nv,0.2,0.2,0.2);
	//Resize(verts,nv,0.1,0.1,0.1);
	pc->Init(x,y,z,verts,nv,1);
	BuildGraphics(pc);
}

int counter_y=0;
int last_counter_y=-1;
int UpdatePhysics() {
	palSphere *ps;
	palBox *pb;
	palCapsule *pc;

			counter_y=pp->GetTime();
#if 1
			if (last_counter_y!=counter_y) {
				srand(31337);
#define GS 2

				for (int j=-GS;j<=GS;j++)
					for (int i=-GS;i<=GS;i++) {
						switch (counter_y%4) {
			case 0:
				ps = PF->CreateSphere();
				ps->Init(i+ufrand()*0.4f,3,j+ufrand()*0.4f, ufrand()*0.25f+0.1f,1);
				BuildGraphics(ps);		
				break;
			case 2:
				pb = PF->CreateBox();
				pb->Init(i+ufrand()*0.4f,3,j+ufrand()*0.4f, ufrand()*0.4+0.1f,ufrand()*0.4+0.1f,ufrand()*0.4+0.1f,1);
				BuildGraphics(pb);		
				break;
			case 3:
				pc = PF->CreateCapsule();
				pc->Init(i+ufrand()*0.4f,3,j+ufrand()*0.4f, ufrand()*0.25+0.1f,ufrand()*0.4+0.1f,1);
				BuildGraphics(pc);		
				break;
			case 1:
				MakeZoid(i+ufrand()*0.4f,3,j+ufrand()*0.4f);
				break;
						}
					}
			}
#endif


	return 0;
}


int main(int argc, char *argv[]) {
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
		g_max_time=atof(argv[3]);
		step_size = atof(argv[4]);
	}
	
	SDL_Event	E;
	if (g_graphics) {
	g_eng = new SDLGLEngine; //create the graphics engine
	g_eng->Init(640,480);	
	}

	if (InitPhysics()<0)
		return -1;

	
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

			UpdatePhysics();

			t.StartSample();
			//update physics
			if (pp)
				pp->Update(step_size);
			t.EndSample();

			//clear the screen, setup the camera
			g_eng->Clear();
			g_eng->SetProjMatrix(M_PI/4.0f,1.0f,0.2f,100.0f);
			g_eng->SetViewMatrix(distance*cos(angle),12,distance*sin(angle),0,0,0,0,1,0);
	
			//display all our graphics objects
			for (unsigned int i=0;i<g_Graphics.size();i++) {
				g_Graphics[i]->Display();
			}

			g_terrain_graphics->SetPosition(0,0,0,0,0,0);
			g_terrain_graphics->Render();

			//flip the screen
			g_eng->Flip();
			last_counter_y=counter_y;
		}

	} //end while


	if (!g_graphics) 
		while (pp->GetTime()<g_max_time) {
			UpdatePhysics();

			t.StartSample();
			//update physics
			if (pp)
				pp->Update(step_size);
			t.EndSample();
			last_counter_y=counter_y;
		}

#if 1
	
	if (!g_graphics) {
	STRING result_time = STRING("stress_time_") + argv[2] + "_" + argv[3] + "_" + argv[4] + ".txt";
	FILE *fout_time = fopen(result_time.c_str(),"w");
	fprintf(fout_time,"%f",t.GetElapsedTime());
	fclose(fout_time);
	}
#endif	
	

	delete g_eng;

	PF->Cleanup();

	return 0;
};