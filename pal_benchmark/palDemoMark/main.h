#ifndef MAIN_H
#define MAIN_H

#ifdef WIN32
	#include <windows.h>
	#pragma comment(lib, "libpal.lib")
	#pragma comment(lib, "Irrlicht.lib")
#endif

//#define QUICK
//#define SHORT_VIDEO_VERSION

#define STRING std::string
#define VECTOR std::vector

#define CUBEFILE "../media/cube.3ds"

//#include "pal/palXMLFactory.h"
#include "pal/palVehicle.h"

#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


extern IrrlichtDevice* g_device;
extern video::IVideoDriver* g_driver;
extern scene::ISceneManager* g_smgr;
extern gui::IGUIEnvironment* g_gui;
extern VECTOR<STRING> g_engines;
extern VECTOR<irr::scene::ISceneNode *> g_engine_nodes;
extern VECTOR<SColor> g_colors;
extern bool g_SceneFinished;

extern void ApplyMaterialToNode(ISceneNode* node); 

class BindObject {
public:
	irr::scene::ISceneNode *node;
	palBody *pb;
	//update irrlicht pos from physics
	void Update() {
		if (!pb) return;
		if (!node) return;
		palMatrix4x4 matrix = pb->GetLocationMatrix();
		core::matrix4 mat;
//		memcpy(mat.M, matrix, sizeof(Real)*4*4);
		memcpy(&mat[0], matrix._mat, sizeof(f32)*4*4);
		node->setPosition(mat.getTranslation());
		node->setRotation(mat.getRotationDegrees());

	}
};

class DemoScene {
public:
	DemoScene() {
		
	}
	virtual void Update() {};
	virtual void Render() {
		g_driver->beginScene(true, true, m_clear_color);
		g_smgr->drawAll();
		g_gui->drawAll();
		g_driver->endScene();
	}
	SColor m_clear_color;
};

class Config {
public:
	video::E_DRIVER_TYPE driverType;
	core::dimension2d<s32> res;
	int bpp;
	bool fullscreen;
	bool Integrator;
	bool Stacking;
	bool Constraints;
	bool Materials;
	bool Collisions;
	VECTOR<STRING> disable_engines;
};

extern VECTOR<BindObject *> g_vbo;

extern VECTOR<STRING> all_engines;

extern bool g_TakeScreenshot;

void RemoveEngine(STRING name);
void ApplyMaterialToNode(ISceneNode* node);
void LoadText();
#endif