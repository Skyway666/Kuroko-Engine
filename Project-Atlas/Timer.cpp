// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	reset = false;
	running = true;
	started_at = SDL_GetTicks();
	time_paused = started_at;
	stopped_at = 0;
}

// ---------------------------------------------
void Timer::Reset()
{
	reset = true;
}

void Timer::Pause() {
	stopped_at = SDL_GetTicks();
	running = false;
}

void Timer::Resume() {
	time_paused += (SDL_GetTicks() - stopped_at);
	running = true;
}

// ---------------------------------------------
Uint32 Timer::Read() const
{
	if (!reset)
	{
		if (running)
			return SDL_GetTicks() - time_paused;
		else
			return stopped_at;
	}
	else
		return 0;
}