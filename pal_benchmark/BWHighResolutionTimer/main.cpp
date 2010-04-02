#include <iostream>
#include "BWAbstractTimer.h"

#ifdef _WIN32
	#include "BWTimerWindows.h"
#else
	#include "BWTimerUnix.h"
#endif

int main (int argc, char * const argv[]) {

	BWObjects::BWHighResolutionTimer *t;
#ifdef _WIN32
	t = new BWObjects::WindowsTimer();
#else
	t = new BWObjects::UnixTimer();
#endif
	
	t->Start();
#ifdef _WIN32
	Sleep(1000);	// Milliseconds to sleep for
#else
	sleep(1);		// Seconds to sleep for
#endif
	t->Stop();
	
	double result = 0;
	
	result = t->GetElapsedTimeInMicroseconds();
	printf("Timer took: %lf microseconds\n", result);
	
	result = t->GetElapsedTimeInMilliseconds();
	printf("Timer took: %lf milliseconds\n", result);
	
	result = t->GetElapsedTimeInSeconds();
	printf("Timer took: %lf seconds\n", result);
	
    return 0;
}
