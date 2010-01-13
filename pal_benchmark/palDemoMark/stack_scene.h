#include "../test_classes/stack_test.h"
#include "main.h"
#include "paltest.h"
#include "standard_scenes.h"

class TestStack: public PAL_Stack_Test<PALTestScene> {
public:

		void StoreData() {
		;
//#pragma message ("!!! TestStack: future, store data here, display later")
	}
	

		float yla;
	void Init(bool spheres) {
		g_SceneFinished = false;
		g_smgr->clear();
		if (spheres) {
			num = 3;
			use_spheres=true;
			max_time = 3;
			yla=1;
		} else {
			num=20;
			use_spheres=false;
			max_time = 10;
			yla=10;
		}
		counter = 0;
		last_counter = 0;
	
		step_size = 0.01f;

		m_clear_color = SColor(255,10,10,10);
		
		g_smgr->addLightSceneNode(0, core::vector3df(-5,5,-2),
			video::SColorf(0.6f, 0.6f, 0.6f), 7.0f);

		if (use_spheres)
			camera = g_smgr->addCameraSceneNode(0, vector3df(0,5,-5), vector3df(0,yla,0));
		else
			camera = g_smgr->addCameraSceneNode(0, vector3df(0,10,-20), vector3df(0,yla,0));

		
		MakeSkybox();
	
		tim = g_device->getTimer();

		PF->SelectEngine(g_engines[counter]);
		CreatePhysics();
		UpdateGUI(g_engines[counter]);
	
		if(tim)
			tim->setTime(0);
	}


	ITimer *tim;
	virtual void doRender() {
		u32 time = tim->getTime();

		float ft = time/10000.0f;
		float angle = 1.8f-ft;
		if (angle < 1.4f)
			angle = 1.4f;
		//if (angle>
		float distance;
		if (use_spheres) {
			distance = 5;
			angle = 1.5f;
		}
		else
			distance = 20;
		camera->setPosition(vector3df(0,distance*cos(angle)+yla,-distance*sin(angle)));
		u32 tdiff = time - m_last_time;
		if (tdiff < 10) {
			g_device->sleep(10-tdiff);
		}
		g_driver->beginScene(true, true, m_clear_color);
		g_smgr->drawAll();
		g_gui->drawAll();
		
		g_driver->endScene();
		m_last_time = time;
	}
};


class SceneStack : public ScriptScene {
public:
	bool m_spheres;
	SceneStack( bool spheres) {
		time = -1;
		m_spheres = spheres;
	}
	TestStack test_stack;
	virtual void Init(ScriptScene *last) {
		g_engines = all_engines;
		if (!m_spheres)
		RemoveEngine("ODE");
		
		test_stack.Init(m_spheres);
#ifdef SHORT_VIDEO_VERSION
		test_stack.m_render_to_physics_ratio=3;
#endif
		pds = &test_stack;
	}
};


class SceneStackSpheres : public ScriptScene {
public:
	SceneStackSpheres() {
		time = -1;
	}
	TestStack test_stack;
	virtual void Init(ScriptScene *last) {
		g_engines = all_engines;
//		g_engines.erase(std::find(g_engines.begin(),g_engines.end(),"ODE"));
		test_stack.Init(true);
		pds = &test_stack;
	}
};

