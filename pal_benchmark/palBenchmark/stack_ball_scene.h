#ifndef STACK_BALL_SCENE_H
#define STACK_BALL_SCENE_H

#include "main.h"
#include "paltest.h"
#include "standard_scenes.h"

class CannonBallWall : public PALTestScene {
public:
	virtual void StoreData() {};
	virtual void SaveData() {};
	int doCreatePhysics() {
		pp = PF->CreatePhysics();
		if (!pp) {
#ifdef _WIN32
			MessageBox(NULL,"Could not start physics!","Error",MB_OK);
#endif
			return -1;
		}
		//initialize gravity
		
		palPhysicsDesc physDesc;
		physDesc.m_vGravity = -9.8f;
		pp->Init( physDesc );
	
		//initialize the ground
		palTerrainPlane *pt= PF->CreateTerrainPlane();
		if (pt) {
			pt->Init(0,0,0,30.0f);
		}
		BuildGraphics(pt);

		palBox *pb;
		int x,y;
		int m_PyrHeight = 9;
		for (y=0;y<m_PyrHeight;y++)	{
					for (x=0;x<y;x++) {
						pb = PF->CreateBox();
						pb->Init(	(float)x, (float)(((m_PyrHeight-1)-y)+0.5f), 0.0f,
									1.0f, 1.0f, 1.0f,
									3.0f 
								);
						BuildGraphics(pb);		
						if (x) {
							pb = PF->CreateBox();
							pb->Init(	(float)-x, (float)(((m_PyrHeight-1)-y)+0.5f), 0.0f,
								1.0f, 1.0f, 1.0f,
								3.0f 
							);
							BuildGraphics(pb);		
						}
					}
				}


		ps = PF->CreateSphere();
		if (!ps) {
#ifdef _WIN32
			MessageBox(NULL,"Could not make sphere!","Error",MB_OK);
#endif
			return -1;
		}
		ps->Init(0,3,-10,1,10);
		BuildGraphics(ps);		
		ps->ApplyImpulse(0,40,215);
		ssflag=false;
		return 0;
	}

	bool ssflag;
	virtual void doInnerUpdateLoop() {
		palVector3 p;
		ps->GetPosition(p);

#if 0
		//========
		//sneak in some screenshot code :D
		if (ssflag) //took a screenshot? turn off screenshot mode then.
			g_TakeScreenshot = false;

		if (!ssflag)
		if (pp->GetTime() > 1) {
			ssflag=true;
			g_TakeScreenshot = true;
		}
#endif
		//========
		//sneak in some gfx
		camera->setTarget(core::vector3df(p.x,p.y,p.z));
		//========
	}

	void Init() {
		g_SceneFinished = false;
		counter = 0;
		last_counter = 0;
		max_time = 6;
		step_size = 0.01f;

		m_clear_color = SColor(255,10,10,10);
		
		camera = g_smgr->addCameraSceneNode(0, vector3df(10,5,-10), vector3df(0,0,0));
		g_smgr->addLightSceneNode(0, vector3df(0,0,-5),
			video::SColorf(0.6f, 0.6f, 0.6f), 5.0f);

		MakeSkybox();
		
		PF->SelectEngine(g_engines[counter]);
		CreatePhysics();
		UpdateGUI(g_engines[counter]);
	}

public:
	palSphere *ps;
};

class SceneCannonBallWall : public ScriptScene {
public:
	SceneCannonBallWall() {
		time = -1;
	}
	CannonBallWall test_cbw;
	virtual void Init(ScriptScene *last) {
		g_engines = all_engines;
		RemoveEngine("ODE");
		test_cbw.Init();
		pds = &test_cbw;
	}
};

#endif
