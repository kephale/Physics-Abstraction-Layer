#include "pal_test.h"
#include "../palBenchmark/paltest.h" // for PALTestScene

/*
	PAL Test Collection
    Copyright (C) 2007  Adrian Boeing

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

template <typename T = PALTestScene> class PAL_Bridge_Test : public T  {
public:
#ifdef TIMEBRIDGE
	// BW: Timer t;
#endif
	float g_error_sum;	
	int num;
	PAL_Bridge_Test() {
		g_error_sum = 0;
	}
protected:
	palBody *Buildn(float x, int num, palMaterials *pm) {
		palBox *pb;
		pb = PF->CreateBox();
		pb->Init(x,(float)num*0.5f,0,2.0f, (float)num,1.0f,(float)num*800.0f);
		if (pm)
			pb->SetMaterial(pm->GetMaterial("sticky"));
		this->BuildGraphics(pb);	
		return pb;
	}

	virtual void StoreData() {
			for (unsigned int i=1;i<vSpheres.size();i++) {
				palVector3 pos1;
				palVector3 pos2;
				palVector3 result;
				vSpheres[i-1]->GetPosition(pos1);
				vSpheres[i  ]->GetPosition(pos2);
				vec_sub(&result,&pos1,&pos2);
				float e=vec_mag(&result);
				g_error_sum+=fabsf(e-1.0f);
			}
	};
	virtual void doInnerUpdateLoop() {
		;
	}

#ifdef TIMEBRIDGE
	virtual void Update() {
		if (!this->pp)
			return;

		// BW: t.StartSample();
		this->pp->Update(step_size);
		// BW: t.EndSample();
		//do inner loop
		doInnerUpdateLoop();
		this->StoreData();
	}
#endif
	
	std::vector<palSphere *> vSpheres;

	int doCreatePhysics() {
	vSpheres.clear();
	this->pp = PF->CreatePhysics();
	if (!this->pp) {
#ifdef _WIN32
		MessageBox(NULL,"Could not start physics!","Error",MB_OK);
#endif
		return -1;
	}
	//initialize gravity
	palPhysicsDesc desc;
	this->pp->Init(desc); //initialize it, set the main gravity vector

	//initialize materials
	palMaterials *pm = PF->CreateMaterials();
	if (pm)
	{
		palMaterialDesc matDesc_Sticky;
		matDesc_Sticky.m_fStatic = 0.9f;
		matDesc_Sticky.m_fKinetic = 0.9f;
		matDesc_Sticky.m_fRestitution = 0.0f;
		pm->NewMaterial("sticky", matDesc_Sticky);
	}

	//initialize the ground
	palTerrainPlane *pt= PF->CreateTerrainPlane();
	if (pt) {
		pt->Init(0,0,0,50.0f);
		//set the material
		if (pm)
			pt->SetMaterial(pm->GetMaterial("sticky"));
	}
	this->BuildGraphics(pt);

	palBody *pb0  = Buildn((float)-num, num,pm);
	palBody *pb1  = Buildn((float) num, num,pm);
	
	palBody *last = pb0;
	for (int i=1;i<num*2 - 1;i++) {
		palSphere *ps= PF->CreateSphere();
		ps->Init((float)i-num+0.5f,(float)num,0,0.2f,0.1f);
		if (last) {
			palSphericalLink *plink = PF->CreateSphericalLink();
			plink->Init(last,ps,(float)i-num,(float)num,0);
		}
		this->BuildGraphics(ps);		
		vSpheres.push_back(ps);
		last = ps;
	}

		palSphericalLink *plink = PF->CreateSphericalLink();
		plink->Init(pb1,last, (float)num, (float)num,0);

		return 0;
	}
};
