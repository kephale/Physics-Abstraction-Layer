#include "../test_classes/collision_test.h"
#include "main.h"
#include "paltest.h"
#include "standard_scenes.h"

class TestCollision: public PAL_Collision_Test<PALTestScene> {
public:
	virtual void SaveData() {
		;
	}
	virtual void StoreData() {
		;
//#pragma message ("!!! TestCollision: future, store data here, display later")
	}
	void Init(float ss) {
		g_SceneFinished = false;
		g_smgr->clear();
		counter = 0;
		last_counter = 0;
#ifdef QUICK
		max_time = 0.1f;
#else
		max_time = 1.5f;
#endif
		step_size = ss;
	
		//step_size = 0.01;
		//step_size = 1/5.f;

		m_clear_color = SColor(255,10,255,10);
		
		g_smgr->addLightSceneNode(0, core::vector3df(-5,5,-2),
			video::SColorf(0.6f, 0.6f, 0.6f), 7.0f);

	
		camera = g_smgr->addCameraSceneNode(0, vector3df(0,1,-2), vector3df(0,-0.5,0));
/*
		IAnimatedMesh* mesh = g_smgr->getMesh("pyramid.3ds");
		ISceneNode* node = g_smgr->addAnimatedMeshSceneNode(mesh);
//		node->setRotation(vector3df(180,90,0));
		ApplyMaterialToNode(node);
		node->setMaterialFlag(EMF_WIREFRAME,true);
*/		
		MakeSkybox();
		
		PF->SelectEngine(g_engines[counter]);
		CreatePhysics();
		UpdateGUI(g_engines[counter]);
	}

	virtual void doRender() {
		
		ITimer *t = g_device->getTimer();
		u32 time = t->getTime();

		float ft = time/10000.0f;
		camera->setPosition(vector3df(2*sin(ft),1,2*cos(ft)));
		/*u32 tdiff = time - m_last_time;
		if (tdiff < 10) {
			g_device->sleep(10-tdiff);
		}*/
		g_driver->beginScene(true, true, m_clear_color);
		g_smgr->drawAll();
		g_gui->drawAll();

		matrix4 m;
		m.makeIdentity();
		g_driver->setTransform(video::ETS_WORLD,m);

		SMaterial mat;
		mat.Lighting= true;
		SColor col(255,127,127,127);
		mat.DiffuseColor = col;
		mat.AmbientColor = col;
		mat.EmissiveColor =col;
		mat.SpecularColor =col;
		mat.BackfaceCulling=false;
		mat.GouraudShading=true;
		mat.MaterialType = EMT_SOLID;
		g_driver->setMaterial(mat);
		int i;
		for (int tri=0;tri<4;tri++) {
			//0-1,1-2
		for (int j=1;j<3;j++) {
			i=tri*3+j;
			vector3df a(verts[inds[i-1]*3+0],verts[inds[i-1]*3+1],verts[inds[i-1]*3+2]);
			vector3df b(verts[inds[i  ]*3+0],verts[inds[i  ]*3+1],verts[inds[i  ]*3+2]);
			g_driver->draw3DLine(a,b,col);
		}
			//0-2
			i=tri*3;
			vector3df a(verts[inds[i+2]*3+0],verts[inds[i+2]*3+1],verts[inds[i+2]*3+2]);
			vector3df b(verts[inds[i  ]*3+0],verts[inds[i  ]*3+1],verts[inds[i  ]*3+2]);
			g_driver->draw3DLine(a,b,col);


		}
		
		g_driver->endScene();
		m_last_time = time;
	}

};

class SceneCollision : public ScriptScene {
public:
	SceneCollision() {
		time = -1;
	}
	TestCollision test_col;
	virtual void Init(ScriptScene *last) {
		g_engines = all_engines;
		RemoveEngine("Novodex");
		RemoveEngine("ODE");
		test_col.Init(0.01f);
#ifdef SHORT_VIDEO_VERSION
		test_col.m_render_to_physics_ratio = 5;
#endif
		pds = &test_col;
	}
};


class SceneCollisionHz : public ScriptScene {
public:
	SceneCollisionHz() {
		time = -1;
	}
	TestCollision test_col;
	virtual void Init(ScriptScene *last) {
		g_engines = all_engines;
		RemoveEngine("Newton");
		RemoveEngine("Novodex");
		RemoveEngine("ODE");
		test_col.Init(1/10.0f);
		pds = &test_col;
	}
};