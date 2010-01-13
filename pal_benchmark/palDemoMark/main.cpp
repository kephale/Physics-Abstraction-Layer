#include "main.h"
//#include "pal_i/Newton_pal.h" //include newton for custom timing

#include "collision_scene.h"
#include "drop_scene.h"
#include "stack_scene.h"
#include "stack_ball_scene.h"
#include "restitution_scene.h"
#include "bridge_scene.h"
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
HINSTANCE g_hInst;

IrrlichtDevice* g_device = 0;
video::IVideoDriver* g_driver = 0;
scene::ISceneManager* g_smgr = 0;
gui::IGUIEnvironment* g_gui = 0;
VECTOR<STRING> g_engines;
VECTOR<STRING> all_engines;
VECTOR<irr::scene::ISceneNode *> g_engine_nodes;
VECTOR<SColor> g_colors;
bool g_TakeScreenshot = false;
//
VECTOR<BindObject *> g_vbo;

bool g_SceneFinished = false;

#include "paltest.h"

void RemoveEngine(STRING name) {
	VECTOR<STRING>::iterator it = std::find(g_engines.begin(),g_engines.end(),name);
	if (it!=g_engines.end())
		g_engines.erase(it);
}

	void ApplyMaterialToNode(ISceneNode* node) {
		if (node) {
			node->setMaterialFlag(EMF_LIGHTING, true);
			node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
			node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);

			node->setMaterialType(EMT_SOLID);
			for (unsigned int ii=0;ii<node->getMaterialCount();ii++) {
				SColor emis(255,64,64,64);
				SColor col(255,128,128,128);
				SColor white(255,255,255,255);
				node->getMaterial(ii).DiffuseColor = col;
				node->getMaterial(ii).AmbientColor = col;
				node->getMaterial(ii).EmissiveColor= emis;
				node->getMaterial(ii).SpecularColor= white;
				node->getMaterial(ii).Shininess = 0.0f;
			}
		}
	}

void LoadText() {
	g_engine_nodes.clear();
	for (unsigned int i=0;i<g_engines.size();i++) {
		char buf[256];
		//sprintf(buf,"../media/%s.3ds",g_engines[i].c_str());
		sprintf_s( buf, 256, "../media/%s.3ds", g_engines[i].c_str() );
		IAnimatedMesh* mesh = g_smgr->getMesh(buf);
		IAnimatedMeshSceneNode* node = g_smgr->addAnimatedMeshSceneNode( mesh );
		node->setRotation(core::vector3df(-90,0,0));
		/*
		node->setMaterialFlag(EMF_LIGHTING, true);
		node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
		node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	for (int ii=0;ii<node->getMaterialCount();ii++) {
		SColor color(255,127,127,127);
		node->getMaterial(ii).DiffuseColor = color;
		node->getMaterial(ii).AmbientColor = color;
		node->getMaterial(ii).EmissiveColor= color;
		node->getMaterial(ii).SpecularColor= color;
	}*/

		g_engine_nodes.push_back(node);
	}
}

VECTOR<irr::scene::ISceneNode *> SelectNodes() {
	static VECTOR<irr::scene::ISceneNode *> ret;
	ret.clear();
	for (unsigned int i=0;i<all_engines.size();i++) {
		if (std::find(g_engines.begin(),g_engines.end(),all_engines[i]) != g_engines.end() ) {
			ret.push_back(g_engine_nodes[i]);
		}
	}
	return ret;
}





class Scene1 : public ScriptScene {
public:
	Scene1() {
		time = 1.f;
	}
	Picture splash;
	virtual void Init(ScriptScene *last) {
		splash.LoadCentral("../media/Benchmark DemoBETA.png");
		pds = &splash;
	}
};



#if 1 //windows
#include "resource.h"

Config g_config;

