#include "test_lib.h"

Float step_size = 0.01f;
PAL_STRING g_engine;

BOOL MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	DWORD ret;
	
	switch (uMsg)
	{
	case WM_CREATE:
		return TRUE;
		break;
	case WM_INITDIALOG:
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Bullet");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Dynamechs [disabled]");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Jiggle");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Meqon [deprecated]");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Newton");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Novodex");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"ODE");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Open Tissue [disabled]");
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"Tokamak");
#ifdef MICROSOFT_VC_6
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"True Axis [disabled]");
#else
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)"True Axis");
#endif
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_SETCURSEL,0,0);
		return TRUE;
		break;
	case WM_COMMAND:	
		switch (LOWORD(wParam)) {
		case IDOK:
			ret=SendDlgItemMessage(hWnd,IDC_LIST1,LB_GETCURSEL,0,0);
			switch (ret) {
			case 0:
				g_engine = "Bullet";
				PF->SelectEngine("Bullet");
				break;
			case 1:
				g_engine = "Dynamechs";
				PF->SelectEngine("Dynamechs");
				break;
			case 2:
				g_engine = "Jiggle";
				PF->SelectEngine("Jiggle");
				break;
			case 3:
				g_engine = "Meqon";
				PF->SelectEngine("Meqon");
				break;
			case 4:
				g_engine = "Newton";
				PF->SelectEngine("Newton");
				break;
			case 5:
				g_engine = "Novodex";
				PF->SelectEngine("Novodex");
				break;
			case 6:
				g_engine = "ODE";
				PF->SelectEngine("ODE");
				break;
			case 7:
				g_engine = "OpenTissue";
				PF->SelectEngine("OpenTissue");
				break;
			case 8:
				g_engine = "Tokamak";
				PF->SelectEngine("Tokamak");
				break;
			case 9:
				g_engine = "TrueAxis";
				PF->SelectEngine("TrueAxis");
				break;
			}
			EndDialog(hWnd, 0);	
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
