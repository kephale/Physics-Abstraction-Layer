#ifndef BW_WINDOWS_TIMER_H
#define BW_WINDOWS_TIMER_H

/*
 *  BWTimerWindows.h
 *  Windows specific Timer class
 *
 *  Created by Brock Woolf on 27/01/10.
 *  Copyright 2010 Blastcube. All rights reserved.
 *
 */

#include "BWHighResolutionTimer.h"
#include <windows.h>

namespace BWObjects {
	
	class WindowsTimer : public HighResolutionTimer {
		
	public:
		WindowsTimer() { QueryPerformanceFrequency(&m_frequency); m_startCount.QuadPart = m_endCount.QuadPart = 0;}
		~WindowsTimer();
		
		void	Start() { m_startTimeInMicroseconds = m_endTimeInMicroseconds = m_elapsedTimeInMicroseconds = 0 ; m_stopped = false; QueryPerformanceCounter(&m_startCount); }
		void	Stop()  { m_stopped = true; QueryPerformanceCounter(&m_endCount); }
		
		double	GetElapsedTimeInMicroseconds() { 
			if ( m_stopped == false ) { QueryPerformanceCounter(&m_endCount); }
			m_startTimeInMicroseconds = m_startCount.QuadPart * (1000000.0 / m_frequency.QuadPart);
			m_endTimeInMicroseconds = m_endCount.QuadPart * (1000000.0 / m_frequency.QuadPart);
			return m_endTimeInMicroseconds - m_startTimeInMicroseconds;
		}
		
		double GetElapsedTimeInMilliseconds() { return this->GetElapsedTimeInMicroseconds() * 0.001; }
		double GetElapsedTimeInSeconds() { return this->GetElapsedTimeInMicroseconds() * 0.000001 ; }
		
	private:
		double			m_startTimeInMicroseconds;
		double			m_endTimeInMicroseconds;
		double			m_elapsedTimeInMicroseconds;
		bool			m_stopped;
		
		LARGE_INTEGER	m_frequency;	// ticks per second
		LARGE_INTEGER	m_startCount;
		LARGE_INTEGER	m_endCount;
	};
}

#endif