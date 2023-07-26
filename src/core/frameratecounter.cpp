#include "frameratecounter.h"

FrameRateCounter::FrameRateCounter(double updateRate) : updateRate(updateRate)
{

}

void FrameRateCounter::Frame(double dt)
{
	frameCount++;
	frameTime += dt;
	if(frameTime > 1.0 / updateRate)
	{
		fps = frameCount * updateRate;
		frameCount = 0;
		frameTime -= 1.0 / updateRate;
	}
}

double FrameRateCounter::Fps() const
{
	return fps;
}
