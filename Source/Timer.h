#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Reset();
	void Pause();
	void Resume();

	Uint32 Read() const;

private:

	bool	running = false;
	bool	reset = false;
	Uint32	started_at;
	Uint32	stopped_at;
	Uint32 time_paused;
};

#endif //__TIMER_H__