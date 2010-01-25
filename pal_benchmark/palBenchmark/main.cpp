#include "main.h"
#include <stdio.h>
#include "paltest.h"
#include "collision_scene.h"
#include "drop_scene.h"
#include "stack_scene.h"
#include "stack_ball_scene.h"
#include "restitution_scene.h"
#include "bridge_scene.h"

#include "strings.h"

#include "dialog.h"
GUIFeedback *dialog = new GUIFeedback();

//#include "pal_i/Newton_pal.h" //include newton for custom timing

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

#ifdef _WIN32
	HINSTANCE g_hInst;
#endif

#ifdef __APPLE__
	#include "CoreFoundation/CoreFoundation.h"	// Needed for settings the current relative path later on
#endif

IrrlichtDevice* g_device = 0;
video::IVideoDriver* g_driver = 0;
scene::ISceneManager* g_smgr = 0;
gui::IGUIEnvironment* g_gui = 0;
std::vector<std::string> g_engines;
std::vector<std::string> all_engines;
std::vector<irr::scene::ISceneNode *> g_engine_nodes;
std::vector<SColor> g_colors;
bool g_TakeScreenshot = false;
std::vector<BindObject *> g_vbo;
bool g_SceneFinished = false;

void RemoveEngine(std::string name) {
	std::vector<std::string>::iterator it = std::find(g_engines.begin(),g_engines.end(),name);
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
		sprintf( buf, "/media/%s.3ds", g_engines[i].c_str() );
#ifdef _WIN32
		// Append the .. as a prefix if Windows, not needed for Mac OS X
		sprintf( buf, "..%s", buf );
#endif
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

std::vector<irr::scene::ISceneNode *> SelectNodes() {
	static std::vector<irr::scene::ISceneNode *> ret;
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
	PictureImage splash;
	virtual void Init(ScriptScene *last) {
		splash.LoadCentral("../media/Benchmark DemoBETA.png");
		pds = &splash;
	}
};

Config g_config;

#ifdef _WIN32
#include "resource.h"
BOOL MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	printf("Pal Benchmark (Windows)\n");
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
		   (std::string("Disable ")+g_engines[ii]).c_str(), /* this is the text which will appear in the button */
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
						std::vector<std::string>::iterator it = std::find(g_config.disable_engines.begin(),g_config.disable_engines.end(),g_engines[i]);
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
					sprintf(buf, "screen_shot%.3d.bmp",g_ssname);

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

#include <cstdlib>

// RELEASE MODE
//#ifndef NDEBUG	// NDEBUG also Disables assertions
#ifdef __APPLE__
int main(int argc, char *argv[]) {
	//g_hInst = 0;
	// ----------------------------------------------------------------------------
	// Mac OS X only
	// This makes relative paths work in C++ in Xcode!
	// Don't forget to do:  #include "CoreFoundation/CoreFoundation.h"
	// ----------------------------------------------------------------------------
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
		std::cout << "Error setting working directory" << std::endl;
	} else {
		CFRelease(resourcesURL);
		chdir(path);
		std::cout << "(Mac OS X) Current working directory: " << std::endl << path << std::endl;
	}
	// ----------------------------------------------------------------------------
	
#elif _WIN32
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT) {
	g_hInst = hInst;

	g_engines.push_back("Bullet");
//	g_engines.push_back("Dynamechs"); //experimental
	g_engines.push_back("Jiggle");
	g_engines.push_back("Newton");
	g_engines.push_back("Novodex");
	g_engines.push_back("ODE");
	g_engines.push_back("Tokamak");
//	g_engines.push_back("OpenTissue"); //experimental
	g_engines.push_back("TrueAxis");
#endif

	//-----------------------------------------------------------------------------
	// Application's Main entry point is here >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//-----------------------------------------------------------------------------

#ifndef _WIN32
	// BW: Mac and Linux only
	// dialog->ErrorDialogue("An error occurred");	// TODO: Replace all the MessageBox alerts with this
	
	//-----------------------------------------------------------------------------
	// Which Tests to run?
	//-----------------------------------------------------------------------------
	g_config.Integrator = true;
	g_config.Stacking = true;
	g_config.Constraints = true;
	g_config.Materials = true;
	g_config.Collisions = true;
	//-----------------------------------------------------------------------------
	
	const int engineCount = 7;
	bool exitSelectionMenu = false;
	std::string input = "";
	
	std::vector<std::string> enginesToEnable;
	
	while (exitSelectionMenu == false) {
		
		std::cout << "-----------------------------PAL Benchmark----------------------------------" << std::endl;
		std::cout << "Type numbers to toggle physics engine (ie: type '13' for Bullet and Newton):" << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << "Bullet   (1)" << std::endl;
		std::cout << "Jiggle   (2)" << std::endl;
		std::cout << "Newton   (3)" << std::endl;
		std::cout << "Novodex  (4)" << std::endl;
		std::cout << "ODE      (5)" << std::endl;
		std::cout << "Tokamak  (6)" << std::endl;
		std::cout << "TrueAxis (7)" << std::endl;

		std::cout << "$ ";
		std::cin >> input;
		
		std::cout << "You have selected: ";
		int currentSelection = 0;
		bool duplicateThisLoop = false;
		bool userEnteredInvalidCharacter = false;
		int enteredCharacters = input.length();
		std::string currentEngineStr = "";
		
		for (int i = 0; (i < engineCount) && (i < input.length()); i++) {
			bool printEngineName = true;
			const std::string currChar = input.substr(i, 1);
			currentSelection = atoi( currChar.c_str() );
		
			switch ( currentSelection ) {
				case 1:
					currentEngineStr = "Bullet"; break;
				case 2:
					currentEngineStr = "Jiggle"; break;
				case 3:
					currentEngineStr = "Newton"; break;
				case 4:
					currentEngineStr = "Novodex"; break;
				case 5:
					currentEngineStr = "ODE"; break;
				case 6:
					currentEngineStr = "Tokamak"; break;
				case 7:
					currentEngineStr = "TrueAxis"; break;					
				default:
					userEnteredInvalidCharacter = true;
					continue;
					break;
			}
			userEnteredInvalidCharacter = false;
			duplicateThisLoop = false;	// Tells us that the user tried to add the same option more than once, so dont print a comma or 'and'
			
			// Search the vector to make sure we haven't already added this engine
			// Enable engine
			std::vector<std::string>::iterator it = std::find(g_engines.begin(),g_engines.end(),currentEngineStr);
			if (it==g_engines.end()) {	// If not found, then add the engine
				
				g_engines.push_back(currentEngineStr);
				printEngineName = true;
				
			} else { // String was already found in the vector
				
				duplicateThisLoop = true;
				enteredCharacters--;
				printEngineName = false;
			}
			
			// Print separators
			if ( userEnteredInvalidCharacter == false )
			if ( i > 0 && i < enteredCharacters && duplicateThisLoop == false) {
				if ( i == enteredCharacters-1 ) {
					std::cout << " and ";
				} else if ( i >= 1 ) {
					std::cout << ", ";
				}
			}
			
			if (printEngineName == true) { std::cout << currentEngineStr; }	// print engine name (ie: Bullet)
			if ( i == engineCount-1 ) { std::cout << std::endl;	}			// print the ending '\n' character
		}
		
		std::cout << std::endl << "Confirm choice? (y/n or q to quit): ";
		std::cin >> input;
		std::cout << std::endl;
		
		if ( input == "y" ) {
			exitSelectionMenu = true;

		} else if ( input == "n" ) {
			exitSelectionMenu = false;
			g_engines.clear();			
		} else if ( input == "q") {
			exit(0);
		} 
}
	
	// Which Engines to use?
/*	std::cout << "PAL Benchmark will run using these engines:" << std::endl;
	for (int i = 0; i < g_engines[i].size()-1; i++) {
		std::cout << g_engines[i] << std::endl;
	}	
*/
	/*
	// Original Default Engines 
	g_engines.push_back("Bullet");
	//	g_engines.push_back("Dynamechs"); //experimental
	g_engines.push_back("Jiggle");
	g_engines.push_back("Newton");
	g_engines.push_back("Novodex");
	g_engines.push_back("ODE");
	g_engines.push_back("Tokamak");
	//	g_engines.push_back("OpenTissue"); //experimental
	g_engines.push_back("TrueAxis");
	*/
	
#endif
	//------------------------------------------------------------------------------------------------------------------------------------------------------

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

#ifdef _WIN32
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);
#endif
	
	for (unsigned int i=0;i<g_config.disable_engines.size();i++) {
		g_engines.erase(std::find( g_engines.begin(), g_engines.end(), g_config.disable_engines[i]));
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
	
	// Create and Initialise Irrlicht Engine and display device (OpenGL/Software/DirectX)
	#ifdef _IRR_OSX_PLATFORM_	// Mac
		g_device = createDevice(	video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
									false, false, false, 0);
	#else						// Windows
		g_device = createDevice(	g_config.driverType, (const core::dimension2d<u32>&)dimension2d<s32>(640, 480), 16,
									false, false, false, 0);
	#endif

	if ( !g_device )  {
		dialog->DisplayErrorDialogue("Could not create selected Irrlicht driver");
		return 1; // could not create selected driver.
	}

	g_driver = g_device->getVideoDriver();
	g_smgr = g_device->getSceneManager();
	g_gui = g_device->getGUIEnvironment();
	g_driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	g_device->setWindowCaption(L"Physics Abstraction Layer Demo Benchmark");
	//g_gui->addStaticText(L"Hello World! This is the Irrlicht Software renderer!",rect<int>(10,10,260,22), true);

	// create event receiver
	MyEventReceiver receiver;
	g_device->setEventReceiver(&receiver);
	g_smgr->clear();

	std::vector<DemoScene*>		scenes;
	std::vector<float>			times;
	std::vector<ScriptScene*>	vss;

	std::string specs = "Tested on an Intel Pentium D 3.00Ghz. August 2007 PAL release.";
	std::string label;
	
	float lslidelength=5;
	float sslidelength=2;
#ifdef SHORT_VIDEO_VERSION
	lslidelength=1.2f;
	sslidelength=0.8f;
#endif

	vss.push_back(new SceneSlide(splashScreen1,2,"v1.1 - 12/11/07"));
	
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	
	if (g_config.Integrator == true) {
		vss.push_back(new SceneSlide(dropImage,sslidelength));
		vss.push_back(new SceneDrop);
		label="Figure shows positional error from cumulative numerical integrators relative to the ideal case normalized to the Symplectic Euler integrator error. A sphere is constructed at the origin and allowed to drop from gravitational forces. Gravity is set to -9.8m/s, and the time step is set to 0.01. The positions presented by the physics engines are then recorded and compared to ideal cases for various integrators. ";
		vss.push_back(new SceneSlide(integratorErrorImage,lslidelength,label+ " " + specs));
	}

	if (g_config.Stacking == true) {	
		vss.push_back(new SceneSlide( boxStackImage ,sslidelength));
		vss.push_back(new SceneStack(false));
		label="Figure shows computational effort of stacked objects. In this test a set of 1x1x1m3 , 1kg cubes are dropped in a stack on top of one another, with a distance of 0.1m between them.   Each cube is displaced by a random amount of maximal 0.1m in both directions parallel to the ground. Automatic body sleeping is disabled.";
		vss.push_back(new SceneSlide( stackEffortImage,lslidelength,label+ " " + specs));
		#ifndef SHORT_VIDEO_VERSION
		{
			vss.push_back(new SceneSlide( sphereStackImage ,sslidelength));
			vss.push_back(new SceneStack(true));
		}
		#endif
		vss.push_back(new SceneSlide( pyramidStackImage ,sslidelength));
		vss.push_back(new SceneCannonBallWall);
	}

	if (g_config.Constraints == true) {
		vss.push_back(new SceneSlide( constraintImage ,sslidelength));
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
		label = "Figure illustrates the constraint error measured from the accumulated difference in the distance between two links minus relative to the initial case. To test the constraints stability a chain of spherical links connecting a number of spheres is simulated. The chain is attached to two boxes with a 1x1m² base and mass 400 times the number of constraints. Each sphere in the chain had a radius of 0.2m, and a mass of 0.1kg.";
		
		vss.push_back(new SceneSlide( constraintErrorImage ,lslidelength,label+ " " + specs));
		label="Figure illustrates the time required to solve the constraints"; 
		vss.push_back(new SceneSlide( constraintTimeImage,lslidelength,label+ " " + specs));
	}

	if (g_config.Materials == true) {
		vss.push_back(new SceneSlide( restitutionImage,sslidelength));
		vss.push_back(new SceneMaterials);
	#ifndef SHORT_VIDEO_VERSION
		vss.push_back(new SceneMatGraph);
	#endif
		vss.push_back(new SceneMatBar);

		label="Figure shows angle of the plane at which the box began movement versus the static friction coefficient. To test the static friction a 5x1x5m box was placed on an inclined plane. A static friction coefficient was assigned to the materials of the box and the plane, and the angle of the plane was then incrementally increased to test the angle at which the box would first start sliding. This process was repeated for the range of static coefficients from 0.1 to 0.7, increasing by 0.1. The angle of the plane was tested in the range of 0 to 0.7 in increments of 0.05 radians.";
		vss.push_back(new SceneSlide( staticFrictionImage ,lslidelength,label+ " " + specs));
	}

	if (g_config.Collisions == true) {
		vss.push_back(new SceneSlide( collision100hzImage ));
		vss.push_back(new SceneCollision);
		
		vss.push_back(new SceneSlide( collision10hzImage ));
		vss.push_back(new SceneCollisionHz);
	}
	vss.push_back(new SceneSlide( palUrlImage ));
	
	//--------------------------------------------------------------------------------------------------------------
	// Irrlicht Render loop
	//--------------------------------------------------------------------------------------------------------------
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
			//sleep(10);
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
			sprintf(buf, "frame%.6d.png",frame);
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
	//--------------------------------------------------------------------------------------------------------------
	// End Irrlicht render loop
	//--------------------------------------------------------------------------------------------------------------
	return 0;
}