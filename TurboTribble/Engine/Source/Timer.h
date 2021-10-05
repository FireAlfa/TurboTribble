#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor, calls Start()
	Timer();

	// Start the timer
	void Start();
	// Stop the timer
	void Stop();

	// Read the timer
	Uint32 Read();

private:

	bool	running;
	Uint32	startedAt;
	Uint32	stoppedAt;
};

#endif //__TIMER_H__