BOOL MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD ret;
	
	switch (uMsg)
	{
	case WM_CREATE:
		return TRUE;
		break;
	case WM_INITDIALOG:
		{
			int i;
		SendDlgItemMessage(hWnd,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"OpenGL");
		SendDlgItemMessage(hWnd,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"Direct3D9");
		SendDlgItemMessage(hWnd,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"Software");
		SendDlgItemMessage(hWnd,IDC_COMBO1,CB_SETCURSEL,0,0);

		g_device = createDevice(EDT_NULL);
		IVideoModeList *ivml = g_device->getVideoModeList();
		int n = ivml->getVideoModeCount();
		char buf[4096];
		for (i=0;i<n;i++) {
		core::dimension2d<u32> dm = ivml->getVideoModeResolution(i);
		int bpp = ivml->getVideoModeDepth(i);
		sprintf_s(buf, 4096, "%d x %d x %d BPP",dm.Width,dm.Height,bpp);
		SendDlgItemMessage(hWnd,IDC_COMBO2,CB_ADDSTRING,0,(LPARAM)buf);
		}
		SendDlgItemMessage(hWnd,IDC_COMBO2,CB_SETCURSEL,n-1,0);

		SendDlgItemMessage(hWnd,IDC_CHECK1,BM_SETCHECK,BST_CHECKED,0);
		SendDlgItemMessage(hWnd,IDC_CHECK2,BM_SETCHECK,BST_CHECKED,0);
		SendDlgItemMessage(hWnd,IDC_CHECK3,BM_SETCHECK,BST_CHECKED,0);
		SendDlgItemMessage(hWnd,IDC_CHECK4,BM_SETCHECK,BST_CHECKED,0);
		SendDlgItemMessage(hWnd,IDC_CHECK5,BM_SETCHECK,BST_CHECKED,0);
#define BUTTON_START IDC_COMBO1+20
		unsigned int ii = 0;
		for (ii=0;ii<g_engines.size();ii++) {
			HWND button = CreateWindow(
           "button", /* this makes a "button" */
           (STRING("Disable ")+g_engines[ii]).c_str(), /* this is the text which will appear in the button */
           WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
           224, /* these four lines are the position and dimensions of the button */
           120+18*ii,
           98,
           16,
           hWnd, /* this is the buttons parent window */
           (HMENU)(BUTTON_START + ii), /* these next two lines pretty much tell windows what to do when the button is pressed */
           g_hInst,
           NULL);

		HFONT hf = (HFONT) SendMessage(GetDlgItem(hWnd,IDC_CHECK1),WM_GETFONT,0,0);
		SendMessage(button, WM_SETFONT, (WPARAM) hf, FALSE);
		}
	}
		return TRUE;
		break;
	case WM_COMMAND:	
		{

			unsigned int i;
			for (i = 0; i < g_engines.size(); i++) {
				if (wParam == i + BUTTON_START) {
					if (SendDlgItemMessage(hWnd,(int)wParam,BM_GETCHECK,0,0) == BST_CHECKED)
						g_config.disable_engines.push_back(g_engines[i]);
					else
					{
						VECTOR<STRING>::iterator it = std::find(g_config.disable_engines.begin(),g_config.disable_engines.end(),g_engines[i]);
						if (it!=g_config.disable_engines.end())
							g_config.disable_engines.erase(it);
						
					}
				}
			} //efi
			switch (LOWORD(wParam)) {
	case IDCANCEL:
		EndDialog(hWnd, 0);	
		exit(0);
		break;
	case IDOK:
		{
			ret = (DWORD)SendDlgItemMessage(hWnd,IDC_COMBO1,CB_GETCURSEL,0,0);
			switch (ret) {
	case 0:
		g_config.driverType = EDT_OPENGL;
		break;
	case 1:
		g_config.driverType = EDT_DIRECT3D9;
		break;
	case 2:
		g_config.driverType = EDT_BURNINGSVIDEO;
		break;
			}
			IVideoModeList *ivml = g_device->getVideoModeList();
			ret = (DWORD)SendDlgItemMessage(hWnd,IDC_COMBO2,CB_GETCURSEL,0,0);
			g_config.res = ivml->getVideoModeResolution(ret);
			g_config.bpp = ivml->getVideoModeDepth(ret);
			if (SendDlgItemMessage(hWnd,IDC_CHECK1,BM_GETCHECK,0,0) == BST_CHECKED)
				g_config.Integrator = true;
			else
				g_config.Integrator = false;

			if (SendDlgItemMessage(hWnd,IDC_CHECK2,BM_GETCHECK,0,0) == BST_CHECKED)
				g_config.Stacking= true;
			else
				g_config.Stacking = false;

			if (SendDlgItemMessage(hWnd,IDC_CHECK3,BM_GETCHECK,0,0) == BST_CHECKED)
				g_config.Constraints= true;
			else
				g_config.Constraints = false;

			if (SendDlgItemMessage(hWnd,IDC_CHECK4,BM_GETCHECK,0,0) == BST_CHECKED)
				g_config.Materials= true;
			else
				g_config.Materials = false;

			if (SendDlgItemMessage(hWnd,IDC_CHECK5,BM_GETCHECK,0,0) == BST_CHECKED)
				g_config.Collisions = true;
			else
				g_config.Collisions = false;

			if (SendDlgItemMessage(hWnd,IDC_CHECKFULL,BM_GETCHECK,0,0) == BST_CHECKED)
				g_config.fullscreen = true;
			else
				g_config.fullscreen = false;

		}
		EndDialog(hWnd, 0);	
			
		break;	
			} //end switch wparam	
		
		}
				break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);	
		exit(0);
		return TRUE;
		break;
			}
			return FALSE;
		}

