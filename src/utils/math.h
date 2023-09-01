#pragma once
#include <cmath>
#include <algorithm>

namespace Math
{
	inline int Sgn(float value)
	{
		return (0.0f < value) - (value < 0.0f);
	}

	inline float PingPong(float value, float length)
	{
		float norm = (value - length) / (length * 2.0f);
		return std::fabsf((norm - std::floorf(norm)) * length * 2.0f - length) / length;
	}

	inline float SmoothStep(float value)
	{
		return (value * value) * (3.0f - 2.0f * value);
	}

	inline float SmootherStep(float value)
	{
		return 6.0f * std::powf(value, 5.0f) - 15.0f * std::powf(value, 4.0f) + 10.0f * std::powf(value, 3.0f);
	}

	inline float Lerp(float from, float to, float t)
	{
		return from + (to - from) * std::clamp(t, 0.0f, 1.0f);
	}

	inline float InverseLerp(float from, float to, float value)
	{
		if(from == to)
		{
			return 0.0f;
		}
		return std::clamp((value - from) / (to - from), 0.0f, 1.0f);
	}
};
