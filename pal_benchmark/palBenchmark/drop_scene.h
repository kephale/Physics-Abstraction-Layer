#include "../test_classes/drop_test.h"
#include "main.h"
#include "paltest.h"
#include "standard_scenes.h"

class TestDrop: public PAL_Drop_Test<PALTestScene> {
public:
	virtual void doInnerUpdateLoop() {
			palVector3 p;
			ps->GetPosition(p);
		//========
		//sneak in some gfx
		camera->setTarget(core::vector3df(0,p.y,0));
		//========
	}
	
	void StoreData() 
	{ 
		//#pragma message ("!!! TestDrop: future, store data here, display later") 
	}
	
	void Init() {
		counter = 0;
		last_counter = 0;
		max_time = 1;
		step_size = 0.01f;

		m_clear_color = SColor(255,10,10,10);
		
		camera = g_smgr->addCameraSceneNode(0, vector3df(0,0,-10), vector3df(0,0,0));
		g_smgr->addLightSceneNode(0, vector3df(0,0,-5),
			video::SColorf(0.6f, 0.6f, 0.6f), 5.0f);

		MakeSkybox();
		
		PF->SelectEngine(g_engines[counter]);
		CreatePhysics();
		UpdateGUI(g_engines[counter]);
	}
	/*virtual void Render() {
	}*/
	
	IAnimatedMeshSceneNode* psnode;
};

class SceneDrop : public ScriptScene {
public:

	SceneDrop() {
		time = -1;
	}

	// 0xBADF00D on test_drop ??????
	TestDrop test_drop;

	virtual void Init(ScriptScene *last) {
		g_engines = all_engines;
		test_drop.Init();
		pds = &test_drop;
	}
};