#endif

int g_ssname = 0;

class MyEventReceiver : public IEventReceiver
{
public:

	bool OnEvent(const SEvent& event)
	{
		// check if user presses the key 'W' or 'D'
		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
		{
				switch (event.KeyInput.Key)
				{
			case irr::KEY_KEY_S: // screenshot
				{
				char buf[256];
				sprintf_s(buf, 256, "screen_shot%.3d.bmp",g_ssname);

				IImage *im = g_driver->createScreenShot();
				g_driver->writeImageToFile(im,buf);
				im->drop();
				g_ssname++;
				}
				return true;
			}
		}

		return false;
	}

private:
	scene::ISceneNode* Terrain;
};

#ifndef NDEBUG
int main(int argc, char*argv[]) {
	g_hInst = 0;
#else
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT) {
	g_hInst = hInst;
#endif
	g_engines.push_back("Bullet");
//	g_engines.push_back("Dynamechs"); //experimental
	g_engines.push_back("Jiggle");
	g_engines.push_back("Newton");
	g_engines.push_back("Novodex");
	g_engines.push_back("ODE");
	g_engines.push_back("Tokamak");
//	g_engines.push_back("OpenTissue"); //experimental
	g_engines.push_back("TrueAxis");
/*
	g_device = createDevice(EDT_NULL);
	IVideoModeList *ivml = g_device->getVideoModeList();
	int n = ivml->getVideoModeCount();
	for (int i=0;i<n;i++) {
		core::dimension2d<s32> dm = ivml->getVideoModeResolution(i);
		int bpp = ivml->getVideoModeDepth(i);
		printf("%.4d x %.4d x %.2d\n",dm.Width,dm.Height,bpp);
	}
*/
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);



	for (unsigned int i=0;i<g_config.disable_engines.size();i++) {
		g_engines.erase(std::find(g_engines.begin(),g_engines.end(),g_config.disable_engines[i]));
	}

	all_engines = g_engines;

	g_colors.push_back(SColor(255,65,111,166));
	g_colors.push_back(SColor(255,168,66,63));
	g_colors.push_back(SColor(255,134,164,74));
	g_colors.push_back(SColor(255,110,84,141));
	g_colors.push_back(SColor(255,218,129,55));
	g_colors.push_back(SColor(255,142,165,203));
	g_colors.push_back(SColor(255,61,150,174));
	g_colors.push_back(SColor(255,206,142,141));
	g_colors.push_back(SColor(255,181,202,146));
	g_colors.push_back(SColor(255,165,151,185));
	g_colors.push_back(SColor(255,140,192,210));

