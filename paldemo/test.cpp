#include "test.h"

// create a 'pool' terrain
void Test::CreatePool(float height,float length_down,float length_up,float width_down,float width_up) {

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
		pTerrain = pool;
		terrain_graphics = graphics_mesh;				// drawing the 'pool'

	} else {
		printf("Error : Could not create a 'pool'\n");
	}
}

void Test::CreateHeightmap(int size, float stretch ) {

			int h;
			float *heights = new float[size*size];
			memset(heights,0,sizeof(float)*size*size);
			for (h=0;h<size*size;h++)
				heights[h]=-ufrand();
			
			palTerrainHeightmap *pth;
			pth = NULL;
			pth = PF->CreateTerrainHeightmap();
			if (pth!=NULL) {
				pth->Init(0,0,0,stretch,stretch,size,size,heights);
				SDLGLPlane *pSDLGLplane = new SDLGLPlane;
				pSDLGLplane->Create(0,0,0,stretch,stretch,size,size,heights);

				pTerrain = pth;
				terrain_graphics = pSDLGLplane;
			} else {
				printf("Error: Could not create a terrain heightmap\n");
			}
	
}

void Test::CreateTerrain(int type, float size)
{
		switch (type) {
		default:
		case 0: //no terrain
			terrain_graphics = NULL;
			break;
		case 2: //heightmap
			CreateHeightmap(4,size);
			break;
		case 3:
			CreatePool(5,5,10,5,10);				// specifying the 'pool' parameter (parameter can be change)
			break;	
		case 4:  //stairmap
			{
			int i;
			float k;
			float s1[18*15];	// 15 stairs

			float ysf = 0.3f;
			for (i=0;i<18*15;i+=18) {
			k=0;
			k = (i/36.0f);

			s1[i+0]=-5;			s1[i+1]=(0+k)*ysf;		s1[i+2]=4-k;	  	 
			s1[i+3]=5;			s1[i+4]=(0+k)*ysf;		s1[i+5]=4-k;		
			s1[i+6]=-5;			s1[i+7]=(0.5+k)*ysf;		s1[i+8]=4-k;	  	 
			s1[i+9]=5;			s1[i+10]=(0.5+k)*ysf;		s1[i+11]=4-k;	
			s1[i+12]=-5;		s1[i+13]=(0.5+k)*ysf;		s1[i+14]=3.5-k;	  	 
			s1[i+15]=5;			s1[i+16]=(0.5+k)*ysf;		s1[i+17]=3.5-k;
			}

			int ind[12*15];
			for(i=0;i<12*15;i+=12) {		// 15 stairs
			k=0;
			k = (i/2);

			ind[i+0]=0+k;			ind[i+1]=1+k; 			ind[i+2]=2+k;		
			ind[i+3]=2+k;			ind[i+4]=3+k; 			ind[i+5]=1+k;	
			ind[i+6]=2+k;			ind[i+7]=3+k; 			ind[i+8]=5+k;		
			ind[i+9]=4+k;			ind[i+10]=5+k; 			ind[i+11]=2+k;	
			}
			
			palTerrainMesh *map;
			map = NULL;
			map = PF->CreateTerrainMesh();
			if (map != NULL) {
				map->Init(0,0,0,s1,6*15,ind,12*15);
				SDL_Mesh  *graphics_mesh = NULL;				// create the stair map
				graphics_mesh = new SDL_Mesh;		
				graphics_mesh -> Init(18*15,12*15,s1,ind);

				pTerrain = map;
				terrain_graphics = graphics_mesh;

			} else {
				printf("Error : Could not create a stair\n");
			}
			}
			break;
		case 5:
			{
				palOrientatedTerrainPlane * pot = dynamic_cast<palOrientatedTerrainPlane *>(PF->CreateObject("palOrientatedTerrainPlane"));
				if (pot){
				pot->Init(0,0,0,sin(0.2),cos(0.2),0,size);
				SDLGLPlane *pSDLGLplane = new SDLGLPlane;
				pSDLGLplane->Create(0,0,0,size,size);
				pSDLGLplane->SetPosition(pot->GetLocationMatrix()._mat);
				terrain_graphics = pSDLGLplane;
				pTerrain = pot;
				}
			}
			break;
		case 6:
			{
				int i,j;
				int dist;
				dist = 2;
				float mult;
				mult = 1.5f;
				for (j=-dist;j<=dist;j++)
					for (i=-dist;i<=dist;i++) {
						palStaticBox *psb = dynamic_cast<palStaticBox *> (PF->CreateObject("palStaticBox"));
						if (psb) {
							psb->Init(i*mult,1.0f,j*mult,0.5,0.25,0.25);
							BuildGraphics(psb);
						}
					}
					
			}
			//no break! go through and make a flat plane!
		case 1: //flat terrain
			palTerrainPlane *pt;
			pt = NULL;
			pt = PF->CreateTerrainPlane();
			if (pt!=NULL) {
				pt->Init(0,0,0,size);
				SDLGLPlane *pSDLGLplane = new SDLGLPlane;
				pSDLGLplane->Create(0,0,0,size,size);
				terrain_graphics = pSDLGLplane;
				pTerrain = pt;
			} else {
				printf("Error: Could not create a terrain plane\n");
			}
			break;

			break;

	}
}


palBodyBase *Test::CreateBody(const char *paltype,float x, float y, float z, float dimx, float dimy, float dimz, float mass, bool gfx) {
	palBodyBase *pb = NULL;
	pb = dynamic_cast<palBodyBase *>(PF->CreateObject(paltype));
	palBox *pbox=dynamic_cast<palBox *>(pb);
	if (pbox) {
		pbox->Init(x,y,z,dimx,dimy,dimz,mass);
	} 
	palStaticBox *psbox = dynamic_cast<palStaticBox *>(pb);
	if (psbox) {
		psbox->Init(x,y,z,dimx,dimy,dimz);
	} 

	palSphere *psphere = dynamic_cast<palSphere *>(pb);
	if (psphere) {
		psphere->Init(x,y,z,dimx,mass);
	}
	palStaticSphere *pssphere = dynamic_cast<palStaticSphere *>(pb);
	if (pssphere) {
		pssphere->Init(x,y,z,dimx);
	} 


	palCapsule *pcylinder = dynamic_cast<palCapsule *>(pb);
	if (pcylinder) {
		pcylinder->Init(x,y,z,dimx,dimy,mass);
	}
	palStaticCapsule *pscylinder = dynamic_cast<palStaticCapsule *>(pb);
	if (pscylinder) {
		pscylinder->Init(x,y,z,dimx,dimy);
	} 

	if (gfx)
	if (pb)
		BuildGraphics(pb);
	return pb;
}

void Test::MakeConvexCone(Float *pVerts)
 {
	int i;
	for (i=0;i<36;i++) {
		pVerts[i*3+0] = sin(i*10*DEG2RAD);
		pVerts[i*3+1] = cos(i*10*DEG2RAD);
		pVerts[i*3+2] = 0;
	}
	for (i=36;i<36+36;i++) {
		pVerts[i*3+0] = 0.5*sin(i*10*DEG2RAD);
		pVerts[i*3+1] = 0.5*cos(i*10*DEG2RAD);
		pVerts[i*3+2] = 0.5;
	}
	i=36+36;
	pVerts[i*3+0] = 0;
	pVerts[i*3+1] = 0;
	pVerts[i*3+2] = 1;
}