//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*
	Abstract:
		
	Author: 
		Adrian Boeing
	Revision History:
		Version 1.2.2: 05/07/08 Havok and VS2008 support
		Version 1.2.1: 13/01/08 Box2D support
		Version 1.2  : 12/01/08 Save config data
		Version 1.1.1: 12/01/08 Fix '/' formatting.
		Version 1.1  : 04/01/08 Added on/off flags
		Version 1.0  : 28/12/07 Initial 
	TODO:
		- Save config
*/

#include <windows.h>
#include <shlobj.h>
#include "resource.h"
#include <stdio.h>

#include <tchar.h>

HINSTANCE g_hInst;

#define N 12
char *enames[]= {"Box2D","Bullet","Havok","IBDS", "Jiggle","Newton","Novodex","ODE", "OpenTissue", "SPE","TrueAxis","Tokamak"};

char *edefaults[]={"Box2D/","bullet/","hk550/","IBDS/","jiglib/","NewtonSDK/sdk/","PhysX/","ODE/","opentissue/","SPE_SDK/","TrueAxisPhysicsSDKNonCommercial/","tokamak/"};

char *tolowers(const char *s) {
	char buf[256];
	memset(buf,0,sizeof(char)*256);
	for (int i=0;i<strlen(s);i++) {
		buf[i]=tolower(s[i]);
	}
	return buf;
}

TCHAR g_szFilename[MAX_PATH];

BOOL GetOpenDirName(TCHAR *dirname, int len, HWND hWnd) {
	BROWSEINFO bi = {0};
	LPITEMIDLIST pidl;
	bi.hwndOwner = hWnd;
	pidl = SHBrowseForFolder( &bi );
	if ( pidl != 0 ) {
		if ( SHGetPathFromIDList ( pidl, dirname ) ) {
			return TRUE;
		}
	}
	return FALSE;
}

WNDPROC wndprocEB;

void CleanSlash(char *sz) {
	int i;
	for (i=0;i<strlen(sz);i++) {
		if (sz[i]=='\\')
			sz[i]='/';
	}
	int l = strlen(sz);
	if (sz[l-1]!='/') {
		sz[l]='/';
		sz[l+1]=0;
	}

	for (i=1;i<strlen(sz);i++) {
		if ((sz[i]==sz[i-1])&&(sz[i]=='/')) {
			memcpy(sz+i-1,sz+i,1+strlen(sz)-i);
			break;
		}

	}
}

void SetDirText(HWND hWnd, char *sz) {
	CleanSlash(sz);
	SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)sz);
}

LRESULT CALLBACK WindowProcEditBox(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch ( uMsg )
  {
    case WM_DROPFILES:
	  	HDROP hDrop = (HDROP) wParam;
			DragQueryFile(hDrop, 0, g_szFilename, MAX_PATH);
			DragFinish(hDrop);
			//SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)g_szFilename);
			SetDirText(hWnd,g_szFilename);
      break;
  }
  return CallWindowProc(wndprocEB,  hWnd, uMsg, wParam, lParam);
}