#if 1
	//load physics
	PF->LoadPALfromDLL(); 

	//let user select physics driver
	//PF->SelectEngine("Bullet");
#endif

	// let user select driver type
//	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
//	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
		// create device
	//g_device = createDevice(driverType, core::dimension2d<s32>(800, 600));
//	g_device = createDevice(driverType, core::dimension2d<s32>(640, 480));

	//g_device = createDevice(g_config.driverType, g_config.res, g_config.bpp, g_config.fullscreen);
	#ifdef _IRR_OSX_PLATFORM_
		g_device = createDevice(	video::EDT_OPENGL, (const core::dimension2d<u32>&)dimension2d<s32>(640, 480), 16,
									false, false, false, 0);
	#else
		g_device = createDevice(	g_config.driverType, (const core::dimension2d<u32>&)dimension2d<s32>(640, 480), 16,
									false, false, false, 0);
	#endif

	if ( !g_device ) return 1; // could not create selected driver.

	g_driver = g_device->getVideoDriver();
	g_smgr = g_device->getSceneManager();
	g_gui = g_device->getGUIEnvironment();
	g_driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	g_device->setWindowCaption(L"Physics Abstraction Layer Demo Benchmark");
//	g_gui->addStaticText(L"Hello World! This is the Irrlicht Software renderer!",rect<int>(10,10,260,22), true);

	// create event receiver
	MyEventReceiver receiver;
	g_device->setEventReceiver(&receiver);

	g_smgr->clear();

	VECTOR<DemoScene *> scenes;
	VECTOR<float> times;
	VECTOR<ScriptScene *> vss;

	STRING specs="Tested on an Intel Pentium D 3.00Ghz. August 2007 PAL release.";
	STRING label;
	float lslidelength=5;
	float sslidelength=2;
#ifdef SHORT_VIDEO_VERSION
	lslidelength=1.2f;
	sslidelength=0.8f;
