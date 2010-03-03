#ifndef BW_ABSTRACT_TIMER_H
#define BW_ABSTRACT_TIMER_H

/*
 *  BWAbstractTimer.h
 *  A cross platform High Resolution Timer class for Windows, Mac OS X and Linux  
 *
 *  Created by Brock Woolf on 27/01/10.
 *  Copyright 2010 Blastcube. All rights reserved.
 *
 */

#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

namespace BWObjects {
	
	using namespace std;
	
	class HighResolutionTimer {
	public:
		virtual void	Start() = 0;
		virtual void	Stop()	= 0;
		virtual	double	GetElapsedTimeInMicroseconds() = 0;
		virtual	double	GetElapsedTimeInMilliseconds() = 0;
		virtual	double	GetElapsedTimeInSeconds() = 0;
	};
}

#endif
