#pragma once
#include "framework.h"
#include "Object.h"

#define DEFAULT_TICKRATE 100
#define DEFAULT_SPT 0.01

namespace seq {
	class Time : public Object
	{
		int tickrate;
		double spt; // seconds per tick
		double cpt; // counts per tick
		double deltaTime;
		double secondsPerCount;
		double ticksPerCount;
		int64_t currentTime;
		double currentTick;
		int64_t deltaA;
		int64_t startTime;
		uint64_t lastTick;
		//double currentFPS;
	public:
		Time();
		~Time();
		const double CurrentTime() const;
		const uint64_t CurrentTick() const;
		const double CurrentDelta() const;
		void Init() override;
		void Tick() override;
		void Destroy() override;
		void Run();
	};
}