BOOL MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	char buf[MAX_PATH];
	BOOL bret;
	int i;
	
	switch (uMsg)
	{
	case WM_CREATE:
		return TRUE;
		break;
	case WM_INITDIALOG:
		{

		SendDlgItemMessage(hWnd,IDC_COMBO_COMPILER,CB_ADDSTRING,0,(LPARAM)"cb-gcc");
		SendDlgItemMessage(hWnd,IDC_COMBO_COMPILER,CB_ADDSTRING,0,(LPARAM)"gnu");
		SendDlgItemMessage(hWnd,IDC_COMBO_COMPILER,CB_ADDSTRING,0,(LPARAM)"vs6");
		SendDlgItemMessage(hWnd,IDC_COMBO_COMPILER,CB_ADDSTRING,0,(LPARAM)"vs2002");
		SendDlgItemMessage(hWnd,IDC_COMBO_COMPILER,CB_ADDSTRING,0,(LPARAM)"vs2003");
		SendDlgItemMessage(hWnd,IDC_COMBO_COMPILER,CB_ADDSTRING,0,(LPARAM)"vs2005");
		SendDlgItemMessage(hWnd,IDC_COMBO_COMPILER,CB_ADDSTRING,0,(LPARAM)"vs2008");
		SendDlgItemMessage(hWnd,IDC_COMBO_COMPILER,CB_SETCURSEL,5,0);
		//idx=SendDlgItemMessage(hWnd,IDC_FILEFILTERCOMBO,CB_GETCURSEL,0,0);
		
		SendDlgItemMessage(hWnd,IDC_COMBO_EXAMPLE,CB_ADDSTRING,0,(LPARAM)"dynamic");
		SendDlgItemMessage(hWnd,IDC_COMBO_EXAMPLE,CB_ADDSTRING,0,(LPARAM)"tokamak");
		SendDlgItemMessage(hWnd,IDC_COMBO_EXAMPLE,CB_SETCURSEL,0,0);

//		DragAcceptFiles(GetDlgItem(hWnd,IDC_BASEE),TRUE);
		//SetWindowLongPtr
		wndprocEB = (WNDPROC)SetWindowLong(GetDlgItem(hWnd, IDC_BASEE),GWL_WNDPROC,(LONG)WindowProcEditBox);
		for (i =0; i < N; i++) {

		#define BUTTON_START IDC_BASEE+20

		HWND button = CreateWindow(
           "static", /* this makes a "static" */
           enames[i], /* this is the text which will appear in the static label */
           WS_VISIBLE | WS_CHILD,
           10, /* these four lines are the position and dimensions of the static */
           50+22*i,
           98,
           16,
           hWnd, /* this is the buttons parent window */
           (HMENU) (BUTTON_START + i), /* these next two lines pretty much tell windows what to do when the button is pressed */
           g_hInst,
           NULL);


		HWND buttoncheck = CreateWindow(
           "button", /* this makes a "button" */
           "", /* this is the text which will appear in the button */
           WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_CHECKBOX,
           90, /* these four lines are the position and dimensions of the button */
           50+22*i,
           15,
           18,
           hWnd, /* this is the buttons parent window */
           (HMENU) (BUTTON_START + i + N * 3), /* these next two lines pretty much tell windows what to do when the button is pressed */
           g_hInst,
           NULL);

		HWND edit = 
			CreateWindow(
           "edit", /* this makes a "edit" */
           "", /* this is the text which will appear in the edit */
           WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,
           110, /* these four lines are the position and dimensions of the button */
           50+22*i,
           230,
           18,
           hWnd, /* this is the buttons parent window */
           (HMENU) (BUTTON_START + i + N), /* these next two lines pretty much tell windows what to do when the button is pressed */
           g_hInst,
           NULL);

		HWND button2 = CreateWindow(
           "button", /* this makes a "button" */
           "...", /* this is the text which will appear in the button */
           WS_VISIBLE | WS_CHILD | WS_TABSTOP,
           350, /* these four lines are the position and dimensions of the button */
           50+22*i,
           18,
           18,
           hWnd, /* this is the buttons parent window */
           (HMENU) (BUTTON_START + i + N * 2), /* these next two lines pretty much tell windows what to do when the button is pressed */
           g_hInst,
           NULL);


		HFONT hf = (HFONT) SendMessage(GetDlgItem(hWnd,IDC_BASEE),WM_GETFONT,0,0);
		SendMessage(button, WM_SETFONT, (WPARAM) hf, FALSE);
		SendMessage(edit, WM_SETFONT, (WPARAM) hf, FALSE);
		SendMessage(button2, WM_SETFONT, (WPARAM) hf, FALSE);

		DragAcceptFiles(edit,TRUE);
		SetWindowLong(edit,GWL_WNDPROC,(LONG)WindowProcEditBox);

		}
		}
		break;
	case WM_COMMAND:
		{
		switch (LOWORD(wParam)) {
			case ID_S:
				{
					FILE *fout = fopen("settings.txt","w");
					if (fout) {
						GetDlgItemText(hWnd,IDC_BASEE,buf,MAX_PATH);
						CleanSlash(buf);
						fprintf(fout,"%s\n",buf);
						for (i=0;i<N;i++) {
							GetDlgItemText(hWnd,i+N+BUTTON_START,buf,MAX_PATH);
							CleanSlash(buf);
							fprintf(fout,"%s\n",buf);
						}
					}
				}
			break;
			case ID_L:
				{
					  FILE *fin = fopen("settings.txt","r");
					  if (fin) {
						  fscanf(fin,"%s\n",buf);
						  SetDlgItemText(hWnd,IDC_BASEE,buf);
						  for (i=0;i<N;i++) {
							  fscanf(fin,"%s\n",buf);
							  SetDlgItemText(hWnd,i+N+BUTTON_START,buf);
						  }
					  }
				}
				break;
			case IDC_BASEB:
				GetOpenDirName(g_szFilename,sizeof(g_szFilename) / sizeof(TCHAR),hWnd);
				//SetDlgItemText(hWnd,IDC_BASEE,g_szFilename);
				SetDirText(GetDlgItem(hWnd,IDC_BASEE),g_szFilename);
				for (i=0;i<N;i++) {
					sprintf(buf,"%s/%s",g_szFilename,edefaults[i]);
					SetDirText(GetDlgItem(hWnd,i+N+BUTTON_START),buf);
					SendDlgItemMessage(hWnd,i+N*3+BUTTON_START,BM_SETCHECK,BST_CHECKED,0);
				}
			break;
/*			case IDCANCEL:
				EndDialog(hWnd, 0);	
				return TRUE;	
			break;*/
			case IDOK:
				{
				FILE *fout = fopen("premake.lua","w");
				if (fout)
				{
				
					//base dir
					GetDlgItemText(hWnd,IDC_BASEE,buf,MAX_PATH);
					CleanSlash(buf);
					fprintf(fout,"lloc = \"%s\"\n",buf);
					
					//engine dir
					for (i=0;i<N;i++) {
						GetDlgItemText(hWnd,i+N+BUTTON_START,buf,MAX_PATH);
						fprintf(fout,"dir%s = \"%s\"\n",enames[i],buf);
					}

					for (i=0;i<N;i++) {
						if (SendDlgItemMessage(hWnd,i+N*3+BUTTON_START,BM_GETCHECK,0,0) == BST_CHECKED) {
							fprintf(fout,"make_%s = true\n",tolowers(enames[i]));
						}
					}

					fprintf(fout,"dofile(\"premake_template.lua\")\n");
				/*	//template
					FILE *fin = fopen("premake_template.lua","r");
					if (fin) {
						char *p;
						while (( p = fgets(buf, MAX_PATH, fin))!=NULL) {
							 fputs(buf, fout);
						}
						fclose(fin);
					} else 
						MessageBox(hWnd,"Could not open premake_template.lua","Error",MB_OK);*/
					fclose(fout);
				}
					else
				MessageBox(hWnd,"Could not open premake.lua","Error",MB_OK);

					char cmd[256];
					GetDlgItemText(hWnd,IDC_COMBO_COMPILER,buf,MAX_PATH);
					sprintf(cmd,"premake --target %s",buf);
					system(cmd);
				}
				MessageBox(hWnd,"Done","Done",MB_OK);
				break;
		}
		WORD x = LOWORD(wParam);
		if ((x>=BUTTON_START + N*2) && (x<BUTTON_START + N*3)) {
			GetOpenDirName(g_szFilename,sizeof(g_szFilename) / sizeof(TCHAR),hWnd);
			//SetDlgItemText(hWnd,x-N,g_szFilename);
			SetDirText(GetDlgItem(hWnd,x-N),g_szFilename);
		}
		if (x>=BUTTON_START + N*3) {
			switch (SendDlgItemMessage(hWnd,x,BM_GETCHECK,0,0) ) {
			case BST_CHECKED:
				SendDlgItemMessage(hWnd,x,BM_SETCHECK,BST_UNCHECKED,0);
				break;
			default:
				SendDlgItemMessage(hWnd,x,BM_SETCHECK,BST_CHECKED,0);
				break;
			}
		}
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);	
		return TRUE;
		break;
	}
	return FALSE;
}


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT) {
	g_hInst	= hInst;
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)MainDialogProc, 0);
	return 0;
}