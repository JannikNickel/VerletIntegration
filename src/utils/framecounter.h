#pragma once
#include <deque>
#include <chrono>

class FrameCounter
{
	using Clock = std::chrono::high_resolution_clock;

public:
	FrameCounter(double updateRate = 2.0f);
	void BeginFrame();
	double EndFrame();
	void Frame(double dt);
	double Framerate() const;
	double Frametime() const;
	size_t FrameCount() const;
	double Time() const;

private:
	double updateRate;
	std::deque<double> frames = {};
	double frameSum = 0.0;
	double time = 0.0;
	size_t framecount = 0;
	Clock::time_point tFrameStart;
};
