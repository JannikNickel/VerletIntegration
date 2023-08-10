#pragma once
#include <deque>

class FrameCounter
{
public:
	FrameCounter(double updateRate = 2.0f);
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
};
