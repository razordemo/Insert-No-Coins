#ifndef TIMER_H
#define TIMER_H

#include <windows.h>

class Timer	// high resolution timer
	{
	protected:
		LARGE_INTEGER timerFrequency;
		LARGE_INTEGER startCount;
		LARGE_INTEGER endCount;

		LARGE_INTEGER beginCount;
		LARGE_INTEGER currentCount;
		
		double tfreq;
		double time;
		double tstart;
		double tstop;
		double tbegin;
		double tcurrent;

	public:
		Timer(void);
		~Timer(void);
		double tcorrection;

		void starttimer(void);
		void stoptimer(void);
		void update(void);

		float elapsed;
		float dt;
	};

#endif