#endif
	vss.push_back(new SceneSlide("../media/Benchmark DemoBETA.png",2,"v1.1 - 12/11/07"));

	if (g_config.Integrator) {
	vss.push_back(new SceneSlide("../media/drop.png",sslidelength));
	vss.push_back(new SceneDrop);
	label="Figure shows positional error from cumulative numerical integrators relative to the ideal case normalized to the Symplectic Euler integrator error. A sphere is constructed at the origin and allowed to drop from gravitational forces. Gravity is set to -9.8m/s, and the time step is set to 0.01. The positions presented by the physics engines are then recorded and compared to ideal cases for various integrators. ";
	vss.push_back(new SceneSlide("../media/integratorerror.png",lslidelength,label+ " " + specs));
	}

	if (g_config.Stacking) {	
	vss.push_back(new SceneSlide("../media/boxstack.png",sslidelength));
	vss.push_back(new SceneStack(false));
	label="Figure shows computational effort of stacked objects. In this test a set of 1x1x1m3 , 1kg cubes are dropped in a stack on top of one another, with a distance of 0.1m between them.   Each cube is displaced by a random amount of maximal 0.1m in both directions parallel to the ground. Automatic body sleeping is disabled.";
	vss.push_back(new SceneSlide("../media/stackeffort.png",lslidelength,label+ " " + specs));
#ifndef SHORT_VIDEO_VERSION
	vss.push_back(new SceneSlide("../media/spherestack.png",sslidelength));
	vss.push_back(new SceneStack(true));
#endif
	vss.push_back(new SceneSlide("../media/pyramidstack.png",sslidelength));
	vss.push_back(new SceneCannonBallWall);
	}

	if (g_config.Constraints) {
	vss.push_back(new SceneSlide("../media/constraint.png",sslidelength));
#ifndef SHORT_VIDEO_VERSION
	vss.push_back(new SceneBridge(3));
	vss.push_back(new SceneBridgeGraph);

	vss.push_back(new SceneBridge(6));
	vss.push_back(new SceneBridgeGraph);

	vss.push_back(new SceneBridge(12));
	vss.push_back(new SceneBridgeGraph);
#else
	vss.push_back(new SceneBridge(12));
#endif
	label="Figure illustrates the constraint error measured from the accumulated difference in the distance between two links minus relative to the initial case. To test the constraints stability a chain of spherical links connecting a number of spheres is simulated.  The chain is attached to two boxes with a 1x1m² base and mass 400 times the number of constraints. Each sphere in the chain had a radius of 0.2m, and a mass of 0.1kg.";
	vss.push_back(new SceneSlide("../media/constrainterror.png",lslidelength,label+ " " + specs));
	label="Figure illustrates the time required to solve the constraints"; 
	vss.push_back(new SceneSlide("../media/constrainttime.png",lslidelength,label+ " " + specs));
	}

	if (g_config.Materials) {
	vss.push_back(new SceneSlide("../media/restitution.png",sslidelength));
	vss.push_back(new SceneMaterials);
#ifndef SHORT_VIDEO_VERSION
	vss.push_back(new SceneMatGraph);
#endif
	vss.push_back(new SceneMatBar);

	label="Figure shows angle of the plane at which the box began movement versus the static friction coefficient. To test the static friction a 5x1x5m box was placed on an inclined plane. A static friction coefficient was assigned to the materials of the box and the plane, and the angle of the plane was then incrementally increased to test the angle at which the box would first start sliding. This process was repeated for the range of static coefficients from 0.1 to 0.7, increasing by 0.1. The angle of the plane was tested in the range of 0 to 0.7 in increments of 0.05 radians.";
	vss.push_back(new SceneSlide("../media/staticfriction.png",lslidelength,label+ " " + specs));
	}

	if (g_config.Collisions) {
	vss.push_back(new SceneSlide("../media/collision100hz.png"));
	vss.push_back(new SceneCollision);
	
	vss.push_back(new SceneSlide("../media/collision10hz.png"));
	vss.push_back(new SceneCollisionHz);
	}
	vss.push_back(new SceneSlide("../media/palurl.png"));
	
	//MakeGraphPlot(points);
	ITimer *t = g_device->getTimer();
	t->setTime(0);
	vss[0]->Init(0);
	int cur_scene = 0;
	char buf[256];
	int frame = 0;
	u32 mslast_time=0;
	while(g_device->run() && ((size_t)cur_scene < vss.size()))
	{
		float time = t->getTime() / 1000.0f;
		u32 mstime;		
		u32 mstdiff;
		mstime = t->getTime();
		mstdiff = mstime - mslast_time;
#if 0
		//printf("time WAS : %.8d (%.4d)\n",mstime,mstdiff);
		while (mstdiff<40) {
			g_device->sleep(1);	
			//Sleep(10);
			mstime = t->getTime();
			t->tick();
			mstdiff = mstime - mslast_time;
			//printf("%d,%d\n",mstime,mstdiff );
		}
		printf("time is : %.8d (%.4d)\n",mstime,mstdiff);
#endif
		//25 fps = 1/25Hz = 
		if (vss[cur_scene]->time > 0) {
			if (time > vss[cur_scene]->time ) {
				t->setTime(0);
				cur_scene++;
				if ((size_t)cur_scene < vss.size())
					vss[cur_scene]->Init(vss[cur_scene-1]);
			}
		} else {
			if (g_SceneFinished) {
				t->setTime(0);
				cur_scene++;
				if ((size_t)cur_scene < vss.size())
					vss[cur_scene]->Init(vss[cur_scene-1]);
			}
		}
		mslast_time = mstime;
		if ((size_t)cur_scene < vss.size())
			vss[cur_scene]->Run();

		if (g_TakeScreenshot) {
			sprintf_s(buf, 256, "frame%.6d.png",frame);
			IImage *im = g_driver->createScreenShot();
			g_driver->writeImageToFile(im,buf);
			im->drop();
		}

//#define MAKEAVI
#if 0
		//if (frame>862) {
		sprintf(buf,"frame%.6d.png",frame);

		IImage *im = g_driver->createScreenShot();
		g_driver->writeImageToFile(im,buf);
		im->drop();
		//}
#endif
		frame++;

	}


	g_device->drop();

	return 0;
}