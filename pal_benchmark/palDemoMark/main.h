#ifndef MAIN_H
#define MAIN_H

// ---------------------------------------------------
//	PAL Benchmark
// ---------------------------------------------------
//	Original verson by Adrian Boeing
//	Ported to Mac OS X by Brock Woolf. 14/01/2010
//
//	This benchmark requires Irrlicht 1.6 and PAL
// ---------------------------------------------------

#ifdef _WIN32
	#include <windows.h>
	#pragma comment(lib, "libpal.lib")
	#pragma comment(lib, "Irrlicht.lib")
#endif

#ifdef __APPLE__
	typedef float Float;
	#include <palMath.h>
	#include <palBodies.h>
#endif

//#define QUICK
//#define SHORT_VIDEO_VERSION

#include "dialog.h"
#include "pal/palVehicle.h"
#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern IrrlichtDevice*							g_device;
extern video::IVideoDriver*						g_driver;
extern scene::ISceneManager*					g_smgr;
extern gui::IGUIEnvironment*					g_gui;
extern std::vector<std::string>					g_engines;
extern std::vector<irr::scene::ISceneNode *>	g_engine_nodes;
extern std::vector<SColor>						g_colors;
extern bool										g_SceneFinished;
extern void										ApplyMaterialToNode(ISceneNode* node); 

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
	std::vector<std::string> disable_engines;
};

extern std::vector<BindObject *> g_vbo;
extern std::vector<std::string> all_engines;
extern bool g_TakeScreenshot;

void RemoveEngine(std::string name);
void ApplyMaterialToNode(ISceneNode* node);
void LoadText();

#endif