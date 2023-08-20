#include "framecounter.h"

FrameCounter::FrameCounter(double updateRate) : updateRate(updateRate)
{

}

void FrameCounter::BeginFrame()
{
	tFrameStart = Clock::now();
}

double FrameCounter::EndFrame()
{
	double dt = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - tFrameStart).count() * 1e-9;
	Frame(dt);
	return dt;
}

void FrameCounter::Frame(double dt)
{
	time += dt;
	frameSum += dt;
	frames.push_back(dt);
	while(frameSum - frames.front() > updateRate)
	{
		frameSum -= frames.front();
		frames.pop_front();
	}
}

double FrameCounter::Framerate() const
{
	return frames.size() / updateRate;
}

double FrameCounter::Frametime() const
{
	if(frames.empty())
	{
		return 0.0;
	}
	return frameSum / frames.size();
}

size_t FrameCounter::FrameCount() const
{
	return framecount;
}

double FrameCounter::Time() const
{
	return time;
}
