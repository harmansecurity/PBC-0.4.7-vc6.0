#include "stdafx.h"

#include <stdio.h>
#include <time.h>

extern "C" {
	double get_time(void);
}

double get_time(void)
{
#ifndef WIN32
    static clock_t last_clock = 0;
    static double acc = 0;

    clock_t this_clock;
    double delta;

    this_clock = clock();

    if (last_clock == 0) {
		delta = 0;
    } else {
		delta = (this_clock - last_clock)/((double)(CLOCKS_PER_SEC/1000));
		if (delta < 0) 
			delta = 0;
    }

    acc += delta;
    last_clock = this_clock;

    return acc;
#else
	static LARGE_INTEGER last_clock;
	static double acc=0;

	LARGE_INTEGER this_clock, timerFrequency;
	double delta, tfreq;

	QueryPerformanceFrequency(&timerFrequency);
	tfreq = (double) timerFrequency.QuadPart;

	QueryPerformanceCounter(&this_clock);

	if(last_clock.QuadPart==0){
		delta=0;
	}
	else{
		delta=(this_clock.QuadPart - last_clock.QuadPart)/tfreq; //second
		if(delta<0)
			delta=0;
	}

    acc += delta;
    last_clock = this_clock;
	return acc;

#endif
}
