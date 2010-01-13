#include "../test_classes/bridge_test.h"
#include "main.h"
#include "paltest.h"
#include "standard_scenes.h"

class TestBridge: public PAL_Bridge_Test<PALTestScene> {
public:
	
	ILightSceneNode *light;
	TestBridge() {
		g_error_sum=-1;
		result.clear();
	}
	VECTOR<float> result;
	void StoreData() {
		result.push_back(g_error_sum);
		g_error_sum = 0;
	}

	void Init(int inum) {
		result.clear();
		g_SceneFinished = false;
		g_smgr->clear();
		num=inum;
#ifdef QUICK
		max_time = 0.5f;
#else
		max_time = 4;
#endif

		counter = 0;
		last_counter = 0;
	
		step_size = 0.01f;

		m_clear_color = SColor(255,10,10,10);
		
		light = g_smgr->addLightSceneNode(0, core::vector3df(-5,5,-2),
			video::SColorf(0.6f, 0.6f, 0.6f), (irr::f32)num);

		float angle = 0.5;
		float distance = (float)num*2.0f;
		camera = g_smgr->addCameraSceneNode(0, vector3df( (irr::f32)distance*cos(angle), (irr::f32)num, (irr::f32)(distance*sin(angle))), vector3df(0,num*0.25f,0));
		
		
		MakeSkybox();
		t = g_device->getTimer();
		
		PF->SelectEngine(g_engines[counter]);
		CreatePhysics();
		UpdateGUI(g_engines[counter]);
		g_error_sum=0;

	}

	ITimer *t;
	virtual void doRender() {
		u32 time = t->getTime();

		float ft = time/5000.0f;
		float angle = ft + 0.5f;
		float distance = (float)num*2.0f;
		camera->setPosition(vector3df(distance*cos(angle),(irr::f32)num,distance*sin(angle)));
		light->setPosition(vector3df(distance*cos(angle),(irr::f32)num,distance*sin(angle)));
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



class SceneBridge: public ScriptScene {
public:
	int m_size;
	SceneBridge(int size) {
		time = -1;
		m_size=size;
	}
	TestBridge test_bridge;
	virtual void Init(ScriptScene *last) {
		printf("INITING SceneBridge %d\n",m_size);
		g_engines = all_engines;		
		RemoveEngine("Jiggle");
		test_bridge.Init(m_size);
#ifdef SHORT_VIDEO_VERSION
		test_bridge.m_render_to_physics_ratio = 6;
#endif
		pds = &test_bridge;
	}
};


class SceneBridgeGraph : public ScriptScene {
public:
	SceneBridgeGraph() {
#ifdef QUICK
		time = 1;
#else
		time = 5;
#endif
	}	

	virtual void Init(ScriptScene *last) {
		g_smgr->clear();
		g_driver->removeAllTextures();
		g_driver->deleteAllDynamicLights();
		
		LoadText();
		static BarGraph bg;
		bg.MakeGraphArea(g_engine_nodes);
		
		SceneBridge *ps = dynamic_cast<SceneBridge *>(last);
		if (!ps)
			return;
		bg.MakeGraphBars(g_engine_nodes,0,0.5,ps->test_bridge.result);
		pds = &bg;
	}
};

