#include "timer.h"
#include <windows.h>

Timer::Timer()
	{
	QueryPerformanceFrequency(&timerFrequency);
	tfreq=(double)timerFrequency.QuadPart;	
	// calibration
	starttimer();
	stoptimer();
	tcorrection=(double)(endCount.QuadPart-startCount.QuadPart);
	tbegin=tstart;
	}

Timer::~Timer()
	{}

void Timer::starttimer(void)
	{
	time=0.0;
	QueryPerformanceCounter(&startCount);
	tstart=(double)startCount.QuadPart;
	}

void Timer::stoptimer(void)
	{
	QueryPerformanceCounter(&endCount);
	tstop=(double)endCount.QuadPart;
	}

void Timer::update(void)
	{
	stoptimer();
	dt=(float)((tstop-tstart-tcorrection)/(tfreq));
	elapsed=(float)((tstop-tbegin)/tfreq);
	starttimer();
	}