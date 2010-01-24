#include "pal_test.h"

#include <pal.h>
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

template <typename T = PALTest> class PAL_Stack_Test : public T  {
public:
#ifdef TIMESTACK
	// BW: Timer t;
#endif
	int num;
	bool g_force_active;

	PAL_Stack_Test() {
		g_force_active = true;
		use_spheres = false;
	}

protected:
	std::vector<palBody *> boxes;
#ifdef TIMESTACK
	virtual void Update() {
		if (!this->pp)
			return;

		// BW: t.StartSample();
		this->pp->Update(step_size);
		// BW: t.EndSample();
		//do inner loop
		doInnerUpdateLoop();
		this->SaveData();
	}
#endif
	virtual void doInnerUpdateLoop() {
		if (g_force_active) 
		for (unsigned int i=0;i<boxes.size();i++) {
			boxes[i]->SetActive(true);
		}
	}
	bool use_spheres;
	int doCreatePhysics() {
		boxes.clear();

		this->pp = PF->CreatePhysics();
		if (!this->pp) {
#ifdef _WIN32
			MessageBox(NULL,"Could not start physics!","Error",MB_OK);
#endif
			return -1;
		}

		if (!use_spheres)
#if 0
		if (dynamic_cast<palNewtonPhysics *>(pp)) {
			this->max_time = 15;
		} else 
#else
		{
			this->max_time = 10;
		}
#endif

		//initialize gravity
		palPhysicsDesc desc;
		this->pp->Init(desc); //initialize it, set the main gravity vector

		palMaterialDesc matDesc_Sticky;
		matDesc_Sticky.m_fStatic = 0.3f;
		matDesc_Sticky.m_fKinetic = 0.3f;
		matDesc_Sticky.m_fRestitution = 0.0f;

		//initialize materials
		palMaterials *pm = PF->CreateMaterials();
		if (pm)
		{
			pm->NewMaterial("sticky", matDesc_Sticky);
		}

		//initialize the ground
		palTerrainPlane *pt= PF->CreateTerrainPlane();
		if (pt) {
			pt->Init(0,0,0,30.0f);
			//set the material
			if (pm)
				pt->SetMaterial(pm->GetMaterial("sticky"));
		}
		BuildGraphics(pt);
	
		srand(31337);
		for (int i=0;i<num;i++) {
			palBody *pb;
			if (!use_spheres) {
			palBox *pbx;
			pbx = PF->CreateBox();
			pbx->Init(this->sfrand()*0.1f,i*1.1f+0.5f,this->sfrand()*0.1f,1,1,1,1);
			pb = pbx;
			} else {
			palSphere *pbx;
			pbx = PF->CreateSphere();
			pbx->Init(0,i*1.1f+0.5f,0,0.5f,1);
			pb=pbx;
			}
			if (pm)
				pb->SetMaterial(pm->GetMaterial("sticky"));
			BuildGraphics(pb);	
			boxes.push_back(pb);
		}

		return 0;
	}
};
