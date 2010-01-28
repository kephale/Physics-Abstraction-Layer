#ifndef BW_UNIX_TIMER_H
#define BW_UNIX_TIMER_H

/*
 *  BWTimerUnix.h
 *  Mac OS X and Linux specific Timer class
 *
 *  Created by Brock Woolf on 27/01/10.
 *  Copyright 2010 Blastcube. All rights reserved.
 *
 */

#include "BWHighResolutionTimer.h"
#include <sys/time.h>

namespace BWObjects {
	
	class UnixTimer : public HighResolutionTimer {

	public:
		UnixTimer() { m_startCount.tv_sec = m_startCount.tv_usec = m_endCount.tv_sec = m_endCount.tv_usec = 0; }
		~UnixTimer();
		
		void	Start() { m_startTimeInMicroseconds = m_endTimeInMicroseconds = m_elapsedTimeInMicroseconds = 0 ; m_stopped = false; gettimeofday(&m_startCount, NULL); }
		void	Stop()  { m_stopped = true; gettimeofday(&m_endCount, NULL); }
		
		double	GetElapsedTimeInMicroseconds() { 
			if ( m_stopped == false ) { gettimeofday(&m_endCount, NULL); }
			m_startTimeInMicroseconds = (m_startCount.tv_sec * 1000000.0) + m_startCount.tv_usec;
			m_endTimeInMicroseconds = (m_endCount.tv_sec * 1000000.0) + m_endCount.tv_usec;
			return m_endTimeInMicroseconds - m_startTimeInMicroseconds;
		}

		double GetElapsedTimeInMilliseconds() { return this->GetElapsedTimeInMicroseconds() * 0.001; }
		double GetElapsedTimeInSeconds() { return this->GetElapsedTimeInMicroseconds() * 0.000001 ; }
		
	private:
		double		m_startTimeInMicroseconds;
		double		m_endTimeInMicroseconds;
		double		m_elapsedTimeInMicroseconds;
		bool		m_stopped;
		
		timeval		m_startCount;
		timeval		m_endCount;
	};
}

#endif