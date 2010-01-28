#include "pal_test.h"
#include "../palBenchmark/paltest.h"

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
template <typename T = PALTestScene> class PAL_Restitution_Test : public T  {
public:
	std::vector<float> pos_01;
	std::vector<float> pos_05;
	std::vector<float> pos_10;

	float pos01_max;
	float pos05_max;
	float pos10_max;

	PAL_Restitution_Test() {
		pos01_max = -1;
		pos05_max = -1;
		pos10_max = -1;
	}
protected:
	virtual void doInnerUpdateLoop() {
		;
	}

	virtual void StoreData() {
			palVector3 pos;
			palVector3 vel;
			ps01->GetPosition(pos);
			ps01->GetLinearVelocity(vel);
			pos.y-=1.5f;
			if (vel.y>0)
				if (pos.y>pos01_max)
					pos01_max = pos.y;
			pos_01.push_back(pos.y);

			ps05->GetPosition(pos);
			ps05->GetLinearVelocity(vel);
			pos.y-=1.5f;
			if (vel.y>0)
				if (pos.y>pos05_max)
					pos05_max = pos.y;
			pos_05.push_back(pos.y);

			ps10->GetPosition(pos);
			ps10->GetLinearVelocity(vel);
			pos.y-=1.5f;
			if (vel.y>0)
				if (pos.y>pos10_max)
					pos10_max = pos.y;
			pos_10.push_back(pos.y);
		#if 0
		palVector3 p;
		ps01->GetPosition(p);
		p01.push_back(p.y - 1.5f);
		ps01->GetLinearVelocity(vel);
		if (vel.y>0)
				if (pos.y>pos01_max)
					pos01_max = pos.y;

		ps05->GetPosition(p);
		p05.push_back(p.y - 1.5f);

		ps10->GetPosition(p);
		p10.push_back(p.y - 1.5f);
#endif
	};

	
#define USE_SPHERES	

	palBox *pb;
#ifdef USE_SPHERES
	palSphere *ps01;
	palSphere *ps05;
	palSphere *ps10;
#else
	palBox *ps01;
	palBox *ps05;
	palBox *ps10;
#endif

	int doCreatePhysics() {
	
		//initialize gravity
		palPhysicsDesc desc;

		this->pp = PF->CreatePhysics();
		if (!this->pp) {
#ifdef _WIN32
			MessageBox(NULL,"Could not start physics!","Error",MB_OK);
#endif
			return -1;
		}
	
		// BW: Week 9
		//palSolver *pSolver = dynamic_cast<palSolver *> (pp);
		//if (pSolver) {
	//		pSolver->SetPE(2);
	//	}

		this->pp->Init(desc); //initialize it, set the main gravity vector

		palTerrainPlane *pt= PF->CreateTerrainPlane();
		if (pt) {
			pt->Init(0,0,0,20.0f);
		}
		this->BuildGraphics(pt);

		//initialize materials
		palMaterials *pm = PF->CreateMaterials();
		if (pm)
		{
			palMaterialDesc matDesc_Sticky;
			matDesc_Sticky.m_fStatic = 0.3f;
			matDesc_Sticky.m_fKinetic = 0.3f;
			matDesc_Sticky.m_fRestitution = 0.0f;
			pm->NewMaterial("sticky", matDesc_Sticky);

			palMaterialDesc matDesc_rest01;
			matDesc_rest01.m_fStatic = 0.9f;
			matDesc_rest01.m_fKinetic = 0.0f;
			matDesc_rest01.m_fRestitution = 0.1f;
			pm->NewMaterial("rest01", matDesc_rest01);

			palMaterialDesc matDesc_rest05;
			matDesc_rest05.m_fStatic = 0.9f;
			matDesc_rest05.m_fKinetic = 0.0f;
			matDesc_rest05.m_fRestitution = 0.5f;
			pm->NewMaterial("rest05", matDesc_rest05);

			palMaterialDesc matDesc_rest10;
			matDesc_rest10.m_fStatic = 0.9f;
			matDesc_rest10.m_fKinetic = 0.0f;
			matDesc_rest10.m_fRestitution = 0.9f;
			pm->NewMaterial("rest10", matDesc_rest10);
		}

	//restitution: 0.1 
	pb = PF->CreateBox();
	pb->Init(-2,0.5f,0,1,1,1,1);
	if (pm)
		pb->SetMaterial(pm->GetMaterial("rest01"));
	this->BuildGraphics(pb);		
#ifdef USE_SPHERES
	ps01 = PF->CreateSphere();
	ps01->Init(-2,2.5f,0,0.5f,1);
#else
	ps01 = PF->CreateBox();
	ps01->Init(-2,2.5f,0,1,1,1,1);
#endif
	if (pm)
		ps01->SetMaterial(pm->GetMaterial("rest01"));
	this->BuildGraphics(ps01);		
	
	//restitution: 0.5
	pb = PF->CreateBox();
	pb->Init(0,0.5f,0,1,1,1,1);
	if (pm)
		pb->SetMaterial(pm->GetMaterial("rest05"));
	this->BuildGraphics(pb);		
#ifdef USE_SPHERES
	ps05 = PF->CreateSphere();
	ps05->Init(0,2.5f,0,0.5f,1);
#else
	ps05 = PF->CreateBox();
	ps05->Init(0,2.5f,0,1,1,1,1);
#endif
	if (pm)
		ps05->SetMaterial(pm->GetMaterial("rest05"));
	this->BuildGraphics(ps05);		

	//restitution: 1.0
	pb = PF->CreateBox();
	pb->Init(2,0.5f,0,1,1,1,1);
	if (pm)
		pb->SetMaterial(pm->GetMaterial("rest10"));
		this->BuildGraphics(pb);		
#ifdef USE_SPHERES
	ps10 = PF->CreateSphere();
	ps10->Init(2,2.5f,0,0.5f,1);
#else
	ps10 = PF->CreateBox();
	ps10->Init(2,2.5f,0,1,1,1,1);
#endif
	if (pm)
		ps10->SetMaterial(pm->GetMaterial("rest10"));
	this->BuildGraphics(ps10);	

	return 0;
	}
};
