#include "Time.h"

namespace seq {
	Time::Time() : deltaTime(-1.0), spt(DEFAULT_SPT), tickrate(DEFAULT_TICKRATE), currentTick(0.0), currentTime(0.0), deltaA(-1), startTime(0.0),
		lastTick(0)
	{
		int64_t cps = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&cps);
		secondsPerCount = 1.0 / (double)cps;
		cpt = spt / secondsPerCount;
		ticksPerCount = 1.0 / cpt;
	}
	Time::~Time()
	{
		Destroy();
	}
	const double Time::CurrentTime() const {
		return currentTime * secondsPerCount;
	}

	const uint64_t Time::CurrentTick() const {
		return lastTick;
	}

	const double Time::CurrentDelta() const {
		return deltaTime;
	}
	void Time::Init()
	{
		int64_t currtime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currtime);
		currentTime = currtime;
		startTime = currtime;
	}
	void Time::Tick()
	{
		// Get Counter Accuracy
		int64_t currtime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currtime);
		currentTime = currtime;

		// Tickrate stuff
		lastTick = (currentTime - startTime) * secondsPerCount * ticksPerCount;

		// Get Delta Time
		deltaTime = (currentTime - deltaA) * secondsPerCount;

		deltaA = currentTime;

		// Check if less than zero
		deltaTime *= deltaTime > 0.0;
	}
	void Time::Destroy()
	{

	}
	void Time::Run()
	{
		Init();
	}
}