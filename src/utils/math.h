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
};
