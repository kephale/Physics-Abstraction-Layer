#ifndef TEST_LIB_H
#define TEST_LIB_H

#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info

#include "../pal/palFactory.h"
#include "../example/graphics.h"
#include "../example/resource.h"
#ifdef WIN32
#include <windows.h>
#endif

//#ifdef NDEBUG
//#pragma comment(lib, "libpal.lib")
//#pragma comment(lib, "libtest.lib")
//#else
//#pragma comment(lib, "libpal.lib")
//#pragma comment(lib, "libtest.lib")
//#endif

extern PAL_STRING g_engine;
extern Float step_size;

#ifdef WIN32
extern BOOL MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//I hesitate to add this. RDTSC, vs timeGetTime vs QueryPerformanceCounter. ?. 
//problems with QPC: http://support.microsoft.com/kb/274323
//RDTSC: http://www.virtualdub.org/blog/pivot/entry.php?id=106 (among other issues..)
class Timer  {
public:
	Timer() {
		m_Total = 0;
	}
	void StartSample() {
		QueryPerformanceCounter((LARGE_INTEGER *)&m_StartTime);
	}
	void EndSample() {
		__int64 endTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&endTime);
		m_Total += endTime - m_StartTime;
	}
	double GetElapsedTime() {
		__int64 freq;
		QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
		return m_Total/(double)freq;
	}
	__int64 m_StartTime; //temp
	__int64 m_Total;
	
};
#endif



#endif