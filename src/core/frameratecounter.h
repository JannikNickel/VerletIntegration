#pragma once

class FrameRateCounter
{
public:
	FrameRateCounter(double updateRate = 2.0f);
	void Frame(double dt);
	double Fps() const;

private:
	double updateRate = 2.0;
	int frameCount = 0;
	double frameTime = 0.0;
	double fps = 0;
};
