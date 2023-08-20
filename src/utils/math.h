#pragma once

namespace Math
{
	static inline int Sgn(float value)
	{
		return (0.0f < value) - (value < 0.0f);
	}
};
