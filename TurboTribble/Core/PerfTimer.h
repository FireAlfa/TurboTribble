#ifndef __PERF_TIMER_H__
#define __PERF_TIMER_H__

#include "p2Defs.h"



class PerfTimer
{
public:

	// Constructor
	PerfTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	uint64	startedAt;
	static uint64 frequency;
};

#endif // !__PERF_TIMER_H__