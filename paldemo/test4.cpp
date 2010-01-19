#include "test4.h"


FACTORY_CLASS_IMPLEMENTATION(Test_4);

void Test_4::InitMaterials() {
		palMaterials *pm=NULL;
		//create the materials collection
		pm = PF->CreateMaterials();		
		if (pm == NULL) {
			printf("could not create materials!\n");
			return;
		}
		//test materials:
		palMaterialDesc desc;
		desc.m_fStatic = 1.0f;
		desc.m_fKinetic = 0.5f;
		desc.m_fRestitution = 0.0;
		pm->NewMaterial("Normal",desc);

		pTerrain->SetMaterial(pm->GetMaterial("Normal") );
}

void Test_4::SetMat(palBody *pb) {
	if (pb==NULL)
		return;
	palMaterials *pm=NULL;
	//create the materials collection
	pm = PF->CreateMaterials();		
	if (pm == NULL) {
		printf("could not create materials!\n");
		return;
	}
	palMaterial *pmat= pm->GetMaterial("Normal");
	if (pmat == NULL) {
		printf("could not find normal material!\n");
		return;
	}
	pb->SetMaterial(pmat);
}

void Test_4::BuildJengaStack(int height, int width,
                                  float dimW,float dimH,
                                  float posX,float posY,float posZ)
{
  float x,y,z;
  for (int iH = 0 ; iH < height ; iH++)
  {
    for (int iBox = -1 ; iBox <= 1 ; ++iBox)
    {
		palBox *pb;
		pb = PF->CreateBox();
			x=iBox*dimW+posX;
			y=iH*dimH+posY;
			z=posZ;
			printf("iH:%d %d\n",iH,iH&1);
		if ((iH&1) == 0) {
			pb->Init(x,y+dimH*0.5f,z,dimW,dimH,dimW*width,1);
		} else {
			pb->Init(z,y+dimH*0.5f,x,dimW*width,dimH,dimW,1);
		}
		
		SetMat(pb);
		BuildGraphics(pb);
    }
  }
}

void Test_4::BuildWall(int height, 
                            int width,
                            float dimX,float dimY,float dimZ,
                            float posX, float posY, float posZ)
{
	float x,y,z;
  for (int iH = 0 ; iH < height ; ++iH)
  {
    for (int iW = 0 ; iW < width ; ++iW)
    {
		x=posX+iW*dimX;
		y=posY+iH*dimY;
		z=posZ;

		if ((iH&1)==0)
			x+=dimX*0.5f;
		
		palBox *pb;
		pb = PF->CreateBox();
		pb->Init(x,y+dimY*0.5f,z,dimX,dimY,dimZ,1);	
		SetMat(pb);
		BuildGraphics(pb);
    }
  }
}


void Test_4::BuildCards() {
	palBox *pb;
	float x,y,z;
	
	for (int i=0;i<2;i++) {
		x=i;
		y=sqrt(2.0)*0.5f;
		z=0;
		pb = PF->CreateBox();
		pb->Init(0,0,0,0.1f,sqrt(2.0),1,1);	
		
		if ((i&1)==0) {
			pb->SetPosition(x,y,z,M_PI*0.23f,0,0);
		} else {
			pb->SetPosition(x,y,z,-M_PI*0.23f,0,0);
		}
		SetMat(pb);
		BuildGraphics(pb);
	}

}

void Test_4::Input(SDL_Event E) {
		int x,y;

		palBox *pb;
		
		switch(E.type) {
		case SDL_KEYDOWN:
			switch (E.key.keysym.sym) {
			case SDLK_1:	
				for (y=0;y<m_PyrHeight;y++)	{
					for (x=0;x<y;x++) {
						pb = PF->CreateBox();
						pb->Init(x,((m_PyrHeight-1)-y)+0.5f,0,1,1,1,3);

						SetMat(pb);
						BuildGraphics(pb);		
						if (x) {
							pb = PF->CreateBox();
							pb->Init(-x,((m_PyrHeight-1)-y)+0.5f,0,1,1,1,3);
							
							SetMat(pb);
							BuildGraphics(pb);		
						}
					}
				}
				
				break;
			case SDLK_2:
				BuildJengaStack(m_JengaHeight,3,1,1,0,0,0);
				break;
			case SDLK_3:
				BuildWall(m_WallHeight,5, 2,1,1, -4,0,0);
				break;
			case SDLK_4:
			//	BuildCards();
				break;
			case SDLK_a:
				m_PyrHeight++;
				break;
			case SDLK_s:
				m_JengaHeight++;
				break;
			case SDLK_d:
				m_WallHeight++;
				break;

			case SDLK_z:
				m_PyrHeight--;
				break;
			case SDLK_x:
				m_JengaHeight--;
				break;
			case SDLK_c:
				m_WallHeight--;
				break;
			case SDLK_SPACE:
				pb = dynamic_cast<palBox *>(CreateBody("palBox",sfrand()*3,ufrand()*3+1.5f,4,0.25f,0.25f,0.25f,5));
				if (pb == NULL) {
					printf("Error: Could not create a box\n");
					return;
				} 
				pb->ApplyImpulse(0,0,-35);
				BuildGraphics(pb);		
				break;
			}
			break;
		}
}
