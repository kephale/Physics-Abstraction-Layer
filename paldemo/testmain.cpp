#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info

//#define DYNAMECHS
//#define NEWTON

#include "../pal/pal.h"
#include "../sdlgl/sdlgl.h"

#ifdef DYNAMECHS
#include "dynamechs_pal.h"
#endif
#ifdef NEWTON
#include "../pal_i/newton_pal.h"
#endif

#ifdef MEMDEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

bool g_quit = false;


#include "../pal/palFactory.h"
//#include "../pal/palXMLFactory.h"

/*
void PrintMatrix(palMatrix4x4 *pm) {
	printf("Location: %f %f %f\n",pm->_41,pm->_42,pm->_43);
}
*/

#include "test.h"
#include "test1.h"

#include "resource.h"

Test *t;
palPhysics *pp = 0;

std::vector<Test *> g_AllTests;

void PopulateTests(HWND hWnd) {
	MAP <STRING, myFactoryObject*>::iterator it;
/*	myFactory mf;
	mf.SetActiveGroup("palTests");
	mf.RebuildRegistry();
*/
//	PF->SetActiveGroup("palTests");
	PF->SelectEngine("palTests");

	it = PF->mRegistry.begin();
	while (it!=PF->mRegistry.end()) {
		printf("reg contains:%s\n",(*it).first.c_str());
		Test *t = dynamic_cast<Test *> ( (*it).second );
		if (t!=NULL) {
			g_AllTests.push_back(t);
			//SendDlgItemMessage(hWnd,IDC_TEST_LIST,LB_ADDSTRING,0,(LPARAM)(*it).first.c_str());			
			SendDlgItemMessage(hWnd,IDC_TEST_LIST,LB_ADDSTRING,0,(LPARAM)t->GetName().c_str());			
		}
		
		it++;
	}
	SendDlgItemMessage(hWnd,IDC_TEST_LIST,LB_SETCURSEL,0,0);
}

BOOL MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD ret;
	int i;
	
	switch (uMsg)
	{
	case WM_CREATE:
		return TRUE;
		break;
	case WM_INITDIALOG:
		//populate the engine list
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"Box2D");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"Bullet");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"IBDS");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"Jiggle");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"Newton");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"Novodex");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"ODE");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"OpenTissue");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"SPE");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"Tokamak");
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_ADDSTRING,0,(LPARAM)"TrueAxis");
				
		SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_SETCURSEL,0,0);

		SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_ADDSTRING,0,(LPARAM)"No Terrain");
		SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_ADDSTRING,0,(LPARAM)"Flat plane");
		SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_ADDSTRING,0,(LPARAM)"Heightmapped");
		SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_ADDSTRING,0,(LPARAM)"Pool mesh");
		SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_ADDSTRING,0,(LPARAM)"Stair mesh");
		SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_ADDSTRING,0,(LPARAM)"Orientated plane");
		SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_ADDSTRING,0,(LPARAM)"Flat plane and grid");
		SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_SETCURSEL,1,0);
	
		PopulateTests(hWnd);
		SetDlgItemText(hWnd,IDC_INSTRUCTIONS,"Please select a test");
		
	
	/*	for (i=0;i<Test::g_Tests.size();i++) {
			SendDlgItemMessage(hWnd,IDC_TEST_LIST,LB_ADDSTRING,0,(LPARAM)Test::g_Tests[i]->GetName().c_str());
		}*/
		return TRUE;
		break;
	case WM_COMMAND:	
		switch (LOWORD(wParam)) {
		case IDOK:
			i = SendDlgItemMessage(hWnd,IDC_TEST_LIST,LB_GETCURSEL,0,0);
			if (i<0) {
				MessageBox(hWnd,"Please select a test","Select test",MB_OK);
			}
			t = g_AllTests[i];

			ret=SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_GETCURSEL,0,0);
			char engine[256];
			SendDlgItemMessage(hWnd,IDC_ENGINE_LIST,LB_GETTEXT,ret,(LPARAM)engine);
			PF->SelectEngine(engine);

			if (SendDlgItemMessage(hWnd,IDC_RADIO_P2,BM_GETSTATE ,0,0)
				== BST_CHECKED)
					SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);

			pp = PF->CreatePhysics();
			if (pp == NULL) {
				MessageBox(hWnd,"Failed to start physics!","Exiting...",MB_OK);
				EndDialog(hWnd, 0);	
				exit(-1);
			}
			pp->Init(0,-9.8f,0);

			i=SendDlgItemMessage(hWnd,IDC_TERRAIN_LIST,LB_GETCURSEL,0,0);
			t->Init(i);
			printf("TERRAIN TYPE: %d\n",i);

			EndDialog(hWnd, 0);	
			break;	
		case IDC_TEST_LIST: 
			switch (HIWORD(wParam)) 
			{ 
			case LBN_SELCHANGE: 
				i = SendDlgItemMessage(hWnd,IDC_TEST_LIST,LB_GETCURSEL,0,0);
				if (i>=0) {
					SetDlgItemText(hWnd,IDC_INSTRUCTIONS,g_AllTests[i]->GetDescription().c_str());
				} else {
					SetDlgItemText(hWnd,IDC_INSTRUCTIONS,"Please select a test");
				}
				break;
			}
			break;
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


int main(int argc, char *argv[]) {
	PF->LoadPALfromDLL(); 
#ifdef INTERNAL_DEBUG
	PF->DisplayAllObjects();
#endif

	int i;
	
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);

#ifdef MEMDEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	_CrtSetDbgFlag(flag); // Set flag to the new value
#endif

	SDL_Event	E;
	g_eng = new SDLGLEngine;
	g_eng->Init(640,480);	

	bool mouse_down=false;
	float angle = M_PIf*0.4f;
	float distance = 12;
	float height = 9;
	bool wireframe = false;
	bool transparent = false;

	while (!g_quit) {
		if(SDL_PollEvent(&E)) {
			switch(E.type) {
				case SDL_QUIT:
					g_quit=true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					mouse_down=true;
					break;
				case SDL_MOUSEBUTTONUP:
					mouse_down=false;
					break;
				case SDL_MOUSEMOTION:
					if (mouse_down)
						angle+=E.motion.xrel*0.01f;
					break;
				case SDL_KEYDOWN:
					switch (E.key.keysym.sym) {
					case SDLK_KP_PLUS:
						distance -= 0.5f;
						break;
					case SDLK_KP_MINUS:
						distance += 0.5f;
						break;
					case SDLK_KP_DIVIDE:
						height -= 0.5f;
						break;
					case SDLK_KP_MULTIPLY:
						height += 0.5f;
						break;
					case SDLK_KP0:
						wireframe=!wireframe;
						break;
					case SDLK_KP1:
						transparent=!transparent;
						break;
					}
				}
				t->Input(E);
		} else {
			t->Update();
			//update
			if (pp)
				pp->Update(0.01f);

			g_eng->Clear();

			g_eng->Wireframe(wireframe);
			g_eng->Transparent(transparent);

			g_eng->SetProjMatrix(M_PIf/4.0f,1.0f,0.2f,100.0f);
			g_eng->SetViewMatrix(distance*cosf(angle),height,distance*sinf(angle),0,0,0,0,1,0);
	
			for (i=0;i<g_Graphics.size();i++) {
				g_Graphics[i]->Display();
			}

			t->AdditionalRender();
			
			if (terrain_graphics)
				terrain_graphics->Render();
			g_eng->Flip();
		}
	} //end while
	
	delete terrain_graphics;
	delete g_eng;

	PF->Cleanup();
	return 0;
};