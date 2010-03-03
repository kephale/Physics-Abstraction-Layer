#ifndef RESTITUTION_TEST
#define RESTITUTION_TEST

#include "../test_classes/restitution_test.h"
#include "main.h"
#include "paltest.h"
#include "standard_scenes.h"

class TestRestitution: public PAL_Restitution_Test<PALTestScene> {
public:
	std::vector<std::vector<float> > dataset01;
	std::vector<std::vector<float> > dataset05;
	std::vector<std::vector<float> > dataset10;

	
	void StoreData() {
		dataset01.push_back(pos_01);
		dataset05.push_back(pos_05);
		dataset10.push_back(pos_10);
		pos_01.clear();
		pos_05.clear();
		pos_10.clear();
	}
	
	void Init() {
		g_SceneFinished = false;
		g_smgr->clear();

		counter = 0;
		last_counter = 0;
#ifdef QUICK
		max_time = 1;
#else
		max_time = 2;
#endif
		step_size = 0.01f;

		m_clear_color = SColor(255,10,10,10);
		
		g_smgr->addLightSceneNode(0, core::vector3df(-5,5,-2),
			video::SColorf(0.6f, 0.6f, 0.6f), 7.0f);

		camera = g_smgr->addCameraSceneNode(0, vector3df(0,5,-5), vector3df(0,1,0));
		
		MakeSkybox();
		
		PF->SelectEngine(g_engines[counter]);
		CreatePhysics();
		UpdateGUI(g_engines[counter]);
	}
	

	//IAnimatedMeshSceneNode* psnode;

};


class SceneMaterials: public ScriptScene {
public:
	SceneMaterials() {
		time = -1;
	}
	TestRestitution test_rest;

	virtual void Init(ScriptScene *last) {
		g_engines = all_engines;
		//remove jiggle and ODE from the restitution test
		RemoveEngine("Jiggle");
		RemoveEngine("ODE");

		test_rest.Init();
#ifdef SHORT_VIDEO_VERSION
		test_rest.m_render_to_physics_ratio = 2;
#endif
		pds = &test_rest;
	}
};



class SceneMatGraph : public ScriptScene {
public:
	SceneMatGraph() {
		time = 5;
	}	
	PlotGraph plg;
	SceneMaterials *ps;
	virtual void Init(ScriptScene *last) {
		g_driver->deleteAllDynamicLights();
		g_smgr->clear();
		LoadText();


		plg.MakeGraphArea(g_engine_nodes,true);
/*
		STRING m_info = "Constraint resolu
		if (m_info.length()>2) {
			dimension2di ss = g_driver->getScreenSize();

			int a = m_info.length();
			BSTR unicodestr = SysAllocStringLen(NULL, a);
			MultiByteToWideChar(CP_ACP, 0, m_info.c_str(), a, unicodestr, a);
			g_gui->addStaticText(unicodestr,rect<s32>(50,ss.Height-50,ss.Width-50,ss.Height-10));
		}
*/
		ps = dynamic_cast<SceneMaterials *>(last);
		plg.MakeGraphPlot(ps->test_rest.dataset05);
		pds = &plg;
	}
};
		

class SceneMatBar : public ScriptScene {
public:
	SceneMatBar() {
#ifdef SHORT_VIDEO_VERSION
		time = 1.5f;
#else
		time = 5;
#endif
	}	
	BarGraph bg;
	SceneMaterials *ps;
	virtual void Init(ScriptScene *last) {
		g_smgr->clear();
		g_gui->clear();
		LoadText();

		IAnimatedMesh* mesh = g_smgr->getMesh( idealImageFile.c_str() );
		IAnimatedMeshSceneNode* node = g_smgr->addAnimatedMeshSceneNode( mesh );
		node->setRotation(core::vector3df(-90,0,0));
		g_engine_nodes.insert(g_engine_nodes.begin(),node);

		bg.MakeGraphArea(g_engine_nodes,true);
		ps = dynamic_cast<SceneMaterials *>(last);
		if (!ps){
			SceneMatGraph *pms = dynamic_cast<SceneMatGraph *>(last);
			ps = pms->ps;
		}
		std::vector<float> data01;
		std::vector<float> data05;
		std::vector<float> data10;
		TestRestitution *pr = &(ps->test_rest);
		unsigned int i;
		data01.push_back(0.01f);
		for (i=0;i<pr->dataset01.size();i++) {
			data01.push_back(* std::max_element(pr->dataset01[i].begin()+45,pr->dataset01[i].end()));
		}
		data05.push_back(0.25f);
		for (i=0;i<pr->dataset05.size();i++) {
			data05.push_back(* std::max_element(pr->dataset05[i].begin()+45,pr->dataset05[i].end()));
		}
		data10.push_back(0.81f);
		for (i=0;i<pr->dataset10.size();i++) {
			data10.push_back(* std::max_element(pr->dataset10[i].begin()+45,pr->dataset10[i].end()));
		}
		bg.MakeGraphBars(g_engine_nodes,0.0f,0.2f,data01,1.0f);
		bg.MakeGraphBars(g_engine_nodes,0.3f,0.2f,data05,1.0f);
		bg.MakeGraphBars(g_engine_nodes,0.6f,0.2f,data10,1.0f);
		pds = &bg;
	}
};

#endif
